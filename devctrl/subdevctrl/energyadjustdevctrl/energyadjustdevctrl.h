#ifndef ENERGYADJUSTDEVCTRL_H
#define ENERGYADJUSTDEVCTRL_H

#include <QObject>
#include <QTime>
#include <QVariant>
#include <QDebug>
#include <QThread>
#include "energyadjustdefine.h"
#include "customtcpserver.h"




class QEnergyAdjustDevCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QEnergyAdjustDevCtrl(QString sComAddress,int nPort);

    ~QEnergyAdjustDevCtrl();

signals:

    void signal_updateEnergyAdjDevStatus(QVariant l_var,bool bShowMoveStatus = true);

    void signal_sendcmd(QString sCmd);

public slots:

    void onDevConnect();



public:


    virtual bool MoveToPrecent(float lfPercent);//设置当前百分比 透过率
    virtual bool moveToHome();//归零操作
    virtual bool stopMove();//停止运动
    virtual bool checkMotorRunning();//检查电机是否正在运动

    bool setMinPowerPos();//设置当前位置为最小能量位置


    energyAdjustDevStatus m_tEnergyAdjDevStatus;
    bool m_bConnect;



private:


    int _sendCmd(QString sCmd,QString& sResponseValue);

    void _sendEnergyAdjDevStatus();

    bool _getCurrentStatus();

    void InitErrorInfo();

    QString _getErrorCodeInfo(int nerrcode);

private:

    union _un_devstatus _EnergyDev_devstatus;//设备的状态
    bool _bMotorEnable;//电机是否可用

    QHash<int,QString> _errorCodeInfolist;//错误码与错误信息描述对应关系

    QTime _current_time;//多线程

    bool _bManualStop;




protected:
    QCustomTcpServer * _pTcpServer;


};

#endif // ENERGYADJUSTDEVCTRL_H
