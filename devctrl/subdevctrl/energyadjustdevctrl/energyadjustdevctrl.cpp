/***************************************************************************
**                                                                        **
**  本文件为能量调节模块的控制程序 232通信
** 设备型号为eksma-990-0075
**                                                                        **
****************************************************************************
**创建人：李刚
** 创建时间：2020-04-10
** 修改记录：
**
****************************************************************************/
#include "energyadjustdevctrl.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif


QEnergyAdjustDevCtrl::QEnergyAdjustDevCtrl(QString sComAddress,int nPort)
{
    _bManualStop = false;

    m_bConnect = false;

    _current_time.start();

    m_tEnergyAdjDevStatus.lfcurrentpresent = 0;

    InitErrorInfo();

    _pTcpServer = new QCustomTcpServer(sComAddress,nPort);

    connect(this,&QEnergyAdjustDevCtrl::signal_sendcmd,_pTcpServer,&QCustomTcpServer::onSendCmd);
  //  connect(_pTcpServer,&QCustomTcpServer::connectdev,this,&QEnergyAdjustDevCtrl::onDevConnect);


}

QEnergyAdjustDevCtrl::~QEnergyAdjustDevCtrl()
{
    QString sResponsevalue;
    _sendCmd("LPA>OFF!",sResponsevalue);

    delete _pTcpServer;



//    if(_serialPort.isOpen())
//    {
//        QString sResponsevalue;
//        _sendCmd("LPA>OFF!\r",sResponsevalue);
//        _serialPort.close();
//    }
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyAdjustDevCtrl::onDevConnect()
{
    m_bConnect = true;
//    _getCurrentStatus();

//    //只更新位置和限位
//    QVariant l_var;
//    l_var.setValue(m_tEnergyAdjDevStatus);
//    emit signal_updateEnergyAdjDevStatus(l_var,false);
}

/******************************************
* 功能:运动到相对位置
* 输入:lfPresent为透过率 45.5% lfpercent = 45.5
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QEnergyAdjustDevCtrl::MoveToPrecent(float lfPercent)
{

    //开始运动 发送当前状态
    _bManualStop = false;
    m_tEnergyAdjDevStatus.sErroInfo = "";
    m_tEnergyAdjDevStatus.nStatus = 0x01;
    _sendEnergyAdjDevStatus();


    QString sCmd = QString("LPA>PWR!_%1").arg(lfPercent);
    QString sResponsevalue="";
    _sendCmd(sCmd,sResponsevalue);//发送运动命令
    if(sResponsevalue.isEmpty())
    {
        m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_SENDCMD_DATA_FAILED);
        m_tEnergyAdjDevStatus.nStatus = 0x03;
        _sendEnergyAdjDevStatus();
        return false;

    }



    QTime l_time;
    l_time.start();
    while(l_time.elapsed() < M_MAX_WAITTIME)
    {
        if(_bManualStop)
        {
            m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_MOVE_UNREACH);
            m_tEnergyAdjDevStatus.nStatus = 0x03;
            _sendEnergyAdjDevStatus();
            return false;
        }

        if(!_getCurrentStatus())
        {
            continue;
        }
        if(_EnergyDev_devstatus.devStatus.targetPositionReached == 1)//是否到达目标位置
        {
            _getCurrentStatus();
            if(fabsf( m_tEnergyAdjDevStatus.lfcurrentpresent - lfPercent)>0.1 )
            {
                m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_MOVE_UNREACH);
                m_tEnergyAdjDevStatus.nStatus = 0x03;
                _sendEnergyAdjDevStatus();
                return false;

            }
            else
            {
                //运动结束
                m_tEnergyAdjDevStatus.sErroInfo = "";
                m_tEnergyAdjDevStatus.nStatus = 0x02;
                _sendEnergyAdjDevStatus();
                return true;
            }

        }

        _sendEnergyAdjDevStatus();
    }

    if(l_time.elapsed() >= M_MAX_WAITTIME)
    {
        m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_TIMEOUT);
        m_tEnergyAdjDevStatus.nStatus = 0x03;
        _sendEnergyAdjDevStatus();
        return false;
    }

    return false;
}

/******************************************
* 功能:归零运动
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QEnergyAdjustDevCtrl::moveToHome()
{
    //开始运动 发送当前状态
    _bManualStop = false;
    m_tEnergyAdjDevStatus.sErroInfo = "";
    m_tEnergyAdjDevStatus.nStatus = 0x01;
    _sendEnergyAdjDevStatus();


    QString sCmd = "LPA>HOME!";//发送归零运动
    QString sResponsevalue="";

    if(_sendCmd(sCmd,sResponsevalue) != E_ENRGYAD_SENDCMD_OK)
    {
        m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_SENDCMD_DATA_FAILED);
        m_tEnergyAdjDevStatus.nStatus = 0x03;
        _sendEnergyAdjDevStatus();
        return false;
    }

    if(sResponsevalue != "LPA>HOME")
    {
        m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_SENDCMD_DATA_FAILED);
        m_tEnergyAdjDevStatus.nStatus = 0x03;
        _sendEnergyAdjDevStatus();
        return false;
    }

    QTime l_time;
    l_time.start();
    while(l_time.elapsed() < M_MAX_WAITTIME)
    {
        if(_bManualStop)
        {
            m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_MOVE_UNREACH);
            m_tEnergyAdjDevStatus.nStatus = 0x03;
            _sendEnergyAdjDevStatus();
            return false;
        }

        if(!_getCurrentStatus())
        {
            continue;
        }
        if(_EnergyDev_devstatus.devStatus.targetPositionReached == 1 && _EnergyDev_devstatus.devStatus.HomingAfterReset == 0x01)//是否到达目标位置
        {
            //运动结束
            m_tEnergyAdjDevStatus.sErroInfo = "";
            m_tEnergyAdjDevStatus.nStatus = 0x02;
            _sendEnergyAdjDevStatus();
            return true;
        }

        _sendEnergyAdjDevStatus();

    }

    if(l_time.elapsed() >= M_MAX_WAITTIME)
    {
        m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_TIMEOUT);
        m_tEnergyAdjDevStatus.nStatus = 0x03;
        _sendEnergyAdjDevStatus();
        return false;
    }

    return false;
}

/******************************************
* 功能:停止运动
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QEnergyAdjustDevCtrl::stopMove()
{
    QString sCmd = "LPA>STP!";
    QString sResponsevalue="";

    _bManualStop = true;

    m_tEnergyAdjDevStatus.sErroInfo = "";
    m_tEnergyAdjDevStatus.nStatus = 0x01;
    _sendEnergyAdjDevStatus();


    _sendCmd(sCmd,sResponsevalue);//发送停止命令
    if(sResponsevalue != "LPA>STP")
    {
        m_tEnergyAdjDevStatus.sErroInfo = _getErrorCodeInfo(E_ENRGYAD_SENDCMD_DATA_FAILED);
        m_tEnergyAdjDevStatus.nStatus = 0x03;
        _sendEnergyAdjDevStatus();
        return false;
    }


    m_tEnergyAdjDevStatus.nStatus = 0x02;
    _sendEnergyAdjDevStatus();

    return true;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QEnergyAdjustDevCtrl::setMinPowerPos()
{
    //开始运动 发送当前状态
    m_tEnergyAdjDevStatus.sErroInfo = "";
    m_tEnergyAdjDevStatus.nStatus = 0x01;
    _sendEnergyAdjDevStatus();


    QString sCmd = "LPA>DEF!";
    QString sResponsevalue="";

    _sendCmd(sCmd,sResponsevalue);//设置当前位置为最小能量位置
    if(sResponsevalue != "LPA>DEF")
    {
        return false;
    }

    return true;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QEnergyAdjustDevCtrl::checkMotorRunning()
{
    if(m_tEnergyAdjDevStatus.nStatus == 0x01)
    {
        return true;
    }

    return false;
}


/******************************************
* 功能:发送数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QEnergyAdjustDevCtrl:: _sendCmd(QString sCmd,QString& sResponseValue)
{

    _pTcpServer->cleardata();
    emit signal_sendcmd(sCmd);

    _current_time.restart();
    while(_current_time.elapsed() < 1000)
    {
        sResponseValue = _pTcpServer->getRcvdata();

        if(!sResponseValue.isEmpty())
        {
            return E_ENRGYAD_SENDCMD_OK;
        }

        QThread::msleep(10);
    }

    return E_ENRGYAD_TIMEOUT;
}
/******************************************
* 功能:获取设备当前的状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QEnergyAdjustDevCtrl::_getCurrentStatus()
{
    QString sCmd = "LPA>STATUS?\r";
    QString sResponsevalue = "";

    if(_sendCmd(sCmd,sResponsevalue) != E_ENRGYAD_SENDCMD_OK)
    {
        return false;
    }

    QStringList l_templist = sResponsevalue.split('>');//LPA>X_Y
    if(l_templist.size() != 2)
    {
        return false;
    }
    QStringList l_valuelist = l_templist[1].split('_');//X_Y
    if(l_valuelist.size() != 2)
    {
        return false;
    }

    _bMotorEnable = l_valuelist[0].toInt()==0?false:true;// 是否使能
    _EnergyDev_devstatus.i16status = l_valuelist[1].toUShort();//设备当前状态

    //限位状态
    m_tEnergyAdjDevStatus.isLeftLimit = _EnergyDev_devstatus.devStatus.leftlimit;
    m_tEnergyAdjDevStatus.isRightLimit = _EnergyDev_devstatus.devStatus.rightlimit;


    sCmd = "LPA>PWR?\r";//查询当前位置 百分比
    if(_sendCmd(sCmd,sResponsevalue) != E_ENRGYAD_SENDCMD_OK)
    {
        return false;
    }

    if(sResponsevalue.contains("PWR"))
    {
        l_templist = sResponsevalue.split('_');//LPA>PWR_X.XXX
        if(l_templist.size() != 2)
        {
            return false;
        }

        m_tEnergyAdjDevStatus.lfcurrentpresent = l_templist[1].toFloat();//当前位置
    }



//    qDebug()<<_EnergyDev_devstatus.i16status<<m_tEnergyAdjDevStatus.lfcurrentpresent<<sResponsevalue;
    return true;
}

/******************************************
* 功能:发送电机当前状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyAdjustDevCtrl::_sendEnergyAdjDevStatus()
{
    QVariant l_var;
    l_var.setValue(m_tEnergyAdjDevStatus);
    emit signal_updateEnergyAdjDevStatus(l_var);
}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyAdjustDevCtrl::InitErrorInfo()
{
    _errorCodeInfolist.insert(E_ENRGYAD_SENDCMD_OK,"无错误");
    _errorCodeInfolist.insert(E_ENRGYAD_CONNECT_ERROR,"连接设备失败");
    _errorCodeInfolist.insert(E_ENRGYAD_SENDCMD_DATA_FAILED,"发送数据失败");
    _errorCodeInfolist.insert(E_ENRGYAD_RECV_DATA_TIMEOUT,"接收数据超时");
    _errorCodeInfolist.insert(E_ENRGYAD_TIMEOUT,"操作超时");
    _errorCodeInfolist.insert(E_ENRGYAD_MOVE_UNREACH,"未调节到目标位置");
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QString QEnergyAdjustDevCtrl::_getErrorCodeInfo(int nerrcode)
{
    return _errorCodeInfolist.value(nerrcode,"未知错误");
}
