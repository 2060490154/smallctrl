#include "energydevctrl.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif
QEnergyDevCtrl::QEnergyDevCtrl(QString sComName,int nPort)
{
    m_bCapData = false;
    //初始化参数
    m_tDevParam.sDevIP = sComName;//串口地址
    for(int i = 0; i < 2; i++)
    {
        m_tDevParam.channelParam[i].i32CurrentMeasureMode = E_MODE_NO_DETECTOR;
        m_tDevParam.channelParam[i].lfTriggerLevel = 0.0;
        m_tDevParam.channelParam[i].lfMeasureValue = 0.0;
        m_tDevParam.channelParam[i].i32CurrentRange = 0;
        m_tDevParam.channelParam[i].i32CurrentWaveLength = 0;
        m_tDevParam.channelParam[i].bConnected = false;
    }



    _pSerialPort = new QSerialPort();
    _pSerialPort->setPortName(sComName);
    _pSerialPort->setBaudRate(QSerialPort::Baud9600);
    _pSerialPort->setDataBits(QSerialPort::Data8);
    _pSerialPort->setParity(QSerialPort::NoParity);
    _pSerialPort->setStopBits(QSerialPort::OneStop);
    _pSerialPort->setFlowControl(QSerialPort::NoFlowControl);

    _capDataTimer = new QTimer(this);
    _capDataTimer->setInterval(100);

    connect(_capDataTimer,&QTimer::timeout,this,&QEnergyDevCtrl::onCapMeasureData);

    _capDataTimer->start();

}

QEnergyDevCtrl::~QEnergyDevCtrl()
{
    setChannelStopCap(0x0);
    setChannelStopCap(0x01);

    if(_pSerialPort->isOpen())//已经连接
    {
        _pSerialPort->clear();
        _pSerialPort->close();
    }

    delete _pSerialPort;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::openDev()
{
    m_tDevParam.bDevConnect = _pSerialPort->isOpen();

    if(!m_tDevParam.bDevConnect)//未连接
    {
        if(!_pSerialPort->open(QIODevice::ReadWrite))
        {
            return E_CONNECT_ERROR;
        }

        m_tDevParam.bDevConnect = true;
        _pSerialPort->clear();//情况数据
    }

    return E_SENDCMD_OK;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrl:: closeDev()
{
    if(_pSerialPort->isOpen())//已经连接
    {
        _pSerialPort->close();
        m_tDevParam.bDevConnect = false;
    }

}

/******************************************
* 功能:发送数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl:: sendCmdData(QString sCmd)
{
    QByteArray l_data;
    l_data.append(sCmd);

    _EnergyDevlock.lock();

    if (this->openDev() == E_CONNECT_ERROR)//打开失败
    {
        _EnergyDevlock.unlock();
         return E_CONNECT_ERROR;
    }

    _pSerialPort->clear();

    int rel = _pSerialPort->write(l_data.data(),l_data.size());
    bool bRel = _pSerialPort->waitForBytesWritten(100);

    _EnergyDevlock.unlock();
    if (rel != l_data.size() || bRel == false)
    {
        return E_SENDCMD_DATA_FAILED;//发送数据失败
    }

    return E_SENDCMD_OK;
}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::getDevParam(int nChannelNo)
{
    if(!this->_pSerialPort->isOpen())
    {
        return E_CONNECT_ERROR;
    }

    QString sCmd = QString("*ST%1").arg(nChannelNo+1);

    int nRel = sendCmdData(sCmd);//当前状态命令
    if(nRel != E_SENDCMD_OK)
    {
        return nRel;
    }

    QByteArray l_RcvData;
    while(_pSerialPort->waitForReadyRead(100))
    {
        QByteArray l_temp =  _pSerialPort->readAll();
        l_RcvData.push_back(l_temp);
    }

    if(l_RcvData.size() != 108)//接收的数据有问题
    {
        //qDebug()<<"***********"<<l_RcvData.size();
        return E_RECV_DATA_TIMEOUT;
    }

    praseDevParam(l_RcvData,nChannelNo);

    return E_SENDCMD_OK;

}

/******************************************
* 功能:解析通道参数信息
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrl::praseDevParam(QByteArray data,int nChannelNo)
{
    bool bOk = false;
    QString sRcvData(data);

    sRcvData = sRcvData.replace("\r\n","");

    QStringList sParamData = sRcvData.split(':');

    for(QString sTempdata:sParamData)
    {
        if(sTempdata.size() == 9)
        {
           int nHeaderIndex = sTempdata.left(5).toInt();//头信息
           QString str = sTempdata.right(4);
           int nValue = str.toInt(&bOk,16);

           if(nHeaderIndex == 0x00)//波长
           {
                m_tDevParam.channelParam[nChannelNo].i32CurrentWaveLength = nValue;
           }

           if(nHeaderIndex == 0x01)//测量范围
           {
                m_tDevParam.channelParam[nChannelNo].i32CurrentRange = nValue;
           }

           if(nHeaderIndex == 0x02)//触发电平 百分比
           {
                m_tDevParam.channelParam[nChannelNo].lfTriggerLevel = (float)nValue/20.48;
           }

           if(nHeaderIndex == 0x05)//外触发模式
           {
                m_tDevParam.channelParam[nChannelNo].i32ExTriggerMode = nValue;
           }

        }
    }
}

/******************************************
* 功能:获取当前探头是否连接
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::getDevConnectStatus(int nChannelNo)
{
    if(!this->_pSerialPort->isOpen())
    {
        return E_CONNECT_ERROR;
    }

    QString sCmd = QString("*SN%1").arg(nChannelNo+1);

    int nRel = sendCmdData(sCmd);//当前状态命令
    if(nRel != E_SENDCMD_OK)
    {
        return nRel;
    }

    QByteArray l_RcvData;
    while(_pSerialPort->waitForReadyRead(100))
    {
        QByteArray l_temp =  _pSerialPort->readAll();
        l_RcvData.push_back(l_temp);
    }

    QString sRcvData(l_RcvData);
    m_tDevParam.channelParam[nChannelNo].bConnected = true;

    if(sRcvData.size() == 0)
    {
        m_tDevParam.channelParam[nChannelNo].bConnected = false;
    }

    return E_SENDCMD_OK;

}

/******************************************
* 功能:设置外触发模式
* 输入:bEnable=true为设置成外触发模式
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::setChannelExTrigger(int nChannelNo,bool bEnable)
{
    if(!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    int nEnable = bEnable==true?1:0;

    QString sCmd = QString("*ET%1%2").arg(nChannelNo+1).arg(nEnable);

    return sendCmdData(sCmd);
}

/******************************************
* 功能:设置触发门限
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::setChannelTriggerLevel(int nChannelNo,float lfTriggerLevel)
{
    if(!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    QString sCmd = QString("*TL%1%2").arg(nChannelNo+1).arg(lfTriggerLevel,4,'g',2,QChar('0'));

    int nRel = sendCmdData(sCmd);
    return nRel;

}
/******************************************
* 功能:设置测量范围
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::setChannelMeauserRange(int nChannelNo,int nRangeIndex)
{

    if(!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }
    QString sCmd = QString("*SC%1%2").arg(nChannelNo+1).arg(nRangeIndex,2,10,QChar('0'));
    int nRel = sendCmdData(sCmd);

    return nRel;

}
/******************************************
* 功能:设置测量波长
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::setChannelWaveLength(int nChannelNo,int nWaveLength)
{
    if(!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    QString sCmd = QString("*PW%1%2").arg(nChannelNo+1).arg(nWaveLength,5,10,QChar('0'));
    int nRel = sendCmdData(sCmd);

    return nRel;

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::setChannelStartCap(int nChannelNo)
{

    if(!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    m_bCapData = false;

    QString sCmd = QString("*SS%1").arg(nChannelNo+1)+"001";//设置AScii码模式
    int nRel = sendCmdData(sCmd);

    sCmd = QString("*CA%1").arg(nChannelNo+1);//获取测试数据
    nRel = sendCmdData(sCmd);

    m_bCapData = nRel == E_SENDCMD_OK?true:false;

    _pSerialPort->clear();

   // m_tDevParam.channelParam[nChannelNo].bNewData = false;//tobe

    m_bCapData = true;


    return nRel;
}

/******************************************
* 功能:暂停数据采集
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyDevCtrl::setChannelStopCap(int nChannelNo)
{

    if(!m_tDevParam.channelParam[nChannelNo].bConnected)
    {
        return E_CONNECT_ERROR;
    }

    m_bCapData = false;

    QString sCmd = QString("*CS%1").arg(nChannelNo+1);//停止连续采集
    int nRel = sendCmdData(sCmd);

    return nRel;
}

/******************************************
* 功能:采集数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrl::onCapMeasureData()
{
    if(m_bCapData == false)
    {
        return;
    }

    _EnergyDevlock.lock();

    //可以采集
    QByteArray rcvData;
    while( _pSerialPort->bytesAvailable() > 0 || _pSerialPort->waitForReadyRead(50))
    {
         QByteArray l_RcvData =  _pSerialPort->readAll();
         rcvData.push_back(l_RcvData);
    }
    _EnergyDevlock.unlock();

    if(rcvData.size() > 0)
    {
        praseMeasureData(rcvData);
    }


}

/******************************************
* 功能:解析测量值数据 1：xxx 2:xxx
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrl::praseMeasureData(QByteArray rcvdata)
{
    QString sRcvData(rcvdata);

    QStringList l_data = sRcvData.split("\r\n");

    for(QString channeldata:l_data)
    {
        if(channeldata.isEmpty())
        {
            continue;
        }
        QStringList l_newdata = channeldata.split(":");;
        if(l_newdata.size() != 2)
        {
            continue;
        }

        int l_nChannelNo = l_newdata[0].toInt();
        float l_lfMeasuredata = l_newdata[1].toFloat()*1000;//转换成mJ
        if(l_nChannelNo == 2 || l_nChannelNo==1 )
        {
            m_tDevParam.channelParam[l_nChannelNo-1].lfMeasureValue = l_lfMeasuredata;
            m_tDevParam.channelParam[l_nChannelNo-1].bNewData = true;
        }

        //qDebug()<<"energy:"<<l_nChannelNo<<l_lfMeasuredata;
    }

}

/******************************************
* 功能:延迟时间  单位为ms
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrl::delayTime(int mesc)
{
    QTime l_time;
    l_time.start();
    while(l_time.elapsed() < mesc)
    {

    }
}




