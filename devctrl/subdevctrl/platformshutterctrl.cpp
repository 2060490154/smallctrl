#include "platformshutterctrl.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QPlatformShutterCtrl::QPlatformShutterCtrl(QString sIPAddress,int nPort,QString sDevName) : QShutterDevCtrl(sIPAddress,nPort,sDevName)
{

}

/******************************************
* 功能:检查设备状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformShutterCtrl::checkDevStatus()
{
    bool bRel = this->m_tShutterDevInfo.nCurrentStatus != M_SHUTTER_STATUS_NONE?true:false;

    return bRel;
}

/******************************************
* 功能:根据输入的任务信息  执行任务操作
* 输入:任务配置信息
* 输出:
* 返回值:执行成功/失败
* 维护记录:
******************************************/
bool QPlatformShutterCtrl::runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{
    this->getDevStatus();

    pTask->m_nTaskStatus = M_TASK_STATUS_RUNNING;
    bool bRel = false;
    switch(pTask->m_nAction)
    {
        case M_SETPARAM_ACTION:
        {
           pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
        }
        break;

        case M_PROCESS_ACTION://打开、关闭动作
        {
            st_shutterparam l_st_shutterparam = pTask->m_tParamData.value<st_shutterparam>();

            if(l_st_shutterparam.i32Mode == 0x01)
            {
                bool bOpen = l_st_shutterparam.i32Param == 0?false:true;
                if(this->SetDevStatus(bOpen))
                {
                     pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
                }
                else
                {
                     pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                     pTask->m_sTaskErrorInfo = "光闸打开/关闭失败";
                }
            }

            if(l_st_shutterparam.i32Mode == 0x02)
            {
                if(this->setProcessStatus(l_st_shutterparam.i32Param,0))
                {
                     pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
                }
                else
                {
                    pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                    pTask->m_sTaskErrorInfo = "光闸设置流程失败";
                }
            }




        }
        break;
    }





    return bRel;

}
