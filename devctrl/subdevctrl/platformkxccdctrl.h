#ifndef QPLATFORMKXCCDCTRL_H
#define QPLATFORMKXCCDCTRL_H

#include <QObject>
#include <QDir>
#include <QThread>
#include "kxccdctrl.h"
#include "platformtaskdefine.h"
#include "workflowdefine.h"
#include "taskinfo.h"



class QPlatformKxccdCtrl:public CKxccdCtrl
{
    Q_OBJECT

public:
    QPlatformKxccdCtrl(int nIndex=0,QString sDevName="",QObject *parent = nullptr);

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//发送的数据内容 用于其他窗口监视该数据

public:
    bool runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);//必须定义

    bool checkDevStatus();//检查设备状态

};

#endif // QPLATFORMKXCCDCTRL_H
