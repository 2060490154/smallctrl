#ifndef SHUTTERDEFINE_H
#define SHUTTERDEFINE_H


#define M_SHUTTER_FREAM_LENGTH 8
#define M_SHUTTER_LOG_FILE QString("FILE:ShutterLog.txt:FILE")
#define M_OUT_MSG(sFileName, msg) sFileName + msg



enum _ShutterStatus
{
    M_SHUTTER_STATUS_NONE = 0,
    M_SHUTTER_STATUS_OPENED = 1,
    M_SHUTTER_STATUS_CLOSED = 2
};

typedef struct _shutterDevInfo
{
    QHostAddress tHostAddress;
    int nCurrentStatus;
    unsigned int nSetLimitData;//设定的门限值
    int nCurrentVoltData;//当前采集的值

    bool bProcessStart;//流程是否开启
    bool bProcessEnd;//流程是否结束
}tShutterDevInfo;

typedef struct _shutterCmd
{
    unsigned char startflag;//起始标志
    unsigned char status;    //0x00打开；0x01关闭
    unsigned char cmdType;//命令类型 11位设置状态 00 为查询  01位设置电压阈值 02为设置流程打开模式
    unsigned char data[2];//数据
    unsigned char endflag;//结束标志

}tShutterCmd;

typedef struct _shutterRcvData
{
    char startflag;//起始标志
    char status;
    char cmdType;//命令类型 11位设置状态 00 为查询  01位设置电压阈值
    char threshold[2];
    char currentdata[2];
    char endflag;//结束标志

}tShutterRcvData;


#endif // SHUTTERDEFINE_H
