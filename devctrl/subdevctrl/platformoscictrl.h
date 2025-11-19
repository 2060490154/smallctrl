#ifndef QPLATFORMOSCICTRL_H
#define QPLATFORMOSCICTRL_H

#include <QObject>
#include <QThread>
#include "oscidevctrl.h"
#include "platformtaskdefine.h"
#include "workflowdefine.h"
#include "taskinfo.h"
#include "devconfig.h"


class QPlatformOsciCtrl : public QOsciDevCtrl
{
    Q_OBJECT
public:
    explicit QPlatformOsciCtrl(QString sIP, QList<tOsciiChannel> vaildChannellist);

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//发送的数据内容 用于其他窗口监视该数据

public slots:

public:
    bool runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);

    bool checkDevStatus();
};

#endif // QPLATFORMOSCICTRL_H
