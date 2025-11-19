#ifndef QSHUTTERCTRL_H
#define QSHUTTERCTRL_H

#include <QObject>
#include <qthread.h>
#include <QtNetwork/QUdpSocket>
#include <QApplication>
#include <QMessageBox>
#include <QTime>
#include <QDebug>
#include "shutterdefine.h"
#include "ishutterbackend.h"

class QShutterDevCtrl :public QObject
{

    Q_OBJECT
public:
    QShutterDevCtrl(QString sIPAddress, int nPort, QString sDevName);
    ~QShutterDevCtrl();

signals:


public slots:
    void ReceiveData();


    //属性
public:

    QUdpSocket* m_pUdpSocket;

    tShutterDevInfo m_tShutterDevInfo;

    QString m_sDevName;

    // 新增：后端及 session 管理
    void setBackend(IShutterBackend* backend) { m_backend = backend; }
    IShutterBackend* backend() const { return m_backend; }

    void setSessionId(int sid) { m_sessionId = sid; }
    int sessionId() const { return m_sessionId; }

    //方法
public:

    // 将原来直接操作硬件的接口改为通过 backend->openShutter/closeShutter 等调用。
    bool SetDevStatus(bool bOPen);//设置光闸状态（若使用 IPC 后端，需要先设置 sessionId）

    bool SetDevLimit(int ndata);//设置阈值  单位为mv

    bool getDevStatus();//查询光闸状态

    void  ParseRcvData(QString sIPAddress, int nPort, char* pbuff, int nSize);

    bool checkDevStatus();//检查是否在线

    bool setProcessStatus(int nPlusCnt, int nDeleay);//设置执行流程状态，并等待流程执行结束




private:
    int         _nPort;
    int _nDevTotalNum;
    bool _bDevStatus;

    // 新增：
    IShutterBackend* m_backend = nullptr;
    int m_sessionId = -1;
};


#endif // QShutterDevCtrl_H