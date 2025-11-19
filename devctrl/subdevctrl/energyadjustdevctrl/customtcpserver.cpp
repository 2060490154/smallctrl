#include "customtcpserver.h"

QCustomTcpServer::QCustomTcpServer(QString sComAddress,int nPort)
{

    _sIPAddress = sComAddress;
    _nPort = nPort;

    _pClientSocket = nullptr;
    m_bConnect = false;

    _pTcpServer = new QTcpServer();
    _pTcpServer->listen(QHostAddress::AnyIPv4,_nPort);

    //有新连接的时候进行处理
    connect(_pTcpServer,&QTcpServer::newConnection,this,&QCustomTcpServer::onNewConnect);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QCustomTcpServer:: ~QCustomTcpServer()
{
    if(_pClientSocket != nullptr)
    {
        _pClientSocket->close();
        delete _pClientSocket;
    }

    if(_pTcpServer != nullptr)
    {
        _pTcpServer->close();
        delete _pTcpServer;
    }

}

/******************************************
* 功能:处理连接
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QCustomTcpServer::onNewConnect()
{
    QTcpSocket* l_pClientSocket = _pTcpServer->nextPendingConnection();
    if(l_pClientSocket->peerAddress().toString() == _sIPAddress && l_pClientSocket->peerPort() == _nPort)
    {
        _pClientSocket = l_pClientSocket;
        connect(_pClientSocket,&QTcpSocket::readyRead,this,&QCustomTcpServer::onReadData);

            //初始化参数
            onSendCmd("LPA>ON!");
            if(_sResponseData == "LPA>ON")
            {
                m_bConnect = true;
            }

            //归零
            onSendCmd("LPA>HOME!");

    }
}

/******************************************
* 功能:读取数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QCustomTcpServer::onReadData()
{
    _lock.lock();
    QByteArray array = _pClientSocket->readAll();
    _sResponseData = QString(array);
   // qDebug()<<"######"<<QString(array)<<_pClientSocket->peerAddress().toString();
    _lock.unlock();


}

/******************************************
* 功能:发送数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QCustomTcpServer::onSendCmd(QString sCmd)
{

    if(_pClientSocket == nullptr)
    {
        return E_ENRGYAD_SENDCMD_DATA_FAILED;
    }

    QByteArray l_data;
    l_data.append(sCmd);

    int nlen = _pClientSocket->write(l_data);

    if(nlen != l_data.size())
    {
        return E_ENRGYAD_SENDCMD_DATA_FAILED;
    }

    _current_time.restart();
    while(_current_time.elapsed() < 10000)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

        if(!_sResponseData.isEmpty())
        {
            return E_ENRGYAD_SENDCMD_OK;
        }
      //  QThread::msleep(100);

    }

    return E_ENRGYAD_TIMEOUT;
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QCustomTcpServer:: cleardata()
{
    _lock.lock();
    _sResponseData = "";//清空数据
    _lock.unlock();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QString QCustomTcpServer:: getRcvdata()
{
    QString sRel = "";
    _lock.lock();
    sRel = _sResponseData;
    _sResponseData = "";//清空数据
    _lock.unlock();

    return sRel;

}
