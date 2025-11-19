#include "workflow.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

Workflow::Workflow(QString sWorkflowName,tCommonInfo* pCommonInfo)
{
    m_sWorkflowName = sWorkflowName;
    m_pCommonInfo = pCommonInfo;

    m_nWorkflowRel = M_TASK_STATUS_UNRUN;

    m_tWorkflow.clear();
}

/******************************************
* 功能:添加任务列表
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void Workflow::addTaskList(Tasklist* pTaskList)
{
    m_tWorkflow.push_back(pTaskList);
}

/******************************************
* 功能:获取所有子任务数量
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int Workflow::getAllTaskNum()
{
    int nNum = 0;
    for(Tasklist* _pTaskList:m_tWorkflow)
    {
         nNum = nNum + _pTaskList->m_tTaskList.size();
    }

    return nNum;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void Workflow::clearAllTask()
{
    for(int i = m_tWorkflow.size() -1; i >= 0; i--)
    {
        delete m_tWorkflow[i];
    }

    m_tWorkflow.clear();
}
