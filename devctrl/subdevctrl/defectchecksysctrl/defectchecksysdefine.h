#ifndef DEFECTCHECKCTRL_DEFINE_H
#define DEFECTCHECKCTRL_DEFINE_H

#include <QUdpSocket>
#include <QHostAddress>

typedef unsigned char       BYTE;


#define M_IDSSYS_WAITFOR_TIME 2000  //ms


//enum SYSTEMSTATUS  //像机工作状态
//{
//    E_SYSTEM_UNKNOWN_STATUS = 0x00,//未知状态
//    E_SYSTEM_INIT_OK = 0x02//系统初始化OK
//};

enum SYSTEMREL
{
    E_SYSTEM_REL_UNKNOWN = 0x00,
    E_SYSTEM_REL_SUC = 0x01,//成功
    E_SYSTEM_REL_FAL = 0x02//失败
};

enum DEFECTRESULT
{
    E_SYSTEM_DEFECT_UNKNOWN = 0x0, //未知
    E_SYSTEM_NODEFECT = 0x01,   //判断无损伤
    E_SYSTEM_DEFECT = 0x02,     //判断有损伤
    E_SYSTEM_DEFECT_CHECK_FAILED = 0x03      //系统判断失败
};


typedef struct _DevSocket
{
    QHostAddress m_tHostIP;
    QString m_sDevIP;
    int m_nInitPort;

    //命令
    int m_nDevCmdPort;//命令发送/接收端口
    int m_nDevRcvPort;
    QUdpSocket* m_pCmdSocket;

}st_DevSocket;


enum COMMAND
{
    E_CMD_LINK_PUBLISH = 0xA0,
    E_CMD_LINK_PREPARE = 0xA1,
    E_CMD_LINK_QUERYRESULT = 0xA2,//查询检测结果
    E_CMD_LINK_QUERYCAPRESULT = 0xA3,//检测图像采集结果
    E_CMD_LINK_CLOSE = 0xA4,//关闭检测
};



enum EXPTYPE
{
    E_EXPTYPE_1ON1 = 0x01,
    E_EXPTYPE_SON1 = 0x02,
    E_EXPTYPE_RON1 = 0x03,
    E_EXPTYPE_RASTER = 0x04
};

#pragma pack(1)

typedef struct _PACK_CMD
{
    BYTE m_Cmd; //命令字
    BYTE m_param[21];  //参数1~20
}st_PACK_CMD;

#pragma pack()


#endif // QIDSSYSDEFINE_H
