#ifndef MOTORDEVDEFINE_H
#define MOTORDEVDEFINE_H
#include <QObject>
enum _MotorDev_Errcode
{
    ERR_NOERR = 0x0 ,
    ERR_UNKNOWN = 0x01,//未知错误
    ERR_PARAERR = 0x02,//参数错误
    ERR_TIMEOUT = 0x03,//操作超时
    ERRORERR_CONTROLLERBUSY = 0x04,//控制卡状态忙
    ERR_CANNOT_CONNECTETH = 0x08,//无法连接错误
    MOTION_ERR_HANDLEERR = 0x09,//卡号错误
    ERR_SENDERR = 0x0A,//数据传输错误
    ERR_POS_SUB_LIMIT = 0x0B,//电机已停止 但位置偏差较大
    ERR_MOTOR_LIMITED = 0x0c,//电机已在限位状态
    ERR_MOTOR_STOP = 0x0d,//电机强制终止运动
};

typedef struct _motorStatusdata
{
    int nMotorStatus;//0-未运动 1-正在运动 2-运动成功 3-运动失败
    int nMotorCurrentPos;//当前位置

    double lfCurrentSpeed;

    int nMotorTargetPos;

    bool  isRightLimit;//是否处于右限位状态
    bool  isLeftLimit;//是否处于左限位状态

    QString sErroInfo;//错误信息

}motorStatusdata;
Q_DECLARE_METATYPE(motorStatusdata)


//伺服电机 pid控制 参数
typedef struct _servomotorctrldata
{
    double lfUiData;
    double lfUdData;
    double lfUiData_bak;
    double lfKp;
    double lfKi;
    double lfKd;
    double lfUdData_bak;

}servomotorctrldata;


#endif // ENERGYDEVDEFINE_H
