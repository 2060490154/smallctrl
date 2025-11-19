#pragma once
#include <QObject>
#include <QLocalSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class ShutterIpcClient : public QObject {
    Q_OBJECT
public:
    explicit ShutterIpcClient(QObject* parent = nullptr);
    ~ShutterIpcClient();

    bool connectToHelper(int timeoutMs = 2000);

    // High-level blocking APIs (simple, easy to call from your main program)
    QJsonArray listDevices(int timeoutMs = 2000);
    int connectDevice(int moduleIndex, int deviceIndex, int timeoutMs = 3000); // returns sessionId or -1
    bool disconnectDevice(int sessionId, int timeoutMs = 2000);

    // Direct control
    QJsonObject comWrite(int sessionId, const QString& text, int timeoutMs = 2000);
    QJsonObject openShutter(int sessionId, int channel = 1, int timeoutMs = 3000);
    QJsonObject closeShutter(int sessionId, int channel = 1, int timeoutMs = 3000);
    QJsonObject readParam(int sessionId, const QString& name, int channel = 0, int timeoutMs = 2000);

    // Logging
    void setLogFile(const QString& path);

private:
    QLocalSocket* m_socket;
    QByteArray m_buffer;
    int m_requestId;
    QString m_logFile;

    void logMsg(const QString& s);
    QJsonObject sendRequestSync(const QJsonObject& req, int timeoutMs = 2000);
};