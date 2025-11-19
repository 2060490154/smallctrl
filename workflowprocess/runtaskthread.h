#ifndef QRUNTASKTHREAD_H
#define QRUNTASKTHREAD_H
#include <QThread>
#include <QRunnable>
#include "workflow.h"
#include "devctrlmanager.h"
#include "platformtaskdefine.h"

class QRunTaskThread :public QRunnable
{

public:
    QRunTaskThread(QDevCtrlManager* pCtrl,TaskInfo* pTaskInfo,tCommonInfo* pCommonInfo,vector<TaskInfo*> pWaitforTaskInfo);

    TaskInfo* m_pTaskInfo;

    vector<TaskInfo*> m_pRelationTaskInfolist;

public:
    void run();

public:
    QDevCtrlManager* _pDevManagerCtrl;
    tCommonInfo* _pCommonInfo;
};

#endif // QRUNTASKTHREAD_H
