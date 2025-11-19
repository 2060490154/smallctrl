/**************************************************
*文件说明:本文件为设备所有实例的管理模块；为任务解析模块提供
*        控制实例;
*        本文件模块作为基础模块，修改时需谨慎
*创建人:李刚
*维护记录:
* 2020-01-16 创建  by lg
******************************************************/
#include "devctrlmanager.h"
#if _MSC_VER >=1600

#pragma execution_character_set("utf-8")
#endif
QDevCtrlManager::QDevCtrlManager(QObject *parent) : QObject(parent)
{


}

QDevCtrlManager::~QDevCtrlManager()
{

}

/******************************************
* 功能:注册设备控制
* 输入:nDevType--设备类型;nDevIndex--设备编号;
*     devCtrl--设备控制实例；
* 输出:_tDevCtrlManagerlist存储该设备
* 返回值:无
* 维护记录:
* 2020-01-16 创建  by lg
******************************************/
void QDevCtrlManager::registerDevInfo(int nDevType,int nDevIndex,void* devCtrl,runTaskFun devRunTaskFun,checkDevStatusFun devstatusCheckfun)
{
    _tDevCtrlManagerlist.append(st_DevCtrlInfo{nDevType,nDevIndex,devCtrl,devRunTaskFun,devstatusCheckfun});//注册设备信息
}

/******************************************
* 功能:获取设备控制实例  为其他界面提供相关的控制需求
* 输入:nDevType--设备类型 nDevIndex--设备编号
* 输出:
* 返回值:
* 维护记录:
******************************************/
void* QDevCtrlManager::getDevCtrl(int nDevType,int nDevIndex)
{
    foreach (st_DevCtrlInfo devctrlItem, _tDevCtrlManagerlist)
    {
        if(devctrlItem.m_nDevIndex == nDevIndex && devctrlItem.m_nDevType == nDevType)
        {
            return devctrlItem.m_pDevCtrl;
        }
    }

    return nullptr;
}

/******************************************
* 功能:获取执行任务函数指针
* 输入:nDevType--设备类型 nDevIndex--设备编号
* 输出:
* 返回值:
* 维护记录:
******************************************/
runTaskFun QDevCtrlManager::getDevRunTaskFun(int nDevType,int nDevIndex)
{
    runTaskFun l_runTaskFun = nullptr;

    foreach (st_DevCtrlInfo devctrlItem, _tDevCtrlManagerlist)
    {
        if(devctrlItem.m_nDevIndex == nDevIndex && devctrlItem.m_nDevType == nDevType)
        {

            l_runTaskFun = devctrlItem.devRunTaskFun;
            break;
        }
    }

    return l_runTaskFun;
}

/******************************************
* 功能:执行控制任务
* 输入:任务配置信息
* 输出:执行状态  成功/失败
* 返回值:
* 维护记录:
******************************************/
int QDevCtrlManager::runDevCtrlTask(TaskInfo* pTask,tCommonInfo* pCommonInfo)
{
    void* pDevCtrl = getDevCtrl(pTask->m_nDevType,pTask->m_nDevNo);
    runTaskFun l_runTaskFun = getDevRunTaskFun(pTask->m_nDevType,pTask->m_nDevNo);


    if(pDevCtrl == nullptr || l_runTaskFun == nullptr)
    {
        pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
        pTask->m_sTaskErrorInfo = "设备未注册";
    }
    else
    {
        l_runTaskFun(pTask,pCommonInfo);
    }



    return pTask->m_nTaskStatus;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QDevCtrlManager::getDevStatus(int nDevType,int nDevIndex)
{
    checkDevStatusFun l_checkDevStatusFun = nullptr;

    foreach (st_DevCtrlInfo devctrlItem, _tDevCtrlManagerlist)
    {
        if(devctrlItem.m_nDevIndex == nDevIndex && devctrlItem.m_nDevType == nDevType)
        {
            l_checkDevStatusFun = devctrlItem.devStatusCheckFun;
            break;
        }
    }

    if(l_checkDevStatusFun != nullptr)
    {
        return l_checkDevStatusFun();
    }

    return false;
}

