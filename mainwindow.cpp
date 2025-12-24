#include "mainwindow.h"
#include "ui_mainwindow.h"
#if _MSC_VER >=1600

#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{

    _pDefectMeasureWidget = nullptr;
	m_pDevCtrlManager = nullptr;

    //数据库相关
    _pDBProcess = new CDbDataProcess(this);
    _pDBProcess->setDbInfo("127.0.0.1",1433,"sa","123456","QTDSN",E_DB_SQLSERV);

    if(!_pDBProcess->connectDB())
    {
        QMessageBox::warning(this,"提示","数据库连接失败！");
    }


    //加载配置文件
    _pPlatformDevConfig = new QDevConfig(this);
    if(!_pPlatformDevConfig->loadConfigFile(QApplication::applicationDirPath()+"/configfile/devconfig.xml"))
    {
        QMessageBox::warning(this,"提示","读取设备配置文件错误，程序无法正常配置!");
        return;
    }

    _pPlatformConfig = new QPlatformConfig(this);
    if(!_pPlatformConfig->loadPlatformConfig(QApplication::applicationDirPath()+"/configfile/platformconfig.xml"))
    {
        QMessageBox::warning(this,"提示","读取平台配置文件错误，程序无法正常配置!");
        return;
    }


    InitDevCtrl();//初始化控制类

    InitDevCtrlWidget();

    InitUI();//初始化界面


    connect(&_checkStatusTime,&QTimer::timeout,this,&MainWindow::checkDevStatus);
    _checkStatusTime.setSingleShot(true);
    _checkStatusTime.start(1000);

   // checkDevStatus();


    //配置文件更新
    connect(_pLightParamWidget,&LightParamWidget::signal_updatePlatformFile,_pPlatformConfig,&QPlatformConfig::onUpdatePlatConfigfile);
    connect(_pMeasurePreWidget,&MeasurePreReadyWidget::signal_updatePlatformFile,_pPlatformConfig,&QPlatformConfig::onUpdatePlatConfigfile);

    //配置文件更新后 更新显示数据
    connect(_pPlatformConfig,&QPlatformConfig::signal_updateshowParam,_pLightParamWidget,&LightParamWidget::showParam);
    connect(_pPlatformConfig,&QPlatformConfig::signal_updateshowParam,_pMeasurePreWidget,&MeasurePreReadyWidget::showParam);
    connect(_pPlatformConfig,&QPlatformConfig::signal_updateshowParam,_pDefectMeasureWidget,&DefectMeasureWidget::showParam);

}

MainWindow::~MainWindow()
{
    releaseDevCtrl();
}
/******************************************
* 功能:窗口关闭
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void MainWindow::closeEvent(QCloseEvent *event)
{
	if (m_pDevCtrlManager != nullptr)
	{
		if (QWorkflowProcessThread::getInstance(m_pDevCtrlManager)->isRunning())
		{
			event->ignore();
			QMessageBox::warning(this, "提示", "有任务在执行，请先终止任务!");
			return;
		}
	}

    if(_pDefectMeasureWidget != nullptr)
    {
        delete _pDefectMeasureWidget;
    }

}

/******************************************
* 功能:初始化设备控制实例，并注册给管理类
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void MainWindow::InitDevCtrl()
{
    m_pDevCtrlManager = new QDevCtrlManager(this);//初始化控制

    foreach (tDevInfo devInfo, _pPlatformDevConfig->m_tDevInfoList)
    {
        switch(devInfo.devType)
        {
            case M_DEV_NEARFIELD_CCD://近场CCD
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformKxccdCtrl* platformKxccdCtrl = new QPlatformKxccdCtrl(devItem.nIndex,devItem.sDevName,this);
                    platformKxccdCtrl->setDevIPAndPort(devItem.sDevIPAddress,devItem.nPort);
                    if(platformKxccdCtrl->openDev())
                    {

                        platformKxccdCtrl->setDevParam(1,6);
                    }
                    m_pDevCtrlManager->registerDevInfo(devInfo.devType,devItem.nIndex,bindRunTask(QPlatformKxccdCtrl,platformKxccdCtrl));
                }
            }
            break;

            case M_DEV_FARFIELD_CCD://远场CCD
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformKxccdCtrl* pZzccdCtrl = new QPlatformKxccdCtrl(devItem.nIndex,devItem.sDevName,this);
                    pZzccdCtrl->setDevIPAndPort(devItem.sDevIPAddress,devItem.nPort);
                    if(pZzccdCtrl->openDev())
                    {
                        pZzccdCtrl->setDevParam(1,6);
                    }

                    m_pDevCtrlManager->registerDevInfo(devInfo.devType,devItem.nIndex,bindRunTask(QPlatformKxccdCtrl,pZzccdCtrl));
                }
            }
            break;

            case M_DEV_MOTOR://电机
            {
                for (tDevItem devItem:devInfo.devItemlist)
                {
                    QPlatformMotorDevCtrl* platformMotorDevCtrl = new QPlatformMotorDevCtrl();
                     m_pDevCtrlManager->registerDevInfo(devInfo.devType,devItem.nIndex,bindRunTask(QPlatformMotorDevCtrl,platformMotorDevCtrl));
                }

            }
            break;


            case M_DEV_ENERGY://能量计
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformEnergyDevCtrl* platformEnergyDevCtrl = new QPlatformEnergyDevCtrl(devItem.sDevIPAddress,devItem.nPort);
                    m_pDevCtrlManager->registerDevInfo(devInfo.devType,devItem.nIndex,bindRunTask(QPlatformEnergyDevCtrl,platformEnergyDevCtrl));
                }
            }
            break;

            case M_DEV_OSCII://示波器
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QList<tOsciiChannel> l_tparam;
                    for(tChannelInfo l_channelItem: devItem.st_ChannelInfo)
                    {
                        l_tparam.push_back({l_channelItem.sChannelName,l_channelItem.nChannelNo});
                    }
                    QPlatformOsciCtrl* platformOsciCtrl = new QPlatformOsciCtrl(devItem.sDevIPAddress,l_tparam);
                    m_pDevCtrlManager->registerDevInfo(devInfo.devType,devItem.nIndex,bindRunTask(QPlatformOsciCtrl,platformOsciCtrl));

                }

            }
            break;

            case M_DEV_SHUTTER://光闸
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformShutterCtrl* platformShutterCtrl = new QPlatformShutterCtrl(devItem.sDevIPAddress,devItem.nPort,devItem.sDevName);
                    m_pDevCtrlManager->registerDevInfo(devInfo.devType,devItem.nIndex,bindRunTask(QPlatformShutterCtrl,platformShutterCtrl));
                }
            }
            break;

            case M_DEV_ENERGY_ADJ://能量调节模块
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformEnergyAdjDevCtrl* platformEnergyAdjCtrl = new QPlatformEnergyAdjDevCtrl(devItem.sDevIPAddress,devItem.nPort);

                    m_pDevCtrlManager->registerDevInfo(devInfo.devType,devItem.nIndex,bindRunTask(QPlatformEnergyAdjDevCtrl,platformEnergyAdjCtrl));

                }
            }
            break;
            case M_DEV_DEFECTCHECK://损伤检测系统
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformDefectCheckCtrl* platformDefectCheckCtrl = new QPlatformDefectCheckCtrl(devItem.sDevIPAddress,devItem.nPort);
                    m_pDevCtrlManager->registerDevInfo(devInfo.devType,devItem.nIndex,bindRunTask(QPlatformDefectCheckCtrl,platformDefectCheckCtrl));
                 }
            }
            break;

            default:
            break;
        }

    }


    //数据处理模块
    QPlatformDataProcessCtrl* platformDataProcessCtrl = new QPlatformDataProcessCtrl();
    m_pDevCtrlManager->registerDevInfo(M_DEV_DATA_PROCESS,0,bindRunTask(QPlatformDataProcessCtrl,platformDataProcessCtrl));
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void MainWindow::InitDevCtrlWidget()
{
    //创建设备单独控制界面
    foreach (tDevInfo devInfo, _pPlatformDevConfig->m_tDevInfoList)
    {
        switch(devInfo.devType)
        {
            case M_DEV_NEARFIELD_CCD://近场CCD
            {
                vector<CKxccdCtrl*>l_KxccdCtrllist;
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    l_KxccdCtrllist.push_back((CKxccdCtrl*)m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex));
                }
                 QMulitCtrlWidget* pKxccdWidget = new QMulitCtrlWidget(l_KxccdCtrllist,this);
                 pKxccdWidget->setWindowTitle(devInfo.sdevTypeName);
                 m_measureDevCtrlWidgetList.push_back(pKxccdWidget);
            }
            break;

            case M_DEV_FARFIELD_CCD://远场CCD
            {
                vector<CKxccdCtrl*>l_ZzccdCtrllist;
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    l_ZzccdCtrllist.push_back((CKxccdCtrl*)m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex));
                }
                 QMulitCtrlWidget* pZzccdWidget = new QMulitCtrlWidget(l_ZzccdCtrllist,this);
                 pZzccdWidget->setWindowTitle(devInfo.sdevTypeName);
                 m_measureDevCtrlWidgetList.push_back(pZzccdWidget);
            }
            break;

            case M_DEV_MOTOR://电机
            {

                vector<tMotorConfigInfo> pConfigList;
                for (tDevItem devItem:devInfo.devItemlist)
                {
                    for(tChannelInfo l_channelInfo:devItem.st_ChannelInfo)
                    {
                        tMotorConfigInfo l_motorConfigInfo;
                        l_motorConfigInfo.nMotorIndex = l_channelInfo.nChannelNo;
                        l_motorConfigInfo.sMotorName = l_channelInfo.sChannelName;
                        l_motorConfigInfo.pMotorCtrl = m_pDevCtrlManager->getDevCtrl(M_DEV_MOTOR,devItem.nIndex);
                        l_motorConfigInfo.nMotorCoeff = static_cast<int>(l_channelInfo.lfCoeff);
                        l_motorConfigInfo.sUnit = l_channelInfo.sUnit;

                        l_motorConfigInfo.tPostionConfig.clear();
                        for(int i = 0; i < l_channelInfo.sGearIndexlist.size(); i++)
                        {
                            tMotorPostionInfo l_motorpos = {l_channelInfo.sGearNamelist[i],l_channelInfo.sGearIndexlist[i].toInt()};
                            l_motorConfigInfo.tPostionConfig.push_back(l_motorpos);
                        }
                        pConfigList.push_back(l_motorConfigInfo);
                    }
                }

                QMotorCtrlWidget* pMotorCtrlWidget = new QMotorCtrlWidget(pConfigList);
                pMotorCtrlWidget->setWindowTitle(devInfo.sdevTypeName);
                m_moveDevCtrlWidgetlist.push_back(pMotorCtrlWidget);

            }
            break;


            case M_DEV_ENERGY://能量计
            {
                vector<tEnergdevconfig>l_EnergyDevConfig;
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    tEnergdevconfig l_devconfig;
                    l_devconfig.pEnergyDevCtrl = (QEnergyDevCtrl*)m_pDevCtrlManager->getDevCtrl(M_DEV_ENERGY,devItem.nIndex);

                    for(tChannelInfo l_channelInfo:devItem.st_ChannelInfo)
                    {
                        l_devconfig.sChannelNamelist.push_back(l_channelInfo.sChannelName);
                        l_devconfig.sChannelNolist.push_back(QString::number(l_channelInfo.nChannelNo));
                    }

                    l_EnergyDevConfig.push_back(l_devconfig);
                }
                QEnergyDevCtrlWidget* pEnergyDevCtrlWidget = new QEnergyDevCtrlWidget(l_EnergyDevConfig);//能量计
                pEnergyDevCtrlWidget->setWindowTitle(devInfo.sdevTypeName);
                m_measureDevCtrlWidgetList.push_back(pEnergyDevCtrlWidget);
            }
            break;

            case M_DEV_OSCII://示波器
            {
                QOsciCtrlWidget* pOsciCtrlWiget = new QOsciCtrlWidget((QOsciDevCtrl*)m_pDevCtrlManager->getDevCtrl(M_DEV_OSCII,devInfo.devItemlist[0].nIndex));
                pOsciCtrlWiget->setWindowTitle(devInfo.sdevTypeName);
                m_measureDevCtrlWidgetList.push_back(pOsciCtrlWiget);
            }
            break;

            case M_DEV_SHUTTER://光闸
            {
                QList<QShutterDevCtrl*>l_shutterctrllist;
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    l_shutterctrllist.push_back((QShutterDevCtrl*)m_pDevCtrlManager->getDevCtrl(M_DEV_SHUTTER,devItem.nIndex));
                }
                QShutterCtrlWidget* pShutterCtrlWidget = new QShutterCtrlWidget(l_shutterctrllist);//光闸
                //pShutterCtrlWidget->m_sDevTypeName = devInfo.sdevTypeName;
                pShutterCtrlWidget->setWindowTitle(devInfo.sdevTypeName);
                m_measureDevCtrlWidgetList.push_back(pShutterCtrlWidget);
            }
            break;
            case M_DEV_ENERGY_ADJ://能量调节模块
            {
                vector<tEnergyAdjDevConfig> pConfigList;
                for (tDevItem devItem:devInfo.devItemlist)
                {
                    tEnergyAdjDevConfig l_ItemConfigInfo;
                    l_ItemConfigInfo.pEnergyAdjDevCtrl = m_pDevCtrlManager->getDevCtrl(M_DEV_ENERGY_ADJ,devItem.nIndex);
                    l_ItemConfigInfo.sDevName = devItem.sDevName;
                    pConfigList.push_back(l_ItemConfigInfo);
                }

                QEnergyAdjDevWidget* pEnergyAdjDevWidget = new QEnergyAdjDevWidget(pConfigList);
                pEnergyAdjDevWidget->setWindowTitle(devInfo.sdevTypeName);
                m_moveDevCtrlWidgetlist.push_back(pEnergyAdjDevWidget);
            }
            break;

            default:
                break;

        }

    }
}
/******************************************
* 功能:初始化界面
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void MainWindow::InitUI()
{

    _pTopWidget = new QTopWidget("小口径损伤测试设备（光路运动及集中控制模块）",":png/appicon.png",this);//初始化顶部显示视图
    _pNavWidget = new QNavWidget(this);//导航栏


    //创建子组件
    _pDefectMeasureWidget = new DefectMeasureWidget(m_pDevCtrlManager,_pDBProcess,&_pPlatformConfig->m_tPlatformConfig,_pPlatformDevConfig,this);//如果需要数据库操作则填入数据库操作指针
    _pLightParamWidget = new LightParamWidget(m_pDevCtrlManager,_pDBProcess,&_pPlatformConfig->m_tPlatformConfig,_pPlatformDevConfig->m_mapChannelNameToInfo,this);//如果需要数据库操作则填入数据库操作指针
    _pMeasurePreWidget = new MeasurePreReadyWidget(m_pDevCtrlManager,_pDBProcess,&_pPlatformConfig->m_tPlatformConfig,_pPlatformDevConfig,this);;

    _pDevConfigWidget = new QDevConfigWidget(_pPlatformDevConfig,this);


    //添加导航栏信息 以及对应的组件信息
    int nGroupID = _pNavWidget->addGroup("流程控制",":png/dev.png");//添加分组
    _pNavWidget->addItemWidget(nGroupID,"测试准备",_pMeasurePreWidget,":png/zero.png");//添加子项
    _pNavWidget->addItemWidget(nGroupID,"光路参数测量",_pLightParamWidget,":png/zero.png");//添加子项
    _pNavWidget->addItemWidget(nGroupID,"损伤阈值测试",_pDefectMeasureWidget,":png/zero.png");//添加子项

    int nGroupID1 = _pNavWidget->addGroup("设备调试控制",":png/dev.png");
    for(QWidget* pItem:m_moveDevCtrlWidgetlist)
    {
        _pNavWidget->addItemWidget(nGroupID1,pItem->windowTitle(),pItem,":png/zero.png");
    }
    for (QWidget* pItem:m_measureDevCtrlWidgetList)
    {
        _pNavWidget->addItemWidget(nGroupID1,pItem->windowTitle(),pItem,":png/zero.png");
    }




    int nGroupID3 = _pNavWidget->addGroup("数据管理",":png/dev.png");
    _pNavWidget->addItemWidget(nGroupID3,"数据查询",new QOfflineDataWidget(_pDBProcess,this),":png/zero.png");



    int nGroupID4 = _pNavWidget->addGroup("系统管理工具",":png/dev.png");
    _pNavWidget->addItemWidget(nGroupID4,"设备信息管理",_pDevConfigWidget,":png/zero.png");

    //布局
    QWidget* pWidget = new QWidget(this);
    this->setWindowOpacity(1.0);
    QGridLayout* playout = new QGridLayout(pWidget);
    playout->setContentsMargins(0, 0, 0, 0);
    playout->setMargin(0);//去除边界空白
    playout->setSpacing(0);

    playout->addWidget(_pTopWidget,0,0,1,1);
    playout->addWidget(_pNavWidget,1,0,20,1);
    this->setCentralWidget(pWidget);
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint |Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);//无边框
    this->showMaximized();

}



/******************************************
* 功能:检查设备连接状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void MainWindow::checkDevStatus()
{
    QString sInfo = "";
    foreach (tDevInfo devInfo, _pPlatformDevConfig->m_tDevInfoList)
    {
        foreach (tDevItem devItem, devInfo.devItemlist)
        {
            if(!m_pDevCtrlManager->getDevStatus(devInfo.devType,devItem.nIndex))
            {
                 sInfo = sInfo +  QString("%1\n").arg(devItem.sDevName);
            }
        }
    }


    if(!sInfo.isEmpty())
    {
        QMessageBox::warning(this,"警告",QString("以下设备连接失败,请检查硬件连接情况!\n%1").arg(sInfo));
    }

}

/******************************************
* 功能:释放所有的控制类内存
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void MainWindow::releaseDevCtrl()//释放所有的控制类
{
    foreach (tDevInfo devInfo, _pPlatformDevConfig->m_tDevInfoList)
    {
        switch(devInfo.devType)
        {
            case M_DEV_NEARFIELD_CCD://近场CCD
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                      QPlatformKxccdCtrl* platformKxccdCtrl = static_cast<QPlatformKxccdCtrl*>(m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex)) ;
                      delete platformKxccdCtrl;
                      platformKxccdCtrl = nullptr;
                }
            }
            break;

            case M_DEV_FARFIELD_CCD://远场CCD
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformKxccdCtrl* pZzccdCtrl = static_cast<QPlatformKxccdCtrl*> (m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex));
                    delete pZzccdCtrl;
                }
            }
            break;

            case M_DEV_MOTOR://电机
            {
                for (tDevItem devItem:devInfo.devItemlist)
                {
                    QPlatformMotorDevCtrl* platformMotorDevCtrl =  static_cast<QPlatformMotorDevCtrl*>(m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex));
                    delete platformMotorDevCtrl;
                    platformMotorDevCtrl = nullptr;
                }

            }
            break;


            case M_DEV_ENERGY://能量计
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformEnergyDevCtrl* platformEnergyDevCtrl = static_cast<QPlatformEnergyDevCtrl*>(m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex));
                    delete platformEnergyDevCtrl;
                    platformEnergyDevCtrl = nullptr;
                }
            }
            break;

            case M_DEV_OSCII://示波器
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformOsciCtrl* platformOsciCtrl = static_cast<QPlatformOsciCtrl*>( m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex));
                    delete platformOsciCtrl;
                    platformOsciCtrl = nullptr;
                }

            }
            break;

            case M_DEV_SHUTTER://光闸
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformShutterCtrl* platformShutterCtrl = static_cast<QPlatformShutterCtrl*> (m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex));
                    delete platformShutterCtrl;
                    platformShutterCtrl = nullptr;
                }
            }
            break;

            case M_DEV_ENERGY_ADJ://能量调节模块
            {
                foreach (tDevItem devItem, devInfo.devItemlist)
                {
                    QPlatformEnergyAdjDevCtrl* platformEnergyAdjCtrl =static_cast<QPlatformEnergyAdjDevCtrl*>(m_pDevCtrlManager->getDevCtrl(devInfo.devType,devItem.nIndex));
                    delete platformEnergyAdjCtrl;
                    platformEnergyAdjCtrl = nullptr;
                 }
            }
            break;

            default:
            break;
        }

    }

    //数据处理模块
    QPlatformDataProcessCtrl* platformDataProcessCtrl = static_cast<QPlatformDataProcessCtrl*> (m_pDevCtrlManager->getDevCtrl(M_DEV_DATA_PROCESS,0));
    delete platformDataProcessCtrl;
    platformDataProcessCtrl = nullptr;

    //损伤检测系统
    QPlatformDefectCheckCtrl* platformDefectCheckCtrl =static_cast<QPlatformDefectCheckCtrl*> (m_pDevCtrlManager->getDevCtrl(M_DEV_DEFECTCHECK,0));
    delete platformDefectCheckCtrl;
    platformDefectCheckCtrl = nullptr;

}

