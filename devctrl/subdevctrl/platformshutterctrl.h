#ifndef QPLATFORMSHUTTERCTRL_H
#define QPLATFORMSHUTTERCTRL_H

#include <QObject>
#include "shutterdevctrl.h"
#include "platformtaskdefine.h"
#include "workflowdefine.h"
#include "taskinfo.h"




class QPlatformShutterCtrl : public QShutterDevCtrl
{
    Q_OBJECT
public:
    explicit QPlatformShutterCtrl(QString sIPAddress,int nPort,QString sDevName);

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//发送的数据内容 用于其他窗口监视该数据

public slots:

public:
    bool runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);

    bool checkDevStatus();
};

#endif // QPLATFORMSHUTTERCTRL_H
