#include "platformmotordevctrl.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QPlatformMotorDevCtrl::QPlatformMotorDevCtrl():QMotorDevCtrl()
{

}
/******************************************
* 功能:检查设备状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformMotorDevCtrl::checkDevStatus()
{
    return this->m_bConnect;
}

/******************************************
* 功能:根据输入的任务信息  执行任务操作
* 输入:任务配置信息
* 输出:
* 返回值:执行成功/失败
* 维护记录:
******************************************/
bool QPlatformMotorDevCtrl::runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{
    st_motorInputdata l_motorInput_data = pTask->m_tParamData.value<st_motorInputdata>();

    pTask->m_nTaskStatus = M_TASK_STATUS_RUNNING;
    bool bRel = false;

    if(l_motorInput_data.nPlusCnt < 0 && l_motorInput_data.nMoveType == 0x01)
    {
        pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
        pTask->m_sTaskErrorInfo = "参数错误，档位信息不能为负";
        return false;
    }


    switch(pTask->m_nAction)
    {
        case M_SETPARAM_ACTION:
        {

        }
        break;

        case M_PROCESS_ACTION:
        {
            bool bRel = false;
            if(l_motorInput_data.nMoveType == 0x01)//档位
            {
                bRel = this->moveToGear(l_motorInput_data.nMotorIndex,l_motorInput_data.nPlusCnt);
            }
            else if(l_motorInput_data.nMoveType == 0x02)//相对运动
            {
                bRel = this->moveToRelationPos(l_motorInput_data.nMotorIndex,l_motorInput_data.nPlusCnt);
            }
            else if(l_motorInput_data.nMoveType == 0x03)//绝对运动
            {
                bRel = this->moveToAbsolutePos(l_motorInput_data.nMotorIndex,l_motorInput_data.nPlusCnt);
            }

            if(bRel == true)
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
            }
            else
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = m_tMotorStatus[l_motorInput_data.nMotorIndex].sErroInfo;
            }
        }
        break;
    }

   // qDebug()<<"motor dev task finished";
    return bRel;

}
