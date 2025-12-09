#ifndef QENERGYDEVCTRL_H
#define QENERGYDEVCTRL_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <qmutex.h>
#include "energydevdefine.h"

// 如果你添加了 adapter 的头文件
#include "energy_slink_adapter.h"

class QEnergyDevCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QEnergyDevCtrl(QString sIP, int nPort);
    ~QEnergyDevCtrl();

signals:
    void signalLog(const QString& log);
public slots:

    void onCapMeasureData();//在线程里面运行  实时监听数据

public:

    void delayTime(int mesc);

    int openDev();

    void closeDev();

    int getDevParam(int nChannelNo);//查询设备参数

    int getDevConnectStatus(int nChannelNo);//查询探头连接状态

    int setChannelExTrigger(int nChannelNo, bool bEnable);//设置通道信息

    int setChannelTriggerLevel(int nChannelNo, float lfTriggerLevel);//设置触发门限

    int setChannelWaveLength(int nChannelNo, int nWaveLength);//设置测量波长

    int setChannelMeauserRange(int nChannelNo, int nRangeIndex);//设置测量范围

    int setChannelStartCap(int nChannelNo);//开始获取测量数据

    int setChannelStopCap(int nChannelNo);//停止获取测量数据

    // 运行时切换为 S-Link 适配器（公有接口，封装内部成员）
    // 直接使用已有的 setUseSLink(...)（如果已定义）或下面的 wrapper
    bool setUseSLink(bool use, const QString& portOrAddress = QString(), int baud = 921600);

    // 新增公有接口（用于 UI 不直接访问私有成员）
    bool isSLinkOpen() const; // 是否已启用且 adapter 已打开端口
    bool enableSLinkPort(const QString& portName, int baud = 921600); // 打开并启用 S-Link（wrapper）
    void disableSLinkPort(); // 关闭 S-Link 适配器（wrapper）

    tDevParam m_tDevParam;

    bool m_bCapData;

    QTimer* _capDataTimer;

    bool getDetectorName(int channel, QString& outName, int timeout = 500);

    
  
  

  

    

 


private:

    int sendCmdData(QString sCmd);//发送数据

    void praseDevParam(QByteArray data, int nChannelNo);

    void praseMeasureData(QByteArray data);//解析测量值数据

    int getTriggerLevel();//获取触发门限
    int getMeauserMode();//获取当前测量模式

    int getMeauserRange();//获取当前测量档位
    int getMeasureWaveLength();//获取测量波长

    QSerialPort* _pSerialPort;

    bool _bHaveRecvData;//标记是否收到数据
    QByteArray _reciveData;//接收到的数据

    QMutex _EnergyDevlock;

    // ----- S-Link 适配相关（保持私有） -----
    bool m_bUseSLink;
    EnergySLinkAdapter* m_pSLinkAdapter;

    // adapter 回填数据的处理（仍然私有）
    void onSLinkAsciiMeasurement(int channel, const QString& valueStr);
    void onSLinkBinaryMeasurement(int channel, double value, const QByteArray& raw);
    

    // 这两个私有函数必须存在并被实现
   
   

    // ... 其它私有成员 ...



    // adapter 回填数据的处理（仍然私有）
    
    

    
    

};
#endif // QENERGYDEVCTRL_H