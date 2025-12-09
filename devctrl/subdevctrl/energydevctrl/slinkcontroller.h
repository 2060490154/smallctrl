#ifndef SLINKCONTROLLER_H
#define SLINKCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QByteArray>
#include <QMutex>

class SLinkController : public QObject
{
    Q_OBJECT
public:
    explicit SLinkController(QObject* parent = nullptr);
    ~SLinkController();

    bool openSerial(const QString& portName, int baud = 921600, QSerialPort::Parity parity = QSerialPort::NoParity,
        QSerialPort::DataBits dataBits = QSerialPort::Data8, QSerialPort::StopBits stopBits = QSerialPort::OneStop);
    void closeSerial();
    bool isOpen() const;

    QByteArray sendCommandAndRead(const QByteArray& cmd, int timeout = 500);
    bool sendCommand(const QByteArray& cmd);

    // 新增：发送命令并读取“全部”响应（直到结束标志或超时）
    QByteArray sendCommandAndReadAll(const QByteArray& cmd, int timeout = 1000);

    bool cmd_VER(QString& outVersion, int timeout = 500);
    bool cmd_RST(int timeout = 1000);
    bool cmd_SS_setMode(int channel, int modeCode);
    bool cmd_SC_setRange(int channel, int rangeCode);
    bool cmd_PW_setWavelength(int channel, int wavelength);
    bool cmd_TL_setTriggerLevel(int channel, const QString& levelStr);
    bool cmd_ET_setExternalTrigger(int channel, int enable);
    bool cmd_CA_startCont(int channel);
    bool cmd_CS_stopCont(int channel);
    bool cmd_CV_getLastValue(int channel, QString& outValue, int timeout = 500);

    static double parseBinaryPair(quint8 byte1, quint8 byte2, bool isWattmeter, int currentRangeValue, bool& ok, int& channelOut);

signals:
    void signalAsciiLine(const QString& line);
    void signalBinaryPoint(int channel, double value, const QByteArray& raw);
    void signalConnected(bool connected);

private slots:
    void onReadyRead();

private:
    QSerialPort* m_serial;
    QByteArray m_readBuffer;
    QTimer m_waitTimer;
    QMutex m_mutex;

    void processAsciiBuffer();
    void processBinaryBuffer();
    bool writeRaw(const QByteArray& data);
};

#endif // SLINKCONTROLLER_H
