#include "taskinfo.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

/******************************************
* 功能:构造任务信息
* 输入:
* sName    任务名称
* nDevType 设备类型
* nDevNo   设备编号
* nAction  执行动作
* paramdata 执行参数
* i32RelationTaskIndex  等待之前的某一步结束                设置后 后续的步骤不延迟，等待index task结束--关联任务 目前支持同时等待两个任务结束 后续扩展
* i32waitForTime     本任务执行时的延迟时间  单位为ms       设置后 该值不会导致后续的任务延时
* 输出:
* 返回值:
* 维护记录:
******************************************/
TaskInfo::TaskInfo(QString sName,int nDevType,int nDevNo,int nAction,QVariant paramdata,int i32waitForTime,initializer_list<int> relationTaskIndexlist)
{
    m_nTaskStatus = M_TASK_STATUS_UNRUN;
    m_sTaskName = sName;
    m_nDevType = nDevType;
    m_nDevNo = nDevNo;
    m_nAction = nAction;
    m_tParamData = paramdata;

    m_i32waitForTime = i32waitForTime;
    m_sTaskErrorInfo="";
    m_bTerimalTask = false;

    m_bDefect = false;

    m_i32RelationTaskIndex.clear();

    for(int index:relationTaskIndexlist)
    {
        if(index >= 0 )
        {
            m_i32RelationTaskIndex.push_back(index);
        }

    }

}

/******************************************
* 功能:获取状态描述
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QString TaskInfo::getTaskStatusText()
{
    QString sRel = "未知状态";
    switch(m_nTaskStatus)
    {
        case M_TASK_STATUS_UNRUN:
            sRel = "未执行";
        break;
        case M_TASK_STATUS_RUNNING:
            sRel = "正在执行";
        break;
        case M_TASK_STATUS_FAILD:
        {
            if(m_sTaskErrorInfo != "")
            {
                sRel = m_sTaskErrorInfo;
            }
            else
            {
                sRel = "任务失败";
            }
        }
        break;
        case M_TASK_STATUS_SUCCESS:
        {
            if(!m_bDefect)
            {
               sRel = "执行成功";
            }
            else
            {
                sRel = "有损伤";
            }
        }
        break;
        case M_TASK_RELATION_FAILED:
            sRel = "关联任务失败，本任务未执行";
        break;
        default:
             sRel = "未知状态";
        break;
    }

    return sRel;

}
/******************************************
* 功能:获取当前状态的颜色
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QColor TaskInfo::getTaskStatusColor()
{
    QColor l_Rel = QColor(0,0,0);
    switch(m_nTaskStatus)
    {
        case M_TASK_STATUS_UNRUN:
            l_Rel = QColor(150,150,150);
        break;
        case M_TASK_STATUS_RUNNING:
            l_Rel = QColor(0,0,155);
        break;
        case M_TASK_STATUS_FAILD:
            l_Rel = QColor(255,0,0);
        break;
        case M_TASK_STATUS_SUCCESS:
            if(!m_bDefect)
            {
                l_Rel = QColor(0,255,0);
            }
            else
            {
                l_Rel = QColor(255,0,0);
            }
        break;
        case M_TASK_RELATION_FAILED:
            l_Rel = QColor(150,150,150);
        break;
    }

    return l_Rel;

}

