#ifndef QPLATFORMDEFECTCHECK_H
#define QPLATFORMDEFECTCHECK_H

#include <QObject>
#include "defectchecksysctrl/defectchecksysctrl.h"
#include "platformtaskdefine.h"
#include "workflowdefine.h"
#include "taskinfo.h"


class QPlatformDefectCheckCtrl : public QDefectCheckSysCtrl
{
    Q_OBJECT
public:
    explicit QPlatformDefectCheckCtrl(QString sIPAddress,int nPort);

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//发送的数据内容 用于其他窗口监视该数据

    void signal_showdefectimage(int nMeasureType,int imagetype,QImage img);
public slots:

public:
    bool runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);

    bool checkDevStatus();
};

#endif // QPLATFORMDEFECTCHECK_H
