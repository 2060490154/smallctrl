#ifndef MOTORCTRLWIDGETDEFINE_H
#define MOTORCTRLWIDGETDEFINE_H

#include "motordevctrlbasic.h"
using namespace std;

//电机档位信息
typedef struct  _tMotorPostionInfo
{
    QString  sPostionName;//位置名称
    int nGearIndex;//档位信息
}tMotorPostionInfo;

//电机数据库配置信息
typedef struct  _tMotorConfigInfo
{
    void* pMotorCtrl;//设备控制类
    int nMotorIndex;
    QString  sMotorName;
    int nMotorCoeff;//对于平移台是1mm对应的步数  对于轮盘为1度对应的度数
    QString sUnit;

    vector<tMotorPostionInfo> tPostionConfig;
}tMotorConfigInfo;

#endif // MOTORCTRLWIDGETDEFINE_H
