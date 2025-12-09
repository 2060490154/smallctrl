#include "energydevctrl.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

#include "energy_slink_adapter.h"

QEnergyDevCtrl::QEnergyDevCtrl(QString sComName, int nPort)
{
    m_bCapData = false;
    // 初始化参数
    m_tDevParam.sDevIP = sComName;//串口地址
    for (int i = 0; i < 2; i++)
    {
        m_tDevParam.channelParam[i].i32CurrentMeasureMode = E_MODE_NO_DETECTOR;
        m_tDevParam.channelParam[i].lfTriggerLevel = 0.0;
        m_tDevParam.channelParam[i].lfMeasureValue = 0.0;
        m_tDevParam.channelParam[i].i32CurrentRange = 0;
        m_tDevParam.channelParam[i].i32CurrentWaveLength = 0;
        m_tDevParam.channelParam[i].bConnected = false;
    }

    _pSerialPort = new QSerialPort();
    _pSerialPort->setPortName(sComName);
    _pSerialPort->setBaudRate(QSerialPort::Baud9600);
    _pSerialPort->setDataBits(QSerialPort::Data8);
    _pSerialPort->setParity(QSerialPort::NoParity);
    _pSerialPort->setStopBits(QSerialPort::OneStop);
    _pSerialPort->setFlowControl(QSerialPort::NoFlowControl);

    _capDataTimer = new QTimer(this);
    _capDataTimer->setInterval(100);

    connect(_capDataTimer, &QTimer::timeout, this, &QEnergyDevCtrl::onCapMeasureData);

    _capDataTimer->start();

    // S-Link adapter defaults
    m_bUseSLink = false;
    m_pSLinkAdapter = nullptr;
}

QEnergyDevCtrl::~QEnergyDevCtrl()
{
    setChannelStopCap(0x0);
    setChannelStopCap(0x01);

    if (_pSerialPort->isOpen())
    {
        _pSerialPort->clear();
        _pSerialPort->close();
    }

    delete _pSerialPort;

    if (m_pSLinkAdapter)
    {
        m_pSLinkAdapter->closePort();
        delete m_pSLinkAdapter;
        m_pSLinkAdapter = nullptr;
    }
}

int QEnergyDevCtrl::openDev()
{
    // 如果启用了 S-Link，优先检查 adapter
    if (m_bUseSLink && m_pSLinkAdapter) {
        return m_pSLinkAdapter->isOpen() ? E_SENDCMD_OK : E_CONNECT_ERROR;
    }

    m_tDevParam.bDevConnect = _pSerialPort->isOpen();

    if (!m_tDevParam.bDevConnect)
    {
        if (!_pSerialPort->open(QIODevice::ReadWrite))
        {
            return E_CONNECT_ERROR;
        }

        m_tDevParam.bDevConnect = true;
        _pSerialPort->clear();
    }

    return E_SENDCMD_OK;
}

void QEnergyDevCtrl::closeDev()
{
    if (m_bUseSLink && m_pSLinkAdapter) {
        m_pSLinkAdapter->closePort();
        m_tDevParam.bDevConnect = false;
        return;
    }

    if (_pSerialPort->isOpen())
    {
        _pSerialPort->close();
        m_tDevParam.bDevConnect = false;
    }
}

int QEnergyDevCtrl::sendCmdData(QString sCmd)
{
    if (m_bUseSLink && m_pSLinkAdapter)
    {
        QString cmd = sCmd.trimmed();
        if (cmd.endsWith("\r") || cmd.endsWith("\n")) cmd.chop(1);

        if (cmd.startsWith("*SS", Qt::CaseInsensitive))
        {
            if (cmd.length() >= 5) {
                int channel = cmd.mid(3, 1).toInt();
                int mode = cmd.mid(4).toInt();
                if (mode == 0) return m_pSLinkAdapter->setBinaryMode(channel) ? E_SENDCMD_OK : E_SENDCMD_ERROR;
                else return m_pSLinkAdapter->setAsciiMode(channel) ? E_SENDCMD_OK : E_SENDCMD_ERROR;
            }
            return E_SENDCMD_ERROR;
        }
        else if (cmd.startsWith("*CA", Qt::CaseInsensitive))
        {
            int channel = cmd.mid(3).toInt();
            return m_pSLinkAdapter->setChannelStartCap(channel) ? E_SENDCMD_OK : E_SENDCMD_ERROR;
        }
        else if (cmd.startsWith("*CS", Qt::CaseInsensitive))
        {
            int channel = cmd.mid(3).toInt();
            return m_pSLinkAdapter->setChannelStopCap(channel) ? E_SENDCMD_OK : E_SENDCMD_ERROR;
        }
        else if (cmd.startsWith("*SC", Qt::CaseInsensitive))
        {
            if (cmd.length() >= 5) {
                int channel = cmd.mid(3, 1).toInt();
                int rangeCode = cmd.mid(4).toInt();
                return m_pSLinkAdapter->setChannelMeauserRange(channel, rangeCode) ? E_SENDCMD_OK : E_SENDCMD_ERROR;
            }
            return E_SENDCMD_ERROR;
        }
        else if (cmd.startsWith("*PW", Qt::CaseInsensitive))
        {
            if (cmd.length() >= 8) {
                int channel = cmd.mid(3, 1).toInt();
                int wl = cmd.mid(4).toInt();
                return m_pSLinkAdapter->setChannelWaveLength(channel, wl) ? E_SENDCMD_OK : E_SENDCMD_ERROR;
            }
            return E_SENDCMD_ERROR;
        }
        else if (cmd.startsWith("*ET", Qt::CaseInsensitive))
        {
            if (cmd.length() >= 5) {
                int channel = cmd.mid(3, 1).toInt();
                int enable = cmd.mid(4).toInt();
                return m_pSLinkAdapter->setChannelExTrigger(channel, enable ? true : false) ? E_SENDCMD_OK : E_SENDCMD_ERROR;
            }
            return E_SENDCMD_ERROR;
        }
        else if (cmd.startsWith("*TL", Qt::CaseInsensitive))
        {
            if (cmd.length() >= 6) {
                int channel = cmd.mid(3, 1).toInt();
                QString levelStr = cmd.mid(4); // e.g. "0002" or "00.5"
                return m_pSLinkAdapter->setTriggerLevel(channel, levelStr) ? E_SENDCMD_OK : E_SENDCMD_ERROR;
            }
            return E_SENDCMD_ERROR;
        }
        else if (cmd.startsWith("*ST", Qt::CaseInsensitive) || cmd.startsWith("*SN", Qt::CaseInsensitive))
        {
            // 查询类命令不通过这个路由，走专门函数 getDevParam/getDevConnectStatus
            return E_SENDCMD_ERROR;
        }
        else
        {
            return E_SENDCMD_ERROR;
        }
    }

    // 原有串口发送实现
    QByteArray l_data;
    l_data.append(sCmd);

    _EnergyDevlock.lock();

    if (this->openDev() == E_CONNECT_ERROR)
    {
        _EnergyDevlock.unlock();
        return E_CONNECT_ERROR;
    }

    _pSerialPort->clear();

    int rel = _pSerialPort->write(l_data.data(), l_data.size());
    bool bRel = _pSerialPort->waitForBytesWritten(100);

    _EnergyDevlock.unlock();
    if (rel != l_data.size() || bRel == false)
    {
        return E_SENDCMD_DATA_FAILED;
    }

    return E_SENDCMD_OK;
}

int QEnergyDevCtrl::getDevParam(int nChannelNo)
{
    if (m_bUseSLink && m_pSLinkAdapter) {
        QByteArray out;
        if (!m_pSLinkAdapter->getDevParam(nChannelNo + 1, out, 1000)) {
            return E_RECV_DATA_TIMEOUT;
        }
        praseDevParam(out, nChannelNo);
        return E_SENDCMD_OK;
    }

    if (!this->_pSerialPort->isOpen())
    {
        return E_CONNECT_ERROR;
    }

    QString sCmd = QString("*ST%1").arg(nChannelNo + 1);

    int nRel = sendCmdData(sCmd);//当前状态命令
    if (nRel != E_SENDCMD_OK)
    {
        return nRel;
    }

    QByteArray l_RcvData;
    while (_pSerialPort->waitForReadyRead(100))
    {
        QByteArray l_temp = _pSerialPort->readAll();
        l_RcvData.push_back(l_temp);
    }

    if (l_RcvData.size() != 108)//接收的数据有问题
    {
        return E_RECV_DATA_TIMEOUT;
    }

    praseDevParam(l_RcvData, nChannelNo);

    return E_SENDCMD_OK;
}

int QEnergyDevCtrl::getDevConnectStatus(int nChannelNo)
{
    if (m_bUseSLink && m_pSLinkAdapter) {
        QString out;
        if (!m_pSLinkAdapter->getDevConnectStatus(nChannelNo + 1, out, 500)) {
            m_tDevParam.channelParam[nChannelNo].bConnected = false;
            return E_SENDCMD_ERROR;
        }
        m_tDevParam.channelParam[nChannelNo].bConnected = !out.isEmpty();
        return E_SENDCMD_OK;
    }

    if (!this->_pSerialPort->isOpen())
    {
        return E_CONNECT_ERROR;
    }

    QString sCmd = QString("*SN%1").arg(nChannelNo + 1);

    int nRel = sendCmdData(sCmd);//当前状态命令
    if (nRel != E_SENDCMD_OK)
    {
        return nRel;
    }

    QByteArray l_RcvData;
    while (_pSerialPort->waitForReadyRead(100))
    {
        QByteArray l_temp = _pSerialPort->readAll();
        l_RcvData.push_back(l_temp);
    }

    QString sRcvData(l_RcvData);
    m_tDevParam.channelParam[nChannelNo].bConnected = true;

    if (sRcvData.size() == 0)
    {
        m_tDevParam.channelParam[nChannelNo].bConnected = false;
    }

    return E_SENDCMD_OK;
}

bool QEnergyDevCtrl::isSLinkOpen() const
{
    return (this->m_bUseSLink && this->m_pSLinkAdapter != nullptr && this->m_pSLinkAdapter->isOpen());
}

bool QEnergyDevCtrl::enableSLinkPort(const QString& portName, int baud)
{
    return setUseSLink(true, portName, baud);
}

void QEnergyDevCtrl::disableSLinkPort()
{
    setUseSLink(false);
}

bool QEnergyDevCtrl::setUseSLink(bool use, const QString& portOrAddress, int baud)
{
    if (use == m_bUseSLink) {
        if (use && m_pSLinkAdapter) return true;
        if (!use) return true;
    }

    if (!use) {
        if (m_pSLinkAdapter) {
            m_pSLinkAdapter->closePort();
            delete m_pSLinkAdapter;
            m_pSLinkAdapter = nullptr;
        }
        m_bUseSLink = false;
        return true;
    }

    if (!m_pSLinkAdapter) {
        m_pSLinkAdapter = new EnergySLinkAdapter(this);
        connect(m_pSLinkAdapter, &EnergySLinkAdapter::signalAsciiMeasurement,
            this, &QEnergyDevCtrl::onSLinkAsciiMeasurement);
        connect(m_pSLinkAdapter, &EnergySLinkAdapter::signalBinaryMeasurement,
            this, &QEnergyDevCtrl::onSLinkBinaryMeasurement);
        connect(m_pSLinkAdapter, &EnergySLinkAdapter::signalLog,
            this, &QEnergyDevCtrl::signalLog);
    }

    bool ok = m_pSLinkAdapter->openPort(portOrAddress, baud);
    if (!ok) {
        delete m_pSLinkAdapter;
        m_pSLinkAdapter = nullptr;
        m_bUseSLink = false;
        return false;
    }

    m_bUseSLink = true;
    return true;
}

void QEnergyDevCtrl::praseDevParam(QByteArray data, int nChannelNo)
{
    bool bOk = false;
    QString sRcvData(data);

    sRcvData = sRcvData.replace("\r\n", "");

    QStringList sParamData = sRcvData.split(':');

    for (QString sTempdata : sParamData)
    {
        if (sTempdata.size() == 9)
        {
            int nHeaderIndex = sTempdata.left(5).toInt();//头信息
            QString str = sTempdata.right(4);
            int nValue = str.toInt(&bOk, 16);

            if (nHeaderIndex == 0x00)//波长
            {
                m_tDevParam.channelParam[nChannelNo].i32CurrentWaveLength = nValue;
            }

            if (nHeaderIndex == 0x01)//测量范围
            {
                m_tDevParam.channelParam[nChannelNo].i32CurrentRange = nValue;
            }

            if (nHeaderIndex == 0x02)//触发电平 百分比
            {
                m_tDevParam.channelParam[nChannelNo].lfTriggerLevel = (float)nValue / 20.48;
            }

            if (nHeaderIndex == 0x05)//外触发模式
            {
                m_tDevParam.channelParam[nChannelNo].i32ExTriggerMode = nValue;
            }

        }
    }
}

void QEnergyDevCtrl::onSLinkAsciiMeasurement(int channel, const QString& valueStr)
{
    bool ok = false;
    double val = valueStr.toDouble(&ok);
    if (!ok) return;

    double val_mJ = val * 1000.0;

    if (channel >= 1 && channel <= 2) {
        int idx = channel - 1;
        m_tDevParam.channelParam[idx].lfMeasureValue = static_cast<float>(val_mJ);
        m_tDevParam.channelParam[idx].bNewData = true;
    }
}

void QEnergyDevCtrl::onSLinkBinaryMeasurement(int channel, double value, const QByteArray& raw)
{
    if (channel >= 1 && channel <= 2) {
        int idx = channel - 1;
        m_tDevParam.channelParam[idx].lfMeasureValue = static_cast<float>(value);
        m_tDevParam.channelParam[idx].bNewData = true;
    }
}

// 下面补充原来文件中用于 UI 的方法（确保这些实现存在并签名一致）
int QEnergyDevCtrl::setChannelExTrigger(int nChannelNo, bool bEnable)
{
    if (!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    int nEnable = bEnable == true ? 1 : 0;

    QString sCmd = QString("*ET%1%2").arg(nChannelNo + 1).arg(nEnable);

    return sendCmdData(sCmd);
}

int QEnergyDevCtrl::setChannelTriggerLevel(int nChannelNo, float lfTriggerLevel)
{
    if (!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    QString sCmd = QString("*TL%1%2").arg(nChannelNo + 1).arg(lfTriggerLevel, 4, 'g', 2, QChar('0'));

    int nRel = sendCmdData(sCmd);
    return nRel;
}

int QEnergyDevCtrl::setChannelMeauserRange(int nChannelNo, int nRangeIndex)
{
    if (!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }
    QString sCmd = QString("*SC%1%2").arg(nChannelNo + 1).arg(nRangeIndex, 2, 10, QChar('0'));
    int nRel = sendCmdData(sCmd);

    return nRel;
}

int QEnergyDevCtrl::setChannelWaveLength(int nChannelNo, int nWaveLength)
{
    if (!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    QString sCmd = QString("*PW%1%2").arg(nChannelNo + 1).arg(nWaveLength, 5, 10, QChar('0'));
    int nRel = sendCmdData(sCmd);

    return nRel;
}

int QEnergyDevCtrl::setChannelStartCap(int nChannelNo)
{
    if (!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    m_bCapData = false;

    QString sCmd = QString("*SS%1").arg(nChannelNo + 1) + "001";//设置AScii码模式
    int nRel = sendCmdData(sCmd);

    sCmd = QString("*CA%1").arg(nChannelNo + 1);//获取测试数据
    nRel = sendCmdData(sCmd);

    m_bCapData = nRel == E_SENDCMD_OK ? true : false;

    _pSerialPort->clear();

    m_bCapData = true;

    return nRel;
}

int QEnergyDevCtrl::setChannelStopCap(int nChannelNo)
{
    if (!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    m_bCapData = false;

    QString sCmd = QString("*CS%1").arg(nChannelNo + 1);//停止连续采集
    int nRel = sendCmdData(sCmd);

    return nRel;
}

void QEnergyDevCtrl::onCapMeasureData()
{
    if (m_bCapData == false)
    {
        return;
    }

    _EnergyDevlock.lock();

    QByteArray rcvData;
    while (_pSerialPort->bytesAvailable() > 0 || _pSerialPort->waitForReadyRead(50))
    {
        QByteArray l_RcvData = _pSerialPort->readAll();
        rcvData.push_back(l_RcvData);
    }
    _EnergyDevlock.unlock();

    if (rcvData.size() > 0)
    {
        praseMeasureData(rcvData);
    }
}

void QEnergyDevCtrl::praseMeasureData(QByteArray rcvdata)
{
    QString sRcvData(rcvdata);

    QStringList l_data = sRcvData.split("\r\n");

    for (QString channeldata : l_data)
    {
        if (channeldata.isEmpty())
        {
            continue;
        }
        QStringList l_newdata = channeldata.split(":");
        if (l_newdata.size() != 2)
        {
            continue;
        }

        int l_nChannelNo = l_newdata[0].toInt();
        float l_lfMeasuredata = l_newdata[1].toFloat() * 1000;//转换成mJ（保留原有行为）
        if (l_nChannelNo == 2 || l_nChannelNo == 1)
        {
            m_tDevParam.channelParam[l_nChannelNo - 1].lfMeasureValue = l_lfMeasuredata;
            m_tDevParam.channelParam[l_nChannelNo - 1].bNewData = true;
        }
    }
}

void QEnergyDevCtrl::delayTime(int mesc)
{
    QTime l_time;
    l_time.start();
    while (l_time.elapsed() < mesc)
    {
    }
}
bool QEnergyDevCtrl::getDetectorName(int channel, QString& outName, int timeout)
{
    if (m_bUseSLink && m_pSLinkAdapter) {
        bool ok = m_pSLinkAdapter->getDetectorName(channel, outName, timeout);
        if (ok) {
            // 存储到设备参数，供 UI 使用（这里是全局字段）
            m_tDevParam.sDevName = outName;
        }
        return ok ? true : false;
    }
    else {
        // 若使用原串口路径：发送 *NA command 并 read
        if (!this->_pSerialPort->isOpen()) return false;
        QString cmd = QString("*NA%1").arg(channel);
        sendCmdData(cmd);
        QByteArray resp;
        while (_pSerialPort->waitForReadyRead(100)) {
            resp.append(_pSerialPort->readAll());
        }
        if (resp.isEmpty()) return false;
        outName = QString::fromUtf8(resp).trimmed();
        m_tDevParam.sDevName = outName;
        return true;
    }
}
