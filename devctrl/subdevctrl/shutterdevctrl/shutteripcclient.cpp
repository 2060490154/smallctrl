#include "shutteripcclient.h"
#include <QElapsedTimer>
#include <QJsonArray>
#include <QDebug>

static const char* SERVER_NAME = "shutter_helper_ipc";

ShutterIpcClient::ShutterIpcClient(QObject* parent) : QObject(parent), m_socket(new QLocalSocket(this)), m_requestId(1) {
    // log socket errors to help debug transient failures
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    connect(m_socket, &QLocalSocket::errorOccurred, this, [this](QAbstractSocket::SocketError err) {
        Q_UNUSED(err);
        logMsg(QString("QLocalSocket errorOccurred: %1").arg(m_socket->errorString()));
        });
#else
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(/* no-op */));
    // For older Qt where errorOccurred might not exist, optionally poll m_socket->errorString() on failures
#endif
}

ShutterIpcClient::~ShutterIpcClient() {
    if (m_socket->isOpen()) m_socket->close();
}


void ShutterIpcClient::setLogFile(const QString& path) {
    m_logFile = path;
}

void ShutterIpcClient::logMsg(const QString& s) {
    if (m_logFile.isEmpty()) return;
    QFile f(m_logFile);
    if (f.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream ts(&f);
        ts << QDateTime::currentDateTime().toString(Qt::ISODate) << " " << s << "\n";
        f.close();
    }
    qDebug().noquote() << s;
}

bool ShutterIpcClient::connectToHelper(int timeoutMs) {
    if (m_socket->state() == QLocalSocket::ConnectedState) return true;
    m_socket->connectToServer(SERVER_NAME);
    if (!m_socket->waitForConnected(timeoutMs)) {
        logMsg(QString("Cannot connect to helper: %1").arg(m_socket->errorString()));
        return false;
    }
    logMsg("Connected to helper");
    return true;
}

QJsonObject ShutterIpcClient::sendRequestSync(const QJsonObject& req, int timeoutMs) {
    // Ensure helper connection
    if (!connectToHelper(timeoutMs)) return QJsonObject();

    // Prepare request with id
    int id = m_requestId++;
    QJsonObject r = req;
    r["id"] = id;
    QByteArray out = QJsonDocument(r).toJson(QJsonDocument::Compact) + "\n";

    // Log outgoing request for diagnostics
    logMsg(QString("sendRequestSync request id=%1: %2").arg(id).arg(QString::fromLatin1(QJsonDocument(r).toJson(QJsonDocument::Compact))));

    // Send request
    qint64 written = m_socket->write(out);
    m_socket->flush();
    // wait for bytes written (short)
    if (!m_socket->waitForBytesWritten(500)) {
        logMsg(QString("sendRequestSync: waitForBytesWritten returned false (errno?) after writing id=%1").arg(id));
    }
    else {
        logMsg(QString("sendRequestSync: bytesWritten=%1 for id=%2").arg(written).arg(id));
    }

    QByteArray replyData;
    QElapsedTimer t; t.start();

    // Keep possible fallback ok response (best-effort)
    QJsonObject lastOkFallback;

    // Read loop
    while (t.elapsed() < timeoutMs) {
        // If socket disconnected, log and attempt a quick reconnect, but continue waiting until timeout
        if (m_socket->state() == QLocalSocket::UnconnectedState) {
            logMsg("sendRequestSync: socket disconnected while waiting for reply; attempting quick reconnect...");
            m_socket->abort();
            m_socket->connectToServer(SERVER_NAME);
            if (m_socket->waitForConnected(500)) {
                logMsg("sendRequestSync: reconnect succeeded");
            }
            else {
                logMsg("sendRequestSync: reconnect failed or timed out (will keep waiting)");
            }
        }

        // Wait for data
        if (m_socket->waitForReadyRead(200)) {
            replyData += m_socket->readAll();
            // Process complete lines
            while (replyData.contains('\n')) {
                int pos = replyData.indexOf('\n');
                QByteArray line = replyData.left(pos).trimmed();
                replyData = replyData.mid(pos + 1);
                if (line.isEmpty()) continue;

                // Log raw helper line
                logMsg(QString("Raw line from helper: %1").arg(QString::fromLatin1(line)));

                QJsonParseError err;
                QJsonDocument doc = QJsonDocument::fromJson(line, &err);
                if (err.error != QJsonParseError::NoError) {
                    logMsg(QString("JSON parse error: %1 line: %2").arg(err.errorString()).arg(QString::fromLatin1(line)));
                    continue;
                }
                QJsonObject obj = doc.object();

                // Exact-id match -> return
                if (obj.contains("id") && obj.value("id").toInt(-1) == id) {
                    logMsg(QString("sendRequestSync matched id=%1 response: %2").arg(id).arg(QString::fromLatin1(QJsonDocument(obj).toJson(QJsonDocument::Compact))));
                    return obj;
                }

                // If status==ok for other messages, remember as fallback (best-effort)
                if (obj.value("status").toString() == "ok") {
                    lastOkFallback = obj;
                    logMsg(QString("sendRequestSync remembered fallback ok-response (id=%1)").arg(obj.value("id").toInt(-1)));
                    // continue searching for exact match
                }
                else {
                    // Non-ok messages logged for debugging
                    logMsg(QString("sendRequestSync received non-matching message: %1").arg(QString::fromLatin1(QJsonDocument(obj).toJson(QJsonDocument::Compact))));
                }
            }
        }
    }

    // Final attempt to read any remaining data before giving up
    if (m_socket->bytesAvailable()) {
        replyData += m_socket->readAll();
        if (!replyData.isEmpty()) {
            logMsg(QString("sendRequestSync final read leftover: %1").arg(QString::fromLatin1(replyData)));
            // try parse leftover lines one final time
            while (replyData.contains('\n')) {
                int pos = replyData.indexOf('\n');
                QByteArray line = replyData.left(pos).trimmed();
                replyData = replyData.mid(pos + 1);
                if (line.isEmpty()) continue;
                QJsonParseError err;
                QJsonDocument doc = QJsonDocument::fromJson(line, &err);
                if (err.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("id") && obj.value("id").toInt(-1) == id) {
                        logMsg(QString("sendRequestSync matched id=%1 on final-read response: %2").arg(id).arg(QString::fromLatin1(QJsonDocument(obj).toJson(QJsonDocument::Compact))));
                        return obj;
                    }
                    if (obj.value("status").toString() == "ok") {
                        lastOkFallback = obj;
                        logMsg(QString("sendRequestSync final-read remembered fallback ok-response (id=%1)").arg(obj.value("id").toInt(-1)));
                    }
                    else {
                        logMsg(QString("sendRequestSync final-read non-matching message: %1").arg(QString::fromLatin1(QJsonDocument(obj).toJson(QJsonDocument::Compact))));
                    }
                }
                else {
                    logMsg(QString("sendRequestSync final-read JSON parse error: %1 line: %2").arg(err.errorString()).arg(QString::fromLatin1(line)));
                }
            }
        }
    }

    // timeout reached
    if (!lastOkFallback.isEmpty()) {
        logMsg(QString("Request id=%1 timed out but using last ok-fallback response: %2").arg(id).arg(QString::fromLatin1(QJsonDocument(lastOkFallback).toJson(QJsonDocument::Compact))));
        return lastOkFallback;
    }

    logMsg(QString("Request id=%1 timed out; no usable response. remaining buffer: %2").arg(id).arg(QString::fromLatin1(replyData)));
    return QJsonObject();
}

QJsonArray ShutterIpcClient::listDevices(int timeoutMs) {
    QJsonObject req; req["cmd"] = "list_devices";
    QJsonObject r = sendRequestSync(req, timeoutMs);
    if (r.isEmpty() || r.value("status").toString() != "ok") return QJsonArray();
    return r.value("payload").toArray();
}

int ShutterIpcClient::connectDevice(int moduleIndex, int deviceIndex, int timeoutMs) {
    QJsonObject req; req["cmd"] = "connect";
    QJsonObject args; args["moduleIndex"] = moduleIndex; args["deviceIndex"] = deviceIndex;
    req["args"] = args;
    QJsonObject r = sendRequestSync(req, timeoutMs);
    if (r.isEmpty()) {
        logMsg(QString("connectDevice: no response for m=%1 d=%2").arg(moduleIndex).arg(deviceIndex));
        return -1;
    }
    // log raw helper response (already logged in sendRequestSync), but do again for clarity
    logMsg(QString("connectDevice raw response: %1").arg(QString::fromLatin1(QJsonDocument(r).toJson(QJsonDocument::Compact))));
    if (r.value("status").toString() != "ok") {
        logMsg(QString("connectDevice failed: %1").arg(r.value("error").toString()));
        return -1;
    }
    QJsonObject payload = r.value("payload").toObject();
    int sid = payload.value("sessionId").toInt(-1);
    if (sid <= 0) {
        logMsg(QString("connectDevice: missing/invalid sessionId in payload for m=%1 d=%2").arg(moduleIndex).arg(deviceIndex));
        return -1;
    }
    return sid;
}

bool ShutterIpcClient::disconnectDevice(int sessionId, int timeoutMs) {
    QJsonObject req; req["cmd"] = "disconnect";
    QJsonObject args; args["sessionId"] = sessionId; req["args"] = args;
    QJsonObject r = sendRequestSync(req, timeoutMs);
    return (!r.isEmpty() && r.value("status").toString() == "ok");
}

QJsonObject ShutterIpcClient::comWrite(int sessionId, const QString& text, int timeoutMs) {
    QJsonObject req; req["cmd"] = "com_write";
    QJsonObject args; args["sessionId"] = sessionId; args["text"] = text;
    req["args"] = args;
    QJsonObject r = sendRequestSync(req, timeoutMs);
    if (r.isEmpty()) return QJsonObject();
    return r;
}

QJsonObject ShutterIpcClient::openShutter(int sessionId, int channel, int timeoutMs) {
    QJsonObject req; req["cmd"] = "open_shutter";
    QJsonObject args; args["sessionId"] = sessionId; args["channel"] = channel;
    req["args"] = args;
    QJsonObject r = sendRequestSync(req, timeoutMs);
    if (r.isEmpty()) {
        // force reconnect once
        m_socket->abort();
        connectToHelper(1000);
        r = sendRequestSync(req, timeoutMs);
    }
    return r;
}

QJsonObject ShutterIpcClient::closeShutter(int sessionId, int channel, int timeoutMs) {
    QJsonObject req; req["cmd"] = "close_shutter";
    QJsonObject args; args["sessionId"] = sessionId; args["channel"] = channel;
    req["args"] = args;
    QJsonObject r = sendRequestSync(req, timeoutMs);
    return r;
}

QJsonObject ShutterIpcClient::readParam(int sessionId, const QString& name, int channel, int timeoutMs) {
    QJsonObject req; req["cmd"] = "read_param";
    QJsonObject args; args["sessionId"] = sessionId; args["name"] = name; args["channel"] = channel;
    req["args"] = args;
    QJsonObject r = sendRequestSync(req, timeoutMs);
    return r;
}