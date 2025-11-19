#include "shutterctrlwidget.h"
#include "shutteripcbackend.h" // for type only (optional)
#include <QJsonArray>
#include <QJsonObject>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QShutterCtrlWidget::QShutterCtrlWidget(QList<QShutterDevCtrl*> pCtrllist, QWidget * parent) : QWidget(parent)
{
    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0, 1);
    playout->setRowStretch(1, 1);
    playout->setRowStretch(2, 4);

    // 仅显示第一个光闸控件（如需多个可改回循环）
    if (!pCtrllist.isEmpty()) {
        QShutterCtrlItemWidget* pShutterItemWidget = new QShutterCtrlItemWidget(pCtrllist.first(), this);
        playout->addWidget(pShutterItemWidget, 0, 0, 1, 1);
    }
}

/*******************************************************************
**功能：子组件
**输入：
**输出：
**返回值：
*******************************************************************/
QShutterCtrlItemWidget::QShutterCtrlItemWidget(QShutterDevCtrl* pCtrl, QWidget* parent) : QGroupBox(parent)
{
    _pShutterCtrl = pCtrl;

    InitUI();

    m_pSetLimitDataButton->setHidden(true);

    connect(m_pOpenButton, &QPushButton::clicked, this, &QShutterCtrlItemWidget::onOpenShutter);
    connect(m_pCloseButton, &QPushButton::clicked, this, &QShutterCtrlItemWidget::onCloseShutter);
    connect(m_pRefreshButton, &QPushButton::clicked, this, &QShutterCtrlItemWidget::onCheckStatus);
    connect(m_pSetLimitDataButton, &QPushButton::clicked, this, &QShutterCtrlItemWidget::onSetLimitData);
    connect(m_psetProcessStatusButton, &QPushButton::clicked, this, &QShutterCtrlItemWidget::onSetProcessStatus);

    // 新增连接相关槽
    connect(this->findChild<QPushButton*>(""), &QPushButton::clicked, this, &QShutterCtrlItemWidget::onRefreshDevices); // no-op placeholder
    connect(this, &QShutterCtrlItemWidget::onRefreshDevices, this, &QShutterCtrlItemWidget::onRefreshDevices); // keep compiler happy

    connect(m_pRefreshDevicesButton, &QPushButton::clicked, this, &QShutterCtrlItemWidget::onRefreshDevices);
    connect(m_pConnectButton, &QPushButton::clicked, this, &QShutterCtrlItemWidget::onConnectDevice);
    connect(m_pDisconnectButton, &QPushButton::clicked, this, &QShutterCtrlItemWidget::onDisconnectDevice);

    setWidgetStyleSheet(this);

    // 启动时不再强制执行关闭操作，用户可根据需要手动刷新并连接
    // onCloseShutter();//启动时自动关闭 改为不自动执行
}

/*******************************************************************
**功能：初始化界面
**输入：
**输出：
**返回值：
*******************************************************************/
void QShutterCtrlItemWidget::InitUI()
{
    QLabel* pDevName = new QLabel(this);
    pDevName->setText(_pShutterCtrl->m_sDevName);
    pDevName->setStyleSheet("color:blue;font-size:16px;");

    QLabel* plabel1 = new QLabel(this);
    plabel1->setText("设备IP:");

    QLabel* plabel2 = new QLabel(this);
    plabel2->setText(_pShutterCtrl->m_tShutterDevInfo.tHostAddress.toString());

    QLabel* plabel3 = new QLabel(this);
    plabel3->setText("当前状态:");

    QLabel* plabel8 = new QLabel(this);
    plabel8->setText("设置发次数");

    m_pStatus = new QLabel(this);

    m_psetPlusCntEdit = new QTextEdit(this);
    m_psetPlusCntEdit->setText("1");

    m_pCloseButton = new QPushButton("挡光");
    m_pOpenButton = new QPushButton("放光");
    m_pRefreshButton = new QPushButton("状态查询");
    m_pSetLimitDataButton = new QPushButton("设置电压门限");

    m_psetProcessStatusButton = new QPushButton("放N发次光");

    // 新增：设备选择与连接控件
    m_pDeviceCombo = new QComboBox(this);
    m_pDeviceCombo->setMinimumWidth(240);
    m_pRefreshDevicesButton = new QPushButton("刷新设备");
    m_pConnectButton = new QPushButton("连接设备");
    m_pDisconnectButton = new QPushButton("断开连接");
    m_pDisconnectButton->setEnabled(false);

    // 日志输出
    m_pLogEdit = new QTextEdit(this);
    m_pLogEdit->setReadOnly(true);
    m_pLogEdit->setMaximumHeight(120);

    m_pCloseButton->setMaximumWidth(100);
    m_pCloseButton->setMinimumHeight(30);

    m_pOpenButton->setMaximumWidth(100);
    m_pOpenButton->setMinimumHeight(30);

    m_pSetLimitDataButton->setMaximumWidth(100);
    m_pSetLimitDataButton->setMinimumHeight(30);

    m_pRefreshButton->setMaximumWidth(100);
    m_pRefreshButton->setMinimumHeight(30);

    m_psetProcessStatusButton->setMaximumWidth(100);
    m_psetProcessStatusButton->setMinimumHeight(30);

    m_psetPlusCntEdit->setMaximumWidth(200);
    m_psetPlusCntEdit->setMaximumHeight(30);

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(new QLabel("设备名:"), 0, 0, 1, 1);
    playout->addWidget(pDevName, 0, 1, 1, 1);
    playout->addWidget(plabel1, 1, 0, 1, 1);
    playout->addWidget(plabel2, 1, 1, 1, 1);
    playout->addWidget(plabel3, 2, 0, 1, 1);
    playout->addWidget(m_pStatus, 2, 1, 1, 1);

    // 放入设备选择行
    playout->addWidget(new QLabel("可用设备:"), 3, 0, 1, 1);
    playout->addWidget(m_pDeviceCombo, 3, 1, 1, 1);
    playout->addWidget(m_pRefreshDevicesButton, 3, 2, 1, 1);
    playout->addWidget(m_pConnectButton, 3, 3, 1, 1);
    playout->addWidget(m_pDisconnectButton, 3, 4, 1, 1);

    playout->addWidget(plabel8, 4, 0, 1, 1);
    playout->addWidget(m_psetPlusCntEdit, 4, 1, 1, 1);

    playout->addWidget(m_pOpenButton, 5, 0, 1, 1);
    playout->addWidget(m_pCloseButton, 5, 1, 1, 1);
    playout->addWidget(m_pRefreshButton, 6, 0, 1, 1);
    playout->addWidget(m_pSetLimitDataButton, 6, 1, 1, 1);
    playout->addWidget(m_psetProcessStatusButton, 6, 1, 1, 1);

    // 日志放在底部
    playout->addWidget(new QLabel("操作日志:"), 7, 0, 1, 1);
    playout->addWidget(m_pLogEdit, 8, 0, 1, 5);

    showStatus();
}

// 日志记录小助手
static void appendLog(QTextEdit* logWidget, const QString& s) {
    if (!logWidget) return;
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    logWidget->append(QString("[%1] %2").arg(now, s));
}

void QShutterCtrlItemWidget::showStatus()
{
    if (_pShutterCtrl->m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_NONE)
    {
        m_pStatus->setText("未知状态");
    }

    if (_pShutterCtrl->m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_OPENED)
    {
        m_pStatus->setText("放光");
    }
    if (_pShutterCtrl->m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_CLOSED)
    {
        m_pStatus->setText("挡光");
    }

    QString scurrentdata = QString("%1").arg(_pShutterCtrl->m_tShutterDevInfo.nCurrentVoltData);
    //m_pshowCurrentDataLabel->setText(scurrentdata);

    QString sdata = QString("%1").arg(_pShutterCtrl->m_tShutterDevInfo.nSetLimitData);
    //  m_pSetLimitEdit->setText(sdata);
}

void QShutterCtrlItemWidget::onRefreshDevices()
{
    if (!_pShutterCtrl->backend()) {
        appendLog(m_pLogEdit, "未配置光闸后端，无法刷新设备");
        QMessageBox::warning(this, "提示", "未配置光闸后端");
        return;
    }

    appendLog(m_pLogEdit, "开始刷新设备...");
    QJsonArray devs = _pShutterCtrl->backend()->listDevices(2000);
    m_pDeviceCombo->clear();

    if (devs.isEmpty()) {
        appendLog(m_pLogEdit, "未检测到设备（listDevices 返回空）");
        QMessageBox::information(this, "提示", "未检测到光闸设备");
        return;
    }

    // 填充 combobox，存储 module/device 索引到 itemData（QVariantMap）
    for (int i = 0; i < devs.size(); ++i) {
        QJsonValue v = devs.at(i);
        QString label;
        QVariantMap meta;
        if (v.isObject()) {
            QJsonObject obj = v.toObject();
            // 常见字段：name/moduleIndex/deviceIndex 或 id 等
            QString name = obj.value("name").toString();
            if (name.isEmpty()) name = obj.value("devName").toString();
            int mi = obj.value("moduleIndex").toInt(-1);
            int di = obj.value("deviceIndex").toInt(-1);
            if (name.isEmpty()) name = QString("device_%1").arg(i);
            label = QString("%1 (m=%2,d=%3)").arg(name).arg(mi).arg(di);
            meta["moduleIndex"] = mi;
            meta["deviceIndex"] = di;
        }
        else {
            label = QString("device_%1").arg(i);
            meta["moduleIndex"] = 0;
            meta["deviceIndex"] = i;
        }
        m_pDeviceCombo->addItem(label, QVariant::fromValue(meta));
    }

    appendLog(m_pLogEdit, QString("找到 %1 个设备").arg(devs.size()));
}

void QShutterCtrlItemWidget::onConnectDevice()
{
    if (!_pShutterCtrl->backend()) {
        appendLog(m_pLogEdit, "未配置光闸后端，无法连接设备");
        QMessageBox::warning(this, "提示", "未配置光闸后端");
        return;
    }

    if (m_pDeviceCombo->count() == 0) {
        appendLog(m_pLogEdit, "设备列表为空，请先刷新设备");
        QMessageBox::information(this, "提示", "请先刷新设备列表");
        return;
    }

    QVariant metaVar = m_pDeviceCombo->currentData();
    QVariantMap meta = metaVar.toMap();
    int mi = meta.value("moduleIndex", 0).toInt();
    int di = meta.value("deviceIndex", 0).toInt();

    appendLog(m_pLogEdit, QString("尝试连接设备 m=%1 d=%2 ...").arg(mi).arg(di));
    int sid = _pShutterCtrl->backend()->connectDevice(mi, di, 3000);
    if (sid <= 0) {
        appendLog(m_pLogEdit, "连接设备失败（connectDevice 返回 <= 0）");
        QMessageBox::warning(this, "提示", "连接光闸失败");
        m_pDisconnectButton->setEnabled(false);
        return;
    }
    _pShutterCtrl->setSessionId(sid);
    appendLog(m_pLogEdit, QString("连接成功，sessionId=%1").arg(sid));
    m_pDisconnectButton->setEnabled(true);
    QMessageBox::information(this, "提示", "光闸已连接");
}

void QShutterCtrlItemWidget::onDisconnectDevice()
{
    if (!_pShutterCtrl->backend()) {
        appendLog(m_pLogEdit, "未配置光闸后端，无法断开设备");
        QMessageBox::warning(this, "提示", "未配置光闸后端");
        return;
    }
    int sid = _pShutterCtrl->sessionId();
    if (sid <= 0) {
        appendLog(m_pLogEdit, "当前没有有效 sessionId，忽略断开");
        QMessageBox::information(this, "提示", "当前没有连接");
        return;
    }
    bool ok = _pShutterCtrl->backend()->disconnectDevice(sid, 2000);
    if (ok) {
        appendLog(m_pLogEdit, QString("断开设备 sessionId=%1 成功").arg(sid));
        _pShutterCtrl->setSessionId(-1);
        m_pDisconnectButton->setEnabled(false);
        QMessageBox::information(this, "提示", "设备已断开");
    }
    else {
        appendLog(m_pLogEdit, QString("断开设备 sessionId=%1 失败").arg(sid));
        QMessageBox::warning(this, "提示", "断开连接失败");
    }
}

void QShutterCtrlItemWidget::onOpenShutter()
{
    // 先检查连接状态
    if (!_pShutterCtrl->backend() || _pShutterCtrl->sessionId() <= 0) {
        QMessageBox::warning(this, "提示", "未连接光闸，请先刷新并连接设备");
        appendLog(m_pLogEdit, "打开失败：未连接光闸");
        return;
    }

    bool ok = _pShutterCtrl->SetDevStatus(true);
    if (!ok) {
        appendLog(m_pLogEdit, "打开光闸失败（SetDevStatus 返回 false）");
        QMessageBox::warning(this, "提示", "打开光闸失败");
        return;
    }
    appendLog(m_pLogEdit, "打开光闸成功");
    showStatus();
}

void QShutterCtrlItemWidget::onCloseShutter()
{
    if (!_pShutterCtrl->backend() || _pShutterCtrl->sessionId() <= 0) {
        QMessageBox::warning(this, "提示", "未连接光闸，请先刷新并连接设备");
        appendLog(m_pLogEdit, "关闭失败：未连接光闸");
        return;
    }

    bool ok = _pShutterCtrl->SetDevStatus(false);
    if (!ok) {
        appendLog(m_pLogEdit, "关闭光闸失败（SetDevStatus 返回 false）");
        QMessageBox::warning(this, "提示", "关闭光闸失败");
        return;
    }
    appendLog(m_pLogEdit, "关闭光闸成功");
    showStatus();
}

//查询状态
void QShutterCtrlItemWidget::onCheckStatus()
{
    if (!_pShutterCtrl->backend() || _pShutterCtrl->sessionId() <= 0) {
        appendLog(m_pLogEdit, "光闸无连接信息");
        QMessageBox::information(this, "提示", "光闸无连接信息");
        return;
    }

    // 1) 首先尝试通过 helper 的 read_param 获取状态（如果 helper/SDK 支持）
    QJsonObject r = _pShutterCtrl->backend()->readParam(_pShutterCtrl->sessionId(), QStringLiteral("status"), 0, 2000);
    if (r.isEmpty()) {
        appendLog(m_pLogEdit, "readParam 返回空，尝试通过串口直接查询（com_write）设备回显");
        // 退到第二种策略：通过串口向设备发送一个常见查询命令并解析回显
        // 尝试常见查询命令，如 "shutter1?" 或 "status?" （不同设备协议不同）
        QString tryCmds[] = { QString("shutter1?\r\n"), QString("status?\r\n"), QString("shutter1 status\r\n") };

        for (const QString& cmd : tryCmds) {
            appendLog(m_pLogEdit, QString("尝试串口查询命令: %1").arg(cmd.trimmed()));
            QJsonObject r2 = _pShutterCtrl->backend()->comWrite(_pShutterCtrl->sessionId(), cmd, 1500);
            if (r2.isEmpty()) {
                appendLog(m_pLogEdit, QString("comWrite(%1) 无响应").arg(cmd.trimmed()));
                continue;
            }
            appendLog(m_pLogEdit, QString("串口原始响应: %1").arg(QString::fromLatin1(QJsonDocument(r2).toJson(QJsonDocument::Compact))));
            // 解析 r2.payload.response
            if (r2.value("status").toString() != "ok") {
                appendLog(m_pLogEdit, QString("comWrite 返回非 ok: %1").arg(r2.value("status").toString()));
                continue;
            }
            if (!r2.contains("payload") || !r2.value("payload").isObject()) {
                appendLog(m_pLogEdit, "comWrite 返回没有 payload 或 payload 非对象");
                continue;
            }
            QJsonObject payload = r2.value("payload").toObject();
            QString resp = payload.value("response").toString().trimmed();
            if (resp.isEmpty()) {
                appendLog(m_pLogEdit, "comWrite payload.response 为空");
                continue;
            }
            QString lower = resp.toLower();
            // 识别常见回显：1/0、open/close、ok 等
            if (lower.contains("open") || lower.contains("ok") || lower == "1") {
                _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_OPENED;
                appendLog(m_pLogEdit, QString("检测到设备处于打开状态（响应：%1）").arg(resp));
                showStatus();
                return;
            }
            else if (lower.contains("close") || lower.contains("closed") || lower == "0") {
                _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_CLOSED;
                appendLog(m_pLogEdit, QString("检测到设备处于关闭状态（响应：%1）").arg(resp));
                showStatus();
                return;
            }
            else {
                appendLog(m_pLogEdit, QString("未能解析设备响应: %1 （请把该原文贴给开发者以便添加解析规则）").arg(resp));
                QMessageBox::information(this, "提示", QString("返回未知响应：%1").arg(resp));
                return;
            }
        }

        // 如果所有串口猜测命令都没有响应
        appendLog(m_pLogEdit, "串口查询命令均未返回可解析响应，无法确认状态");
        QMessageBox::information(this, "提示", "状态查询失败（设备无回显/未实现 status 参数），请检查设备或使用手动命令调试");
        return;
    }

    // r 不为空，解析 helper 的标准返回
    appendLog(m_pLogEdit, QString("原始查询响应: %1").arg(QString::fromLatin1(QJsonDocument(r).toJson(QJsonDocument::Compact))));
    if (r.value("status").toString() != "ok") {
        appendLog(m_pLogEdit, QString("查询状态返回非 ok: %1").arg(r.value("status").toString()));
        // 如果 error 是 parameter not found，则提示并建议使用串口查询
        QString err = r.value("error").toString();
        appendLog(m_pLogEdit, QString("readParam error: %1").arg(err));
        QMessageBox::information(this, "提示", QString("状态查询失败：%1。尝试使用串口查询。").arg(err));
        return;
    }

    if (!r.contains("payload") || !r.value("payload").isObject()) {
        appendLog(m_pLogEdit, "查询状态返回的 payload 缺失或不是对象，无法解析");
        QMessageBox::information(this, "提示", "状态查询返回格式不对");
        return;
    }

    QJsonObject payload = r.value("payload").toObject();
    // 尝试多种字段名
    if (payload.contains("value")) {
        int v = payload.value("value").toInt(-1);
        if (v == 1) _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_OPENED;
        else if (v == 2) _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_CLOSED;
        else _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_NONE;
        appendLog(m_pLogEdit, QString("状态解析成功：value=%1").arg(v));
        showStatus();
        return;
    }

    if (payload.contains("isOpen")) {
        bool b = payload.value("isOpen").toBool();
        _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = b ? M_SHUTTER_STATUS_OPENED : M_SHUTTER_STATUS_CLOSED;
        appendLog(m_pLogEdit, QString("状态解析成功：isOpen=%1").arg(b));
        showStatus();
        return;
    }

    if (payload.contains("status")) {
        QString s = payload.value("status").toString();
        if (s.toLower().contains("open")) _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_OPENED;
        else if (s.toLower().contains("close") || s.toLower().contains("shut")) _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_CLOSED;
        else _pShutterCtrl->m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_NONE;
        appendLog(m_pLogEdit, QString("状态解析成功：status=%1").arg(s));
        showStatus();
        return;
    }

    appendLog(m_pLogEdit, QString("无法解析 payload，请查看 raw payload 并贴给开发者：%1").arg(QString::fromLatin1(QJsonDocument(payload).toJson(QJsonDocument::Compact))));
    QMessageBox::information(this, "提示", "状态查询返回未知格式，请查看日志并把响应贴给开发者以便适配");
}
//设置门限值
void QShutterCtrlItemWidget::onSetLimitData()
{
    //    int nData = m_pSetLimitEdit->toPlainText().toInt();
    //    if(nData <= 0)
    //    {
    //        QMessageBox::warning(this,"提示","门限设置必须大于0");
    //        return;
    //    }


    //    bool bRel = _pShutterCtrl->SetDevLimit(nData);
    //    if(bRel == false)
    //    {
    //        QMessageBox::warning(this,"提示","设置门限值失败");
    //        return;
    //    }

    showStatus();

}

//设置流程
void QShutterCtrlItemWidget::onSetProcessStatus()
{
    int nPlusCnt = m_psetPlusCntEdit->toPlainText().toInt();
    int nDelayTime = 0;

    if (nPlusCnt <= 0)
    {
        QMessageBox::warning(this, "提示", "脉冲数必须大于0");
        return;
    }

    bool bRel = _pShutterCtrl->setProcessStatus(nPlusCnt, nDelayTime);

    if (!bRel)
    {
        QMessageBox::warning(this, "错误", "执行超时");
        return;
    }

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QShutterCtrlItemWidget::setWidgetStyleSheet(QWidget* pWidget)
{
    QPalette pal(pWidget->palette());
    pal.setColor(QPalette::Background, QColor(255, 255, 255));
    pWidget->setAutoFillBackground(true);
    pWidget->setPalette(pal);

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);// 阴影偏移
    shadowEffect->setColor(QColor(180, 180, 180));// 阴影颜色;
    shadowEffect->setBlurRadius(10);// 阴影半径;
    pWidget->setGraphicsEffect(shadowEffect);// 给窗口设置上当前的阴影效果;
}
