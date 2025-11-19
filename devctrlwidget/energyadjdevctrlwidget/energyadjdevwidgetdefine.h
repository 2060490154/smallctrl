#ifndef ENERGYADJDEVWIDGETDEFINE_H
#define ENERGYADJDEVWIDGETDEFINE_H

using namespace std;



//能量调节模块配置信息
typedef struct  _tEnergyAdjDevConfig
{
    void* pEnergyAdjDevCtrl;//设备控制类
    QString  sDevName;

}tEnergyAdjDevConfig;

#endif // ENERGYADJDEVWIDGETDEFINE_H
