#include "platformoscictrl.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QPlatformOsciCtrl::QPlatformOsciCtrl(QString sIP, QList<tOsciiChannel> vaildChannellist):QOsciDevCtrl(sIP, vaildChannellist)
{

}

/******************************************
* 功能:检查设备状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformOsciCtrl::checkDevStatus()
{
    return this->getDevStatus();
}

/******************************************
* 功能:根据输入的任务信息  执行任务操作
* 输入:任务配置信息
* 输出:
* 返回值:执行成功/失败
* 维护记录:
******************************************/
bool QPlatformOsciCtrl::runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{
    pTask->m_nTaskStatus = M_TASK_STATUS_RUNNING;
    bool bRel = false;
    switch(pTask->m_nAction)
    {
        case M_SETPARAM_ACTION:
        {
            if(!this->openDev())
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "打开示波器失败";
                return false;
            }

//            if(!this->setTriggerChannel(0x01))
//            {
//                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
//                pTask->m_sTaskErrorInfo = "设置触发通道失败";
//                return false;
//            }

            if(!this->setTriggerMode(0x01))
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "设置触发模式失败";
                return false;
            }

            pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
        }
        break;

        case M_PROCESS_ACTION:
        {
            if(!this->openDev())
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "打开示波器失败";
                return false;
            }

            float lfsum = 0.0;
            int nCnt = 0;
            QTime l_time;
            l_time.start();
            while(l_time.elapsed() < 15000)
            {
                float lfpluswidth = this->getMeasureParam(0x01);
                if(lfpluswidth != 0.0)
                {
                    lfsum = lfpluswidth + lfsum;
                    nCnt++;
                }

                if(nCnt >=10)
                {
                    lfsum = lfsum/nCnt;
                    lfsum = lfsum*1e9;//ns
                    pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
                    emit this->signal_devdata(pTask->m_nDevType,pTask->m_nDevNo,QVariant(lfsum));

                    //缓存数据
                    tTaskData l_taskdata;
                    l_taskdata.i32DevType = M_DEV_OSCII;
                    l_taskdata.i32DevNo = 0x0;
                    l_taskdata.vardata = QVariant(lfsum);
                    pCommonInfo->tTaskDatalist.append(l_taskdata);//存储分光比参数
                    return true;
                }
                QThread::msleep(1000);
            }


            if(l_time.elapsed() > 15000)
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "采集超时";
            }

        }
        break;
    }



    return bRel;

}
