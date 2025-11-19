#include "platformenergydevctrl.h"
#include <qthread.h>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QPlatformEnergyDevCtrl::QPlatformEnergyDevCtrl(QString sIP,int nPort):QEnergyDevCtrl(sIP,nPort)
{

}

/******************************************
* 功能:检查设备状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformEnergyDevCtrl::checkDevStatus()
{
    return m_tDevParam.bDevConnect;
}

/******************************************
* 功能:根据输入的任务信息  执行任务操作
* 输入:任务配置信息
* 输出:
* 返回值:执行成功/失败
* 维护记录:
******************************************/
bool QPlatformEnergyDevCtrl::runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{

    int nCnt = 0;
    int nMaxCnt = 0;
    QTime l_time;

    pTask->m_nTaskStatus = M_TASK_STATUS_RUNNING;

    st_EnergyDevTaskInputData l_data = pTask->m_tParamData.value<st_EnergyDevTaskInputData>();

    if(l_data.nChannelNo == -1)//参数配置错误
    {
        pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
        pTask->m_sTaskErrorInfo = "通道参数配置错误";
        return false;
    }

    //超时设置
    int l_timeout = 1000;

    nMaxCnt = l_data.ncapDataCnt;

    if(l_data.ncapDataCnt > 1)//只有分光比的时候会使用
    {
        l_timeout = 12000*l_data.ncapDataCnt;
        nMaxCnt = nMaxCnt + 1;
    }


    switch(pTask->m_nAction)
    {
        case M_SETPARAM_ACTION:
        {
            int nRel = E_SENDCMD_OK;
            int nRel2 = E_SENDCMD_OK;
            int nRel1 = E_SENDCMD_OK;

            this->setChannelStopCap(l_data.nChannelNo);//先暂停
            QThread::msleep(100);

            this->m_tDevParam.channelParam[l_data.nChannelNo].bNewData = false;


            if(l_data.bInitMode == true)
            {
                nRel = this->setChannelWaveLength(l_data.nChannelNo,l_data.nWavelength);
                QThread::msleep(100);

                //设置外触发
                nRel2 = this->setChannelExTrigger(l_data.nChannelNo,true);
                QThread::msleep(100);

            }

            nRel1 = this->setChannelMeauserRange(l_data.nChannelNo,l_data.nMeasureRange);
            QThread::msleep(100);



            this->setChannelStartCap(l_data.nChannelNo);//继续采集

            if(nRel == E_SENDCMD_OK && nRel1 == E_SENDCMD_OK&& nRel2 == E_SENDCMD_OK)
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
            }
            else
            {                
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "设置参数失败";
            }

        }
        break;

        case M_PROCESS_ACTION:
        {
         //   this->setChannelStartCap(l_data.nChannelNo);


            l_time.start();
            while(l_time.elapsed() <= l_timeout)
            {
                if(pTask->m_bTerimalTask)
                {
                    return false;
                }

                if(this->m_tDevParam.channelParam[l_data.nChannelNo].bNewData)//新数据
                {
                    this->m_tDevParam.channelParam[l_data.nChannelNo].bNewData = false;//清除标志
                    //使用能量计修正系数进行能量修正
                    float lfMeasurValue =this->m_tDevParam.channelParam[l_data.nChannelNo].lfMeasureValue*l_data.lfCoeff;

                    //存储数据 增加通道编号
                    if(l_data.bSaveData == true)
                    {
//                        float lfFluxData = lfMeasurValue*0.001/pCommonInfo->lfArea;//计算通量 单位为J/cm^2
                        bool bRel = pCommonInfo->_pDBProcess->onSaveEnergyData(pCommonInfo->sExpNo,pTask->m_nDevNo,l_data.nChannelNo,l_data.nPointNo,l_data.nShotIndex,l_data.lfEnergySection,lfMeasurValue);

                        if(bRel == false)
                        {
                            pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                            pTask->m_sTaskErrorInfo = "能量数据存储数据库失败";
                            break;
                        }
                    }
                    else//不存储数据库  则保存在临时缓存中
                    {
                        tTaskData l_taskdata;
                        l_taskdata.i32DevType = pTask->m_nDevType;
                        l_taskdata.i32DevNo = pTask->m_nDevNo;
                        l_taskdata.i32ChannelNo = l_data.nChannelNo;
                        l_taskdata.i32DataIndex = nCnt;
                        l_taskdata.vardata.setValue(lfMeasurValue);

                        QVariant l_var;
                        l_var.setValue(l_taskdata);
                        if(nCnt != 0)//分光比测试时 剔除掉第一个数据
                        {
                            pCommonInfo->addTaskData(l_taskdata);

                            emit signal_devdata(l_taskdata.i32DevType,l_taskdata.i32DevNo,l_var);
                        }
                    }

                    nCnt++;


                    if(nCnt >= nMaxCnt)
                    {
                        pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
                        break;
                    }

                }
            }

          //  this->setChannelStopCap(l_data.nChannelNo);//暂停

            if(nCnt < nMaxCnt)
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "采集数据超时";
                qDebug()<<"能量计数据采集超时"<<l_time.elapsed();
            }
        }
        break;
    }


    return true;

}

