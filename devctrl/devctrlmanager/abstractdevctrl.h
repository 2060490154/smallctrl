#ifndef QABSTRACTDEVCTRL_H
#define QABSTRACTDEVCTRL_H

#include <QObject>
#include "workflow.h"

class QAbstractDevCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QAbstractDevCtrl(QObject *parent = nullptr);

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//发送的数据内容 用于其他窗口监视该数据

public slots:

public:
    virtual bool runTask(TaskInfo* pTask,tCommonInfo* pCommonInfo=nullptr,void* pClass=nullptr)=0;//执行任务 同时负责更新任务状态等信息
};

#endif // QABSTRACTDEVCTRL_H
