
#include "energy_slink_adapter.h"
#include <QDebug>

EnergySLinkAdapter::EnergySLinkAdapter(QObject* parent)
    : QObject(parent),
    m_link(new SLinkController(this))
{
    connect(m_link, &SLinkController::signalAsciiLine, this, &EnergySLinkAdapter::onAsciiLine);
    connect(m_link, &SLinkController::signalBinaryPoint, this, &EnergySLinkAdapter::onBinaryPoint);
}

EnergySLinkAdapter::~EnergySLinkAdapter()
{
    closePort();
}

bool EnergySLinkAdapter::openPort(const QString& portName, int baud)
{
    bool ok = m_link->openSerial(portName, baud);
    emit signalLog(QString("[Adapter] openPort %1 baud=%2 ok=%3").arg(portName).arg(baud).arg(ok));
    return ok;
}

void EnergySLinkAdapter::closePort()
{
    emit signalLog("[Adapter] closePort");
    m_link->closeSerial();
}

bool EnergySLinkAdapter::isOpen() const
{
    return m_link->isOpen();
}

bool EnergySLinkAdapter::setChannelMeauserRange(int channel, int rangeCode)
{
    emit signalLog(QString("[Adapter] setRange ch%1 code=%2").arg(channel).arg(rangeCode));
    return m_link->cmd_SC_setRange(channel, rangeCode);
}

bool EnergySLinkAdapter::setChannelWaveLength(int channel, int wavelength)
{
    emit signalLog(QString("[Adapter] setWavelength ch%1 %2").arg(channel).arg(wavelength));
    return m_link->cmd_PW_setWavelength(channel, wavelength);
}

bool EnergySLinkAdapter::setChannelStartCap(int channel)
{
    emit signalLog(QString("[Adapter] start capture ch%1").arg(channel));
    return m_link->cmd_CA_startCont(channel);
}

bool EnergySLinkAdapter::setChannelStopCap(int channel)
{
    emit signalLog(QString("[Adapter] stop capture ch%1").arg(channel));
    return m_link->cmd_CS_stopCont(channel);
}

bool EnergySLinkAdapter::setChannelExTrigger(int channel, bool enable)
{
    emit signalLog(QString("[Adapter] setExTrigger ch%1 enable=%2").arg(channel).arg(enable));
    return m_link->cmd_ET_setExternalTrigger(channel, enable ? 1 : 0);
}

bool EnergySLinkAdapter::setAsciiMode(int channel)
{
    emit signalLog(QString("[Adapter] setAsciiMode ch%1").arg(channel));
    return m_link->cmd_SS_setMode(channel, 1);
}

bool EnergySLinkAdapter::setBinaryMode(int channel)
{
    emit signalLog(QString("[Adapter] setBinaryMode ch%1").arg(channel));
    return m_link->cmd_SS_setMode(channel, 0);
}

bool EnergySLinkAdapter::setTriggerLevel(int channel, const QString& levelStr)
{
    emit signalLog(QString("[Adapter] setTriggerLevel ch%1 level=%2").arg(channel).arg(levelStr));
    return m_link->cmd_TL_setTriggerLevel(channel, levelStr);
}

bool EnergySLinkAdapter::getVersion(QString& ver)
{
    emit signalLog("[Adapter] getVersion");
    return m_link->cmd_VER(ver);
}

bool EnergySLinkAdapter::getDetectorName(int channel, QString& outName, int timeout)
{
    if (!m_link->isOpen()) {
        emit signalLog("[Adapter] getDetectorName: link not open");
        return false;
    }
    QString cmd = QString("*NA%1").arg(channel);
    emit signalLog(QString("[Adapter] send %1").arg(cmd));
    QByteArray resp = m_link->sendCommandAndRead(cmd.toUtf8(), timeout);
    if (resp.isEmpty()) {
        emit signalLog(QString("[Adapter] getDetectorName empty for %1").arg(cmd));
        return false;
    }
    outName = QString::fromUtf8(resp).trimmed();
    emit signalLog(QString("[Adapter] detectorName: %1").arg(outName));
    return true;
}

bool EnergySLinkAdapter::getZeroOffset(int channel, QString& outValue, int timeout)
{
    if (!m_link->isOpen()) {
        emit signalLog("[Adapter] getZeroOffset: link not open");
        return false;
    }
    QString cmd = QString("*DO%1").arg(channel);
    emit signalLog(QString("[Adapter] send %1").arg(cmd));
    QByteArray resp = m_link->sendCommandAndRead(cmd.toUtf8(), timeout);
    if (resp.isEmpty()) {
        emit signalLog(QString("[Adapter] getZeroOffset empty for %1").arg(cmd));
        return false;
    }
    outValue = QString::fromUtf8(resp).trimmed();
    emit signalLog(QString("[Adapter] zeroOffset: %1").arg(outValue));
    return true;
}

bool EnergySLinkAdapter::getDevParam(int channel, QByteArray& outRaw, int timeout)
{
    if (!m_link->isOpen()) {
        emit signalLog("[Adapter] getDevParam: link not open");
        return false;
    }

    QString cmd = QString("*ST%1").arg(channel);
    emit signalLog(QString("[Adapter] send %1 (readAll)").arg(cmd));
    QByteArray resp = m_link->sendCommandAndReadAll(cmd.toUtf8(), timeout);
    if (resp.isEmpty()) {
        emit signalLog(QString("[Adapter] getDevParam: empty response for %1").arg(cmd));
        return false;
    }
    emit signalLog(QString("[Adapter] getDevParam raw:\n%1").arg(QString::fromUtf8(resp)));
    outRaw = resp;
    return true;
}

bool EnergySLinkAdapter::getDevConnectStatus(int channel, QString& outStr, int timeout)
{
    if (!m_link->isOpen()) {
        emit signalLog("[Adapter] getDevConnectStatus: link not open");
        return false;
    }
    QString cmd = QString("*SN%1").arg(channel);
    emit signalLog(QString("[Adapter] send %1 (readAll)").arg(cmd));
    QByteArray resp = m_link->sendCommandAndReadAll(cmd.toUtf8(), timeout);
    if (resp.isEmpty()) {
        emit signalLog(QString("[Adapter] getDevConnectStatus: empty resp for %1").arg(cmd));
        return false;
    }
    outStr = QString::fromUtf8(resp).trimmed();
    emit signalLog(QString("[Adapter] getDevConnectStatus raw: %1").arg(outStr));
    return true;
}

void EnergySLinkAdapter::onAsciiLine(const QString& line)
{
    QString s = line.trimmed();
    if (s.isEmpty()) return;
    QStringList parts = s.split(':');
    if (parts.size() >= 2) {
        bool ok = false;
        int ch = parts[0].toInt(&ok);
        if (!ok) {
            emit signalLog(QString("[Adapter] onAsciiLine unparsable: %1").arg(s));
            return;
        }
        QString val = parts[1].trimmed();
        emit signalAsciiMeasurement(ch, val);
    }
    else {
        emit signalLog(QString("[Adapter] S-Link ASCII line: %1").arg(s));
    }
}

void EnergySLinkAdapter::onBinaryPoint(int channel, double value, const QByteArray& raw)
{
    emit signalBinaryMeasurement(channel, value, raw);
    emit signalLog(QString("[Adapter] binary point ch%1 val=%2 raw=%3").arg(channel).arg(value).arg(QString(raw.toHex())));
}