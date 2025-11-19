#ifndef PLATFORMENERGYDEVCTRL_H
#define PLATFORMENERGYDEVCTRL_H

#include <QObject>
#include <QMutex>
#include "energydevctrl.h"
#include "platformtaskdefine.h"
#include "workflowdefine.h"
#include "taskinfo.h"

class QPlatformEnergyDevCtrl:public QEnergyDevCtrl
{
        Q_OBJECT

public:
    QPlatformEnergyDevCtrl(QString sIP,int nPort);

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//必须定义

public slots:

public:
    bool runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);//必须定义和实现的函数

    bool checkDevStatus();//设备是否连接

private:



};

#endif // PLATFORMENERGYDEVCTRL_H
