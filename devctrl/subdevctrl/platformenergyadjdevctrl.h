#ifndef PLATFORMENERGYADJDEVCTRL_H
#define PLATFORMENERGYADJDEVCTRL_H

#include <QObject>
#include <QMutex>

#include "platformtaskdefine.h"
#include "workflowdefine.h"
#include "taskinfo.h"

#include "energyadjustdevctrl.h"

class QPlatformEnergyAdjDevCtrl:public QEnergyAdjustDevCtrl
{
        Q_OBJECT

public:
    QPlatformEnergyAdjDevCtrl(QString sIP,int nPort);

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//必须定义

public slots:

public:
    bool runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);//必须定义和实现的函数

    bool checkDevStatus();//设备是否连接

private:



};

#endif // PLATFORMENERGYADJDEVCTRL_H
