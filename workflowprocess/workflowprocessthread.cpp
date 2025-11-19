/**************************************************
*文件说明:QWorkflowProcessThread负责任务流程的解析 调度和执行
*在实际项目中由于设备复用 同时只能执行一个流程 因此该类为单例模式
*创建人:李刚
*维护记录:
*2020-01-16 创建 by lg
*2020-03-04 增加异常结束流程处理，任务在执行过程中失败时默认直接执行最后一个流程作为结束流程 需要注意
*
******************************************************/
#include "workflowprocessthread.h"
#include <QApplication>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif


QWorkflowProcessThread*QWorkflowProcessThread::_pInstance = nullptr;//实现单例模式
QWorkflowProcessThread::Cleaner QWorkflowProcessThread::_cleaner;

QWorkflowProcessThread::QWorkflowProcessThread(QDevCtrlManager* pDevCtrlManager)
{
    _pDevCtrlManager = pDevCtrlManager;
    _bManualStopTask = false;
    _RunTaskThreadPool.setMaxThreadCount(6);

    _i32CurrentTaskCnt = -1;

    _pWorkflow = nullptr;

}

QWorkflowProcessThread::~QWorkflowProcessThread()
{
    runTaskListStop();

}
/******************************************
* 功能:单例模式 创建实例 或返回已创建的实例
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QWorkflowProcessThread* QWorkflowProcessThread::getInstance(QDevCtrlManager* pDevCtrlManager)
{
    if(_pInstance == nullptr)
    {
        _pInstance = new QWorkflowProcessThread(pDevCtrlManager);
    }

    return _pInstance;
}
/******************************************
* 功能:检查当前线程是否正在运行
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QWorkflowProcessThread::checkThreadRunning()
{
    if(_pInstance != nullptr)
    {
        if(_RunTaskThreadPool.activeThreadCount() != 0)
        {
            return true;
        }

        if(_pInstance->isRunning())
        {
            return true;
        }
    }
    return false;
}


/*************************************************************************
功能：等待所有任务执行结果
输入：pSteplist 上一个流程指针
输出:  无
返回值：执行结果
****************************************************************************/
_Taskstatus QWorkflowProcessThread::waitForTaskListRel()
{
    int nTaskCnt = 0;
    _Taskstatus nReturnRel = M_TASK_STATUS_SUCCESS;

    while(!_bManualStopTask)
    {
        if(_RunTaskThreadPool.activeThreadCount() == 0)
        {
            break;
        }
        msleep(1);
    }

    if(_bManualStopTask)//是否终止流程
    {
        nReturnRel = M_TASK_STATUS_FAILD;//执行失败
        terminateRunningTask();//终止正在执行的任务
        _RunTaskThreadPool.waitForDone(-1);

        _bManualStopTask = false;

        return nReturnRel;
    }

    //所有线程均结束 获取流程执行结果
    for(Tasklist* _pTaskList:_pWorkflow->m_tWorkflow)
    {
        for(TaskInfo*pTaskItem: _pTaskList->m_tTaskList)
        {
            if(nTaskCnt <= _i32CurrentTaskCnt)
            {
                if(pTaskItem->m_nTaskStatus == M_TASK_STATUS_FAILD)
                {
                    nReturnRel = M_TASK_STATUS_FAILD;//执行失败
                    return nReturnRel;
                }
            }

            nTaskCnt++;
        }
     }

    return nReturnRel;
}

/******************************************
* 功能:设置当前的流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowProcessThread::setWorkflowlist(Workflow* pworkflow)
{
    _pWorkflow = pworkflow;

    m_sCurrentWorkflowName = _pWorkflow->m_sWorkflowName;
}

/******************************************
* 功能:终止流程 先终止工作流程，再执行设置的终止流程;
*      这些流程约定为短时间流程(约束条件)
* 输入:pTaskList执行流程 可以为空=nullptr
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QWorkflowProcessThread::runTaskListStop()
{
    _bManualStopTask = true;//设置新开启流程终止

    msleep(10);

   return true;
}

/******************************************
* 功能:初始化流程中所有任务的状态 为未执行状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowProcessThread::InitTaskListStatus()
{

    for(size_t i32Index = 0; i32Index < _pWorkflow->m_tWorkflow.size(); i32Index++)
    {
        Tasklist* _pTaskList = _pWorkflow->m_tWorkflow[i32Index];
        _pTaskList->m_nTasklistStatus = M_TASK_STATUS_UNRUN;

        for(size_t i = 0; i < _pTaskList->m_tTaskList.size();i++)
        {
            TaskInfo* pTaskItem = _pTaskList->m_tTaskList[i];
            pTaskItem->m_nTaskStatus = M_TASK_STATUS_UNRUN;
            pTaskItem->m_sTaskErrorInfo = "";
            pTaskItem->m_bTerimalTask = false;
            pTaskItem->m_bDefect = false;
        }
    }

    _pWorkflow->m_nWorkflowRel = M_TASK_STATUS_UNRUN;
    if(_pWorkflow->m_pCommonInfo != nullptr)
    {
        _pWorkflow->m_pCommonInfo->tTaskDatalist.clear();//清除缓存数据
    }

}

/******************************************
* 功能:强制终止所有正在运行的任务 通过任务中的m_bTerimalTask属性完成
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowProcessThread::terminateRunningTask()
{
    int nTaskCnt = 0;

    _RunTaskThreadPool.clear();//清除排队的线程

    for(size_t i32Index = 0; i32Index < _pWorkflow->m_tWorkflow.size(); i32Index++)
    {
        Tasklist* _pTaskList = _pWorkflow->m_tWorkflow[i32Index];

        for(size_t i = 0; i < _pTaskList->m_tTaskList.size();i++)
        {
            TaskInfo* pTaskItem = _pTaskList->m_tTaskList[i];
            if(nTaskCnt <= _i32CurrentTaskCnt)
            {
                if(pTaskItem->m_nTaskStatus == M_TASK_STATUS_RUNNING || pTaskItem->m_nTaskStatus == M_TASK_STATUS_UNRUN)
                {
                    pTaskItem->m_bTerimalTask = true;
                    pTaskItem->m_nTaskStatus = M_TASK_STATUS_FAILD;
                    pTaskItem->m_sTaskErrorInfo = "人为终止流程";
                }
            }
            else
            {
                return;
            }

            nTaskCnt++;

        }
    }

}

/******************************************
* 功能:执行测试流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowProcessThread::run()
{
    _i32CurrentTaskCnt = -1;
    _bManualStopTask = false;
    InitTaskListStatus();//初始化流程状态
    _pWorkflow->m_nWorkflowRel = M_TASK_STATUS_RUNNING;//初始化整个流程的状态

    for(size_t i32Index = 0; i32Index < _pWorkflow->m_tWorkflow.size(); i32Index++)
    {
        Tasklist* _pTaskList = _pWorkflow->m_tWorkflow[i32Index];
        _pTaskList->m_nTasklistStatus = M_TASK_STATUS_RUNNING;

        for(size_t i = 0; i < _pTaskList->m_tTaskList.size();i++)
        {
            TaskInfo* pTaskItem = _pTaskList->m_tTaskList[i];

            QRunTaskThread* pThread;
            vector<TaskInfo*> pRelationTaskInfolist;
            for(int i32RelationTaskIndex:pTaskItem->m_i32RelationTaskIndex)//有无关联步骤
            {
                if(i32RelationTaskIndex >=0 && i32RelationTaskIndex < static_cast<int>(_pTaskList->m_tTaskList.size()) && i32RelationTaskIndex != static_cast<int>(i))
                {
                   pRelationTaskInfolist.push_back(_pTaskList->m_tTaskList[i32RelationTaskIndex]);
                }
            }


            pThread= new QRunTaskThread(_pDevCtrlManager,pTaskItem,_pWorkflow->m_pCommonInfo,pRelationTaskInfolist);
            _RunTaskThreadPool.start(pThread);

            _i32CurrentTaskCnt++;
        }

        _pTaskList->m_nTasklistStatus = waitForTaskListRel();//上一个tasklist结束才能执行下一个tasklist
        if(_pTaskList->m_nTasklistStatus != M_TASK_STATUS_SUCCESS)
        {
            _pWorkflow->m_nWorkflowRel = M_TASK_STATUS_FAILD;//终止流程
            if(i32Index <= _pWorkflow->m_tWorkflow.size() -2 && _pWorkflow->m_tWorkflow.size() >=2)//执行异常或人为终止，则直接跳过其他步骤 执行最后一步
            {
                i32Index = _pWorkflow->m_tWorkflow.size() -2;
            }
            else
            {
                return;
            }
        }

    }

    //获取流程列表执行结果
    _pWorkflow->m_nWorkflowRel = waitForTaskListRel();
}

