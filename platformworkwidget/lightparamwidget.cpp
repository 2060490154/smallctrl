/***************************************************************************
**                                                                        **
** 文件描述：光路参数测量界面
**                                                                        **
****************************************************************************
** 创建人：
** 创建时间：
** 修改记录：
**
****************************************************************************/
#include "lightparamwidget.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

LightParamWidget::LightParamWidget(QDevCtrlManager* pPlatformDevCtrlManager,CDbDataProcess* pDBProcess,tPlatformConfig* pPlatformConfig,QHash<QString,tChannelInfo>devNameToInfo,QWidget *parent) : QWidget(parent)
{
    m_pMeasureLightParamWorkflow = nullptr;

    m_nWaveLengthType = 0;

    m_tPlatformConfig = pPlatformConfig;

    _pPlatformDevCtrlManager = pPlatformDevCtrlManager;

    _tCommonInfo._pDBProcess = pDBProcess;

    _mapChannelNameToInfo = devNameToInfo;

    initUI();

    showParam();
    createMeasureWorkflow();


    //日志显示消息
    connect(_pMeasureLightParamWorkflowWidget,&QWorkflowWidget::showlogInfo,_pLogWidget,&QLogWidget::onShowlog);
    connect(pDBProcess,&CDbDataProcess::signal_showErrorInfo,_pLogWidget,&QLogWidget::onShowlog);

    //保存参数按钮
    connect(_pRefreshParamBtn,&QPushButton::clicked,this,&LightParamWidget::onSaveParam);

    //workflow完成的消息处理
    connect(_pMeasureLightParamWorkflowWidget,&QWorkflowWidget::finishedProcess,this,&LightParamWidget::onFinishWorkflow);

    //近场监视窗口
    QPlatformKxccdCtrl* pkxccdCtrl = static_cast<QPlatformKxccdCtrl*>(_pPlatformDevCtrlManager->getDevCtrl(M_DEV_NEARFIELD_CCD,0));
    connect(pkxccdCtrl,&QPlatformKxccdCtrl::signal_devdata,_pMonitKxccdImageWidget,&QMonitKxccdImgWidget::onUpdateImage);

    //监视近场图像处理结果
    QPlatformDataProcessCtrl* pDataProcess = static_cast<QPlatformDataProcessCtrl*>(_pPlatformDevCtrlManager->getDevCtrl(M_DEV_DATA_PROCESS,0));//数据处理模块
    connect(pDataProcess,&QPlatformDataProcessCtrl::signal_devdata,_pMonitKxccdImageWidget,&QMonitKxccdImgWidget::onUpdateImage);

    //能量数据采集
    QPlatformEnergyDevCtrl* pEnergyDevCtrl = static_cast<QPlatformEnergyDevCtrl*>(_pPlatformDevCtrlManager->getDevCtrl(M_DEV_ENERGY,0));
    connect(pEnergyDevCtrl,&QPlatformEnergyDevCtrl::signal_devdata,_pMonitEnergyCoeffWidget,&QMonitEnergyCoeffWidget::onUpdateEnergyData);


    //数据处理结果监视
    pDataProcess = static_cast<QPlatformDataProcessCtrl*>(_pPlatformDevCtrlManager->getDevCtrl(M_DEV_DATA_PROCESS,0));//数据处理模块
    connect(pDataProcess,&QPlatformDataProcessCtrl::signal_devdata,_pMonitEnergyCoeffWidget,&QMonitEnergyCoeffWidget::onUpdateEnergyData);

    //科学CCD保护
    connect(_pMeasureLaserAreaCheckBox,&QCheckBox::clicked,this,&LightParamWidget::onMeasureLaserAreaCheck);


}

LightParamWidget::~LightParamWidget()
{

    if(m_pMeasureLightParamWorkflow != nullptr)
    {
        m_pMeasureLightParamWorkflow->clearAllTask();
        delete m_pMeasureLightParamWorkflow;
    }

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QWidget* LightParamWidget::createParamWidget()
{
    QWidget* pwidget = new QWidget(this);

    _pCurrentWaveLengthLabel = new QLabel();

    _pCurrentWaveLengthLabel->setStyleSheet("color:blue;font-size:16px;");

    _pLaserInputEnergyEdit = new QTextEdit("0.0");

    _pRefreshParamBtn = new QPushButton("保存参数",pwidget);//参数保存按钮

    _pEnergySectionEdit = new QLineEdit("0.0",pwidget);//能量衰减比例
    _pMeasureLaserAreaCheckBox = new QCheckBox("光斑面积测量",pwidget);//光斑面积
    _pMeasureLaserPlusCheckBox = new QCheckBox("脉宽测量",pwidget);//脉宽
    _pMeasureLightCoeffCheckBox = new QCheckBox("光路分光比系数",pwidget);//光路分光比系数

    _pCurrentLaserAreaLabel = new QLabel(pwidget);
    _pCurrentLightCoeffLabel = new QLabel(pwidget);
    _pCurrentPlusWidthLabel = new QLabel(pwidget);

    _pCurrentLaserAreaLabel->setMaximumHeight(30);
    _pCurrentLightCoeffLabel->setMaximumHeight(30);
    _pCurrentPlusWidthLabel->setMaximumHeight(30);
    _pEnergySectionEdit->setMinimumHeight(30);
    _pCurrentWaveLengthLabel->setMaximumHeight(30);
    _pLaserInputEnergyEdit->setMaximumHeight(30);

    QGridLayout* playout = new QGridLayout(pwidget);


    int nRowCnt = 0;
    playout->addWidget(new QLabel("当前光路:"),nRowCnt,0,1,1);
    playout->addWidget(_pCurrentWaveLengthLabel,nRowCnt,1,1,1);

    nRowCnt++;

    playout->addWidget(new QLabel("能量设置(%):"),nRowCnt,0,1,1);
    playout->addWidget(_pEnergySectionEdit,nRowCnt,1,1,1);

    playout->addWidget(new QLabel("激光器输出能量(mJ):"),nRowCnt,2,1,1);
    playout->addWidget(_pLaserInputEnergyEdit,nRowCnt,3,1,1);


    nRowCnt++;

    playout->addWidget(new QLabel("测量参数选择:"),nRowCnt,0,1,1);
    playout->addWidget(_pMeasureLaserAreaCheckBox,nRowCnt,1,1,1);
    playout->addWidget(_pMeasureLaserPlusCheckBox,nRowCnt,2,1,1);
    playout->addWidget(_pMeasureLightCoeffCheckBox,nRowCnt,3,1,1);

    nRowCnt++;

    playout->addWidget(new QLabel("光斑面积(cm²):"),nRowCnt,0,1,1);
    playout->addWidget(_pCurrentLaserAreaLabel,nRowCnt,1,1,1);
    playout->addWidget(new QLabel("分光比:"),nRowCnt,2,1,1);
    playout->addWidget(_pCurrentLightCoeffLabel,nRowCnt,3,1,1);
    playout->addWidget(new QLabel("脉宽(ns):"),nRowCnt,4,1,1);
    playout->addWidget(_pCurrentPlusWidthLabel,nRowCnt,5,1,1);

    nRowCnt++;

    playout->addWidget(_pRefreshParamBtn,nRowCnt,5,1,1);


    for(int i = 0; i < 6; i++)
    {
        playout->setColumnStretch(i,1);
    }
    for(int i = 0; i <= nRowCnt; i++)
    {
        playout->setRowStretch(i,1);
    }

    setWidgetStyleSheet(pwidget);

    return pwidget;

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void LightParamWidget::initUI()
{
    _pMeasureLightParamWorkflowWidget = new QWorkflowWidget(_pPlatformDevCtrlManager,bindCheckConditionFun(LightParamWidget::checkRunCondition,this));

   _pMonitKxccdImageWidget = new QMonitKxccdImgWidget(this);
   _pMonitEnergyCoeffWidget = new QMonitEnergyCoeffWidget(this);


   _pMonitWidgetTab = new QTabWidget(this);
   _pMonitWidgetTab->addTab(_pMonitKxccdImageWidget,"近场CCD图像");
   _pMonitWidgetTab->addTab(_pMonitEnergyCoeffWidget,"分光比测量数据");




    _pLogWidget = new QLogWidget(this);


    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0,2);
    playout->setRowStretch(1,2);
    playout->setRowStretch(2,1);
    for(int i = 0; i < 6; i++)
    {
        playout->setColumnStretch(i,1);
    }

    playout->addWidget(createParamWidget(),0,0,1,4);

    playout->addWidget(_pMonitWidgetTab,0,4,2,2);//数据监视窗口

    playout->addWidget(_pMeasureLightParamWorkflowWidget,1,0,1,4);

    playout->addWidget(_pLogWidget,2,0,1,7);


}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void LightParamWidget::createMeasureWorkflow()
{
    if(m_pMeasureLightParamWorkflow == nullptr)
    {
        m_pMeasureLightParamWorkflow = new Workflow("光路参数测量",&_tCommonInfo);
    }
    m_pMeasureLightParamWorkflow->clearAllTask();

    //能量透过比例
    float lfdiscreamValue = _pEnergySectionEdit->text().toFloat();
    float lfEnergy = _pLaserInputEnergyEdit->toPlainText().toFloat();

    //能量计参数
    QVariant l_energy_dev_measure_var,l_energy_dev_monit_var;
    st_EnergyDevTaskInputData l_energy_data;
    l_energy_data.ncapDataCnt = M_ENERGY_COEFF_CNT;
//    l_energy_data.nMeasureRange = 21;
    l_energy_data.bSaveData = false;
    l_energy_data.nWavelength = m_nWaveLengthType ==0?1064:355;


    //主能量计
    l_energy_data.nMeasureRange = l_energy_data.caclMeasureRange(lfEnergy,lfdiscreamValue,M_MAIN_ENERGY_SAMPLE);
    l_energy_data.lfCoeff = _mapChannelNameToInfo.value("主能量计").lfCoeff;
    l_energy_data.nChannelNo = _mapChannelNameToInfo.value("主能量计").nChannelNo;
    l_energy_data.bInitMode = true;
    l_energy_dev_measure_var.setValue(l_energy_data);//主能量计

    //监视能量计
    l_energy_data.nMeasureRange = l_energy_data.caclMeasureRange(lfEnergy,lfdiscreamValue,M_MONIT_ENERGY_SAMPLE);
    l_energy_data.lfCoeff = _mapChannelNameToInfo.value("监视能量计").lfCoeff;
    l_energy_data.nChannelNo = _mapChannelNameToInfo.value("监视能量计").nChannelNo;
    l_energy_data.bInitMode = true;
    l_energy_dev_monit_var.setValue(l_energy_data);//监视能量计

    //CCD采集
    st_ccdinputdata l_st_ccdinputdata;
    l_st_ccdinputdata.nActionType = 0x02;//光斑面积测量
    l_st_ccdinputdata.nImageCnt = 10;//采集十副图
    QVariant l_ccdtask_var;
    l_ccdtask_var.setValue(l_st_ccdinputdata);


   //光闸参数
   st_shutterparam l_st_shutterparam;
   l_st_shutterparam.i32Mode = 0x01;//正常模式
   l_st_shutterparam.i32Param = 0x0;//关闭
   QVariant l_shutter_close_var,l_shutter_open_var,l_shutter_process;
   l_shutter_close_var.setValue(l_st_shutterparam);
   l_st_shutterparam.i32Param = 1;//打开
   l_shutter_open_var.setValue(l_st_shutterparam);
   l_st_shutterparam.i32Mode = 0x02;//流程模式
   l_st_shutterparam.i32Param = 0x1;//放一个脉冲
   l_shutter_process.setValue(l_st_shutterparam);

   int nShutterDevNo = m_nWaveLengthType == 0?0:1;
   QString sShutterName = m_nWaveLengthType == 0?"1064nm光闸":"355nm光闸";

   int nAdjustEnergyDevNo = m_nWaveLengthType == 0?0:1;


   //光斑面积数据处理
   st_dataprocessInput l_dataprocessInput;
   l_dataprocessInput.nProcessType = E_MEASUREPARAM_AREA;
   QVariant l_dataprocess_area_var;
   l_dataprocess_area_var.setValue(l_dataprocessInput);

   //分光比数据处理参数
   l_dataprocessInput.nProcessType = E_MEASUREPARAM_COEFF;
   l_dataprocessInput.nParam1 = _mapChannelNameToInfo.value("主能量计").nChannelNo;
   l_dataprocessInput.nParam2 = _mapChannelNameToInfo.value("监视能量计").nChannelNo;
   QVariant l_dataprocess_coeff_var;
   l_dataprocess_coeff_var.setValue(l_dataprocessInput);

   //电机参数
   st_motorInputdata l_motor_input;
   QVariant l_motor_var;



    //准备流程
    vector<Tasklist*> l_standbyTasklist = createStandByTasklist();
    for(Tasklist* pTasklist:l_standbyTasklist)
    {
        m_pMeasureLightParamWorkflow->addTaskList(pTasklist);
    }


    Tasklist* pTasklist = new Tasklist("测量设备准备");
    m_pMeasureLightParamWorkflow->addTaskList(pTasklist);

    //能量衰减轮盘调节
    int i32EnergyHomeTask = pTasklist->addTask("能量调节模块归零",M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_SETPARAM_ACTION,QVariant(0),0);
    pTasklist->addTask(QString("能量调节至%1%").arg(lfdiscreamValue),M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_PROCESS_ACTION,QVariant(lfdiscreamValue),0,{i32EnergyHomeTask});

    if(_pMeasureLaserAreaCheckBox->isChecked())//进行面积测量 CCD设置工作模式
    {
        pTasklist->addTask("近场相机设置外触发模式",M_DEV_NEARFIELD_CCD,0,M_SETPARAM_ACTION,QVariant(0x02));
        //衰减轮盘设置
        int nGearIndex = m_tPlatformConfig->getGearIndex(0x0,E_MOTOR_NEARFIELD_WHEEL,lfdiscreamValue*lfEnergy/100.0);

        l_motor_input.nMotorIndex = E_MOTOR_NEARFIELD_WHEEL;
        l_motor_input.nMoveType = 0x01;
        l_motor_input.nPlusCnt = nGearIndex;
        l_motor_var.setValue(l_motor_input);
        pTasklist->addTask(QString("衰减轮盘设置档位:%1").arg(nGearIndex),M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var);

    }
    else
    {
        l_motor_input.nMoveType = 0x01;
        l_motor_input.nMotorIndex = E_MOTOR_NEARFIELD_WHEEL;
        l_motor_input.nPlusCnt = 0x0;
        l_motor_var.setValue(l_motor_input);
        pTasklist->addTask("CCD衰减轮盘至保护位",M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var);
    }



    if(_pMeasureLightCoeffCheckBox->isChecked())//进行分光比测试 设置波长
    {
        int taskid = pTasklist->addTask("监视能量计设置",M_DEV_ENERGY,0,M_SETPARAM_ACTION,l_energy_dev_monit_var/*,0,{taskid}*/);
        int taskid0 = pTasklist->addTask("主能量计设置",M_DEV_ENERGY,0,M_SETPARAM_ACTION,l_energy_dev_measure_var,100,{taskid});

    }

    if(_pMeasureLaserPlusCheckBox->isChecked())//进行脉宽测试 设置示波器为触发状态
    {
        pTasklist->addTask("设置示波器参数",M_DEV_OSCII,0,M_SETPARAM_ACTION);
    }


    //采集流程
    Tasklist* pMeasureTasklist = new Tasklist("数据采集");
    m_pMeasureLightParamWorkflow->addTaskList(pMeasureTasklist);

    int i32TaskIndex = pMeasureTasklist->addTask(sShutterName+"打开",M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_open_var);

    if(_pMeasureLaserAreaCheckBox->isChecked())//面积测量 数据采集
    {
        pMeasureTasklist->addTask("采集图像数据",M_DEV_NEARFIELD_CCD,0,M_PROCESS_ACTION,l_ccdtask_var,0,{i32TaskIndex});//0x02--面积测量
    }

    if(_pMeasureLightCoeffCheckBox->isChecked())//分光比测量 数据采集
    {
        pMeasureTasklist->addTask("主能量计采集数据",M_DEV_ENERGY,0,M_PROCESS_ACTION,l_energy_dev_measure_var,0,{i32TaskIndex});
        pMeasureTasklist->addTask("监视能量计采集数据",M_DEV_ENERGY,0,M_PROCESS_ACTION,l_energy_dev_monit_var,0,{i32TaskIndex});
    }

    if(_pMeasureLaserPlusCheckBox->isChecked())//脉宽测量 数据采集
    {
        pMeasureTasklist->addTask("示波器采集脉宽数据",M_DEV_OSCII,0,M_PROCESS_ACTION,0,0,{i32TaskIndex});
    }



    //数据处理流程
    Tasklist* pDataProcess = new Tasklist("数据处理");
    m_pMeasureLightParamWorkflow->addTaskList(pDataProcess);

    pDataProcess->addTask(sShutterName+"关闭",M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_close_var);


    if(_pMeasureLaserAreaCheckBox->isChecked())//光斑面积测量 数据处理
    {
        pDataProcess->addTask("计算光斑面积",M_DEV_DATA_PROCESS,0,M_PROCESS_ACTION,l_dataprocess_area_var);//0x02--面积测量
    }

    if(_pMeasureLightCoeffCheckBox->isChecked())//分光比测量 数据处理
    {
        pDataProcess->addTask("计算能量系数",M_DEV_DATA_PROCESS,0,M_PROCESS_ACTION,l_dataprocess_coeff_var);
    }

    if(_pMeasureLaserPlusCheckBox->isChecked())//脉宽测量 数据处理
    {
       // pDataProcess->addTask("计算能量系数",M_DEV_DATA_PROCESS,0,M_PROCESS_ACTION,l_dataprocess_coeff_var);
    }


    //结束流程
    Tasklist* pFinishTask = new Tasklist("结束流程");
    m_pMeasureLightParamWorkflow->addTaskList(pFinishTask);
    int finishTaskIndex = pFinishTask->addTask(sShutterName+"关闭",M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_close_var);
    pFinishTask->addTask("载物台光闸关闭",M_DEV_SHUTTER,2,M_PROCESS_ACTION,l_shutter_close_var);

    if(_pMeasureLaserAreaCheckBox->isChecked())//光斑面积测量 数据处理
    {
        l_motor_input.nMoveType = 0x01;
        l_motor_input.nMotorIndex = E_MOTOR_NEARFIELD_WHEEL;
        l_motor_input.nPlusCnt = 0x0;
        l_motor_var.setValue(l_motor_input);
        pFinishTask->addTask("CCD衰减轮盘至保护位",M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{finishTaskIndex});
    }

    _pMeasureLightParamWorkflowWidget->setWorkflow(m_pMeasureLightParamWorkflow);
}

vector<Tasklist*> LightParamWidget::createStandByTasklist()
{
    vector<Tasklist*> l_tasklist;

    st_motorInputdata l_motor_input;
    QVariant l_var;
    l_motor_input.nMoveType = 0x01;


    //光闸参数

    st_shutterparam l_st_shutterparam;
    l_st_shutterparam.i32Mode = 0x01;//正常模式
    l_st_shutterparam.i32Param = 0x0;//关闭
    QVariant l_shutter_close_var,l_shutter_open_var;
    l_shutter_close_var.setValue(l_st_shutterparam);
    l_st_shutterparam.i32Param = 1;//打开
    l_shutter_open_var.setValue(l_st_shutterparam);

    //准备流程
    Tasklist* pShutterTasklist = new Tasklist("光闸关闭");
    l_tasklist.push_back(pShutterTasklist);
    pShutterTasklist->addTask("1064nm光闸关闭",M_DEV_SHUTTER,0,M_PROCESS_ACTION,l_shutter_close_var);
    pShutterTasklist->addTask("355nm光闸关闭",M_DEV_SHUTTER,1,M_PROCESS_ACTION,l_shutter_close_var);

    if(_pMeasureLightCoeffCheckBox->isChecked())//只有分光比测试时 才需要打开载物台光闸
    {
        pShutterTasklist->addTask("载物台光闸打开",M_DEV_SHUTTER,2,M_PROCESS_ACTION,l_shutter_open_var);
    }
    else
    {
        pShutterTasklist->addTask("载物台光闸关闭",M_DEV_SHUTTER,2,M_PROCESS_ACTION,l_shutter_close_var);
    }

//    if(m_nWaveLengthType == 0 )
//    {
//        Tasklist* pStandbytasklist = new Tasklist("切换光路至1064nm");
//        l_tasklist.push_back(pStandbytasklist);

//        l_motor_input.nMotorIndex = E_MOTOR_LIGHT_CHANGE;
//        l_motor_input.nPlusCnt = 0x01;//to be confirm
//        l_var.setValue(l_motor_input);
//        pStandbytasklist->addTask("平移台切出",M_DEV_MOTOR,0,M_PROCESS_ACTION,l_var);

//        l_motor_input.nMotorIndex = E_MOTOR_PLATFORM;
//        l_motor_input.nPlusCnt = 0x01;//to be confirm
//        l_var.setValue(l_motor_input);
//        pStandbytasklist->addTask("光路调节平移台",M_DEV_MOTOR,0,M_PROCESS_ACTION,l_var);
//    }

//    if(m_nWaveLengthType== 1 )
//    {
//        Tasklist* pStandbytasklist = new Tasklist("切换光路至355nm");
//        l_tasklist.push_back(pStandbytasklist);

//        l_motor_input.nMotorIndex = E_MOTOR_LIGHT_CHANGE;
//        l_motor_input.nPlusCnt = 0x02;//to be confirm
//        l_var.setValue(l_motor_input);
//        pStandbytasklist->addTask("平移台切入",M_DEV_MOTOR,0,M_PROCESS_ACTION,l_var);

//        l_motor_input.nMotorIndex = E_MOTOR_PLATFORM;
//        l_motor_input.nPlusCnt = 0x02;//to be confirm
//        l_var.setValue(l_motor_input);
//        pStandbytasklist->addTask("光路调节平移台",M_DEV_MOTOR,0,M_PROCESS_ACTION,l_var);
//    }

    return l_tasklist;

}

/******************************************
* 功能:显示参数
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void LightParamWidget::showParam()
{
    m_nWaveLengthType = m_tPlatformConfig->m_nCurrentWaveLength;

    float lfCurrentArea = m_tPlatformConfig->m_st_laserParam[m_nWaveLengthType].m_lfArea;
    float lfCurrentCoeff = m_tPlatformConfig->m_st_laserParam[m_nWaveLengthType].m_lfK;
    float lfPlusWidth = m_tPlatformConfig->m_st_laserParam[m_nWaveLengthType].m_lfPlusWidth;

    _pCurrentLaserAreaLabel->setText(QString::number(lfCurrentArea));
    _pCurrentLightCoeffLabel->setText(QString::number(lfCurrentCoeff));
    _pCurrentPlusWidthLabel->setText(QString::number(lfPlusWidth));


    _pCurrentWaveLengthLabel->setText(m_tPlatformConfig->m_st_laserParam[m_tPlatformConfig->m_nCurrentWaveLength].m_sLaserName);

}

/******************************************
* 功能:保存参数 更新显示
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void LightParamWidget::onSaveParam()
{
    if(_pRefreshParamBtn->text()=="保存参数")
    {
        float lfdiscreamValue = _pEnergySectionEdit->text().toFloat();

        if(lfdiscreamValue < 0.0 || lfdiscreamValue > 100.0 || _pEnergySectionEdit->text().isEmpty())
        {
            QMessageBox::warning(this,"提示","能量透过率设置错误，该数据范围为[0,100],请重新设置！");
            return;
        }

        if(_pMeasureLightCoeffCheckBox->isChecked())//分光比测量时需要知道激光器输出能量
        {
            float lfEnergy = _pLaserInputEnergyEdit->toPlainText().toFloat();

            if(lfEnergy <= 0.0 ||_pLaserInputEnergyEdit->toPlainText().isEmpty())
            {
                QMessageBox::warning(this,"提示","请输入激光器输出能量！");
                return;
            }
        }

        //灰化按钮
        _pMeasureLaserAreaCheckBox->setEnabled(false);
        _pMeasureLaserPlusCheckBox->setEnabled(false);
        _pMeasureLightCoeffCheckBox->setEnabled(false);
        _pEnergySectionEdit->setEnabled(false);
        _pLaserInputEnergyEdit->setEnabled(false);

        //更新测量流程
        createMeasureWorkflow();

        _pRefreshParamBtn->setText("修改参数");

    }
    else
    {
        _pMeasureLaserAreaCheckBox->setEnabled(true);
        _pMeasureLaserPlusCheckBox->setEnabled(true);
        _pMeasureLightCoeffCheckBox->setEnabled(true);
        _pEnergySectionEdit->setEnabled(true);
        _pLaserInputEnergyEdit->setEnabled(true);

        _pRefreshParamBtn->setText("保存参数");
    }

}




 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void LightParamWidget::setWidgetStyleSheet(QWidget* pWidget)
 {
     QPalette pal(pWidget->palette());
     pal.setColor(QPalette::Background, QColor(255,255,255));
     pWidget->setAutoFillBackground(true);
     pWidget->setPalette(pal);

     QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
     shadowEffect->setOffset(0, 0);// 阴影偏移
     shadowEffect->setColor(QColor(180,180,180));// 阴影颜色;
     shadowEffect->setBlurRadius(10);// 阴影半径;
     pWidget->setGraphicsEffect(shadowEffect);// 给窗口设置上当前的阴影效果;

 }

/******************************************
* 功能:检查流程运行时是否满足条件
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 bool LightParamWidget::checkRunCondition(QString sworkflowName)
 {
    if(_pRefreshParamBtn->text() !="修改参数")
    {
        QMessageBox::warning(this,"提示","请先保存参数!");
        return false;
    }

    updateCommonInfo();//更新公共数据

    if(_pMeasureLaserAreaCheckBox->isChecked())//光斑面积测量 清空缓存文件夹
    {
        QDir l_dir;//清空文件夹内容
        QString sPath = QApplication::applicationDirPath()+"/tempdata";
        if(l_dir.exists(sPath))
        {
            QDirIterator DirsIterator(sPath, QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
            while(DirsIterator.hasNext())
            {
                if (!l_dir.remove(DirsIterator.next())) // 删除文件操作如果返回否，那它就是目录
                {
                    QDir(DirsIterator.filePath()).removeRecursively(); // 删除目录本身以及它下属所有的文件及目录
                }
            }
        }
        else
        {
             l_dir.mkdir(sPath);
        }
        _pMonitKxccdImageWidget->clearAllImage();
    }

    if(_pMeasureLightCoeffCheckBox->isChecked())
    {
        _pMonitEnergyCoeffWidget->clearAllData();
    }


    //存储当前实验信息
    if(!_tCommonInfo._pDBProcess->onSaveExpInfo(_tCommonInfo.sExpNo,_tCommonInfo.sSampleName,_tCommonInfo.nMeasureType,_tCommonInfo.nwavelengthType,0.0,0.0,0.0,0.0,0.0))
    {
        return false;
    }


    return true;
 }

 /******************************************
 * 功能:更新公共信息 用于后续流程数据处理和数据保存使用
 * 输入:
 * 输出:
 * 返回值:
 * 维护记录:
 ******************************************/
 void LightParamWidget::updateCommonInfo()
 {
     _tCommonInfo.nMeasureType = M_MEASURETYPE_LASERPARAM;
     _tCommonInfo.nwavelengthType = m_tPlatformConfig->m_nCurrentWaveLength;
     _tCommonInfo.sExpNo = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");// 生成实验编号
 }

 /******************************************
 * 功能:工作流程结束后的处理
 * 输入:sworkflowName为流程名称 taskstatus执行结果
 * 输出:
 * 返回值:
 * 维护记录:
 ******************************************/
 void LightParamWidget::onFinishWorkflow(QString sworkflowName,_Taskstatus taskstatus)
 {
     float lfM = 0.0;
     float lfArea  = -1.0;
     float lfCoeff  = -1.0;
     float lfPlusWidth  = -1.0;
     if(_tCommonInfo.nMeasureType == M_MEASURETYPE_LASERPARAM&& taskstatus == M_TASK_STATUS_SUCCESS)//参数测量 且执行成功
     {
         for(tTaskData l_taskdata:_tCommonInfo.tTaskDatalist)
         {
             if(l_taskdata.i32DevType == M_DEV_DATA_PROCESS && l_taskdata.i32DevNo == E_MEASUREPARAM_AREA)
             {

                 st_areaParam l_st_area_param = l_taskdata.vardata.value<st_areaParam>();
                 lfArea = l_st_area_param.lfArea;
                 lfM = l_st_area_param.lfM;
             }
             if(l_taskdata.i32DevType == M_DEV_DATA_PROCESS && l_taskdata.i32DevNo == E_MEASUREPARAM_COEFF)
             {
                 lfCoeff = l_taskdata.vardata.value<float>();
             }
             if(l_taskdata.i32DevType == M_DEV_OSCII)
             {
                 lfPlusWidth = l_taskdata.vardata.value<float>();
             }
         }

         if(lfArea >= 0.0)
         {
             QString str = QString("测量的光斑面积为%1 调制度为%2,是否更新该数据?").arg(lfArea).arg(lfM);
             if(QMessageBox::information(this,"光斑面积更新提示",str,QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
             {
                 //更新配置文件
                m_tPlatformConfig->m_st_laserParam[m_nWaveLengthType].m_lfArea = lfArea;
                m_tPlatformConfig->m_st_laserParam[m_nWaveLengthType].m_lfM = lfM;
                emit signal_updatePlatformFile();

                //保存数据至数据库中
                _tCommonInfo._pDBProcess->onSaveLightParamResult(_tCommonInfo.sExpNo,_tCommonInfo.nwavelengthType,E_MEASUREPARAM_AREA,lfArea);
             }
         }

         if(lfCoeff >= 0.0)
         {
             QString str = QString("测量的分光比为%1,是否更新该数据?").arg(lfCoeff);
             if(QMessageBox::information(this,"分光比更新提示",str,QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
             {
                   //更新配置文件
                m_tPlatformConfig->m_st_laserParam[m_nWaveLengthType].m_lfK = lfCoeff;
                emit signal_updatePlatformFile();

                //保存数据至数据库中
                _tCommonInfo._pDBProcess->onSaveLightParamResult(_tCommonInfo.sExpNo,_tCommonInfo.nwavelengthType,E_MEASUREPARAM_COEFF,lfCoeff);

             }
         }

         if(lfPlusWidth >= 0.0)
         {
             QString str = QString("测量的脉宽为%1,是否更新该数据?").arg(lfPlusWidth);
             if(QMessageBox::information(this,"脉宽数据更新提示",str,QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
             {
                   //更新配置文件
                m_tPlatformConfig->m_st_laserParam[m_nWaveLengthType].m_lfPlusWidth = lfPlusWidth;
                emit signal_updatePlatformFile();

                //保存数据至数据库中
                _tCommonInfo._pDBProcess->onSaveLightParamResult(_tCommonInfo.sExpNo,_tCommonInfo.nwavelengthType,E_MEASUREPARAM_PLUS_WIDTH,lfPlusWidth);

             }

         }

     }

     //更新显示
     showParam();

 }


 void LightParamWidget:: onMeasureLaserAreaCheck()
 {
     if(_pMeasureLaserAreaCheckBox->isChecked())//光斑面积测量
     {
         _pEnergySectionEdit->setText("1.0");
         _pEnergySectionEdit->setEnabled(false);
     }
     else
     {
         _pEnergySectionEdit->setEnabled(true);
     }

 }

