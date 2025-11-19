/***************************************************************************
**                                                                        **
**QRunTask为任务执行线程，负责任务的分发    **
**                                                                        **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "runtaskthread.h"
#include <qdebug.h>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QRunTaskThread::QRunTaskThread(QDevCtrlManager* pCtrl,TaskInfo* pTaskInfo,tCommonInfo* pCommonInfo,vector<TaskInfo*> pWaitforTaskInfo)
{
    _pDevManagerCtrl = pCtrl;
    _pCommonInfo = pCommonInfo;
    m_pTaskInfo = pTaskInfo;
    m_pRelationTaskInfolist = pWaitforTaskInfo;
}

void QRunTaskThread::run()
{
    qDebug()<<m_pTaskInfo->m_sTaskName<<"begin"<<_pCommonInfo->nMeasureType;

    if(m_pRelationTaskInfolist.size() > 0)
    {

        //是否等待设置的关联步骤结束
        while(!m_pTaskInfo->m_bTerimalTask)//没有强制终止
        {
            int i32SucessTaskNum = 0;
            for(TaskInfo* pRelationTaskInfo:m_pRelationTaskInfolist)
            {
                //非通用功能 ron1 最大能量测试时 若出现损伤 后续流程不执行  直接跳过
                if((_pCommonInfo->nMeasureType == M_MEASURETYPE_ROn1 || _pCommonInfo->nMeasureType == M_MEASURE_MAX_ENERGY_CONFIRM) && pRelationTaskInfo->m_nTaskStatus == M_TASK_STATUS_SUCCESS && pRelationTaskInfo->m_bDefect == true)//Ron1
                {
                    m_pTaskInfo->m_nTaskStatus = M_TASK_RELATION_FAILED;
                    return;
                }

                if(pRelationTaskInfo->m_nTaskStatus == M_TASK_STATUS_FAILD || pRelationTaskInfo->m_nTaskStatus ==  M_TASK_RELATION_FAILED)//关联步骤有一个失败 则不执行后续操作
                {
                    m_pTaskInfo->m_nTaskStatus = M_TASK_RELATION_FAILED;
                    return;
                }

                if(pRelationTaskInfo->m_nTaskStatus == M_TASK_STATUS_SUCCESS )//只有关联步骤执行成功才执行后面的步骤
                {
                    i32SucessTaskNum++;
                }
            }
            if(i32SucessTaskNum == static_cast<int>(m_pRelationTaskInfolist.size()))
            {
                break;
            }

            QThread::msleep(1);
        }
    }

    QTime l_time;
    l_time.start();
    while(l_time.elapsed() < m_pTaskInfo->m_i32waitForTime)//任务执行前所需进行的延迟时间
    {
        if(m_pTaskInfo->m_bTerimalTask)//是否强制终止
        {
            return;
        }
        QThread::msleep(1);
    }


    qDebug()<<m_pTaskInfo->m_sTaskName<<"run"<<_pCommonInfo->nMeasureType;

   _pDevManagerCtrl->runDevCtrlTask(m_pTaskInfo,_pCommonInfo);// 执行任务

   qDebug()<<m_pTaskInfo->m_sTaskName<<"end"<<_pCommonInfo->nMeasureType;

}
