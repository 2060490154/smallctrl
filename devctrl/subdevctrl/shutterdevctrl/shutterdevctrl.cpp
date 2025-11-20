/***************************************************************************
**                                                                                               **
**QShutterDevCtrl为光闸设备控制类                                     **
** 提供的功能函数为：
**
****************************************************************************
**创建人：李刚
**创建时间：2018.11.17
**修改记录：
**
****************************************************************************/
#include "shutterdevctrl.h"
#include  "shutteripcbackend.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

QShutterDevCtrl::QShutterDevCtrl(QString sIPAddress,int nPort,QString sDevName)
{
    _nPort = nPort;

    m_sDevName = sDevName;

    m_pUdpSocket = new QUdpSocket(this);
    connect(m_pUdpSocket,&QUdpSocket::readyRead,this,&QShutterDevCtrl::ReceiveData);

    bool result=m_pUdpSocket->bind(_nPort);		//绑定到指定的端口上
    if(!result)
    {
        return;
    }

    //初始化设备信息
    m_tShutterDevInfo.tHostAddress.setAddress(sIPAddress);
    m_tShutterDevInfo.bProcessStart = false;
    m_tShutterDevInfo.bProcessEnd = false;
    m_tShutterDevInfo.nCurrentVoltData = 0;
    m_tShutterDevInfo.nSetLimitData = 0;
    m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_NONE;


    _bDevStatus = checkDevStatus();
}

QShutterDevCtrl::~QShutterDevCtrl()
{
    SetDevStatus(false);//保护性关闭
}
// 替换或插入以下函数体（确保函数签名与原文件一致）
bool QShutterDevCtrl::SetDevStatus(bool bOpen)
{
    if (!m_backend) return false;
    if (m_sessionId <= 0) return false;
    const int channel = 1;
    // use longer timeout to avoid client-side retry storms
    QJsonObject r = bOpen ? m_backend->openShutter(m_sessionId, channel, 5000)
        : m_backend->closeShutter(m_sessionId, channel, 5000);

    if (r.isEmpty()) {
        qWarning() << "SetDevStatus: helper returned empty (timeout)";
        // do NOT immediately resend repeatedly; instead try a single read to check device state
        QJsonObject q = m_backend->readParam(m_sessionId, "status", 0, 3000);
        if (!q.isEmpty() && q.value("status").toString() == "ok") {
            int v = q.value("payload").toObject().value("value").toInt(-1);
            if (v == 1) m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_OPENED;
            else if (v == 0) m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_CLOSED;
            return (bOpen && m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_OPENED) ||
                (!bOpen && m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_CLOSED);
        }
        // show diagnostic to user: helper timed out, avoid blind retry
        qWarning() << "SetDevStatus: no response and no status confirmation; not retrying automatically.";
        return false;
    }

    // helper returned something; if error include attempts
    if (r.value("status").toString() != "ok") {
        qWarning() << "SetDevStatus: helper returned error:" << r.value("error").toString();
        // Extract attempts if present to log for debugging
        if (r.contains("payload") && r.value("payload").isObject()) {
            QJsonObject pl = r.value("payload").toObject();
            qDebug() << "Helper payload attempts:" << QJsonDocument(pl).toJson(QJsonDocument::Compact);
        }
        return false;
    }

    // status ok: parse payload.state/value
    if (r.contains("payload") && r.value("payload").isObject()) {
        QJsonObject pl = r.value("payload").toObject();
        int st = pl.value("state").toInt(pl.value("value").toInt(-1));
        if (st == 1) m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_OPENED;
        else if (st == 0) m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_CLOSED;
        // if state unknown (-1), we treat as failure (do not optimistically accept)
        if (st == -1) {
            qWarning() << "SetDevStatus: helper returned ambiguous state (-1).";
            return false;
        }
        return (bOpen && m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_OPENED) ||
            (!bOpen && m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_CLOSED);
    }

    return false;
}
/*******************************************************************
**功能：获取状态
**输入：
**输出：
**返回值：
*******************************************************************/
bool QShutterDevCtrl::getDevStatus()
{
    return _bDevStatus;
}

/*******************************************************************
**功能：判断是否通信正常
**输入：
**输出：
**返回值：
*******************************************************************/
bool QShutterDevCtrl::checkDevStatus()
{
    bool bRel = false;

    tShutterCmd cmddata;
    cmddata.startflag = 0xff;
    cmddata.status = 0x01;
    cmddata.cmdType = 0x00;//查询
    cmddata.data[0] = 0x77;
    cmddata.data[1] = 0x0B;
    cmddata.endflag = 0xee;


    m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_NONE;//清除当前状态
    m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDevInfo.tHostAddress,_nPort);//发送查询命令



    //延时等待是否连接成功
    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < 1000 )
    {

        QCoreApplication::processEvents();

        if( m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_NONE)
        {
            bRel = false;
        }
        else
        {
            bRel = true;
            break;
        }

        QThread::msleep(10);
    }

    return bRel;
}


/*******************************************************************
**功能：设置打开关闭状态
**输入：
**输出：
**返回值：
*******************************************************************/


/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
bool QShutterDevCtrl::SetDevLimit(int ndata)
{
  bool bRel = false;
  tShutterCmd cmddata;

  cmddata.startflag = 0xff;
  cmddata.status = 0x00;
  cmddata.cmdType = 0x01;//设置电压门限
  cmddata.data[0] = ndata&0xff;
  cmddata.data[1] = (ndata>>8)&0xff;
  cmddata.endflag = 0xee;

  m_tShutterDevInfo.nSetLimitData  = 0;

  m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDevInfo.tHostAddress,_nPort);

  //延时等待是否连接成功
  QTime dieTime = QTime::currentTime();
  dieTime.start();
  while( dieTime.elapsed() < 1000 )
  {
      QCoreApplication::processEvents();

      if(m_tShutterDevInfo.nSetLimitData != 0)
      {
          bRel = true;
          break;
      }
  }

  return bRel;

}

/*******************************************************************
**功能：接收消息处理
**输入：
**输出：
**返回值：
*******************************************************************/
void QShutterDevCtrl::ReceiveData()
{
    QHostAddress sender;
    quint16 senderport;
    char buff[256];

    while(m_pUdpSocket->hasPendingDatagrams())
    {
        int nSize = m_pUdpSocket->pendingDatagramSize();
        m_pUdpSocket->readDatagram(buff,nSize,&sender,&senderport);

        ParseRcvData(sender.toString(), senderport,buff,nSize);
    }

}

/*******************************************************************
**功能：解析接收的数据 更新对应设备的状态
**输入：
**输出：
**返回值：
*******************************************************************/
void  QShutterDevCtrl::ParseRcvData(QString sIPAddress,int nPort,char* pbuff,int nSize)
{
    if(nPort != _nPort )
    {
        return;
    }
    QString sIp = sIPAddress.split(":")[3];


    if(m_tShutterDevInfo.tHostAddress.toString() != sIp)
    {
       return;
    }


    //解析帧头帧尾
    int tem_nHeadPos = -1;
    for (int i=0; i<8; i++)
    {
        if ((pbuff[i]&0xff) == 0xff)
        {
            tem_nHeadPos = i;
            if (i != 0)
            {
                QString tem_curFrame = QString::number(pbuff[i]&0xff,16);
                for (int j=1; j<8; j++)
                {
                    tem_curFrame += (QString(" ") + QString::number(pbuff[i+j]&0xff,16));
                }
                qWarning()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,QString("[检测到帧头不在0位，在第%1位；当前帧为：%2]").arg(i).arg(tem_curFrame));
            }
            if ((pbuff[i + M_SHUTTER_FREAM_LENGTH - 1]&0xff) != 0xee) //非正常帧尾
            {
                QString tem_curFrame = QString::number(pbuff[i]&0xff,16);
                for (int j=1; j<8; j++)
                {
                    tem_curFrame += (QString(" ") + QString::number(pbuff[i+j]&0xff,16));
                }
                qWarning()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,QString("[检测到不正常的帧尾，当前帧为：%1]").arg(tem_curFrame));
                return;
            }
            else
            {
                break;
            }
        }
    }
    if (tem_nHeadPos == -1)
    {
        QString tem_curFrame = QString::number(pbuff[0]&0xff,16);
        for (int j=1; j<8; j++)
        {
            tem_curFrame += (QString(" ") + QString::number(pbuff[0+j]&0xff,16));
        }
        qWarning()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,QString("[当前帧未检测到帧头，当前帧为：%1]").arg(tem_curFrame));
        return;
    }


    //解析数据 ff 01 11 00 00(设定值) 00 00 (采集值) ee   第二个字节为状态打开/关闭  第三个字节为设定/查询
    if( pbuff[tem_nHeadPos+1] == 0x00 && pbuff[tem_nHeadPos+2] != 0x01)
    {
        m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_CLOSED;
    }
    if( pbuff[tem_nHeadPos+1] == 0x01&& pbuff[tem_nHeadPos+2] != 0x01)
    {
        m_tShutterDevInfo.nCurrentStatus = M_SHUTTER_STATUS_OPENED;
    }

    if( pbuff[tem_nHeadPos+2] == 0x01 || pbuff[tem_nHeadPos+2] == 0x00)
    {
        unsigned short data1 =  (pbuff[tem_nHeadPos+6]<<8)&0xff00;
        unsigned short data2 = pbuff[tem_nHeadPos+5]&0xff;

        m_tShutterDevInfo.nCurrentVoltData = data1  + data2 ;

        data1 =  (pbuff[tem_nHeadPos+4]<<8)&0xff00;
        data2 = pbuff[tem_nHeadPos+3]&0xff;
        m_tShutterDevInfo.nSetLimitData = data1+data2;
//        m_tShutterDev[nIndex].nCurrentStatus = M_SHUTTER_STATUS_QUERYFINISH;
    }

    if( pbuff[tem_nHeadPos+2] == 0x22)//流程
    {
        QString tem_curFrame = QString::number(pbuff[0]&0xff,16);
        for (int j=1; j<8; j++)
        {
            tem_curFrame += (QString(" ") + QString::number(pbuff[0+j]&0xff,16));
        }
       // qWarning()<<M_OUT_MSG(M_SHUTTER_LOG_FILE,QString("[解析到当前帧为：%1  时间：%2]").arg(tem_curFrame).arg(QDateTime::currentDateTime().toString("hh_mm_ss_zzz")));

        unsigned short data =  (pbuff[tem_nHeadPos+5])&0xff;

        if(data == 0xAA)//开始
        {
             m_tShutterDevInfo.bProcessStart = true ;
             m_tShutterDevInfo.bProcessEnd = false ;
        }

        if(data == 0xBB)//结束
        {
             m_tShutterDevInfo.bProcessEnd = true ;
        }
    }

    if ( ((pbuff[tem_nHeadPos+2])&0xff) == 0xAA)  //外部触发关断光闸
    {
        m_tShutterDevInfo.bProcessEnd = true ;
    }

}


/*******************************************************************
**功能：设置执行流程状态，并等待流程执行结束
**输入：nPlusCnt为脉冲计数  nDeleay为延时时间（ms）
**输出：
**返回值：
*******************************************************************/
bool QShutterDevCtrl::setProcessStatus(int nPlusCnt,int nDeleay)
{
    bool bRel = false;
    tShutterCmd cmddata;


    cmddata.startflag = 0xff;
    cmddata.status = 0x00;  //open
    cmddata.cmdType = 0x22;//设置流程状态
    cmddata.data[0] = nPlusCnt&0xff;
    cmddata.data[1] =nDeleay&0xff;
    cmddata.endflag = 0xee;

    m_tShutterDevInfo.bProcessStart = false ;
    m_tShutterDevInfo.bProcessEnd = false ;


    m_pUdpSocket->writeDatagram((char*)&cmddata,6,m_tShutterDevInfo.tHostAddress,_nPort);

    //延时等待
    QTime dieTime = QTime::currentTime();
    dieTime.start();
    while( dieTime.elapsed() < 5000 )
    {
        QCoreApplication::processEvents();
        //收到开始状态
        if(m_tShutterDevInfo.bProcessStart  == true && m_tShutterDevInfo.bProcessEnd == true)
        {
            bRel = true;
            break;
        }
    }

    return bRel;
}
