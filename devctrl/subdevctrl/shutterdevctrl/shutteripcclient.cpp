#include "shutteripcclient.h"
#include <QElapsedTimer>
#include <QJsonArray>
#include <QDebug>

static const char* SERVER_NAME = "shutter_helper_ipc";

ShutterIpcClient::ShutterIpcClient(QObject* parent) : QObject(parent), m_socket(new QLocalSocket(this)), m_requestId(1) {
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
    if (!connectToHelper(timeoutMs)) return QJsonObject();
    int id = m_requestId++;
    QJsonObject r = req;
    r["id"] = id;
    QByteArray out = QJsonDocument(r).toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(out);
    m_socket->flush();

    QByteArray replyData;
    QElapsedTimer t; t.start();
    while (t.elapsed() < timeoutMs) {
        if (m_socket->waitForReadyRead(100)) {
            replyData += m_socket->readAll();
            while (replyData.contains('\n')) {
                int pos = replyData.indexOf('\n');
                QByteArray line = replyData.left(pos).trimmed();
                replyData = replyData.mid(pos + 1);
                QJsonParseError err;
                QJsonDocument doc = QJsonDocument::fromJson(line, &err);
                if (err.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("id") && obj.value("id").toInt(-1) == id) {
                        return obj;
                    }
                    // ignore other messages for this simple sync client
                }
            }
        }
    }
    // timeout
    logMsg(QString("Request id=%1 timed out").arg(id));
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
    if (r.isEmpty() || r.value("status").toString() != "ok") return -1;
    return r.value("payload").toObject().value("sessionId").toInt(-1);
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