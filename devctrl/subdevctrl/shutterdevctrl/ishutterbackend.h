#pragma once
#pragma once
// IShutterBackend - 抽象光闸后端接口
// 任何具体后端（本地驱动/IPC helper）都实现此接口。

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>

class IShutterBackend : public QObject {
    Q_OBJECT
public:
    explicit IShutterBackend(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IShutterBackend() {}

    // 列出设备
    virtual QJsonArray listDevices(int timeoutMs = 2000) = 0;

    // 连接设备，返回 sessionId 或 -1
    virtual int connectDevice(int moduleIndex, int deviceIndex, int timeoutMs = 3000) = 0;

    // 断开设备
    virtual bool disconnectDevice(int sessionId, int timeoutMs = 2000) = 0;

    // 打开/关闭、串口写、读参数等操作，返回完整 JSON 响应对象
    virtual QJsonObject openShutter(int sessionId, int channel = 1, int timeoutMs = 3000) = 0;
    virtual QJsonObject closeShutter(int sessionId, int channel = 1, int timeoutMs = 3000) = 0;
    virtual QJsonObject comWrite(int sessionId, const QString& text, int timeoutMs = 2000) = 0;
    virtual QJsonObject readParam(int sessionId, const QString& name, int channel = 0, int timeoutMs = 2000) = 0;
};