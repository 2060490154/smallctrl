#pragma once

#include "ishutterbackend.h"
#include "shutteripcclient.h"
#include <QString>

class ShutterIpcBackend : public IShutterBackend {
    Q_OBJECT
public:
    explicit ShutterIpcBackend(QObject* parent = nullptr);
    ~ShutterIpcBackend() override;

    QJsonArray listDevices(int timeoutMs = 2000) override;
    int connectDevice(int moduleIndex, int deviceIndex, int timeoutMs = 3000) override;
    bool disconnectDevice(int sessionId, int timeoutMs = 2000) override;
    QJsonObject openShutter(int sessionId, int channel = 1, int timeoutMs = 3000) override;
    QJsonObject closeShutter(int sessionId, int channel = 1, int timeoutMs = 3000) override;
    QJsonObject comWrite(int sessionId, const QString& text, int timeoutMs = 2000) override;
    QJsonObject readParam(int sessionId, const QString& name, int channel = 0, int timeoutMs = 2000) override;

    void setHelperPath(const QString& path) { m_helperPath = path; }
    void setLogFile(const QString& path) { m_client.setLogFile(path); }

    bool ensureHelperRunning(int maxWaitMs = 3000);

private:
    ShutterIpcClient m_client;
    QString m_helperPath;
};