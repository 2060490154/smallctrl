
#ifndef ENERGY_SLINK_ADAPTER_H
#define ENERGY_SLINK_ADAPTER_H

#include <QObject>
#include "slinkcontroller.h"

class EnergySLinkAdapter : public QObject
{
    Q_OBJECT
public:
    explicit EnergySLinkAdapter(QObject* parent = nullptr);
    ~EnergySLinkAdapter();

    bool openPort(const QString& portName, int baud = 921600);
    void closePort();
    bool isOpen() const;

    bool setChannelMeauserRange(int channel, int rangeCode);
    bool setChannelWaveLength(int channel, int wavelength);
    bool setChannelStartCap(int channel);
    bool setChannelStopCap(int channel);
    bool setChannelExTrigger(int channel, bool enable);
    bool setAsciiMode(int channel);
    bool setBinaryMode(int channel);
    bool setTriggerLevel(int channel, const QString& levelStr);

    bool getVersion(QString& ver);

    // 新增：读取探测器名称（*NA）
    bool getDetectorName(int channel, QString& outName, int timeout = 500);

    // 新增：读取零点偏移（*DO）
    bool getZeroOffset(int channel, QString& outValue, int timeout = 500);

    // 新增：用于获取完整 ST 数据（已有 getDevParam 用于 byte block）
    bool getDevParam(int channel, QByteArray& outRaw, int timeout = 1000);
    bool getDevConnectStatus(int channel, QString& outStr, int timeout = 500);

signals:
    void signalAsciiMeasurement(int channel, const QString& valueStr);
    void signalBinaryMeasurement(int channel, double value, const QByteArray& raw);

    // 新增：log 信号，UI 订阅显示
    void signalLog(const QString& log);

private slots:
    void onAsciiLine(const QString& line);
    void onBinaryPoint(int channel, double parsedValue, const QByteArray& raw);

private:
    SLinkController* m_link;
};

#endif // ENERGY_SLINK_ADAPTER_H