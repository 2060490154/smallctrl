#include "energydevctrlwidget.h"
#include <QtSerialPort/QSerialPortInfo>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QEnergyDevCtrlWidget::QEnergyDevCtrlWidget(const std::vector<tEnergdevconfig>& pDevConfig, QWidget* parent)
    : QWidget(parent)
{
    _pEnergyDevCtrllist = pDevConfig;

    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0, 1);
    playout->setRowStretch(1, 1);
    playout->setRowStretch(2, 4);

    for (const tEnergdevconfig& devItem : _pEnergyDevCtrllist)
    {
        for (int i = 0; i < devItem.sChannelNamelist.size(); i++)
        {
            QString sTemp = devItem.sChannelNolist[i];
            int nPort = sTemp.toInt();
            QEnergyDevCtrlItemWidget* pDevItemWidget = new QEnergyDevCtrlItemWidget(devItem.pEnergyDevCtrl, nPort, devItem.sChannelNamelist[i], this);
            playout->addWidget(pDevItemWidget, i / 2, i % 2, 1, 1);//2行2列排列
        }
    }
}

/********** QEnergyDevCtrlItemWidget **********/

QEnergyDevCtrlItemWidget::QEnergyDevCtrlItemWidget(QEnergyDevCtrl* pCtrl, int nChannelNo, QString sDevName, QWidget* parent)
    : QGroupBox(parent),
    _pEnergyDevCtrl(pCtrl),
    _sDevName(sDevName),
    _nChannelNo(nChannelNo),
    _pPortCombo(nullptr),
    _pPortRefreshButton(nullptr),
    _pDetectorNameLabel(nullptr),
    _pRangeLabel(nullptr),
    _pWaveLengthLabel(nullptr),
    _pTriggerLevelLabel(nullptr),
    _pGainLabel(nullptr),
    _pOffsetLabel(nullptr),
    _pSamplingRateLabel(nullptr),
    _pSamplingPeriodLabel(nullptr),
    _pTotalDurationLabel(nullptr),
    _pLogTextEdit(nullptr),
    _pMeasureRangeComBox(nullptr),
    _pTriggerLevelEdit(nullptr),
    _pTriggerModeComBox(nullptr),
    _pMeasureWaveLengthComBox(nullptr),
    _pDevStatusLabel(nullptr),
    _pMeasureValueEdit(nullptr),
    _pCheckMeasureValueButton(nullptr),
    _pOpenButton(nullptr),
    _pRefreshStatusButton(nullptr),
    _pSetParamButton(nullptr)
{
    _MeasureRangeVaulelist = QList<QString>() << "3.0mJ" << "10.0mJ" << "30.0mJ" << "100.0mJ" << "300.0mJ" << "1J" << "3J";
    _i32MeasureRangeStartIndex = 19;
    _MeasureWaveLengthlist = QList<QString>() << "1064nm" << "355nm";

    InitUI(); // Now InitUI creates all controls & layout

    foreach(QString sItem, _MeasureRangeVaulelist) if (_pMeasureRangeComBox) _pMeasureRangeComBox->addItem(sItem);
    foreach(QString sItem, _MeasureWaveLengthlist) if (_pMeasureWaveLengthComBox) _pMeasureWaveLengthComBox->addItem(sItem);

    // Connect UI signals (guard pointers)
    if (_pOpenButton) connect(_pOpenButton, &QPushButton::clicked, this, &QEnergyDevCtrlItemWidget::onConnectDev);
    if (_pRefreshStatusButton) connect(_pRefreshStatusButton, &QPushButton::clicked, this, &QEnergyDevCtrlItemWidget::onRefreshParam);
    if (_pCheckMeasureValueButton) connect(_pCheckMeasureValueButton, &QPushButton::clicked, this, &QEnergyDevCtrlItemWidget::onCheckMeasureValue);
    if (_pSetParamButton) connect(_pSetParamButton, &QPushButton::clicked, this, &QEnergyDevCtrlItemWidget::onSetParam);
    if (_pPortRefreshButton && _pPortCombo) {
        connect(_pPortRefreshButton, &QPushButton::clicked, this, [this]() {
            _pPortCombo->clear();
            const auto ports = QSerialPortInfo::availablePorts();
            for (const QSerialPortInfo& info : ports) _pPortCombo->addItem(info.portName());
            });
    }

    // show initial state
    showDevParam();

    // connect controller log to UI
    if (_pEnergyDevCtrl && _pLogTextEdit) {
        connect(_pEnergyDevCtrl, &QEnergyDevCtrl::signalLog, this, [this](const QString& log) {
            if (_pLogTextEdit) _pLogTextEdit->append(log);
            });
    }
}

void QEnergyDevCtrlItemWidget::InitUI()
{
    // 控件创建
    _pTriggerLevelEdit = new QTextEdit();
    _pTriggerLevelEdit->setMaximumHeight(30);

    _pMeasureRangeComBox = new QComboBox();
    _pMeasureRangeComBox->setMinimumHeight(30);

    _pTriggerModeComBox = new QComboBox();
    _pTriggerModeComBox->setMinimumHeight(30);
    _pTriggerModeComBox->addItem("实时采集");
    _pTriggerModeComBox->addItem("外触发采集");

    _pMeasureWaveLengthComBox = new QComboBox();
    _pMeasureWaveLengthComBox->setMinimumHeight(30);

    _pDevStatusLabel = new QLabel();

    QLabel* pDevName = new QLabel(this);
    pDevName->setText(_sDevName);
    pDevName->setStyleSheet("color:blue;font-size:16px;");

    _pMeasureValueEdit = new QTextEdit("0.0");
    _pMeasureValueEdit->setMaximumHeight(30);

    _pCheckMeasureValueButton = new QPushButton("查询测量值");
    _pOpenButton = new QPushButton("打开");
    _pRefreshStatusButton = new QPushButton("刷新参数");
    _pSetParamButton = new QPushButton("设置参数");

    // 新增端口下拉与刷新按钮
    _pPortCombo = new QComboBox();
    _pPortRefreshButton = new QPushButton("刷新端口");
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& info : ports) {
        _pPortCombo->addItem(info.portName());
    }

    // 新增参数显示标签（成员变量在头文件已声明）
    _pDetectorNameLabel = new QLabel("N/A");
    _pRangeLabel = new QLabel("N/A");
    _pWaveLengthLabel = new QLabel("N/A");
    _pTriggerLevelLabel = new QLabel("N/A");
    _pGainLabel = new QLabel("N/A");
    _pOffsetLabel = new QLabel("N/A");
    _pSamplingRateLabel = new QLabel("N/A");
    _pSamplingPeriodLabel = new QLabel("N/A");
    _pTotalDurationLabel = new QLabel("N/A");

    _pLogTextEdit = new QTextEdit();
    _pLogTextEdit->setReadOnly(true);
    _pLogTextEdit->setMinimumHeight(120);

    // 按钮尺寸
    _pCheckMeasureValueButton->setMaximumWidth(100);
    _pCheckMeasureValueButton->setMinimumHeight(30);
    _pOpenButton->setMaximumWidth(100);
    _pOpenButton->setMinimumHeight(30);
    _pRefreshStatusButton->setMaximumWidth(100);
    _pRefreshStatusButton->setMinimumHeight(30);
    _pSetParamButton->setMaximumWidth(100);
    _pSetParamButton->setMinimumHeight(30);

    // 布局：把所有控件加入到一个局部布局变量中（此变量不需要对外）
    QGridLayout* playout = new QGridLayout(this);

    // 第一行：设备名 + 连接状态 + 端口选择
    playout->addWidget(pDevName, 0, 0, 1, 1);
    playout->addWidget(_pDevStatusLabel, 0, 1, 1, 1);
    playout->addWidget(new QLabel("端口:"), 0, 2, 1, 1);
    playout->addWidget(_pPortCombo, 0, 3, 1, 1);
    playout->addWidget(_pPortRefreshButton, 0, 4, 1, 1);

    // 参数输入 / 选择行
    playout->addWidget(new QLabel("测量范围:"), 1, 0, 1, 1);
    playout->addWidget(_pMeasureRangeComBox, 1, 1, 1, 1);

    playout->addWidget(new QLabel("测量波长:"), 2, 0, 1, 1);
    playout->addWidget(_pMeasureWaveLengthComBox, 2, 1, 1, 1);

    playout->addWidget(new QLabel("触发模式"), 3, 0, 1, 1);
    playout->addWidget(_pTriggerModeComBox, 3, 1, 1, 1);

    playout->addWidget(new QLabel("触发门限(%):"), 4, 0, 1, 1);
    playout->addWidget(_pTriggerLevelEdit, 4, 1, 1, 1);

    playout->addWidget(new QLabel("当前测量值(mJ):"), 5, 0, 1, 1);
    playout->addWidget(_pMeasureValueEdit, 5, 1, 1, 1);

    // 操作按钮
    playout->addWidget(_pSetParamButton, 7, 0, 1, 1);
    playout->addWidget(_pRefreshStatusButton, 7, 1, 1, 1);

    // 新增参数显示区（放在下面）
    playout->addWidget(new QLabel("探测器名称:"), 8, 0, 1, 1);
    playout->addWidget(_pDetectorNameLabel, 8, 1, 1, 3);

    playout->addWidget(new QLabel("量程:"), 9, 0, 1, 1);
    playout->addWidget(_pRangeLabel, 9, 1, 1, 1);
    playout->addWidget(new QLabel("波长:"), 9, 2, 1, 1);
    playout->addWidget(_pWaveLengthLabel, 9, 3, 1, 1);

    playout->addWidget(new QLabel("触发电平:"), 10, 0, 1, 1);
    playout->addWidget(_pTriggerLevelLabel, 10, 1, 1, 1);
    playout->addWidget(new QLabel("倍增系数:"), 10, 2, 1, 1);
    playout->addWidget(_pGainLabel, 10, 3, 1, 1);

    playout->addWidget(new QLabel("偏移系数:"), 11, 0, 1, 1);
    playout->addWidget(_pOffsetLabel, 11, 1, 1, 1);
    playout->addWidget(new QLabel("采样率:"), 11, 2, 1, 1);
    playout->addWidget(_pSamplingRateLabel, 11, 3, 1, 1);

    playout->addWidget(new QLabel("采样周期:"), 12, 0, 1, 1);
    playout->addWidget(_pSamplingPeriodLabel, 12, 1, 1, 1);
    playout->addWidget(new QLabel("总时长:"), 12, 2, 1, 1);
    playout->addWidget(_pTotalDurationLabel, 12, 3, 1, 1);

    // 日志
    playout->addWidget(new QLabel("日志:"), 13, 0, 1, 1);
    playout->addWidget(_pLogTextEdit, 14, 0, 1, 4);

    // 设置行列伸缩（可按需调整）
    playout->setRowStretch(14, 1);
    playout->setColumnStretch(3, 1);
}

void QEnergyDevCtrlItemWidget::showDevParam()
{
    if (_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].bConnected)
    {
        QPixmap l_pixmap(":/energypng/connect.png");
        _pDevStatusLabel->setPixmap(l_pixmap);
        _pDevStatusLabel->setToolTip("已连接");
    }
    else
    {
        QPixmap l_pixmap(":/energypng/disconnect.png");
        _pDevStatusLabel->setPixmap(l_pixmap);
        _pDevStatusLabel->setToolTip("未连接");
    }

    //测量范围
    int idx = _pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].i32CurrentRange - _i32MeasureRangeStartIndex;
    if (idx >= 0 && idx < _pMeasureRangeComBox->count())
        _pMeasureRangeComBox->setCurrentIndex(idx);
    else
        _pMeasureRangeComBox->setCurrentIndex(0);

    //测量波长
    int nIndex = -1;
    for (int i = 0; i < _pMeasureWaveLengthComBox->count(); i++)
    {
        QString sWavelength = _pMeasureWaveLengthComBox->itemText(i);
        QString sCurrentLen = QString("%1nm").arg(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].i32CurrentWaveLength);

        if (sWavelength == sCurrentLen)
        {
            nIndex = i;
            break;
        }
    }
    _pMeasureWaveLengthComBox->setCurrentIndex(nIndex);

    _pTriggerLevelEdit->setText(QString("%1").arg(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].lfTriggerLevel));

    if (_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].bConnected)
    {
        _pOpenButton->setText("断开");
    }
    else
    {
        _pOpenButton->setText("连接");
    }

    //触发模式
    int nTriggerModeIndex = _pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].i32ExTriggerMode == 0 ? 0 : 1;
    _pTriggerModeComBox->setCurrentIndex(nTriggerModeIndex);

}

void QEnergyDevCtrlItemWidget::refreshAllParams()
{
    // 确保控制器存在
    if (!_pEnergyDevCtrl) {
        if (_pLogTextEdit) _pLogTextEdit->append("[UI] No controller");
        return;
    }

    // 如果 S-Link 未打开，尝试使用所选端口打开（UI 已提供端口下拉）
    if (!_pEnergyDevCtrl->isSLinkOpen()) {
        QString portName = _pPortCombo ? _pPortCombo->currentText().trimmed() : QString();
        if (!portName.isEmpty()) {
            bool ok = _pEnergyDevCtrl->enableSLinkPort(portName, 921600);
            if (!ok) {
                if (_pLogTextEdit) _pLogTextEdit->append(QString("[UI] Failed to open S-Link on %1").arg(portName));
                return;
            }
            if (_pLogTextEdit) _pLogTextEdit->append(QString("[UI] Opened S-Link on %1").arg(portName));
        }
        else {
            if (_pLogTextEdit) _pLogTextEdit->append("[UI] No port selected");
            return;
        }
    }

    // 1) 获取探测器名称
    QString detector;
    if (_pEnergyDevCtrl->getDetectorName(_nChannelNo + 1, detector, 1000)) {
        _pDetectorNameLabel->setText(detector);
        if (_pLogTextEdit) _pLogTextEdit->append(QString("[UI] Detector: %1").arg(detector));
    }
    else {
        _pDetectorNameLabel->setText("N/A");
        if (_pLogTextEdit) _pLogTextEdit->append("[UI] getDetectorName failed");
    }

    // 2) 获取 *ST 参数（会由 controller 的 getDevParam 调用 praseDevParam 填充 m_tDevParam）
    if (_pEnergyDevCtrl->getDevParam(_nChannelNo) == E_SENDCMD_OK) {
        // 更新界面字段
        int range = _pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].i32CurrentRange;
        _pRangeLabel->setText(QString::number(range));
        int wl = _pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].i32CurrentWaveLength;
        _pWaveLengthLabel->setText(QString("%1 nm").arg(wl));
        float trig = _pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].lfTriggerLevel;
        _pTriggerLevelLabel->setText(QString("%1 %").arg(trig));
        if (_pLogTextEdit) _pLogTextEdit->append("[UI] ST params updated");
    }
    else {
        _pRangeLabel->setText("N/A");
        _pWaveLengthLabel->setText("N/A");
        _pTriggerLevelLabel->setText("N/A");
        if (_pLogTextEdit) _pLogTextEdit->append("[UI] getDevParam failed");
    }

    // 3) 零点偏移 (*DO)
    // 暂未实现读取 DO，显示 N/A
    _pOffsetLabel->setText("N/A");

    // 4) 其他厂商专属字段显示 N/A（除非你提供相应串口命令）
    _pGainLabel->setText("N/A");
    _pSamplingRateLabel->setText("N/A");
    _pSamplingPeriodLabel->setText("N/A");
    _pTotalDurationLabel->setText("N/A");

    if (_pLogTextEdit) _pLogTextEdit->append("[UI] refreshAllParams complete");
}

void QEnergyDevCtrlItemWidget::onRefreshParam()
{
    refreshAllParams();
}

void QEnergyDevCtrlItemWidget::onConnectDev()
{
    if (_pOpenButton->text() == "打开")
    {
        QString portName = _pPortCombo->currentText().trimmed();
        if (portName.isEmpty()) {
            QMessageBox::warning(this, "提示", "请选择串口端口！");
            return;
        }

        // 使用公有 wrapper 打开 S-Link 端口
        bool ok = _pEnergyDevCtrl->enableSLinkPort(portName, 921600);
        if (!ok) {
            QMessageBox::warning(this, "提示", "打开设备失败（S-Link 适配）!");
            return;
        }

        // 更新探头连接状态（getDevConnectStatus 路由到 adapter）
        if (_pEnergyDevCtrl->getDevConnectStatus(_nChannelNo) != E_SENDCMD_OK) {
            QMessageBox::warning(this, "提示", "获取设备连接状态失败！");
        }

        _pOpenButton->setText("断开");
        showDevParam();
    }
    else if (_pOpenButton->text() == "断开")
    {
        _pEnergyDevCtrl->disableSLinkPort();
        _pOpenButton->setText("打开");
        showDevParam();
    }
}

void QEnergyDevCtrlItemWidget::onCheckMeasureValue()
{
    if (_pEnergyDevCtrl->isSLinkOpen()) {
        // adapter 更新 m_tDevParam（通过 signal），直接读取缓存值
    }
    else {
        if (_pEnergyDevCtrl->openDev() != E_SENDCMD_OK)
        {
            QMessageBox::warning(this, "提示", "连接设备失败!");
            return;
        }
        if (_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].bConnected == false)
        {
            QMessageBox::warning(this, "提示", "探头未连接!");
            return;
        }
    }

    _pMeasureValueEdit->setText(QString("%1").arg(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].lfMeasureValue));
}

void QEnergyDevCtrlItemWidget::onSetParam()
{
    // 先暂停采集
    _pEnergyDevCtrl->setChannelStopCap(_nChannelNo);//先暂停采集
    delayTime(100);

    int i32MeasuerRange = _pMeasureRangeComBox->currentIndex() + _i32MeasureRangeStartIndex;
    if (_pEnergyDevCtrl->setChannelMeauserRange(_nChannelNo, i32MeasuerRange) != E_SENDCMD_OK)
    {
        QMessageBox::warning(this, "提示", "设置测量范围失败!");
    }

    delayTime(100);

    int nWavelength = _pMeasureWaveLengthComBox->currentIndex() == 0 ? 1064 : 355;
    if (nWavelength != 0)
    {
        if (_pEnergyDevCtrl->setChannelWaveLength(_nChannelNo, nWavelength) != E_SENDCMD_OK)
        {
            QMessageBox::warning(this, "提示", "设置测量波长失败!");
        }
    }
    else
    {
        QMessageBox::warning(this, "提示", "测量波长参数错误!");
    }

    delayTime(100);

    float lfTiggerLevel = _pTriggerLevelEdit->toPlainText().toFloat();
    // 使用 adapter 路由 *TL 命令（setChannelTriggerLevel 会在 sendCmdData 中路由）
    if (_pEnergyDevCtrl->setChannelTriggerLevel(_nChannelNo, lfTiggerLevel) != E_SENDCMD_OK)
    {
        QMessageBox::warning(this, "提示", "设置触发门限失败!");
    }

    delayTime(100);

    //设置触发模式
    bool bExtTrigger = _pTriggerModeComBox->currentIndex() == 1 ? true : false;
    if (_pEnergyDevCtrl->setChannelExTrigger(_nChannelNo, bExtTrigger) != E_SENDCMD_OK)
    {
        QMessageBox::warning(this, "提示", "设置触发模式失败!");
    }
    delayTime(100);

    // 刷新并重启采集
    onRefreshParam();
    _pEnergyDevCtrl->setChannelStartCap(_nChannelNo);//开始采集
}

void QEnergyDevCtrlItemWidget::onShowMeasureVaule()
{
    _pMeasureValueEdit->setText(QString("%1").arg(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].lfMeasureValue));
}

void QEnergyDevCtrlItemWidget::delayTime(int time)
{
    QTime l_time;
    l_time.start();
    while (l_time.elapsed() < time)
    {
    }
}
void QEnergyDevCtrlItemWidget::setWidgetStyleSheet(QWidget* pWidget)
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