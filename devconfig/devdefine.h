#ifndef PLATFORMDEVDEFINE_H
#define PLATFORMDEVDEFINE_H
#include <QObject>
#include <iostream>
using namespace std;

//设备类型定义
enum eDevType {
    M_DEV_MOTOR = 1,//电机控制
    M_DEV_ENERGY = 2,//能量计
    M_DEV_OSCII = 3,//示波器
    M_DEV_SHUTTER = 4,//光闸
    M_DEV_NEARFIELD_CCD = 5,//近场CCD
    M_DEV_FARFIELD_CCD = 6,//远场CCD
    M_DEV_ENERGY_ADJ = 7,//能量调节模块
    M_DEV_DATA_PROCESS = 8, //数据处理
    M_DEV_DEFECTCHECK = 9//损伤诊断系统
};


typedef struct _channelInfo
{
   int nChannelNo;
   QString sChannelName;

   float lfCoeff;//1计量位移对应的脉冲数 能量计修正系数

   //电机轴信息 不是电机的设备忽略

   QString sUnit;//计量单位
   QStringList sGearIndexlist;
   QStringList sGearNamelist;
}tChannelInfo;

typedef struct _DevItem
{
   int nIndex;
   QString sDevName;
   QString sDevIPAddress;

   int nPort;

   vector<tChannelInfo> st_ChannelInfo;//具有通道属性的设备

}tDevItem;

typedef struct _DevInfo
{
    eDevType devType;
    QString sdevTypeName;
    std::vector<tDevItem> devItemlist;

}tDevInfo;





#endif // PLATFORMDEVDEFINE_H
