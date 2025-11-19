#ifndef ENERGYDEVDEFINE_H
#define ENERGYDEVDEFINE_H


#define M_ENERGY_DEV_WAITFOR_TIME 3000



enum _EnergyDev_errcode
{
    E_SENDCMD_OK = 0x0 ,
    E_SENDCMD_ERROR = 0x01,
    E_CONNECT_ERROR = 0x02,
    E_SENDCMD_BUSY = 0x03,
    E_SENDCMD_DATA_LEN_ERROR = 0x04,
    E_SENDCMD_DATA_FAILED = 0x05,
    E_RECV_DATA_TIMEOUT = 0x06
};

enum _MeasureMode
{
  E_MODE_POWER = 0,//功率测量
  E_MODE_ENERGY = 1,//能量测量
  E_MODE_SSE  = 2,
  E_MODE_DBM = 6,
  E_MODE_NO_DETECTOR = 7//无探头
};


typedef struct  _tEnergyDevChannel
{
    QString m_sChannelName;
    int m_nChannelNum;

    int     i32DevPort;

    float lfMeasureValue;//获取的测量值 单位为mJ
    int   i32CurrentRange;

    int  i32CurrentWaveLength;//测量波长

    _MeasureMode   i32CurrentMeasureMode;
    float        lfTriggerLevel;

    int i32ExTriggerMode;//0--off 1--on

    bool  bConnected;//探头是否连接

    bool bNewData = false;

}tEnergyDevChannel;

typedef struct _devparam
{
    QString sDevName;

    QString sDevIP;

    bool bDevConnect;//设备是否连接

    tEnergyDevChannel channelParam[2];

}tDevParam;

#endif // ENERGYDEVDEFINE_H
