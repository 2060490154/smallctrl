#ifndef ENERGYADJUSTDEFINE_H
#define ENERGYADJUSTDEFINE_H
#include <QObject>


#define M_MAX_WAITTIME 1000*60 //1min
enum _EnergyADJDev_errcode
{
    E_ENRGYAD_SENDCMD_OK = 0x0 ,
    E_ENRGYAD_TIMEOUT = 0x01,
    E_ENRGYAD_CONNECT_ERROR = 0x02,
    E_ENRGYAD_SENDCMD_BUSY = 0x03,
    E_ENRGYAD_SENDCMD_DATA_FAILED = 0x04,
    E_ENRGYAD_RECV_DATA_TIMEOUT = 0x05,
    E_ENRGYAD_MOVE_UNREACH = 0x06
};


typedef struct _devstatus
{
    unsigned short int driverErr:1;//驱动错误
    unsigned short int devTempwarning:1;//高温警告
    unsigned short int devOverTemp:1;//超温
    unsigned short int driverloaderr:1;//设备驱动加载错误
    unsigned short int loadWarning:1;//open load on phase A or B
    unsigned short int voltageErr:1;//under voltage error
    unsigned short int memErr:1;//external memory error
    unsigned short int resetOccurred:1;//reset has occurred
    unsigned short int leftlimit:1;//left limit switch is pressed
    unsigned short int rightlimit:1;//right limit switch is pressed
    unsigned short int stallguard:1;//stall guard flag is active
    unsigned short int standstill:1;//motor is stand still
    unsigned short int targetvelocityReached:1;//motor target velocity reached
    unsigned short int targetPositionReached:1;//target position reached
    unsigned short int HomingAfterReset:1;//重启后正在归零
    unsigned short int devCalibration:1;//设备校准
}st_devstatus;

union _un_devstatus
{
    unsigned short int i16status;
    st_devstatus devStatus;
};



typedef struct _energyAdjustDevStatus
{
    int nStatus;//0-未运动 1-正在运动 2-运动成功 3-运动失败
    float lfcurrentpresent;//当前位置

    bool  isRightLimit;//是否处于右限位状态
    bool  isLeftLimit;//是否处于左限位状态

    QString sErroInfo;//错误信息

}energyAdjustDevStatus;
Q_DECLARE_METATYPE(energyAdjustDevStatus)

#endif // ENERGYADJUSTDEFINE_H
