#include "defectchecksysctrl.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif


QDefectCheckSysCtrl::QDefectCheckSysCtrl(QString sIPAddress,int nPort,QObject *parent) : QObject(parent)
{
    m_tDevSocket.m_pCmdSocket = NULL;

    setDevIPAndPort(sIPAddress,nPort);
}


QDefectCheckSysCtrl::~QDefectCheckSysCtrl()
{}


/******************************
*功能:设置IDS系统ip和port
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QDefectCheckSysCtrl::setDevIPAndPort(QString sIP, int nPort)
{
    m_tDevSocket.m_sDevIP = sIP;
    m_tDevSocket.m_nInitPort = nPort;
    createSocket(m_tDevSocket.m_sDevIP,m_tDevSocket.m_nInitPort);
    return true;
}

/******************************
*功能:控制系统向检测系统发布实验信息
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QDefectCheckSysCtrl::publishExpInfo(QString sExpNo,int nMeasureType)
{
    bool l_bRel = false;

    m_sErrInfo = "";

    m_nSysStatus = E_SYSTEM_REL_UNKNOWN;

    st_PACK_CMD l_tPackCmd;
    memset(l_tPackCmd.m_param,'\0',21);

    l_tPackCmd.m_Cmd = E_CMD_LINK_PUBLISH;
    l_tPackCmd.m_param[0] = nMeasureType;
    memcpy(&l_tPackCmd.m_param[1],sExpNo.toLatin1().data(),sExpNo.length());

    if(!sendCmd(l_tPackCmd))
    {
        m_sErrInfo = "发送命令失败";
        return false;
    }


    //延迟等待是否连接成功
    QTime l_cmdTime = QTime::currentTime();
    l_cmdTime.start();
    while(l_cmdTime.elapsed() < M_IDSSYS_WAITFOR_TIME)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        if (m_nSysStatus != E_SYSTEM_REL_UNKNOWN)
        {
            break;
        }
    }


    if(m_nSysStatus == E_SYSTEM_REL_FAL)
    {
        m_sErrInfo = "发布实验信息失败";
    }
    else if(m_nSysStatus == E_SYSTEM_REL_UNKNOWN)
    {
        m_sErrInfo = "损伤检测无应答";
    }
    else if (m_nSysStatus == E_SYSTEM_REL_SUC)
    {
        l_bRel = true;
    }


    return l_bRel;
}

/******************************
*功能:IDS系统开始准备
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
bool QDefectCheckSysCtrl::prepareDefectSys(int nPointNo,int nShotNo)
{
    bool l_bRel = false;

    m_sErrInfo = "";

    m_nPrepareStatus = E_SYSTEM_REL_UNKNOWN;

    st_PACK_CMD l_tPackCmd;
    memset(l_tPackCmd.m_param,'\0',21);

    l_tPackCmd.m_Cmd = E_CMD_LINK_PREPARE;
    l_tPackCmd.m_param[0] = nPointNo;
    l_tPackCmd.m_param[1] = nShotNo;

    if(!sendCmd(l_tPackCmd))
    {
        m_sErrInfo = "发送命令失败";
        return false;
    }



    //延迟等待是否连接成功
    QTime l_cmdTime = QTime::currentTime();
    l_cmdTime.start();
    while(l_cmdTime.elapsed() < M_IDSSYS_WAITFOR_TIME)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        if (m_nPrepareStatus != E_SYSTEM_REL_UNKNOWN)
        {
            break;
        }
    }

    if(m_nPrepareStatus == E_SYSTEM_REL_FAL)
    {
        m_sErrInfo = "损伤检测系统准备失败";
    }
    else if(m_nPrepareStatus == E_SYSTEM_REL_UNKNOWN)
    {
        m_sErrInfo = "损伤检测系统无应答";
    }
    else if (m_nPrepareStatus == E_SYSTEM_REL_SUC)
    {
        l_bRel = true;
    }



    return l_bRel;
}

/******************************
*功能:查询IDS系统损伤检测结果
*输入:
*输出:
*返回值:
*修改记录:无
*******************************/
int QDefectCheckSysCtrl::queryDefectResult(int nPointNo,int nShotNo)
{
    int l_nRel = E_SYSTEM_DEFECT_UNKNOWN;
    m_sErrInfo = "获取检测结果失败";

    m_nResultStatus = E_SYSTEM_DEFECT_UNKNOWN;

    st_PACK_CMD l_tPackCmd;
    memset(l_tPackCmd.m_param,'\0',21);

    l_tPackCmd.m_Cmd = E_CMD_LINK_QUERYRESULT;
    l_tPackCmd.m_param[0] = nPointNo;
    l_tPackCmd.m_param[1] = nShotNo;

    if(!sendCmd(l_tPackCmd))
    {
        m_sErrInfo = "发送命令失败";
        return false;
    }

    //延迟等待是否连接成功
    QTime l_cmdTime = QTime::currentTime();
    l_cmdTime.start();
    while(l_cmdTime.elapsed() < M_IDSSYS_WAITFOR_TIME)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);

        if(m_nResultStatus != E_SYSTEM_DEFECT_UNKNOWN)
        {
            m_sErrInfo = "";
            l_nRel = m_nResultStatus;
            break;
        }
    }

    if(m_nResultStatus == E_SYSTEM_DEFECT)
    {
        m_sErrInfo = "有损伤";
    }

    if(m_nResultStatus == E_SYSTEM_DEFECT_CHECK_FAILED)
    {
        m_sErrInfo = "损伤检测失败";
    }

    if(m_nResultStatus == E_SYSTEM_DEFECT_UNKNOWN)
    {
        m_sErrInfo = "损伤检测系统无应答";
    }


    return l_nRel;
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QDefectCheckSysCtrl::queryCapResult()
{
    bool bRel = false;
    m_sErrInfo = "";

    m_nCapImageStatus = E_SYSTEM_REL_UNKNOWN;

    st_PACK_CMD l_tPackCmd;
    memset(l_tPackCmd.m_param,'\0',21);

    l_tPackCmd.m_Cmd = E_CMD_LINK_QUERYCAPRESULT;

    if(!sendCmd(l_tPackCmd))
    {
        m_sErrInfo = "发送命令失败";
        return false;
    }

    //延迟等待是否连接成功
    QTime l_cmdTime = QTime::currentTime();
    l_cmdTime.start();
    while(l_cmdTime.elapsed() < M_IDSSYS_WAITFOR_TIME)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);

        if(m_nCapImageStatus != E_SYSTEM_REL_UNKNOWN)
        {
            break;
        }
    }

    if(m_nCapImageStatus == E_SYSTEM_REL_FAL)
    {
        m_sErrInfo = "图像采集失败";
    }

    if(m_nCapImageStatus == E_SYSTEM_REL_UNKNOWN)
    {
        m_sErrInfo = "损伤检测系统无应答";
    }

    if(m_nCapImageStatus == E_SYSTEM_REL_SUC)
    {
        bRel = true;
    }

    return bRel;

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QDefectCheckSysCtrl::closeDefectsys()
{
    bool bRel = false;
    m_sErrInfo = "";

    m_nCloseStatus = E_SYSTEM_REL_UNKNOWN;

    st_PACK_CMD l_tPackCmd;
    memset(l_tPackCmd.m_param,'\0',21);

    l_tPackCmd.m_Cmd = E_CMD_LINK_CLOSE;

    if(!sendCmd(l_tPackCmd))
    {
        m_sErrInfo = "发送命令失败";
        return false;
    }

    //延迟等待是否连接成功
    QTime l_cmdTime = QTime::currentTime();
    l_cmdTime.start();
    while(l_cmdTime.elapsed() < M_IDSSYS_WAITFOR_TIME)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);

        if(m_nCloseStatus != E_SYSTEM_REL_UNKNOWN)
        {
            break;
        }
    }

    if(m_nCloseStatus == E_SYSTEM_REL_FAL)
    {
        m_sErrInfo = "损失检测系统结束流程失败";
    }

    if(m_nCloseStatus == E_SYSTEM_REL_UNKNOWN)
    {
        m_sErrInfo = "损伤检测系统无应答";
    }

    if(m_nCloseStatus == E_SYSTEM_REL_SUC)
    {
        bRel = true;
    }

    return bRel;

}



/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QDefectCheckSysCtrl::createSocket(const QString sIP, int nPort)
{
    if (m_tDevSocket.m_pCmdSocket != NULL)
    {
        delete m_tDevSocket.m_pCmdSocket;
        m_tDevSocket.m_pCmdSocket = NULL;
    }
    m_tDevSocket.m_nDevCmdPort = nPort;
    m_tDevSocket.m_nDevRcvPort = nPort;
    m_tDevSocket.m_tHostIP.setAddress(sIP);

    //初始化设备套接字并绑定
    m_tDevSocket.m_pCmdSocket = new QUdpSocket(this);
    m_tDevSocket.m_pCmdSocket->bind(m_tDevSocket.m_nDevRcvPort,QUdpSocket::ShareAddress);
    connect(m_tDevSocket.m_pCmdSocket,&QUdpSocket::readyRead,this,&QDefectCheckSysCtrl::rcvCmdData);
    return true;
}



bool QDefectCheckSysCtrl::sendCmd(st_PACK_CMD& l_tPackCmd)
{
    bool l_bRel = false;


    int l_nLen = m_tDevSocket.m_pCmdSocket->writeDatagram((char*)&l_tPackCmd,sizeof(st_PACK_CMD),m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevCmdPort);
    if (l_nLen > 0)
    {
        l_bRel = true;
    }
    return l_bRel;


//    switch (nCmdID)
//    {
//    case E_CMD_LINK_PUBLISH:
//    {
//        l_tPackCmd.m_Para1 = pExpInfos->m_nExpType;
//        memcpy(l_tPackCmd.m_Para2.m_cExpInfo,pExpInfos->m_sExpNo.toLatin1().data(),pExpInfos->m_sExpNo.length());
//        *(l_tPackCmd.m_Para2.m_cExpInfo + pExpInfos->m_sExpNo.length()) = '\0';

//        int l_nLen = m_tDevSocket.m_pCmdSocket->writeDatagram((char*)&l_tPackCmd,sizeof(st_PACK_CMD),m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevCmdPort);
//        if (l_nLen > 0)
//        {
//            l_bRel = true;
//        }
//        return l_bRel;
//        //;
//    }
//        break;
//    case E_CMD_LINK_PREPARE:
//    {
//        l_tPackCmd.m_Para1 = pExpInfos->m_nPointNo;
//        l_tPackCmd.m_Para2.m_Data[0] = pExpInfos->m_nShotNo;
//        int l_nLen = m_tDevSocket.m_pCmdSocket->writeDatagram((char*)&l_tPackCmd,sizeof(st_PACK_CMD),m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevCmdPort);
//        if (l_nLen > 0)
//        {
//            l_bRel = true;
//        }
//        return l_bRel;
//    }
//        break;
//    case E_CMD_LINK_QUERYRESULT:
//    {
//        l_tPackCmd.m_Para1 = pExpInfos->m_nPointNo;
//        l_tPackCmd.m_Para2.m_Data[0] = pExpInfos->m_nShotNo;
//        int l_nLen = m_tDevSocket.m_pCmdSocket->writeDatagram((char*)&l_tPackCmd,sizeof(st_PACK_CMD),m_tDevSocket.m_tHostIP,m_tDevSocket.m_nDevCmdPort);
//        if (l_nLen >0)
//        {
//            l_bRel = true;
//        }
//        return l_bRel;
//    }
//    }
}

void QDefectCheckSysCtrl::rcvCmdData()
{
    QHostAddress l_sender;
    quint16 l_nsenderport;
    char l_cBuff[100]={0};
    while(m_tDevSocket.m_pCmdSocket->hasPendingDatagrams())
    {
        m_tDevSocket.m_pCmdSocket->readDatagram(l_cBuff,sizeof(l_cBuff),&l_sender,&l_nsenderport);
        cmdProcess(l_cBuff);
    }
}


void QDefectCheckSysCtrl::cmdProcess(char *buff)
{
    st_PACK_CMD* l_pCmdPack = (st_PACK_CMD*)buff;

    switch (l_pCmdPack->m_Cmd)
    {
        case E_CMD_LINK_PUBLISH:
        {
            m_nSysStatus = l_pCmdPack->m_param[0];
        }

        break;

        case E_CMD_LINK_PREPARE:
        {
            m_nPrepareStatus =l_pCmdPack->m_param[0];
        }
        break;

        case E_CMD_LINK_QUERYCAPRESULT:
        {
            m_nCapImageStatus = l_pCmdPack->m_param[0];
        }
        break;

        case E_CMD_LINK_QUERYRESULT:
        {
            m_nResultStatus = l_pCmdPack->m_param[0];
            m_nDefectTimes = l_pCmdPack->m_param[1];

        }
        break;
        case E_CMD_LINK_CLOSE:
        {
            m_nCloseStatus = l_pCmdPack->m_param[0];
        }
        break;

    }
}



