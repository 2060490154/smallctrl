#include "platformenergyadjdevctrl.h"
#include <qthread.h>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QPlatformEnergyAdjDevCtrl::QPlatformEnergyAdjDevCtrl(QString sIP,int nPort):QEnergyAdjustDevCtrl(sIP,nPort)
{

}

/******************************************
* 功能:检查设备状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformEnergyAdjDevCtrl::checkDevStatus()
{
    return _pTcpServer->m_bConnect;
}

/******************************************
* 功能:根据输入的任务信息  执行任务操作
* 输入:任务配置信息
* 输出:
* 返回值:执行成功/失败
* 维护记录:
******************************************/
bool QPlatformEnergyAdjDevCtrl::runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{

    pTask->m_nTaskStatus = M_TASK_STATUS_RUNNING;

    float lfValue = pTask->m_tParamData.value<float>();//当前需要调节到的透过率数据


    switch(pTask->m_nAction)
    {
        case M_SETPARAM_ACTION:
        {
            if(this->moveToHome())
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
            }
            else
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = m_tEnergyAdjDevStatus.sErroInfo;
            }
        }
        break;

        case M_PROCESS_ACTION:
        {
            if(this->MoveToPrecent(lfValue))
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
            }
            else
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = m_tEnergyAdjDevStatus.sErroInfo;
            }
        }
        break;
    }


    return true;

}

