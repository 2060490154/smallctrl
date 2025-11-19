#ifndef DEFECTCHECKCTRL_H
#define DEFECTCHECKCTRL_H

#include <QObject>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include "defectchecksysdefine.h"

class QDefectCheckSysCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QDefectCheckSysCtrl(QString sIPAddress,int nPort,QObject *parent = 0);
    ~QDefectCheckSysCtrl();

public:
    bool publishExpInfo(QString sExpNo,int nMeasureType);
    bool prepareDefectSys(int nPointNo,int nShotNo=0);
    bool  queryCapResult();//查询采集是否结束
    int queryDefectResult(int nPointNo,int nShotNo=0);    //返回值0表示判断失败，1表示有损伤，2表示无损伤
    bool setDevIPAndPort(QString sIP,int nPort);

    bool closeDefectsys();

private:
    bool sendCmd(st_PACK_CMD& l_tPackCmd);
    void rcvCmdData();//接收命令
    void cmdProcess(char* buff);
    bool createSocket(const QString sIP, int nPort);//创建socket


public:
    bool m_bOpenSys;
    int m_nSysWorkMode;
    int m_nSysStatus;   //系统状态。包括连接、未连接和接受实验信息成功
    int m_nPrepareStatus;   //系统准备状态（每一点/发次）。包括准备成功和准备失败。
    int m_nResultStatus;    //系统判断损伤结果判断。包括有损伤、无损伤和判断失败。
    int m_nCapImageStatus;
    int m_nDefectTimes; //保存有损伤点的发次信息
    int m_nCloseStatus;
    st_DevSocket m_tDevSocket;  //设备套接字，包含命令socket和数据socket

    QString m_sErrInfo;

};

#endif // QDefectCheckSysCtrl_H
