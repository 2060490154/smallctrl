/**************************************************
*文件说明:本文件为kxccd控制定制功能，继承与kxccd控制类
*创建人:李刚
*维护记录:
*2020-01-16 创建  by lg
******************************************************/
#include "platformkxccdctrl.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QPlatformKxccdCtrl::QPlatformKxccdCtrl(int nIndex,QString sDevName,QObject *parent):CKxccdCtrl(nIndex,sDevName,parent)
{

}

/******************************************
* 功能:检查设备状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformKxccdCtrl::checkDevStatus()
{
    return this->isDevOpen();
}

/******************************************
* 功能:根据输入的任务信息  执行任务操作
* 输入:任务配置信息
* 输出:
* 返回值:执行成功/失败
* 维护记录:
******************************************/
bool QPlatformKxccdCtrl::runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{
    pTask->m_nTaskStatus = M_TASK_STATUS_RUNNING;
    bool bRel = false;

    switch(pTask->m_nAction)
    {
        case M_SETPARAM_ACTION:
        {
            int l_i32workmode = pTask->m_tParamData.value<int>();
            if(!this->openDev())
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "设备连接失败";
            }
            this->setDevParam(1,6);//gain

            if(this->setWorkMode(l_i32workmode))
            {
                m_tRcvKxccdData.m_bImageReCapOK = false;
                pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
            }
            else
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "设置参数失败";
            }

            QThread::msleep(100);

            if(this->startCap())
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
            }
            else
            {
                pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                pTask->m_sTaskErrorInfo = "开始采集失败";
            }
        }
        break;

        case M_PROCESS_ACTION://采集操作  0x02--面积测量 0x01-正常采集
        {
            st_ccdinputdata l_st_ccdinputdata = pTask->m_tParamData.value<st_ccdinputdata>();

            if(l_st_ccdinputdata.nActionType == 0x02)//面积测量
            {
                m_tRcvKxccdData.m_bImageReCapOK = false;//清除状态

                QThread::sleep(3);

                int nCnt = 0;

                QTime l_time;
                l_time.start();

                while(l_time.elapsed() < M_MAX_WAIT_TIME_CCD)
                {
                    if(pTask->m_bTerimalTask)
                    {
                        return false;
                    }


                    if(m_tRcvKxccdData.m_bImageReCapOK == true)
                    {

                        st_ccdimage l_st_ccdimage;
                        QString sfilePath = QString("./tempdata/%1_%2.bmp").arg(pCommonInfo->sExpNo).arg(nCnt);

                        _MutexLock.lock();//锁存数据
                        m_higImg.saveBmpImage(sfilePath);//保存图片
                        m_tRcvKxccdData.m_bImageReCapOK = false;
                        _MutexLock.unlock();

                        //保存至数据库中
                        QFile l_file(sfilePath);
                        l_file.open(QFile::ReadOnly);
                        QByteArray l_data = l_file.readAll();
                        l_file.close();

                        pCommonInfo->_pDBProcess->onSaveCCDData(pCommonInfo->sExpNo,pCommonInfo->nwavelengthType,l_data);

                        //显示数据
                        QVariant var;
                        l_st_ccdimage.sImageFilePath = QString("./tempdata/%1_%2.bmp").arg(pCommonInfo->sExpNo).arg(nCnt);
                        l_st_ccdimage.i32ImageIndex = nCnt;
                        var.setValue(l_st_ccdimage);
                        emit this->signal_devdata(pTask->m_nDevType,pTask->m_nDevNo,var);

                        nCnt++;
                    }
                    if(nCnt >= l_st_ccdinputdata.nImageCnt)
                    {
                        break;
                    }

                    QThread::msleep(10);
                }

                if(l_time.elapsed() >= M_MAX_WAIT_TIME_CCD && nCnt < l_st_ccdinputdata.nImageCnt)
                {
                    pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
                    pTask->m_sTaskErrorInfo = "数据采集超时";
                }
                else
                {
                    pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
                }

            }

        }
        break;
    }


    return bRel;

}


