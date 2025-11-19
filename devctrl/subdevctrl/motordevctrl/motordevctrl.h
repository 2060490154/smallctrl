#ifndef QMotorDevCtrl_H
#define QMotorDevCtrl_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <qmutex.h>
#include <QHash>
#include "motordevctrlbasic.h"
#include "motordevdefine.h"
#include "LTDMC.h"
#include "platformtaskdefine.h"

#define M_MOTOR_CARD 0
#define M_MAX_WAIT_TIME (60*1000) //最大60S

typedef struct _st_motorstatus
{
    uint16 sevoalarm:1;
    uint16 rightlimit:1;
    uint16 leftlimit:1;
    uint16 emg:1;
    uint16 org:1;
    uint16 resv:11;

}st_motorstatus;

class QMotorDevCtrl : public QMotorDevCtrlBasic
{
    Q_OBJECT
public:
    explicit QMotorDevCtrl();
    ~QMotorDevCtrl();

signals:

public slots:


public:

    virtual bool moveToGear(int nMotorIndex,int nGearIndex) override;//运动到特定档位
    virtual bool moveToRelationPos(int nMotorIndex,int nPos) override;//运动到指定位置 相对运动
    virtual bool moveToAbsolutePos(int nMotorIndex,int nPos) override;//运动到指定位置 绝对位置运动
    virtual bool moveToHome(int nMotorIndex) override;
    virtual bool checkMotorRunning(int nMotorIndex) override;//检查电机是否正在运动
    virtual bool stopMove(int nMotorIndex) override;//停止运动

    virtual QVariant getMotorStatusInfo(int nMotorIndex) override;//获取电机状态

    bool m_bConnect;//控制器连接状态

    motorStatusdata m_tMotorStatus[6];//电机当前状态

private:

    short _initCard();//初始化板卡

    void InitProfile(int nMotorIndex);//初始化速度曲线参数

    short _stepmotor_moveMotorToRelationPos(int nMotorIndex,long nPlusCnt);//相对位置运动

    //伺服电机控制运动到指定步数
    short _seromotor_moveToPos1(int nMotorIndex,int nPos);//伺服电机 运动特定步数
    short _seromotor_moveToPos(int nMotorIndex,int nPos);//伺服电机 运动特定步数


    short _moveToHome(int nMotorIndex);//发送寻零位操作

    int _getMotorGearConfig(int nMotorIndex,int nGearIndex);//获取电机档位配置

    void _sendMotorStatus(int nMotorIndex);

    void InitErrorInfo();

    QString getMotorErrorCodeInfo(int nMotorIndex);

    void getMotorStatus(int nMotorIndex);

    QTimer _MotorTimer;

    DWORD _nCardNo;

    QMutex _CardLock;

    QHash<int,QString> _errorCodeInfolist;

    bool _bStopMotorMove;//强制停止

    servomotorctrldata _stServoMotorCtrlData[2];//伺服电机pid控制参数  x y轴





};


#endif // QMotorDevCtrl_H
