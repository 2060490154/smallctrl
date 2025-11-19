/***************************************************************************
**                                                                        **
** 文件描述：损伤测试界面
**                                                                        **
****************************************************************************
** 创建人：
** 创建时间：
** 修改记录：
**
****************************************************************************/
#include "defectMeasurewidget.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

DefectMeasureWidget::DefectMeasureWidget(QDevCtrlManager* pPlatformDevCtrlManager,CDbDataProcess* pDBProcess,tPlatformConfig* pPlatformConfig,QDevConfig* pDevConfig,QWidget *parent) : QWidget(parent)
{

    m_p1On1workflow = nullptr;

    m_pROn1workflow = nullptr;

    _pPointList =  nullptr;

    _pMeasureReslutWidget = nullptr;

    m_tPlatformConfig = pPlatformConfig;

    _pPlatformDevCtrlManager = pPlatformDevCtrlManager;

    _tCommonInfo._pDBProcess = pDBProcess;

    _pUpdatePointStatusTimer = new QTimer(this);//刷新点状态定时器

    _mapChannelNameToInfo = pDevConfig->m_mapChannelNameToInfo;

    _pDevConfig = pDevConfig;

    initUI();

    showParam();

    //扫描点参数更新
    _pPointAreaWidget->onUpdatePointUI(0x01,_tWorkflowParam.m_dStep,_tWorkflowParam.m_dWidth,_tWorkflowParam.m_dHigh);


    //测试流程相关


    QVariant l_var;
    l_var.setValue(_tWorkflowParam);
    onUpdateWorkflow(l_var);//更新流程


    //日志显示消息
 //   connect(_pStandbyWorkflowWiget,&QWorkflowWidget::showlogInfo,this,&DefectMeasureWidget::onShowlog);
    connect(_pMeasureWorkflowWiget,&QWorkflowWidget::showlogInfo,_pLogWidget,&QLogWidget::onShowlog);
    connect(pDBProcess,&CDbDataProcess::signal_showErrorInfo,_pLogWidget,&QLogWidget::onShowlog);

    //更新显示和更新流程
    connect(_pParamWidget,&ParamWidget::updatePointUI,_pPointAreaWidget,&QPointAreaWidget::onUpdatePointUI);
    connect(_pParamWidget,&ParamWidget::updateWorkFlow,this,&DefectMeasureWidget::onUpdateWorkflow);

    //定时器
    connect(_pUpdatePointStatusTimer,&QTimer::timeout,this,&DefectMeasureWidget::onUpdatePointStatus);

    //workflow完成的消息处理
  //  connect(_pStandbyWorkflowWiget,&QWorkflowWidget::finishedProcess,this,&DefectMeasureWidget::onFinishWorkflow);
    connect(_pMeasureWorkflowWiget,&QWorkflowWidget::finishedProcess,this,&DefectMeasureWidget::onFinishWorkflow);

    //显示检测过程图像  目前没有使用
    QPlatformDefectCheckCtrl* pdefectctrl = static_cast<QPlatformDefectCheckCtrl*>(_pPlatformDevCtrlManager->getDevCtrl(M_DEV_DEFECTCHECK,0));
    connect(pdefectctrl,&QPlatformDefectCheckCtrl::signal_showdefectimage,_pDefectCheckImageWidget,&QDefectCheckImageWidget::onshowImage);

    //启动点位状态刷新定时器
    _pUpdatePointStatusTimer->start(500);

}

DefectMeasureWidget::~DefectMeasureWidget()
{
    if(_pUpdatePointStatusTimer->isActive())
    {
        _pUpdatePointStatusTimer->stop();
    }

    if(m_p1On1workflow != nullptr)
    {
        m_p1On1workflow->clearAllTask();
        delete m_p1On1workflow;
    }


    if(m_pROn1workflow != nullptr)
    {
        m_pROn1workflow->clearAllTask();
        delete m_pROn1workflow;
    }


}

void DefectMeasureWidget::initUI()
{

    _pMeasureWorkflowWiget = new QWorkflowWidget(_pPlatformDevCtrlManager,bindCheckConditionFun(DefectMeasureWidget::checkRunCondition,this));

    _pPointAreaWidget = new QPointAreaWidget(100,100,M_SIZETYPE_RECT,10);


    _pLogWidget = new QLogWidget(this);

    _pTabWidget = new QTabWidget();

    _pParamWidget = new ParamWidget(this);
    _pParamWidget->setMaximumHeight(300);

    _pDefectCheckImageWidget = new QDefectCheckImageWidget();
    _pDefectCheckImageWidget->setModal(false);

    //_pTabWidget->addTab(_pPointAreaWidget,"损伤点状态显示");
   // _pTabWidget->addTab(_pDefectCheckImageWidget,"检测图像");

    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0,4);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,1);
    for(int i = 0; i < 6; i++)
    {
        playout->setColumnStretch(i,1);
    }

    playout->addWidget(_pParamWidget,0,0,1,4);
    playout->addWidget(_pPointAreaWidget,0,4,3,2);

//    playout->addWidget(_pStandbyWorkflowWiget,1,0,2,2);
    playout->addWidget(_pMeasureWorkflowWiget,1,0,2,4);
    playout->addWidget(_pLogWidget,3,0,1,7);

}

/******************************************
* 功能:创建1On1流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void DefectMeasureWidget::create1On1workflow()
{
    double l_dMoveStep_x = 0.0;
    double l_dMoveStep_y = 0.0;
    double l_sum_move_x = 0.0;//x轴运动所有距离
    double l_sum_move_y = 0.0;//y轴运动所有距离
    double l_dxPos_bak = 0.0;
    double l_dyPos_bak= 0.0;
    QString sStepName;

    if(m_p1On1workflow == nullptr)
    {
        m_p1On1workflow = new Workflow("1On1测试流程",&_tCommonInfo);
    }
    m_p1On1workflow->clearAllTask();

    if(_pPointList == nullptr)
    {
        return;
    }

    //电机参数
    st_motorInputdata l_motor_input;
    QVariant l_motor_var;

    //光闸参数
    st_shutterparam l_st_shutterparam;
    l_st_shutterparam.i32Mode = 0x01;//正常模式
    l_st_shutterparam.i32Param = 0x0;//关闭
    QVariant l_shutter_close_var;//光闸关闭任务变量
    QVariant l_shutter_open_var;//光闸打开任务变量
    QVariant l_shutter_process;//光闸走流程时任务变量
    l_shutter_close_var.setValue(l_st_shutterparam);
    l_st_shutterparam.i32Param = 1;//打开
    l_shutter_open_var.setValue(l_st_shutterparam);
    l_st_shutterparam.i32Mode = 0x02;//流程模式
    l_st_shutterparam.i32Param = 0x1;//放一个脉冲
    l_shutter_process.setValue(l_st_shutterparam);


   int nShutterDevNo = _tWorkflowParam.m_nWaveLengthType == 0?0:1;
   QString sShutterName = _tWorkflowParam.m_nWaveLengthType == 0?"1064nm光闸":"355nm光闸";

   int nAdjustEnergyDevNo = _tWorkflowParam.m_nWaveLengthType == 0?0:1;

   //能量计参数
   QVariant l_energy_dev_monit_var;
   st_EnergyDevTaskInputData l_energy_data;
   l_energy_data.ncapDataCnt = 1;
   l_energy_data.bSaveData = true;
   l_energy_data.nMeasureRange = 20;
   l_energy_data.nWavelength = _tWorkflowParam.m_nWaveLengthType ==0?1064:355;
   l_energy_data.lfCoeff = _mapChannelNameToInfo.value("监视能量计").lfCoeff;
   l_energy_data.nChannelNo = _mapChannelNameToInfo.value("监视能量计").nChannelNo;
   l_energy_dev_monit_var.setValue(l_energy_data);

   //数据处理
   st_dataprocessInput l_data;
   l_data.nProcessType = E_MEASUREPARAM_1On1;
   l_data.nParam1 = _mapChannelNameToInfo.value("监视能量计").nChannelNo;
   QVariant l_dataprocessInput;
   l_dataprocessInput.setValue(l_data);

   //损伤检测模块
   st_defectcheckInputdata l_defectcheck_data;
   QVariant l_defectcheck_data_var;
   l_defectcheck_data.nParamType = E_PARAM_PUBLISH;//发布实验信息
   l_defectcheck_data_var.setValue(l_defectcheck_data);


    //准备流程
    Tasklist* pStandTasklist = new Tasklist("准备");
    m_p1On1workflow->addTaskList(pStandTasklist);
    int shutter0TaskID = pStandTasklist->addTask("1064nm光闸关闭",M_DEV_SHUTTER,0,M_PROCESS_ACTION,l_shutter_close_var);
    int shutter1TaskID =pStandTasklist->addTask("355nm光闸关闭",M_DEV_SHUTTER,1,M_PROCESS_ACTION,l_shutter_close_var);
    int i32Task = pStandTasklist->addTask("载物台光闸打开",M_DEV_SHUTTER,2,M_PROCESS_ACTION,l_shutter_open_var);
    pStandTasklist->addTask("发布实验信息",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,0,{i32Task});
   // pStandTasklist->addTask("监视能量计参数设置",M_DEV_ENERGY,0x00,M_SETPARAM_ACTION,l_energy_dev_monit_var,0,{i32Task});
    int i32EnergyHomeTask = pStandTasklist->addTask("能量调节模块归零",M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_SETPARAM_ACTION,QVariant(0),0,{i32Task});
    pStandTasklist->addTask(QString("调节至能量%1%").arg(_tWorkflowParam.m_lfMinimumEnergy),M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_PROCESS_ACTION,QVariant(_tWorkflowParam.m_lfMinimumEnergy),0,{i32Task,i32EnergyHomeTask});

    l_motor_input.nMoveType = 0x01;//档位运动
    l_motor_input.nMotorIndex = E_MOTOR_NEARFIELD_WHEEL;
    l_motor_input.nPlusCnt = 0x0;
    l_motor_var.setValue(l_motor_input);
    pStandTasklist->addTask("CCD衰减轮盘至保护位",M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{shutter0TaskID,shutter1TaskID});

    //能量计参数设置

    l_energy_data.bInitMode = true;
    l_energy_data.nMeasureRange = 21;
    l_energy_dev_monit_var.setValue(l_energy_data);
    pStandTasklist->addTask("监视能量计设置工作模式",M_DEV_ENERGY,0x00,M_SETPARAM_ACTION,l_energy_dev_monit_var);

    //正式扫描流程
    l_dxPos_bak = (*_pPointList)[0].xPos;
    l_dyPos_bak = (*_pPointList)[0].yPos;


    int i32DefectCapImgTaskID = -1;//损伤检测图像采集任务
    int i32ShutterTaskID = -1;
    int i32DefectPrepareTaskID = -1;
    int i32EnergySetTaskID = -1;//能量计参数设置

    int i32TimeDelay = 1000;


    for(int i = 0; i < _pPointList->size();i++)
    {
        //计算调节能量与最大能量的关系
        int l_nEnergySection = i /_tWorkflowParam.m_n1On1PointNum;//能量段
        float lfEnergyPrecent = _tWorkflowParam.m_lfMinimumEnergy + l_nEnergySection *_tWorkflowParam.m_lfEnergyIncream;
        if(lfEnergyPrecent > _tWorkflowParam.m_lfMaximumEnergy)//lfEnergyPrecent 为百分比
        {
            break;
        }

        //运动
        Tasklist* pTasklist = new Tasklist(QString("第%1点").arg(i+1));
        m_p1On1workflow->addTaskList(pTasklist);
        pTasklist->m_nPonitIndex = i;


        //损伤检测系统准备
        l_defectcheck_data.nParamType = E_PARAM_PREPARE;//准备
        l_defectcheck_data.nPointNo = i;
        l_defectcheck_data.nShotNo = 0x1;
        l_defectcheck_data.lfEnergySection = lfEnergyPrecent;
        l_defectcheck_data_var.setValue(l_defectcheck_data);
        i32DefectPrepareTaskID = pTasklist->addTask("损伤诊断系统准备",M_DEV_DEFECTCHECK,0x0,M_SETPARAM_ACTION,l_defectcheck_data_var,i32TimeDelay);

        //能量计参数设置
        l_energy_data.lfEnergySection = lfEnergyPrecent;
        l_energy_data.nPointNo =  i;
        l_energy_data.nShotIndex = 0x1;
        l_energy_data.nMeasureRange = l_energy_data.caclMeasureRange(_tWorkflowParam.m_lfLaserEnergy,lfEnergyPrecent,M_MONIT_ENERGY_SAMPLE);
        l_energy_data.lfEnergySection = l_nEnergySection;
        l_energy_data.bInitMode = false;

        l_energy_dev_monit_var.setValue(l_energy_data);
        i32EnergySetTaskID = pTasklist->addTask("监视能量计参数设置",M_DEV_ENERGY,0x00,M_SETPARAM_ACTION,l_energy_dev_monit_var);
        //测试期间 先按照光闸进行驱动整个流程的方式  后期测试完成后可修改
        i32ShutterTaskID =  pTasklist->addTask(sShutterName+"发射1发并关闭",M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_process,0,{i32DefectPrepareTaskID,i32EnergySetTaskID});


        //能量计采集
        pTasklist->addTask("监视能量计采集",M_DEV_ENERGY, 0x0, M_PROCESS_ACTION,l_energy_dev_monit_var,0,{i32ShutterTaskID});

        //损伤检测系统图像采集状态查询
        l_defectcheck_data.nParamType = E_PARAM_CAPIMAGE;
        l_defectcheck_data_var.setValue(l_defectcheck_data);
        i32DefectCapImgTaskID = pTasklist->addTask("损伤诊断系统图像采集",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,200,{i32ShutterTaskID});

        l_defectcheck_data.nParamType = E_PARAM_DEFECT_REL;
        l_defectcheck_data_var.setValue(l_defectcheck_data);
        pTasklist->addTask("损伤判断结果",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,0,{i32DefectCapImgTaskID});

        // 等待能量计采集后 调节能量 节省时间  修改代码时 注意这个break
        if((i+1) %_tWorkflowParam.m_n1On1PointNum == 0)
        {
            l_nEnergySection = (i+1) /_tWorkflowParam.m_n1On1PointNum;//能量段
            lfEnergyPrecent = _tWorkflowParam.m_lfMinimumEnergy + l_nEnergySection *_tWorkflowParam.m_lfEnergyIncream;
            if(lfEnergyPrecent > _tWorkflowParam.m_lfMaximumEnergy)//lfEnergyPrecent 为百分比
            {
                break;
            }
            pTasklist->addTask(QString("调节能量,能量提升至%1%").arg(lfEnergyPrecent),M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_PROCESS_ACTION,QVariant(lfEnergyPrecent),0,{i32ShutterTaskID});
        }

        //计算载物台运动距离
        if(i < _pPointList->size() -1 )
        {
            QPointInfo l_pointInfo = (*_pPointList)[i+1];
            l_dMoveStep_x = l_pointInfo.xPos - l_dxPos_bak;
            l_dMoveStep_y = l_pointInfo.yPos - l_dyPos_bak;
            l_dxPos_bak = l_pointInfo.xPos;
            l_dyPos_bak = l_pointInfo.yPos;
        }
        else
        {
            l_dMoveStep_x = 0.0;
            l_dMoveStep_y = 0.0;
        }

        l_dMoveStep_y = l_dMoveStep_y *(1.0);
        l_dMoveStep_x = l_dMoveStep_x *(-1.0);



        l_sum_move_x = l_sum_move_x + l_dMoveStep_x;
        l_sum_move_y = l_sum_move_y + l_dMoveStep_y;

        //等待显微系统结束后 运动到下一个点 节省时间
        if(l_dMoveStep_x != 0)
        {
            sStepName = QString("运动至第%1点(x相对移动%2 mm)").arg(i+2).arg(l_dMoveStep_x);

            l_motor_input.nMoveType = 0x02;
            l_motor_input.nMotorIndex = E_MOTOR_X;
            l_motor_input.nPlusCnt  = l_dMoveStep_x*_pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_X);
            l_motor_var.setValue(l_motor_input);
            pTasklist->addTask(sStepName,M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{i32DefectCapImgTaskID});
        }
        if(l_dMoveStep_y != 0)
        {
            sStepName = QString("运动至第%1点(y相对移动%2 mm)").arg(i+2).arg(l_dMoveStep_y);

            l_motor_input.nMoveType = 0x02;
            l_motor_input.nMotorIndex = E_MOTOR_Y;
            l_motor_input.nPlusCnt  = l_dMoveStep_y*_pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Y);
            l_motor_var.setValue(l_motor_input);
            pTasklist->addTask(sStepName,M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{i32DefectCapImgTaskID});
        }    
    }


    Tasklist* pSysReset = new Tasklist("运动部件复位");
    m_p1On1workflow->addTaskList(pSysReset);
    int i32CloseShutter = pSysReset->addTask("关闭载物台光闸",M_DEV_SHUTTER,0x02,M_PROCESS_ACTION,l_shutter_close_var);
    l_motor_input.nMoveType = 0x02;
    l_motor_input.nMotorIndex = E_MOTOR_X;
    l_motor_input.nPlusCnt  = -1*l_sum_move_x*_pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Y);
    l_motor_var.setValue(l_motor_input);
    pSysReset->addTask(QString("X轴复位:%1mm").arg(-1*l_sum_move_x),M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{i32CloseShutter});

    l_motor_input.nMoveType = 0x02;
    l_motor_input.nMotorIndex = E_MOTOR_Y;
    l_motor_input.nPlusCnt  = -1*l_sum_move_y*_pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Y);
    l_motor_var.setValue(l_motor_input);
    pSysReset->addTask(QString("Y轴复位:%1mm").arg(-1*l_sum_move_y),M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{i32CloseShutter});



    Tasklist* pFinishedTask = new Tasklist("结束流程");
    m_p1On1workflow->addTaskList(pFinishedTask);
    pFinishedTask->addTask("关闭"+sShutterName,M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_close_var);
    pFinishedTask->addTask("关闭载物台光闸",M_DEV_SHUTTER,0x02,M_PROCESS_ACTION,l_shutter_close_var);
    l_defectcheck_data.nParamType = E_PARAM_DEFECT_CLOSE;//结束
    l_defectcheck_data_var.setValue(l_defectcheck_data);
    pFinishedTask->addTask("损伤系统结束流程",M_DEV_DEFECTCHECK,0x0,M_SETPARAM_ACTION,l_defectcheck_data_var);



    _pMeasureWorkflowWiget->setWorkflow(m_p1On1workflow);

}

/******************************************
* 功能:创建ROn1流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void DefectMeasureWidget::createROn1workflow()
{
    double l_dMoveStep_x = 0.0;
    double l_dMoveStep_y = 0.0;
    double l_sum_move_x = 0.0;//x轴运动所有距离
    double l_sum_move_y = 0.0;//y轴运动所有距离
    double l_dxPos_bak = 0.0;
    double l_dyPos_bak= 0.0;
    QString sStepName;

    if(m_pROn1workflow == nullptr)
    {
        m_pROn1workflow = new Workflow("ROn1测试流程",&_tCommonInfo);
    }
    m_pROn1workflow->clearAllTask();

    if(_pPointList == nullptr)
    {
        return;
    }

    //电机参数
    st_motorInputdata l_motor_input;
    QVariant l_motor_var;

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


    int nShutterDevNo = _tWorkflowParam.m_nWaveLengthType == 0?0:1;
    QString sShutterName = _tWorkflowParam.m_nWaveLengthType == 0?"1064nm光闸":"355nm光闸";

    int nAdjustEnergyDevNo = _tWorkflowParam.m_nWaveLengthType == 0?0:1;

    //数据处理
    st_dataprocessInput l_data;
    l_data.nProcessType = E_MEASUREPARAM_ROn1;
    l_data.nParam1 = _mapChannelNameToInfo.value("监视能量计").nChannelNo;
    QVariant l_dataprocessInput;
    l_dataprocessInput.setValue(l_data);

    //能量计参数
    QVariant l_energy_dev_monit_var;
    st_EnergyDevTaskInputData l_energy_data;
    l_energy_data.ncapDataCnt = 1;
    l_energy_data.bSaveData = true;
    l_energy_data.nMeasureRange = 20;//19-3mJ
    l_energy_data.nWavelength = _tWorkflowParam.m_nWaveLengthType ==0?1064:355;
    l_energy_data.lfCoeff = _mapChannelNameToInfo.value("监视能量计").lfCoeff;
    l_energy_data.nChannelNo = _mapChannelNameToInfo.value("监视能量计").nChannelNo;
    l_energy_dev_monit_var.setValue(l_energy_data);

    //损伤检测模块
    st_defectcheckInputdata l_defectcheck_data;
    QVariant l_defectcheck_data_var;
    l_defectcheck_data.nParamType = E_PARAM_PUBLISH;//发布实验信息
    l_defectcheck_data_var.setValue(l_defectcheck_data);

    int i32TimeDelay = 0;



    //准备流程
    Tasklist* pStandTasklist = new Tasklist("准备");
    m_pROn1workflow->addTaskList(pStandTasklist);
    int shutter0taskID = pStandTasklist->addTask("1064nm光闸关闭",M_DEV_SHUTTER,0,M_PROCESS_ACTION,l_shutter_close_var);
    int shutter1taskID = pStandTasklist->addTask("355nm光闸关闭",M_DEV_SHUTTER,1,M_PROCESS_ACTION,l_shutter_close_var);
    int i32Task = pStandTasklist->addTask("载物台光闸打开",M_DEV_SHUTTER,2,M_PROCESS_ACTION,l_shutter_open_var);
    pStandTasklist->addTask("发布实验信息",M_DEV_DEFECTCHECK, 0x0, M_SETPARAM_ACTION,l_defectcheck_data_var,0,{i32Task});
    //pStandTasklist->addTask("监视能量计参数设置",M_DEV_ENERGY,0x00,M_SETPARAM_ACTION,l_energy_dev_monit_var,0,{i32Task});
    int i32EnergyHomeTask = pStandTasklist->addTask("能量调节模块归零",M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_SETPARAM_ACTION,QVariant(0),0,{i32Task});
    pStandTasklist->addTask(QString("调节至能量%1%").arg(_tWorkflowParam.m_lfMinimumEnergy),M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_PROCESS_ACTION,QVariant(_tWorkflowParam.m_lfMinimumEnergy),0,{i32Task,i32EnergyHomeTask});

    l_motor_input.nMoveType = 0x01;
    l_motor_input.nMotorIndex = E_MOTOR_NEARFIELD_WHEEL;
    l_motor_input.nPlusCnt = 0x0;
    l_motor_var.setValue(l_motor_input);
    pStandTasklist->addTask("CCD衰减轮盘至保护位",M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{shutter0taskID,shutter1taskID});

    //能量计参数设置

    l_energy_data.bInitMode = true;
    l_energy_data.nMeasureRange = 21;
    l_energy_dev_monit_var.setValue(l_energy_data);
    pStandTasklist->addTask("监视能量计设置工作模式",M_DEV_ENERGY,0x00,M_SETPARAM_ACTION,l_energy_dev_monit_var);


    //正式扫描流程
    l_dxPos_bak = (*_pPointList)[0].xPos;
    l_dyPos_bak = (*_pPointList)[0].yPos;


    for(int i = 0; i < _pPointList->size();i++)
    {

        int i32ShutterTaskID = -1;//当前能量段 放光任务ID
        int i32DefectCheckTaskID = -1;//损伤点检测系统处理任务ID
        int i32DefectCapImgTaskID = -1;//损伤点检测系统图像采集任务ID
        int i32EnergyAdjustTaskID = -1;//能量调节任务ID
        int i32DefectprepareTaskID = -1;//检测系统准备
       // int i32MoveTaskID = -1;//运动任务
        int i32EnergyCapTaskID = -1;
        int i32EnergySetTaskID = -1;


        //计算载物台运动距离
        QPointInfo l_pointInfo = (*_pPointList)[i];
        l_dMoveStep_x = l_pointInfo.xPos - l_dxPos_bak;
        l_dMoveStep_y = l_pointInfo.yPos - l_dyPos_bak;
        l_dMoveStep_y = l_dMoveStep_y*(1.0);
        l_dMoveStep_x = l_dMoveStep_x *(-1.0);
        l_dxPos_bak = l_pointInfo.xPos;
        l_dyPos_bak = l_pointInfo.yPos;

        l_sum_move_x = l_sum_move_x + l_dMoveStep_x;
        l_sum_move_y = l_sum_move_y + l_dMoveStep_y;


        Tasklist* pMoveTasklist = new Tasklist(QString("运动至第%1点").arg(i+1));
        m_pROn1workflow->addTaskList(pMoveTasklist);
        pMoveTasklist->m_nPonitIndex = -1;

        //运动
        if(l_dMoveStep_x != 0)
        {
            sStepName = QString("运动至第%1点(x相对移动%2 mm)").arg(i+1).arg(l_dMoveStep_x);
            l_motor_input.nMoveType = 0x02;
            l_motor_input.nMotorIndex = E_MOTOR_X;
            l_motor_input.nPlusCnt  = l_dMoveStep_x*_pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_X);
            l_motor_var.setValue(l_motor_input);
            pMoveTasklist->addTask(sStepName,M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var);
        }
        if(l_dMoveStep_y != 0)
        {
            sStepName = QString("运动至第%1点(y相对移动%2 mm)").arg(i+1).arg(l_dMoveStep_y);
            l_motor_input.nMoveType = 0x02;
            l_motor_input.nMotorIndex = E_MOTOR_Y;
            l_motor_input.nPlusCnt  = l_dMoveStep_y*_pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Y);
            l_motor_var.setValue(l_motor_input);
            pMoveTasklist->addTask(sStepName,M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var);
        }


        Tasklist* pTasklist = new Tasklist(QString("第%1点").arg(i+1));
        m_pROn1workflow->addTaskList(pTasklist);
        pTasklist->m_nPonitIndex = i;

        for(int nEnergyCnt = 0; nEnergyCnt < _tWorkflowParam.m_nEnergySectionNum; nEnergyCnt++)
        {
            float lfEnergyPrecent = _tWorkflowParam.m_lfMinimumEnergy + nEnergyCnt*_tWorkflowParam.m_lfEnergyIncream;

            if(nEnergyCnt == 0 && i > 0)//调整至初始能量
            {
                i32TimeDelay = 1000;
                i32EnergyAdjustTaskID = pTasklist->addTask(QString("调整能量至%1%").arg(lfEnergyPrecent),M_DEV_ENERGY_ADJ, nAdjustEnergyDevNo, M_PROCESS_ACTION,QVariant(lfEnergyPrecent),0,{i32ShutterTaskID});
            }
            else if(nEnergyCnt > 0)
            {
                 i32TimeDelay = 0;
                 i32EnergyAdjustTaskID = pTasklist->addTask(QString("能量提升至%1%").arg(lfEnergyPrecent),M_DEV_ENERGY_ADJ, nAdjustEnergyDevNo, M_PROCESS_ACTION,QVariant(lfEnergyPrecent),0,{i32ShutterTaskID});
            }


            //损伤检测系统准备
            l_defectcheck_data.nParamType = E_PARAM_PREPARE;//准备
            l_defectcheck_data.nPointNo = i;
            l_defectcheck_data.nShotNo = nEnergyCnt;
            l_defectcheck_data.lfEnergySection = lfEnergyPrecent;
            l_defectcheck_data_var.setValue(l_defectcheck_data);
            i32DefectprepareTaskID = pTasklist->addTask("损伤诊断系统准备",M_DEV_DEFECTCHECK,0x0,M_SETPARAM_ACTION,l_defectcheck_data_var,i32TimeDelay,{i32DefectCheckTaskID});

            //能量计参数设置
            l_energy_data.lfEnergySection = lfEnergyPrecent;
            l_energy_data.nPointNo =  i;
            l_energy_data.nShotIndex = nEnergyCnt;
            l_energy_data.bInitMode = false;
            l_energy_data.nMeasureRange = l_energy_data.caclMeasureRange(_tWorkflowParam.m_lfLaserEnergy,lfEnergyPrecent,M_MONIT_ENERGY_SAMPLE);
            l_energy_dev_monit_var.setValue(l_energy_data);
            i32EnergySetTaskID = pTasklist->addTask("监视能量计参数设置",M_DEV_ENERGY,0x00,M_SETPARAM_ACTION,l_energy_dev_monit_var,0,{i32EnergyCapTaskID});


            //测试期间 先按照光闸进行驱动整个流程的方式  后期测试完成后可修改
            //等待能量调节、损伤检测任务任务结束
            i32ShutterTaskID =  pTasklist->addTask(sShutterName+"发射1发并关闭",M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_process,0,{i32DefectprepareTaskID,i32EnergyAdjustTaskID,i32EnergySetTaskID});

            //能量计采集
            i32EnergyCapTaskID = pTasklist->addTask("监视能量计采集并计算通量",M_DEV_ENERGY, 0x0, M_PROCESS_ACTION,l_energy_dev_monit_var,0,{i32ShutterTaskID});

            //等待显微系统结束
            l_defectcheck_data.nParamType = E_PARAM_CAPIMAGE;//采集图像状态
            l_defectcheck_data_var.setValue(l_defectcheck_data);
            i32DefectCapImgTaskID = pTasklist->addTask("损伤诊断系统图像采集",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,200,{i32ShutterTaskID});


            l_defectcheck_data.nParamType = E_PARAM_DEFECT_REL;//查询损伤检测结果
            l_defectcheck_data_var.setValue(l_defectcheck_data);
            i32DefectCheckTaskID = pTasklist->addTask("损伤判断结果",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,0,{i32DefectCapImgTaskID});
        }


    }


//    Tasklist* pDataProcess = new Tasklist("数据处理");
//    m_pROn1workflow->addTaskList(pDataProcess);
//    pDataProcess->addTask("ROn1数据处理",M_DEV_DATA_PROCESS,0x0,M_PROCESS_ACTION,l_dataprocessInput);


    Tasklist* pSysReset = new Tasklist("运动部件复位");
    m_pROn1workflow->addTaskList(pSysReset);
    int i32CloseShutter = pSysReset->addTask("关闭载物台光闸",M_DEV_SHUTTER,0x02,M_PROCESS_ACTION,l_shutter_close_var);

    l_motor_input.nMoveType = 0x02;
    l_motor_input.nMotorIndex = E_MOTOR_X;
    l_motor_input.nPlusCnt  = -1*l_sum_move_x*_pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Y);
    l_motor_var.setValue(l_motor_input);
    pSysReset->addTask(QString("X轴复位 距离%1mm").arg(-1*l_sum_move_x),M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{i32CloseShutter});
    l_motor_input.nMoveType = 0x02;
    l_motor_input.nMotorIndex = E_MOTOR_Y;
    l_motor_input.nPlusCnt  = -1*l_sum_move_y*_pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Y);
    l_motor_var.setValue(l_motor_input);
    pSysReset->addTask(QString("Y轴复位 距离%1mm").arg(-1*l_sum_move_y),M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,0,{i32CloseShutter});


    Tasklist* pFinishedTask = new Tasklist("结束流程");//手动停止时 会自动执行本步骤
    m_pROn1workflow->addTaskList(pFinishedTask);
    pFinishedTask->addTask("关闭"+sShutterName,M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_close_var);
    pFinishedTask->addTask("关闭载物台光闸",M_DEV_SHUTTER,0x02,M_PROCESS_ACTION,l_shutter_close_var);
    l_defectcheck_data.nParamType = E_PARAM_DEFECT_CLOSE;//结束
    l_defectcheck_data_var.setValue(l_defectcheck_data);
    pFinishedTask->addTask("损伤系统结束流程",M_DEV_DEFECTCHECK,0x0,M_SETPARAM_ACTION,l_defectcheck_data_var);

    _pMeasureWorkflowWiget->setWorkflow(m_pROn1workflow);

}


 /******************************************
* 功能:更新流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void DefectMeasureWidget::onUpdateWorkflow(QVariant var)
 {
    _tWorkflowParam =  var.value<tWorkflowParam>();

    _pPointList = _pPointAreaWidget->getPointlist();

    if(_tWorkflowParam.m_nMeasureType == M_MEASURETYPE_1On1)
    {
        create1On1workflow();//1on1流程
    }

    if(_tWorkflowParam.m_nMeasureType == M_MEASURETYPE_ROn1)
    {
        createROn1workflow();//Ron1流程
    }


    _pLogWidget->onShowlog("参数保存，测试流程更新完成！");
 }




 /******************************************
* 功能:更新点的状态  用于pointareawidget显示
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void DefectMeasureWidget:: onUpdatePointStatus()
{
    int nMaxPointNum = 0;

    Workflow* l_pWorkflow = nullptr;
    if(_tWorkflowParam.m_nMeasureType == M_MEASURETYPE_1On1)
    {
        l_pWorkflow = m_p1On1workflow;
    }

    if(_tWorkflowParam.m_nMeasureType == M_MEASURETYPE_ROn1)
    {
        l_pWorkflow = m_pROn1workflow;
    }

    for(int i = 0; i < _pPointList->size();i++)
    {
        foreach (Tasklist* pTasklist, l_pWorkflow->m_tWorkflow) {
            if(pTasklist->m_nPonitIndex == i)
            {
                 (*_pPointList)[i].nStatus = pTasklist->m_nTasklistStatus;
                //执行成功 显示是否有损伤
                if(pTasklist->m_nTasklistStatus == M_TASK_STATUS_SUCCESS)
                {
                    for(TaskInfo* pTaskInfo:pTasklist->m_tTaskList)
                    {
                        if(pTaskInfo->m_nDevType == M_DEV_DEFECTCHECK && pTaskInfo->m_bDefect == true)
                        {
                            (*_pPointList)[i].nStatus = 0x04;//有损伤
                            break;
                        }
                    }

                }

                //统计有效点数  1on1时可能没有用完
                nMaxPointNum = nMaxPointNum > pTasklist->m_nPonitIndex?nMaxPointNum: pTasklist->m_nPonitIndex;

                break;
            }
        }

    }

   _pPointAreaWidget->m_nValidPointNum = nMaxPointNum == 0?0:nMaxPointNum+1;


}
 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void DefectMeasureWidget::setWidgetStyleSheet(QWidget* pWidget)
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
 bool DefectMeasureWidget::checkRunCondition(QString sworkflowName)
 {
    if(!_pParamWidget->checkParamSaved())
    {
        QMessageBox::warning(this,"提示","请先保存参数!");
        return false;
    }

    updateCommonInfo();//更新公共数据

    if(sworkflowName == "1On1测试流程")
    {
        _tCommonInfo.nMeasureType = M_MEASURETYPE_1On1;
    }


    if(sworkflowName == "ROn1测试流程")
    {
        _tCommonInfo.nMeasureType = M_MEASURETYPE_ROn1;
    }

    //存储当前实验信息
    if(!_tCommonInfo._pDBProcess->onSaveExpInfo(_tCommonInfo.sExpNo,_tCommonInfo.sSampleName,_tCommonInfo.nMeasureType,
                                                _tCommonInfo.nwavelengthType,_tCommonInfo.lfCoeff,_tCommonInfo.lfArea,
                                                _tCommonInfo.lfAngle,_tCommonInfo.lfPlusWidth,_tCommonInfo.lfM,
                                                _tCommonInfo.sTaskNo,_tCommonInfo.lfExpTemp,_tCommonInfo.lfExpHum,_tCommonInfo.lfExpdirty))
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
 void DefectMeasureWidget::updateCommonInfo()
 {
     _tCommonInfo.sSampleName = _tWorkflowParam.m_sSampleName;
     _tCommonInfo.nMeasureType = _tWorkflowParam.m_nMeasureType;

     _tCommonInfo.nwavelengthType = _tWorkflowParam.m_nWaveLengthType;

     _tCommonInfo.lfArea = _tWorkflowParam.m_dManualArea[_tWorkflowParam.m_nWaveLengthType];
     _tCommonInfo.lfCoeff = _tWorkflowParam.m_dEnergyCoeff[_tWorkflowParam.m_nWaveLengthType];
     _tCommonInfo.lfPlusWidth = _tWorkflowParam.m_dPlusWidth[_tWorkflowParam.m_nWaveLengthType];
     _tCommonInfo.lfAngle = _tWorkflowParam.m_lfAngle;
     _tCommonInfo.lfM =_tWorkflowParam.m_dM[_tWorkflowParam.m_nWaveLengthType];

    _tCommonInfo.sTaskNo =_tWorkflowParam.m_sExpTaskNo;
    _tCommonInfo.lfExpTemp =_tWorkflowParam.m_lfExpTemp;
    _tCommonInfo.lfExpHum =_tWorkflowParam.m_lfExphum;
    _tCommonInfo.lfExpdirty =_tWorkflowParam.m_lfExpdirty;


     _tCommonInfo.sExpNo = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");// 生成实验编号
 }

 /******************************************
 * 功能:工作流程结束后的处理
 * 输入:sworkflowName为流程名称 taskstatus执行结果
 * 输出:
 * 返回值:
 * 维护记录:
 ******************************************/
 void DefectMeasureWidget::onFinishWorkflow(QString sworkflowName,_Taskstatus taskstatus)
 {
     //执行结束 显示执行结果
     if((_tCommonInfo.nMeasureType == M_MEASURETYPE_1On1 || _tCommonInfo.nMeasureType == M_MEASURETYPE_ROn1)&&taskstatus == M_TASK_STATUS_SUCCESS)
     {
         if(_pMeasureReslutWidget != nullptr)
         {
             _pMeasureReslutWidget->close();
             delete _pMeasureReslutWidget;
         }

        _pMeasureReslutWidget = new QMeasureRelProcessWidget(_tCommonInfo._pDBProcess);

        _pMeasureReslutWidget->resize(1000,800);
        _pMeasureReslutWidget->setWindowTitle("实验数据");

        QStringList l_expNolist;
        l_expNolist.push_back(_tCommonInfo.sExpNo);

        _pMeasureReslutWidget->getExplistData(l_expNolist);

        _pMeasureReslutWidget->showExpData(_pMeasureReslutWidget->m_ExpdataInfolist);

        _pMeasureReslutWidget->onExpDataProcess();
        _pMeasureReslutWidget->setWindowModality(Qt::ApplicationModal);
        _pMeasureReslutWidget->show();
     }
 }

 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void DefectMeasureWidget::showParam()
 {
     //更新配置参数
     for(int i = 0; i < M_LASER_CNT; i++)
     {
        _pParamWidget->m_tworkflowParam.m_dManualArea[i] = m_tPlatformConfig->m_st_laserParam[i].m_lfArea;
        _pParamWidget->m_tworkflowParam.m_dEnergyCoeff[i] = m_tPlatformConfig->m_st_laserParam[i].m_lfK;
        _pParamWidget->m_tworkflowParam.m_dPlusWidth[i] = m_tPlatformConfig->m_st_laserParam[i].m_lfPlusWidth;
        _pParamWidget->m_tworkflowParam.m_sLaserName[i] = m_tPlatformConfig->m_st_laserParam[i].m_sLaserName;
        _pParamWidget->m_tworkflowParam.m_dM[i] = m_tPlatformConfig->m_st_laserParam[i].m_lfM;
     }
     _pParamWidget->m_tworkflowParam.m_nWaveLengthType = m_tPlatformConfig->m_nCurrentWaveLength;
     _pParamWidget->showParam();//更新参数显示

     _tWorkflowParam =  _pParamWidget->m_tworkflowParam;//更新参数

 }
