#include "slinkcontroller.h"
#include <QThread>
#include <QDebug>
#include <QStringList>
#include <QElapsedTimer>

SLinkController::SLinkController(QObject* parent)
    : QObject(parent),
    m_serial(new QSerialPort(this))
{
    connect(m_serial, &QSerialPort::readyRead, this, &SLinkController::onReadyRead);
}

SLinkController::~SLinkController()
{
    closeSerial();
}

bool SLinkController::openSerial(const QString& portName, int baud,
    QSerialPort::Parity parity, QSerialPort::DataBits dataBits,
    QSerialPort::StopBits stopBits)
{
    if (m_serial->isOpen()) m_serial->close();
    m_serial->setPortName(portName);
    m_serial->setBaudRate(baud);
    m_serial->setParity(parity);
    m_serial->setDataBits(dataBits);
    m_serial->setStopBits(stopBits);
    bool ok = m_serial->open(QIODevice::ReadWrite);
    qDebug() << "[SLinkController] openSerial" << portName << "baud" << baud << "ok=" << ok;
    emit signalConnected(ok);
    return ok;
}

void SLinkController::closeSerial()
{
    if (m_serial->isOpen()) {
        m_serial->clear();
        m_serial->close();
    }
    emit signalConnected(false);
}

bool SLinkController::isOpen() const
{
    return m_serial->isOpen();
}

QByteArray SLinkController::sendCommandAndRead(const QByteArray& cmd, int timeout)
{
    QMutexLocker locker(&m_mutex);
    QByteArray crcmd = cmd;
    if (!cmd.endsWith("\r")) crcmd += '\r';
    if (!writeRaw(crcmd)) return QByteArray();

    QByteArray result;
    int waited = 0;
    const int waitStep = 10;
    while (waited < timeout) {
        QThread::msleep(waitStep);
        waited += waitStep;
        if (!m_readBuffer.isEmpty()) {
            int idx = m_readBuffer.indexOf("\r\n");
            if (idx >= 0) {
                result = m_readBuffer.left(idx);
                m_readBuffer.remove(0, idx + 2);
                break;
            }
            else {
                if (waited + waitStep >= timeout) {
                    result = m_readBuffer;
                    m_readBuffer.clear();
                    break;
                }
            }
        }
    }
    qDebug() << "[SLinkController] sendCommandAndRead cmd=" << cmd << "resp=" << result;
    return result;
}

bool SLinkController::sendCommand(const QByteArray& cmd)
{
    QMutexLocker locker(&m_mutex);
    QByteArray crcmd = cmd;
    if (!cmd.endsWith("\r")) crcmd += '\r';
    return writeRaw(crcmd);
}

bool SLinkController::writeRaw(const QByteArray& data)
{
    if (!m_serial->isOpen()) {
        qDebug() << "[SLinkController] writeRaw failed: serial not open";
        return false;
    }
    qint64 wrote = m_serial->write(data);
    if (wrote == -1) {
        qDebug() << "[SLinkController] writeRaw write returned -1";
        return false;
    }
    m_serial->flush();
    qDebug() << "[SLinkController] writeRaw wrote:" << data;
    return true;
}

// 新增实现：发送命令并读取“全部”响应，直到检测到结束标志或超时
QByteArray SLinkController::sendCommandAndReadAll(const QByteArray& cmd, int timeout)
{
    QMutexLocker locker(&m_mutex);
    if (!m_serial->isOpen()) {
        qDebug() << "[SLinkController] sendCommandAndReadAll: serial not open";
        return QByteArray();
    }

    QByteArray crcmd = cmd;
    if (!cmd.endsWith("\r")) crcmd += '\r';
    if (m_serial->write(crcmd) == -1) {
        qDebug() << "[SLinkController] sendCommandAndReadAll: write failed";
        return QByteArray();
    }
    m_serial->flush();

    qDebug() << "[SLinkController] sendCommandAndReadAll sent:" << cmd;

    QByteArray accum;
    QElapsedTimer t;
    t.start();
    const QByteArray terminator = ":100000000"; // 协议示例的传输结束标志
    while (t.elapsed() < timeout) {
        if (m_serial->waitForReadyRead(100)) {
            QByteArray chunk = m_serial->readAll();
            if (!chunk.isEmpty()) {
                accum.append(chunk);
                // 在收到数据后，检查是否包含终止标志（以 ASCII 行形式）
                if (accum.contains(terminator)) {
                    break;
                }
            }
        }
        else {
            QThread::msleep(10);
        }
    }

    qDebug() << "[SLinkController] sendCommandAndReadAll resp size=" << accum.size();
    // 移除可能的多余前后空白
    return accum;
}

void SLinkController::onReadyRead()
{
    QByteArray rec = m_serial->readAll();
    if (rec.isEmpty()) return;
    m_readBuffer.append(rec);

    // 按协议：ASCII 模式会发送以 CRLF 结尾的文本；二进制数据以原始字节对出现。
    if (m_readBuffer.contains('\n')) {
        processAsciiBuffer();
    }
    else {
        processBinaryBuffer();
    }
}

void SLinkController::processAsciiBuffer()
{
    while (true) {
        int idx = m_readBuffer.indexOf("\r\n");
        if (idx < 0) break;
        QByteArray line = m_readBuffer.left(idx);
        m_readBuffer.remove(0, idx + 2);
        QString sline = QString::fromUtf8(line);
        emit signalAsciiLine(sline);
    }
}

void SLinkController::processBinaryBuffer()
{
    while (m_readBuffer.size() >= 2) {
        quint8 b1 = static_cast<quint8>(m_readBuffer.at(0));
        quint8 b2 = static_cast<quint8>(m_readBuffer.at(1));
        bool dummyOk;
        int chout = 0;
        bool isWatt = ((b1 >> 5) & 0x01) != 0;
        double parsed = SLinkController::parseBinaryPair(b1, b2, isWatt, 1, dummyOk, chout);
        QByteArray raw = m_readBuffer.left(2);
        emit signalBinaryPoint(chout, parsed, raw);
        m_readBuffer.remove(0, 2);
    }
}

bool SLinkController::cmd_VER(QString& outVersion, int timeout)
{
    QByteArray resp = sendCommandAndRead("*VER", timeout);
    if (resp.isEmpty()) return false;
    outVersion = QString::fromUtf8(resp).trimmed();
    return true;
}

bool SLinkController::cmd_RST(int timeout)
{
    return sendCommand("*RST");
}

bool SLinkController::cmd_SS_setMode(int channel, int modeCode)
{
    if (channel != 1 && channel != 2) return false;
    char buf[16];
    snprintf(buf, sizeof(buf), "*SS%d%03d", channel, modeCode);
    return sendCommand(QByteArray(buf));
}

bool SLinkController::cmd_SC_setRange(int channel, int rangeCode)
{
    if (channel != 1 && channel != 2) return false;
    char buf[16];
    snprintf(buf, sizeof(buf), "*SC%d%02d", channel, rangeCode);
    return sendCommand(QByteArray(buf));
}

bool SLinkController::cmd_PW_setWavelength(int channel, int wavelength)
{
    if (channel != 1 && channel != 2) return false;
    char buf[32];
    snprintf(buf, sizeof(buf), "*PW%d%05d", channel, wavelength);
    return sendCommand(QByteArray(buf));
}

bool SLinkController::cmd_TL_setTriggerLevel(int channel, const QString& levelStr)
{
    if (channel != 1 && channel != 2) return false;
    QByteArray s = QString("*TL%1%2").arg(channel).arg(levelStr).toUtf8();
    return sendCommand(s);
}

bool SLinkController::cmd_ET_setExternalTrigger(int channel, int enable)
{
    if (channel != 1 && channel != 2) return false;
    char buf[8];
    snprintf(buf, sizeof(buf), "*ET%d%d", channel, (enable ? 1 : 0));
    return sendCommand(QByteArray(buf));
}

bool SLinkController::cmd_CA_startCont(int channel)
{
    if (channel != 1 && channel != 2) return false;
    char buf[8];
    snprintf(buf, sizeof(buf), "*CA%d", channel);
    return sendCommand(QByteArray(buf));
}

bool SLinkController::cmd_CS_stopCont(int channel)
{
    if (channel != 1 && channel != 2) return false;
    char buf[8];
    snprintf(buf, sizeof(buf), "*CS%d", channel);
    return sendCommand(QByteArray(buf));
}

bool SLinkController::cmd_CV_getLastValue(int channel, QString& outValue, int timeout)
{
    if (channel != 1 && channel != 2) return false;
    char buf[8];
    snprintf(buf, sizeof(buf), "*CV%d", channel);
    QByteArray resp = sendCommandAndRead(QByteArray(buf), timeout);
    if (resp.isEmpty()) return false;
    outValue = QString::fromUtf8(resp).trimmed();
    return true;
}

double SLinkController::parseBinaryPair(quint8 byte1, quint8 byte2, bool isWattmeter, int currentRangeValue, bool& ok, int& channelOut)
{
    ok = false;
    int C1 = (byte1 >> 7) & 0x01;
    channelOut = C1 + 1;

    if (!isWattmeter) {
        int msb = ((byte1 & 0x10) >> 4) << 8;
        msb |= (byte1 & 0x0F) << 4;
        int lsb = byte2 & 0x0F;
        int code = (msb | lsb) & 0x0FFF;
        ok = true;
        return currentRangeValue * (double(code) / 4096.0);
    }
    else {
        int sign = (byte1 >> 5) & 0x01;
        int msb = byte1 & 0x0F;
        int lsb = byte2 & 0x3F;
        int raw = (msb << 6) | lsb;
        if (sign) {
            int mask = (1 << 10) - 1;
            int twos = ((~raw) & mask) + 1;
            raw = -twos;
        }
        ok = true;
        return currentRangeValue * (double(raw) / 2048.0);
    }
}