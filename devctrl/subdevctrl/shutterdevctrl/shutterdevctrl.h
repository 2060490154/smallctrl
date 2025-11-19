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


class QShutterDevCtrl:public QObject
{

        Q_OBJECT
public:
    QShutterDevCtrl(QString sIPAddress,int nPort,QString sDevName);
    ~QShutterDevCtrl();

signals:


public slots:
    void ReceiveData();


    //属性
public:

    QUdpSocket* m_pUdpSocket;

    tShutterDevInfo m_tShutterDevInfo;

    QString m_sDevName;



    //方法
public:

    bool SetDevStatus(bool bOPen);//设置光闸状态

    bool SetDevLimit(int ndata);//设置阈值  单位为mv

    bool getDevStatus();//查询光闸状态

    void  ParseRcvData(QString sIPAddress,int nPort,char* pbuff,int nSize);

    bool checkDevStatus();//检查是否在线

    bool setProcessStatus(int nPlusCnt,int nDeleay);//设置执行流程状态，并等待流程执行结束

private:


private:
     int         _nPort;
     int _nDevTotalNum;
     bool _bDevStatus;


};

#endif // QShutterDevCtrl_H
