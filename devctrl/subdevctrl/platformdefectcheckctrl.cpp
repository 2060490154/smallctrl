/***************************************************************************
**       与损伤检测系统相关的通信                                             **
**
**                                                                        **
****************************************************************************
**创建人：
** 创建时间：
** 修改记录：
**
****************************************************************************/
#include "platformdefectcheckctrl.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QPlatformDefectCheckCtrl::QPlatformDefectCheckCtrl(QString sIPAddress,int nPort):QDefectCheckSysCtrl(sIPAddress,nPort)
{

}

/******************************************
* 功能:检查设备状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformDefectCheckCtrl::checkDevStatus()
{
    return true;
}

/******************************************
* 功能:根据输入的任务信息  执行任务操作
* 输入:任务配置信息
* 输出:
* 返回值:执行成功/失败
* 维护记录:
******************************************/
bool QPlatformDefectCheckCtrl::runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{

    pTask->m_nTaskStatus = M_TASK_STATUS_RUNNING;

    st_defectcheckInputdata l_data = pTask->m_tParamData.value<st_defectcheckInputdata>();
    bool bRel = false;

    switch(l_data.nParamType)
    {
        case E_PARAM_PUBLISH://发布实验信息
        {
            bRel = publishExpInfo(pCommonInfo->sExpNo,pCommonInfo->nMeasureType);
        }
        break;

        case E_PARAM_PREPARE://准备
        {
            bRel = prepareDefectSys(l_data.nPointNo,l_data.nShotNo);
        }
        break;

        case E_PARAM_CAPIMAGE://查询图像采集状态
        {
            bRel = queryCapResult();

//            QImage l_image;
//            if(pCommonInfo->_pDBProcess->getDefectCheckImage(pCommonInfo->sExpNo,l_data.nPointNo,l_data.nShotNo,0x0,l_image))
//            {
//                emit signal_showdefectimage(pCommonInfo->nMeasureType,0x0,l_image);
//            }

        }
        break;

        case E_PARAM_DEFECT_CLOSE://结束
        {
            bRel = closeDefectsys();
        }
        break;

        case E_PARAM_DEFECT_REL://查询检测结果
        {
            int nRel = queryDefectResult(l_data.nPointNo,l_data.nShotNo);
            if(nRel == E_SYSTEM_NODEFECT || nRel == E_SYSTEM_DEFECT)//不存在损伤 存在损伤
            {
                pTask->m_bDefect = nRel == E_SYSTEM_NODEFECT?false:true;
                bRel = true;

                //获取打靶后的图像 用于显示
//                QImage l_image;
//                if(pCommonInfo->_pDBProcess->getDefectCheckImage(pCommonInfo->sExpNo,l_data.nPointNo,l_data.nShotNo,0x1,l_image))
//                {
//                    emit signal_showdefectimage(pCommonInfo->nMeasureType,0x1,l_image);
//                }

                //存储检测结果
                if(!pCommonInfo->_pDBProcess->onSaveDefectInfo(pCommonInfo->sExpNo,l_data.nPointNo,l_data.nShotNo,l_data.lfEnergySection,pTask->m_bDefect))
                {
                    bRel = false;
                    m_sErrInfo = "存储检测结果失败";
                }

            }
            else if(nRel == E_SYSTEM_DEFECT_CHECK_FAILED || nRel == E_SYSTEM_DEFECT_UNKNOWN)
            {
                bRel = false;
            }
        }
        break;
    }

    if(bRel)
    {
        pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
    }
    else
    {
        pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
        pTask->m_sTaskErrorInfo = m_sErrInfo;
    }



    return true;

}
