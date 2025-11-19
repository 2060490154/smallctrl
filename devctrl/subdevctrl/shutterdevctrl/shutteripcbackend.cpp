#include "shutteripcbackend.h"
#include <QProcess>
#include <QThread>
#include <QDebug>

ShutterIpcBackend::ShutterIpcBackend(QObject* parent) : IShutterBackend(parent), m_client(this) {
    m_client.setLogFile("shutter_client.log");
}

ShutterIpcBackend::~ShutterIpcBackend() {}

QJsonArray ShutterIpcBackend::listDevices(int timeoutMs) {
    if (!m_client.connectToHelper(timeoutMs)) {
        if (!m_helperPath.isEmpty() && ensureHelperRunning(2000)) {
            if (!m_client.connectToHelper(timeoutMs)) return QJsonArray();
        }
        else {
            return QJsonArray();
        }
    }
    return m_client.listDevices(timeoutMs);
}

int ShutterIpcBackend::connectDevice(int moduleIndex, int deviceIndex, int timeoutMs) {
    if (!m_client.connectToHelper(timeoutMs)) {
        if (!m_helperPath.isEmpty() && ensureHelperRunning(2000)) {
            if (!m_client.connectToHelper(timeoutMs)) return -1;
        }
        else {
            return -1;
        }
    }
    return m_client.connectDevice(moduleIndex, deviceIndex, timeoutMs);
}

bool ShutterIpcBackend::disconnectDevice(int sessionId, int timeoutMs) {
    return m_client.disconnectDevice(sessionId, timeoutMs);
}

QJsonObject ShutterIpcBackend::openShutter(int sessionId, int channel, int timeoutMs) {
    return m_client.openShutter(sessionId, channel, timeoutMs);
}

QJsonObject ShutterIpcBackend::closeShutter(int sessionId, int channel, int timeoutMs) {
    return m_client.closeShutter(sessionId, channel, timeoutMs);
}

QJsonObject ShutterIpcBackend::comWrite(int sessionId, const QString& text, int timeoutMs) {
    return m_client.comWrite(sessionId, text, timeoutMs);
}

QJsonObject ShutterIpcBackend::readParam(int sessionId, const QString& name, int channel, int timeoutMs) {
    return m_client.readParam(sessionId, name, channel, timeoutMs);
}

bool ShutterIpcBackend::ensureHelperRunning(int maxWaitMs) {
    if (m_helperPath.isEmpty()) return false;
    bool started = QProcess::startDetached(m_helperPath);
    if (!started) {
        qWarning() << "Shutter helper startDetached failed:" << m_helperPath;
        return false;
    }
    int waited = 0;
    const int step = 200;
    while (waited < maxWaitMs) {
        QThread::msleep(step);
        waited += step;
        if (m_client.connectToHelper(200)) return true;
    }
    return false;
}