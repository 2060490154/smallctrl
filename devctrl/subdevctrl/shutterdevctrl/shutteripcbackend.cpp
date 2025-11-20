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
int ShutterIpcBackend::connectDevice(int moduleIndex, int deviceIndex, int timeoutMs)
{
    // Build candidate index pairs (try to tolerate 0-based/1-based mismatches)
    QList<QPair<int, int>> candidates;
    candidates.append(qMakePair(moduleIndex, deviceIndex));
    candidates.append(qMakePair(moduleIndex + 1, deviceIndex + 1)); // try +1 (0->1)
    if (moduleIndex > 0 && deviceIndex > 0) candidates.append(qMakePair(moduleIndex - 1, deviceIndex - 1)); // try -1

    // Deduplicate candidates while preserving order
    QList<QPair<int, int>> uniq;
    for (const QPair<int, int>& c : candidates) {
        bool found = false;
        for (const QPair<int, int>& u : uniq) {
            if (u.first == c.first && u.second == c.second) { found = true; break; }
        }
        if (!found) uniq.append(c);
    }

    // Try each candidate until one succeeds (connectDevice returns sessionId > 0)
    for (const QPair<int, int>& c : uniq) {
        qDebug() << "ShutterIpcBackend::connectDevice trying moduleIndex=" << c.first << " deviceIndex=" << c.second;
        int sid = m_client.connectDevice(c.first, c.second, timeoutMs);
        if (sid > 0) {
            qDebug() << "ShutterIpcBackend::connectDevice success sessionId=" << sid << "(used m=" << c.first << " d=" << c.second << ")";
            return sid;
        }
        else {
            qDebug() << "ShutterIpcBackend::connectDevice attempt failed for m=" << c.first << " d=" << c.second;
        }
    }

    qDebug() << "ShutterIpcBackend::connectDevice: all attempts failed";
    return -1;
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