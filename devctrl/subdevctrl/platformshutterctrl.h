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
    explicit QPlatformShutterCtrl(QString sIPAddress, int nPort, QString sDevName);

    void setBackend(IShutterBackend* backend) { m_backend = backend; }
    IShutterBackend* backend() const { return m_backend; }

public slots:

public:
    bool runTask(TaskInfo* pTask, tCommonInfo* pCommonInfo = nullptr);
    bool checkDevStatus();

private:
    IShutterBackend* m_backend = nullptr;
};


#endif // QPLATFORMSHUTTERCTRL_H
