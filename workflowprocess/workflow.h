#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <QObject>
#include "workflowdefine.h"
#include "taskinfo.h"
using namespace std;



//工作流程
class Workflow
{

public:
    Workflow(QString sWorkflowName,tCommonInfo* pCommonInfo);


    void addTaskList(Tasklist* pTaskList);

    void clearAllTask();

    int getAllTaskNum();


public:
    vector<Tasklist*> m_tWorkflow;//正常流程

    QString m_sWorkflowName;

    tCommonInfo* m_pCommonInfo;

    _Taskstatus m_nWorkflowRel;//整个workflow执行的结果  具体定义参见taskstatus

};


#endif // WORKFLOW_H
