#ifndef MOTORDEVCTRLBASIC_H
#define MOTORDEVCTRLBASIC_H
#include <QObject>
#include <QVariant>


class QMotorDevCtrlBasic:public QObject
{

    Q_OBJECT

signals:
    void signal_updateMotorStatus(int nMotorIndex,QVariant varStatus);


public:
    QMotorDevCtrlBasic();

public:

    virtual bool moveToGear(int nMotorIndex,int nGearIndex)=0;//运动到特定档位
    virtual bool moveToRelationPos(int nMotorIndex,int nPos)=0;//运动到指定位置 相对运动
    virtual bool moveToAbsolutePos(int nMotorIndex,int nPos) = 0;//绝对位置运动
    virtual bool moveToHome(int nMotorIndex)=0;//归零操作
    virtual bool stopMove(int nMotorIndex)=0;//停止运动
    virtual bool checkMotorRunning(int nMotorIndex)=0;//检查电机是否正在运动

    virtual QVariant getMotorStatusInfo(int nMotorIndex)=0;//获取电机当前状态

};

#endif // MOTORDEVCTRLBASIC_H
