#ifndef QPlatformMotorDevCtrl_H
#define QPlatformMotorDevCtrl_H

#include <QObject>
#include <QDebug>
#include "platformtaskdefine.h"
#include "workflowdefine.h"
#include "taskinfo.h"
#include "motordevctrl.h"

class QPlatformMotorDevCtrl:public QMotorDevCtrl
{
public:
    QPlatformMotorDevCtrl();

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//发送的数据内容 用于其他窗口监视该数据

public:
    bool runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);//必须定义
    bool checkDevStatus();//检查设备状态
};

#endif // QPlatformMotorDevCtrl_H
