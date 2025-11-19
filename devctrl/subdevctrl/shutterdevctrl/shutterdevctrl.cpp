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
bool QShutterDevCtrl::SetDevStatus(bool bOPen)
{
    // 如果没有后端，不能用 IPC 控制
    if (!m_backend) {
        qWarning() << "QShutterDevCtrl::SetDevStatus: no backend configured";
        return false;
    }

    if (m_sessionId <= 0) {
        qWarning() << "QShutterDevCtrl::SetDevStatus: invalid sessionId, call backend->connectDevice first";
        return false;
    }

    // 1) 发送 open/close 命令到 helper
    QJsonObject resp;
    if (bOPen) {
        resp = m_backend->openShutter(m_sessionId, 1, 3000);
    }
    else {
        resp = m_backend->closeShutter(m_sessionId, 1, 3000);
    }

    if (resp.isEmpty()) {
        qWarning() << "QShutterDevCtrl::SetDevStatus: backend returned empty response";
        return false;
    }

    // 如果 helper 报错则返回失败
    QString statusStr = resp.value("status").toString();
    if (statusStr != "ok") {
        qWarning() << "QShutterDevCtrl::SetDevStatus: helper reported error:" << resp.value("error").toString()
            << " raw:" << QString::fromUtf8(QJsonDocument(resp).toJson(QJsonDocument::Compact));
        return false;
    }

    // helper 返回 ok。尝试解析 payload 的 response（设备回显），若存在则记录（便于排错）
    QString deviceEcho;
    if (resp.contains("payload") && resp.value("payload").isObject()) {
        QJsonObject payload = resp.value("payload").toObject();
        if (payload.contains("response")) deviceEcho = payload.value("response").toString();
    }

    if (!deviceEcho.isEmpty()) {
        qDebug() << "QShutterDevCtrl::SetDevStatus: device echoed:" << deviceEcho;
        // 根据回显的内容可以更严格判断是否成功（如包含 OK/1 等）
        QString le = deviceEcho.trimmed().toLower();
        if (le.contains("ok") || le.contains("1") || le.contains("open")) {
            m_tShutterDevInfo.nCurrentStatus = bOPen ? M_SHUTTER_STATUS_OPENED : M_SHUTTER_STATUS_CLOSED;
            return true;
        }
        else if (le.contains("0") || le.contains("close") || le.contains("closed")) {
            m_tShutterDevInfo.nCurrentStatus = bOPen ? M_SHUTTER_STATUS_CLOSED : M_SHUTTER_STATUS_OPENED;
            // 回显和期望不一致，返回 false
            qWarning() << "QShutterDevCtrl::SetDevStatus: device echo contradicts requested state:" << deviceEcho;
            return false;
        }
        else {
            // 无法断言，从 helper 的 ok 判定为成功（保守处理：认为命令已发出）
            m_tShutterDevInfo.nCurrentStatus = bOPen ? M_SHUTTER_STATUS_OPENED : M_SHUTTER_STATUS_CLOSED;
            return true;
        }
    }
    else {
        // 没有回显（common case），但 helper 已确认接收到并返回 ok
        // 许多硬件不会回显，不能通过 read_param("status") 检查（该参数在 SDK 中不存在）
        // 因此以 helper 返回的 ok 为主，设置本地状态并返回成功
        qDebug() << "QShutterDevCtrl::SetDevStatus: helper returned ok but no device echo; assuming command sent.";
        m_tShutterDevInfo.nCurrentStatus = bOPen ? M_SHUTTER_STATUS_OPENED : M_SHUTTER_STATUS_CLOSED;
        return true;
    }
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
