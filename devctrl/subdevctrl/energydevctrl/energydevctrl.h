#ifndef QENERGYDEVCTRL_H
#define QENERGYDEVCTRL_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <qmutex.h>
#include "energydevdefine.h"


class QEnergyDevCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QEnergyDevCtrl(QString sIP,int nPort);
    ~QEnergyDevCtrl();

signals:


public slots:

    void onCapMeasureData();//在线程里面运行  实时监听数据


public:

    void delayTime(int mesc);

    int openDev();

    void closeDev();

    int getDevParam(int nChannelNo);//查询设备参数

    int getDevConnectStatus(int nChannelNo);//查询探头连接状态

    int setChannelExTrigger(int nChannelNo,bool bEnable);//设置通道信息

    int setChannelTriggerLevel(int nChannelNo,float lfTriggerLevel);//设置触发门限

    int setChannelWaveLength(int nChannelNo,int nWaveLength);//设置测量波长

    int setChannelMeauserRange(int nChannelNo,int nRangeIndex);//设置测量范围

    int setChannelStartCap(int nChannelNo);//开始获取测量数据

    int setChannelStopCap(int nChannelNo);//停止获取测量数据

    tDevParam m_tDevParam;

    bool m_bCapData;

    QTimer* _capDataTimer;





private:


    int sendCmdData(QString sCmd);//发送数据

    void praseDevParam(QByteArray data,int nChannelNo);

    void praseMeasureData(QByteArray data);//解析测量值数据

    int getTriggerLevel();//获取触发门限
    int getMeauserMode();//获取当前测量模式

    int getMeauserRange();//获取当前测量档位
    int getMeasureWaveLength();//获取测量波长




    QSerialPort* _pSerialPort;

   // QTcpSocket* _pTcpSocket;


    bool _bHaveRecvData;//标记是否收到数据
    QByteArray _reciveData;//接收到的数据


    QMutex _EnergyDevlock;
};


#endif // QENERGYDEVCTRL_H
