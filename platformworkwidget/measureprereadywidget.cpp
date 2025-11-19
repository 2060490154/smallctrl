/***************************************************************************
**                                                                        **
** 文件描述：测量前准备界面
**                                                                        **
****************************************************************************
** 创建人：
** 创建时间：
** 修改记录：
**
****************************************************************************/
#include "measureprereadywidget.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

MeasurePreReadyWidget::MeasurePreReadyWidget(QDevCtrlManager* pPlatformDevCtrlManager,CDbDataProcess* pDBProcess,tPlatformConfig* pPlatformConfig,QDevConfig* pDevConfig,QWidget *parent) : QWidget(parent)
{
    m_pWaveLengthChangeWorkflow = nullptr;

    m_pMaxEnergyConfirmWorkflow = nullptr;

    m_nWaveLengthType = 0;

    m_nMeasureType = 0;

    m_lfMaxEnergy = 0.0;
    m_lfMinEnergy = 0.0;

    m_lfEnergyIncream = 0.0;
    m_nEnergyIncreamCnt = 1;


    m_tPlatformConfig = pPlatformConfig;

    _pPlatformDevCtrlManager = pPlatformDevCtrlManager;

    _tCommonInfo._pDBProcess = pDBProcess;

    _mapChannelNameToInfo = pDevConfig->m_mapChannelNameToInfo;
    _pDevConfig = pDevConfig;

    initUI();

    showParam();


    //日志显示消息
    connect(_pWaveLengthChangeWorkflowWidget,&QWorkflowWidget::showlogInfo,_pLogWidget,&QLogWidget::onShowlog);
    connect(pDBProcess,&CDbDataProcess::signal_showErrorInfo,_pLogWidget,&QLogWidget::onShowlog);
    connect(_pMaxEnergyConfirmWidget,&QWorkflowWidget::showlogInfo,_pLogWidget,&QLogWidget::onShowlog);

    //保存参数
    connect(_pSaveParamButton,&QPushButton::clicked,this,&MeasurePreReadyWidget::onSaveParam);

    //任务结束消息处理
    connect(_pWaveLengthChangeWorkflowWidget,&QWorkflowWidget::finishedProcess,this,&MeasurePreReadyWidget::onFinishWorkflow);
    connect(_pMaxEnergyConfirmWidget,&QWorkflowWidget::finishedProcess,this,&MeasurePreReadyWidget::onFinishWorkflow);

    //测量模式选择
    void (QComboBox::*fn)(int) = &QComboBox::currentIndexChanged;
    connect(_pMeasureTypeSelectComBox,fn,this,&MeasurePreReadyWidget::onMeasureTypeSelect);

    //运动按钮
     void (QButtonGroup::*buttonfun)(int) = &QButtonGroup::buttonClicked;
    connect(_pMoveButtonGroup,buttonfun,this,&MeasurePreReadyWidget::onMoveButtonClick);


//    QUIQss::setTxtQss(_pMoveDistanceEdit, "#DCE4EC", "#1ABC9C");
//    QUIQss::setTxtQss(_pRatioEdit, "#DCE4EC", "#1ABC9C");
//    QUIQss::setTxtQss(_pEnergyMinEdit, "#DCE4EC", "#1ABC9C");
//    QUIQss::setTxtQss(_pEnergyIncreamEdit, "#DCE4EC", "#1ABC9C");
//    QUIQss::setTxtQss(_pEnergyIncreamCntEdit, "#DCE4EC", "#1ABC9C");


}

MeasurePreReadyWidget::~MeasurePreReadyWidget()
{

    if(m_pWaveLengthChangeWorkflow != nullptr)
    {
        m_pWaveLengthChangeWorkflow->clearAllTask();
        delete m_pWaveLengthChangeWorkflow;
    }

    if(m_pMaxEnergyConfirmWorkflow != nullptr)
    {
        m_pMaxEnergyConfirmWorkflow->clearAllTask();
        delete m_pMaxEnergyConfirmWorkflow;
    }


}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QWidget* MeasurePreReadyWidget::createParamWidget()
{
    QWidget* pwidget = new QWidget(this);

    _pCurrentWaveLengthLabel = new QLabel(this);
    _pWaveLengthSelectComBox = new QComboBox(pwidget);//光路选择
    _pMeasureTypeSelectComBox = new QComboBox(pwidget);
    _pEnergyMinEdit = new QLineEdit(this);
    _pEnergyMaxlabel = new QLabel(this);
    _pEnergyIncreamEdit = new QLineEdit(this);
    _pEnergyMinLabel = new QLabel(this);
    _pEnergyIncreamCntEdit = new QLineEdit(this);
    _pSaveParamButton = new QPushButton("保存参数",this);

    _pEnergyMinLabel->setText("能量设置(%)");

    for(int i = 0; i < M_LASER_CNT; i++)
    {
        _pWaveLengthSelectComBox->addItem(m_tPlatformConfig->m_st_laserParam[i].m_sLaserName);
    }

    _pMeasureTypeSelectComBox->addItem("1On1");
    _pMeasureTypeSelectComBox->addItem("ROn1");

    _pWaveLengthSelectComBox->setCurrentIndex(0);
    _pWaveLengthSelectComBox->setMinimumHeight(25);

    _pMeasureTypeSelectComBox->setCurrentIndex(0);
    _pMeasureTypeSelectComBox->setMinimumHeight(25);

    _pEnergyMinEdit->setMinimumHeight(25);
    _pEnergyMaxlabel->setMinimumHeight(25);
    _pEnergyIncreamEdit->setMinimumHeight(25);
    _pCurrentWaveLengthLabel->setMaximumHeight(30);
    _pEnergyIncreamCntEdit->setMinimumHeight(25);
    _pCurrentWaveLengthLabel->setStyleSheet("color:blue;font-size:16px;");
    _pEnergyMaxlabel->setStyleSheet("color:blue;font-size:16px;");
    _pEnergyMaxlabel->setText("0.0");

    _pSaveParamButton->setMaximumWidth(100);


    QGridLayout* playout = new QGridLayout(pwidget);


    int nRowCnt = 0;


    playout->addWidget(new QLabel("当前光路:"),nRowCnt,0,1,1);
    playout->addWidget(_pCurrentWaveLengthLabel,nRowCnt,1,1,1);
    nRowCnt++;

    playout->addWidget(new QLabel("光路选择:"),nRowCnt,0,1,1);
    playout->addWidget(_pWaveLengthSelectComBox,nRowCnt,1,1,1);

    playout->addWidget(new QLabel("测量模式:"),nRowCnt,2,1,1);
    playout->addWidget(_pMeasureTypeSelectComBox,nRowCnt,3,1,1);

    nRowCnt++;

    playout->addWidget(_pEnergyMinLabel,nRowCnt,0,1,1);
    playout->addWidget(_pEnergyMinEdit,nRowCnt,1,1,1);

    playout->addWidget(new QLabel("能量递增(%):"),nRowCnt,2,1,1);
    playout->addWidget(_pEnergyIncreamEdit,nRowCnt,3,1,1);

    nRowCnt++;
    playout->addWidget(new QLabel("能量递增台阶数:"),nRowCnt,0,1,1);
    playout->addWidget(_pEnergyIncreamCntEdit,nRowCnt,1,1,1);


    nRowCnt++;

    playout->addWidget(new QLabel("测量出的最大能量(%):"),nRowCnt,0,1,1);
    playout->addWidget(_pEnergyMaxlabel,nRowCnt,1,1,1);

    playout->addWidget(_pSaveParamButton,nRowCnt,3,1,1);

    for(int i = 0; i < 5; i++)
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
QWidget* MeasurePreReadyWidget::createMotorWidget()
{

    QWidget* pwidget = new QWidget(this);

    QLabel* pLabel = new QLabel(this);
    pLabel->setText("载物台操作");
    pLabel->setStyleSheet("color:blue;font-size:16px;");
    pLabel->setMaximumHeight(30);

    _pLeftMoveButton = new QPushButton(this);
    _pRightMoveButton = new QPushButton(this);
    _pUpMoveButton = new QPushButton(this);
    _pDownMoveButton = new QPushButton(this);
    _pRatioMoveButton = new QPushButton("旋转台运动",this);
    _pRatioEdit = new QLineEdit(this);


    _pLeftMoveButton -> setIcon(QIcon(":/png/left.png")); //将图片设置到按钮上
    _pLeftMoveButton -> setIconSize(QSize(50,50));//根据实际调整图片大小
    _pLeftMoveButton->setFlat(true);

    _pRightMoveButton -> setIcon(QIcon(":/png/right.png")); //将图片设置到按钮上
    _pRightMoveButton -> setIconSize(QSize(50,50));//根据实际调整图片大小
    _pRightMoveButton->setFlat(true);

    _pUpMoveButton -> setIcon(QIcon(":/png/up.png")); //将图片设置到按钮上
    _pUpMoveButton -> setIconSize(QSize(50,50));//根据实际调整图片大小
    _pUpMoveButton->setFlat(true);

    _pDownMoveButton -> setIcon(QIcon(":/png/down.png")); //将图片设置到按钮上
    _pDownMoveButton -> setIconSize(QSize(50,50));//根据实际调整图片大小
    _pDownMoveButton->setFlat(true);

    _pMoveButtonGroup = new QButtonGroup(this);
    _pMoveButtonGroup->addButton(_pLeftMoveButton,0x1);
    _pMoveButtonGroup->addButton(_pRightMoveButton,0x2);
    _pMoveButtonGroup->addButton(_pUpMoveButton,0x3);
    _pMoveButtonGroup->addButton(_pDownMoveButton,0x4);
    _pMoveButtonGroup->addButton(_pRatioMoveButton,0x5);


    _pMoveDistanceEdit = new QLineEdit("0.0",this);
    _pMoveDistanceEdit->setToolTip("mm");
    _pMoveDistanceEdit->setAlignment(Qt::AlignCenter);

    _pMoveDistanceEdit->setStyleSheet("color:blue;font-size:16px;");


    QGridLayout* playout = new QGridLayout(pwidget);
    for(int i = 0; i < 8; i++)
    {
        playout->setColumnStretch(i,1);
    }

    playout->addWidget(pLabel,0,0,1,1);

    playout->addWidget(_pLeftMoveButton,2,2,1,1);

    playout->addWidget(_pRightMoveButton,2,4,1,1);


    playout->addWidget(_pUpMoveButton,1,3,1,1);
    playout->addWidget(_pDownMoveButton,3,3,1,1);
    playout->addWidget(_pMoveDistanceEdit,2,3,1,1);

    playout->addWidget(new QLabel("旋转台角度(°)"),4,0,1,1);
    playout->addWidget(_pRatioEdit,4,1,1,2);
    playout->addWidget(_pRatioMoveButton,4,3,1,1);




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
void MeasurePreReadyWidget::initUI()
{
    _pWaveLengthChangeWorkflowWidget = new QWorkflowWidget(_pPlatformDevCtrlManager,bindCheckConditionFun(MeasurePreReadyWidget::checkRunCondition,this));

    _pMaxEnergyConfirmWidget = new QWorkflowWidget(_pPlatformDevCtrlManager,bindCheckConditionFun(MeasurePreReadyWidget::checkRunCondition,this));

    _pLogWidget = new QLogWidget(this);

    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0,2);
    playout->setRowStretch(1,2);
    playout->setRowStretch(2,1);
    for(int i = 0; i < 6; i++)
    {
        playout->setColumnStretch(i,1);
    }

    playout->addWidget(createParamWidget(),0,0,1,3);

    playout->addWidget(createMotorWidget(),0,3,1,3);



    playout->addWidget(_pWaveLengthChangeWorkflowWidget,1,0,1,3);
    playout->addWidget(_pMaxEnergyConfirmWidget,1,3,1,3);


    playout->addWidget(_pLogWidget,2,0,1,6);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void MeasurePreReadyWidget::createWaveLengthChangeWorkflow()
{
    if(m_pWaveLengthChangeWorkflow == nullptr)
    {
        m_pWaveLengthChangeWorkflow = new Workflow("光路切换至1064nm",&_tCommonInfo);
    }

    m_pWaveLengthChangeWorkflow->clearAllTask();

    m_pWaveLengthChangeWorkflow->m_sWorkflowName = m_nWaveLengthType == 0?"光路切换至1064nm":"光路切换至355nm";

    st_motorInputdata l_motor_input;
    QVariant l_var;
    l_motor_input.nMoveType = 0x01;


    //光闸参数
    st_shutterparam l_st_shutterparam;
    l_st_shutterparam.i32Mode = 0x01;//正常模式
    l_st_shutterparam.i32Param = 0x0;//关闭
    QVariant l_shutter_close_var;
    l_shutter_close_var.setValue(l_st_shutterparam);


    //准备流程
    Tasklist* pShutterTasklist = new Tasklist("光闸关闭");
    m_pWaveLengthChangeWorkflow->addTaskList(pShutterTasklist);
    pShutterTasklist->addTask("1064nm光闸关闭",M_DEV_SHUTTER,0,M_PROCESS_ACTION,l_shutter_close_var);
    pShutterTasklist->addTask("355nm光闸关闭",M_DEV_SHUTTER,1,M_PROCESS_ACTION,l_shutter_close_var);
    pShutterTasklist->addTask("载物台光闸关闭",M_DEV_SHUTTER,2,M_PROCESS_ACTION,l_shutter_close_var);

    Tasklist* pStandbytasklist = new Tasklist("切换光路");
    m_pWaveLengthChangeWorkflow->addTaskList(pStandbytasklist);

    if(m_nWaveLengthType == 0 )
    {
        l_motor_input.nMotorIndex = E_MOTOR_LIGHT_CHANGE;
        l_motor_input.nPlusCnt = 0x01;//to be confirm
        l_var.setValue(l_motor_input);
        pStandbytasklist->addTask("平移台切出",M_DEV_MOTOR,0,M_PROCESS_ACTION,l_var);

        l_motor_input.nMotorIndex = E_MOTOR_PLATFORM;
        l_motor_input.nPlusCnt = 0x01;//to be confirm
        l_var.setValue(l_motor_input);
        pStandbytasklist->addTask("光路调节平移台",M_DEV_MOTOR,0,M_PROCESS_ACTION,l_var);
    }

    if(m_nWaveLengthType == 1 )
    {
        l_motor_input.nMotorIndex = E_MOTOR_LIGHT_CHANGE;
        l_motor_input.nPlusCnt = 0x00;//to be confirm
        l_var.setValue(l_motor_input);
        pStandbytasklist->addTask("平移台切入",M_DEV_MOTOR,0,M_PROCESS_ACTION,l_var);

        l_motor_input.nMotorIndex = E_MOTOR_PLATFORM;
        l_motor_input.nPlusCnt = 0x00;//to be confirm
        l_var.setValue(l_motor_input);
        pStandbytasklist->addTask("光路调节平移台",M_DEV_MOTOR,0,M_PROCESS_ACTION,l_var);
    }

    _pWaveLengthChangeWorkflowWidget->setWorkflow(m_pWaveLengthChangeWorkflow);
}

void  MeasurePreReadyWidget::createMaxEergyConfirmWorkflow()
{
    if(m_pMaxEnergyConfirmWorkflow == nullptr)
    {
        m_pMaxEnergyConfirmWorkflow = new Workflow("最大能量确认流程",&_tCommonInfo);
    }
    m_pMaxEnergyConfirmWorkflow->clearAllTask();

    m_pMaxEnergyConfirmWorkflow->m_sWorkflowName = m_nMeasureType == 0?"1On1最大能量确认流程":"ROn1最大能量确认流程";

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

    //损伤检测模块
    st_defectcheckInputdata l_defectcheck_data;
    QVariant l_defectcheck_data_var;
    l_defectcheck_data.nParamType = E_PARAM_PUBLISH;//发布实验信息
    l_defectcheck_data.nPointNo = 0x1;
    l_defectcheck_data.nShotNo = 0x01;
    l_defectcheck_data_var.setValue(l_defectcheck_data);


    int nShutterDevNo = m_nWaveLengthType == 0?0:1;
    QString sShutterName = m_nWaveLengthType == 0?"1064nm光闸":"355nm光闸";

    int nAdjustEnergyDevNo = m_nWaveLengthType == 0?0:1;



    //光闸关闭流程
    Tasklist* pStandbyTasklist = new Tasklist("光闸关闭");
    m_pMaxEnergyConfirmWorkflow->addTaskList(pStandbyTasklist);
    pStandbyTasklist->addTask("1064nm光闸关闭",M_DEV_SHUTTER,0,M_PROCESS_ACTION,l_shutter_close_var);
    pStandbyTasklist->addTask("355nm光闸关闭",M_DEV_SHUTTER,1,M_PROCESS_ACTION,l_shutter_close_var);
    int i32Task = pStandbyTasklist->addTask("载物台光闸打开",M_DEV_SHUTTER,2,M_PROCESS_ACTION,l_shutter_open_var);
    int i32EnergyHomeTask = pStandbyTasklist->addTask("能量调节模块归零",M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_SETPARAM_ACTION);
    pStandbyTasklist->addTask(QString("能量调节%1%").arg(m_lfMinEnergy),M_DEV_ENERGY_ADJ,nAdjustEnergyDevNo,M_PROCESS_ACTION,QVariant(m_lfMinEnergy),0,{i32EnergyHomeTask});
    pStandbyTasklist->addTask("发布实验信息",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,100,{i32Task});

    l_motor_input.nMoveType = 0x01;
    l_motor_input.nMotorIndex = E_MOTOR_NEARFIELD_WHEEL;
    l_motor_input.nPlusCnt = 0x0;
    l_motor_var.setValue(l_motor_input);
    pStandbyTasklist->addTask("CCD衰减轮盘至保护位",M_DEV_MOTOR,0x0,M_PROCESS_ACTION,l_motor_var,200);


    //1on1
    if(m_nMeasureType == 0x0)
    {
        Tasklist* p1On1Tasklist = new Tasklist("1On1能量确认");
        m_pMaxEnergyConfirmWorkflow->addTaskList(p1On1Tasklist);

        //损伤检测系统准备
        l_defectcheck_data.nParamType = E_PARAM_PREPARE;//准备
        l_defectcheck_data_var.setValue(l_defectcheck_data);
        int i32DefectPrepareTaskID = p1On1Tasklist->addTask("损伤诊断系统准备",M_DEV_DEFECTCHECK,0x0,M_SETPARAM_ACTION,l_defectcheck_data_var);


        int i32TaskIndex = p1On1Tasklist->addTask("光闸放光-1发次",M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_process,0,{i32DefectPrepareTaskID});

        //损伤检测系统图像采集状态查询
        l_defectcheck_data.nParamType = E_PARAM_CAPIMAGE;
        l_defectcheck_data_var.setValue(l_defectcheck_data);
        int i32DefectCapImgTaskID = p1On1Tasklist->addTask("损伤诊断系统图像采集",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,0,{i32TaskIndex});

        l_defectcheck_data.nParamType = E_PARAM_DEFECT_REL;
        l_defectcheck_data_var.setValue(l_defectcheck_data);
        p1On1Tasklist->addTask("损伤判断结果",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,0,{i32DefectCapImgTaskID});

    }

    //Ron1能量确认流程
    if(m_nMeasureType == 0x01)
    {
        int i32EnergyAdjustTaskID = -1;
        int i32DefectprepareTaskID = -1;
        int i32DefectCheckTaskID = -1;
        int i32ShutterTaskID = -1;
        int i32DefectCapImgTaskID = -1;

        Tasklist* pROn1Tasklist = new Tasklist("ROn1能量确认");
        m_pMaxEnergyConfirmWorkflow->addTaskList(pROn1Tasklist);

        for(int nEnergyCnt = 0; nEnergyCnt < m_nEnergyIncreamCnt; nEnergyCnt++)
        {
            float lfEnergyPrecent = m_lfMinEnergy + nEnergyCnt*m_lfEnergyIncream;

             if(nEnergyCnt > 0)
            {
                 i32EnergyAdjustTaskID = pROn1Tasklist->addTask(QString("能量提升至%1%").arg(lfEnergyPrecent),M_DEV_ENERGY_ADJ, nAdjustEnergyDevNo, M_PROCESS_ACTION,QVariant(lfEnergyPrecent),0,{i32ShutterTaskID});
            }


            //损伤检测系统准备

             l_defectcheck_data.nParamType = E_PARAM_PREPARE;//准备
             l_defectcheck_data.nPointNo = 0x01;
             l_defectcheck_data.nShotNo = nEnergyCnt;
             l_defectcheck_data_var.setValue(l_defectcheck_data);
             i32DefectprepareTaskID = pROn1Tasklist->addTask("损伤诊断系统准备",M_DEV_DEFECTCHECK,0x0,M_SETPARAM_ACTION,l_defectcheck_data_var,0,{i32DefectCheckTaskID});


            //等待能量调节、损伤检测任务任务结束
            i32ShutterTaskID =  pROn1Tasklist->addTask(sShutterName+"发射1发并关闭",M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_process,0,{i32DefectprepareTaskID,i32EnergyAdjustTaskID});

            //等待显微系统结束
            l_defectcheck_data.nParamType = E_PARAM_CAPIMAGE;//采集图像状态
            l_defectcheck_data_var.setValue(l_defectcheck_data);
            i32DefectCapImgTaskID = pROn1Tasklist->addTask("损伤诊断系统图像采集",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,0,{i32ShutterTaskID});


            l_defectcheck_data.nParamType = E_PARAM_DEFECT_REL;//查询损伤检测结果
            l_defectcheck_data_var.setValue(l_defectcheck_data);
            i32DefectCheckTaskID = pROn1Tasklist->addTask("损伤判断结果",M_DEV_DEFECTCHECK, 0x0, M_PROCESS_ACTION,l_defectcheck_data_var,0,{i32DefectCapImgTaskID});

        }

    }



    Tasklist* pFinishedTask = new Tasklist("结束流程");
    m_pMaxEnergyConfirmWorkflow->addTaskList(pFinishedTask);
    pFinishedTask->addTask("关闭"+sShutterName,M_DEV_SHUTTER,nShutterDevNo,M_PROCESS_ACTION,l_shutter_close_var);
    pFinishedTask->addTask("关闭载物台光闸",M_DEV_SHUTTER,0x02,M_PROCESS_ACTION,l_shutter_close_var);

    l_defectcheck_data.nParamType = E_PARAM_DEFECT_CLOSE;//结束
    l_defectcheck_data_var.setValue(l_defectcheck_data);
    pFinishedTask->addTask("损伤系统结束流程",M_DEV_DEFECTCHECK,0x0,M_SETPARAM_ACTION,l_defectcheck_data_var);


    _pMaxEnergyConfirmWidget->setWorkflow(m_pMaxEnergyConfirmWorkflow);

}

/******************************************
* 功能:显示参数
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void MeasurePreReadyWidget::showParam()
{
    _pCurrentWaveLengthLabel->setText(m_tPlatformConfig->m_st_laserParam[m_tPlatformConfig->m_nCurrentWaveLength].m_sLaserName);

    _pEnergyIncreamCntEdit->setText(QString::number(m_nEnergyIncreamCnt));
    _pEnergyIncreamEdit->setText(QString::number(m_lfEnergyIncream));
    _pEnergyMinEdit->setText(QString::number(m_lfMinEnergy));
    _pEnergyMaxlabel->setText(QString::number(m_lfMaxEnergy));
    _pRatioEdit->setText(QString::number(0.0));

    onMeasureTypeSelect(0);

    createWaveLengthChangeWorkflow();

    createMaxEergyConfirmWorkflow();

}

 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void MeasurePreReadyWidget::setWidgetStyleSheet(QWidget* pWidget)
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
 bool MeasurePreReadyWidget::checkRunCondition(QString sworkflowName)
 {
     if(_pSaveParamButton->text() =="保存参数")
     {
        QMessageBox::warning(this,"提示","请保存参数");
        return false;
     }

     //提示确认是否已经切换光路
     if(sworkflowName.contains("最大能量"))
     {
         if(m_tPlatformConfig->m_nCurrentWaveLength != m_nWaveLengthType)
         {
            QString str;
            str = QString("当前光路为%1,选择光路为%2,请先执行光路切换任务,当前任务将终止执行!").arg(m_tPlatformConfig->m_st_laserParam[m_tPlatformConfig->m_nCurrentWaveLength].m_sLaserName).arg(m_tPlatformConfig->m_st_laserParam[m_nWaveLengthType].m_sLaserName);
            QMessageBox::warning(this,"提示",str);
            return false;
         }

         updateCommonInfo();
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
void MeasurePreReadyWidget::onMeasureTypeSelect(int nIndex)
{
    if(nIndex == 0)
    {
       _pEnergyMinLabel->setText("能量设置(%)");
       _pEnergyIncreamEdit->setEnabled(false);
       _pEnergyIncreamCntEdit->setEnabled(false);
    }
    else
    {
        _pEnergyMinLabel->setText("最小能量设置");
        _pEnergyIncreamEdit->setEnabled(true);
        _pEnergyIncreamCntEdit->setEnabled(true);
    }


}

 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void MeasurePreReadyWidget::onSaveParam()
 {
     if(_pSaveParamButton->text()=="保存参数")
     {
         m_nWaveLengthType = _pWaveLengthSelectComBox->currentIndex();
         m_nMeasureType = _pMeasureTypeSelectComBox->currentIndex();

         m_lfMinEnergy = _pEnergyMinEdit->text().toFloat();
         m_lfEnergyIncream = _pEnergyIncreamEdit->text().toFloat();
         m_nEnergyIncreamCnt = _pEnergyIncreamCntEdit->text().toInt();

         if(m_nEnergyIncreamCnt < 1 && m_nMeasureType == 0x01)
         {
             QMessageBox::warning(this,"提示","能量台阶设置错误，参数应大于等于1");
             return;
         }

         float lfMaxEnergy = m_lfMinEnergy + m_lfEnergyIncream*(m_nEnergyIncreamCnt-1);
         if(lfMaxEnergy > 100 && m_nMeasureType == 0x01)//Ron1时使用
         {
             QMessageBox::warning(this,"提示","最大能量超过100%,参数设置错误");
             return;
         }

         if(m_lfMinEnergy > 100 || m_lfMinEnergy < 0)
         {
             QMessageBox::warning(this,"提示","能量设置错误，参数范围为(0,100]");
             return;
         }

         //灰化按钮
         _pMeasureTypeSelectComBox->setEnabled(false);
         _pWaveLengthSelectComBox->setEnabled(false);
         _pEnergyIncreamCntEdit->setEnabled(false);
         _pEnergyIncreamEdit->setEnabled(false);
         _pEnergyMinEdit->setEnabled(false);

         //更新测量流程
         updateCommonInfo();//公共信息暂时未使用
         createWaveLengthChangeWorkflow();
         createMaxEergyConfirmWorkflow();

         _pSaveParamButton->setText("修改参数");
     }
     else
     {
         _pMeasureTypeSelectComBox->setEnabled(true);
         _pWaveLengthSelectComBox->setEnabled(true);
         _pEnergyIncreamCntEdit->setEnabled(true);
         _pEnergyIncreamEdit->setEnabled(true);
         _pEnergyMinEdit->setEnabled(true);

         onMeasureTypeSelect(_pMeasureTypeSelectComBox->currentIndex());

         _pSaveParamButton->setText("保存参数");
     }

 }


 /******************************************
 * 功能:更新公共信息 用于后续流程数据处理和数据保存使用
 * 输入:
 * 输出:
 * 返回值:
 * 维护记录:
 ******************************************/
 void MeasurePreReadyWidget::updateCommonInfo()
 {
     _tCommonInfo.nMeasureType = M_MEASURE_MAX_ENERGY_CONFIRM;
     _tCommonInfo.sExpNo = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");// 生成实验编号
 }

 /******************************************
 * 功能:工作流程结束后的处理
 * 输入:sworkflowName为流程名称 taskstatus执行结果
 * 输出:
 * 返回值:
 * 维护记录:
 ******************************************/
 void MeasurePreReadyWidget::onFinishWorkflow(QString sworkflowName,_Taskstatus taskstatus)
 {
     if(sworkflowName.contains("光路切换"))
     {
         if(taskstatus == M_TASK_STATUS_SUCCESS)
         {
             //更新当前光路
             m_tPlatformConfig->m_nCurrentWaveLength = m_nWaveLengthType;
             //更新配置文件
             emit signal_updatePlatformFile();

             //更新显示
             _pCurrentWaveLengthLabel->setText(m_tPlatformConfig->m_st_laserParam[m_tPlatformConfig->m_nCurrentWaveLength].m_sLaserName);
         }
         return;
     }

     QString str  = "";
     if(sworkflowName.contains("能量确认"))
     {
         if(taskstatus == M_TASK_STATUS_SUCCESS)
         {
             int nEnergyIncreamCnt = -2;
             for(Tasklist* pTasklist:m_pMaxEnergyConfirmWorkflow->m_tWorkflow)
             {
                 nEnergyIncreamCnt++;
                 for(TaskInfo* pTaskInfo:pTasklist->m_tTaskList)
                 {
                     if(pTaskInfo->m_nDevNo == M_DEV_DEFECTCHECK && pTaskInfo->m_bDefect)
                     {
                         //出现损伤  给出最大能量提示
                         if(m_nMeasureType == 0x0)//1On1
                         {
                             m_lfMaxEnergy = m_lfMinEnergy;
                         }
                         if(m_nMeasureType == 0x1)//ROn1
                         {
                             m_lfMaxEnergy = m_lfMinEnergy + m_lfEnergyIncream*nEnergyIncreamCnt;
                         }

                         str = QString("出现损伤,当前能量衰减模块为%1%").arg(m_lfMaxEnergy);
                         _pEnergyMaxlabel->setText(QString::number(m_lfMaxEnergy));


                         _pLogWidget->onShowlog(str);
                         QMessageBox::information(this,"提示",str);
                         return;
                     }
                 }
             }
         }
     }

     if(str.isEmpty())
     {
         QMessageBox::information(this,"提示","未出现损伤");
     }

 }

 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void MeasurePreReadyWidget::onMoveButtonClick(int nButtonId)
 {
     QString sMoveInfo = "";
     int l_nMotorIndex = -1;
     float l_lfDistance = _pMoveDistanceEdit->text().toFloat();
     int l_i32Coeff = 0;
     switch (nButtonId) {
     case 0x01://向左
            l_nMotorIndex = E_MOTOR_X;
            l_lfDistance = l_lfDistance*(-1);
            l_i32Coeff = _pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_X);
            sMoveInfo="开始向左运动..";
         break;
     case 0x02://向右
            l_nMotorIndex = E_MOTOR_X;
            l_i32Coeff = _pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_X);
            sMoveInfo="开始向右运动..";
         break;
     case 0x03://向上
            l_nMotorIndex = E_MOTOR_Y;
            l_lfDistance = l_lfDistance*(-1);
            l_i32Coeff = _pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Y);
            sMoveInfo="开始向上运动..";
         break;
     case 0x04://向下
            l_nMotorIndex = E_MOTOR_Y;
            l_i32Coeff = _pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Y);
            sMoveInfo="开始向下运动..";
         break;
     case 0x05://旋转台
            l_nMotorIndex = E_MOTOR_Z;
            l_lfDistance = _pRatioEdit->text().toFloat();
            if(l_lfDistance >= 60 || l_lfDistance < 0)
            {
                QMessageBox::information(this,"警告","旋转角度范围为[0,60],请重新输入！");
                return;
            }
            l_i32Coeff = _pDevConfig->getChannelCoeff(M_DEV_MOTOR,0x0,E_MOTOR_Z);
            sMoveInfo="开始调整旋转台角度..";
         break;
     default:
         return;
     }


     int nPlus = l_lfDistance*l_i32Coeff;//计算脉冲数

     QMotorDevCtrlBasic* pMotorCtrl = static_cast<QMotorDevCtrlBasic*>(_pPlatformDevCtrlManager->getDevCtrl(M_DEV_MOTOR,0x0));
     if(pMotorCtrl == nullptr)
     {
         _pLogWidget->onShowlog("电机设备未注册!");
         return;
     }

     if(!pMotorCtrl->checkMotorRunning(l_nMotorIndex))
     {
         _pLogWidget->onShowlog(sMoveInfo);
         QFuture<bool> l_future;
         if(l_nMotorIndex == E_MOTOR_Z)
         {
             //旋转轴为绝对运动
              l_future = QtConcurrent::run(pMotorCtrl,&QMotorDevCtrlBasic::moveToAbsolutePos,l_nMotorIndex,nPlus);
         }
         else
         {
             l_future = QtConcurrent::run(pMotorCtrl,&QMotorDevCtrlBasic::moveToRelationPos,l_nMotorIndex,nPlus);
         }

          QTime l_time;
         l_time.start();
         while(l_time.elapsed() < 1000*60)
         {
             QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
             if(l_future.isFinished())
             {
                 QVariant l_var =  pMotorCtrl->getMotorStatusInfo(l_nMotorIndex);
                 onShowMotorStatus(l_nMotorIndex,l_var);
                 break;
             }
         }

         if(l_time.elapsed() >= 1000*60)
         {
            _pLogWidget->onShowlog("运动超时!");
         }
    }
     else
     {
         _pLogWidget->onShowlog("当前电机正在运动，无法执行当前操作!");
     }

 }


 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void MeasurePreReadyWidget::onShowMotorStatus(int nMotorIndex, QVariant varStatus)
 {

     QString sMotorName = "";
     if(nMotorIndex == E_MOTOR_X)
     {
         sMotorName = "水平轴";
     }
     if(nMotorIndex == E_MOTOR_Y)
     {
         sMotorName = "垂直轴";
     }
     if(nMotorIndex == E_MOTOR_Z)
     {
         sMotorName = "旋转轴";
     }
     motorStatusdata tCurrentStatus = varStatus.value<motorStatusdata>();

     if(tCurrentStatus.nMotorStatus == 0x0)//未运动
     {
         _pLogWidget->onShowlog(sMotorName+"未运动");
     }
     else if(tCurrentStatus.nMotorStatus == 0x1)//正在运动
     {
          _pLogWidget->onShowlog(sMotorName+"正在运动");
     }
     else if(tCurrentStatus.nMotorStatus == 0x2)//运动成功
     {
         _pLogWidget->onShowlog(sMotorName+"运动到位");
     }
     else if(tCurrentStatus.nMotorStatus == 0x03)//运动失败
     {
          _pLogWidget->onShowlog(sMotorName+"  "+tCurrentStatus.sErroInfo,true);
     }

 }
