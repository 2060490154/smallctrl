#ifndef QCustomTcpServer_H
#define QCustomTcpServer_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QMutex>
#include <QTime>
#include <qthread.h>
#include "energyadjustdefine.h"


class QCustomTcpServer:public QObject
{
    Q_OBJECT
public:
    QCustomTcpServer(QString sComAddress,int nPort);

    ~QCustomTcpServer();

    void cleardata();
    QString getRcvdata();

signals:

    void connectdev();//设备已链接

public slots:

    void onNewConnect();
    void onReadData();//读取数据
    int onSendCmd(QString sCmd);


public:

    bool m_bConnect;




private:
    QTcpSocket* _pClientSocket;//客户端--能量调节模块的socket
    QTcpServer* _pTcpServer;//监听socket
    QString _sIPAddress;//客户端IP

    QString _sResponseData;//接收到的数据
    int _nPort;

    QMutex _lock;

    QTime _current_time;

};

#endif // QCustomTcpServer_H
