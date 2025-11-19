/**************************************************
*文件说明:工作流程界面
*创建人:李刚
*维护记录:
*2020-02-11 创建
******************************************************/
#include "workflowwidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif


QWorkflowWidget::QWorkflowWidget(QDevCtrlManager* pPlatformDevCtrlManager,checkRunConditionFun pcheckRunConditionFun)
{

    _pWorkflowlist = nullptr;

    _checkRunConditionFun = pcheckRunConditionFun;

    _pPlatformDevCtrlManager = pPlatformDevCtrlManager;

    _pStatusImage = new QImage();

    _pTaskProcessThread = QWorkflowProcessThread::getInstance(_pPlatformDevCtrlManager);

    _pUpdateStatusTimer = new QTimer(this);

    _pWorkFlowDetailWidget = nullptr;

    initWidget();

    _pProcessBar->setHidden(true);


    connect(_pTaskProcessThread,&QWorkflowProcessThread::finished,this,&QWorkflowWidget::onTaskFinished);

    connect(_pUpdateStatusTimer,&QTimer::timeout,this,&QWorkflowWidget::onUpdateStatus);

    connect(_pShowDetailButton,&QPushButton::clicked,this,&QWorkflowWidget::onShowDetailInfo);

    connect(_pStartProcessButton,&QPushButton::clicked,this,&QWorkflowWidget::onStartProcessTask);


    _pUpdateStatusTimer->start(1000);


    //应用样式
    QFile file(":qss/workflowui.qss");
    file.open(QFile::ReadOnly);
    QString stylesheet = file.readAll();
    this->setStyleSheet(stylesheet);


    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);// 阴影偏移
    shadowEffect->setColor(QColor(180,180,180));// 阴影颜色;
    shadowEffect->setBlurRadius(10);// 阴影半径;
    this->setGraphicsEffect(shadowEffect);// 给窗口设置上当前的阴影效果;

}


/******************************************
* 功能:析构  清理资源
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QWorkflowWidget::~QWorkflowWidget()
{
    if(_pWorkFlowDetailWidget != nullptr)
    {
        _pWorkFlowDetailWidget->close();
        delete _pWorkFlowDetailWidget;
    }

    delete _pStatusImage;

}


/******************************************
* 功能:设置Tasklist 用于任务的执行
* 输入:tTaskList  任务列表
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::setWorkflow(Workflow*tworkflowlist)
{
    _pWorkflowlist = tworkflowlist;

    _pNameLabel->setText(_pWorkflowlist->m_sWorkflowName);

    if(_pWorkFlowDetailWidget != nullptr)
    {
        _pWorkFlowDetailWidget->close();
        delete _pWorkFlowDetailWidget;
        _pWorkFlowDetailWidget = nullptr;
    }

}


/******************************************
* 功能:初始化界面
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::initWidget()
{
    this->setObjectName("workflowwidget");

    _pStartProcessButton = new QPushButton(M_START_PROCESS_TEXT,this);

    _pShowDetailButton = new QPushButton("查看详细",this);
    _pProcessBar = new RingsProgressbar(this);

    _pNameLabel = new QLabel("",this);
    _pNameLabel->setStyleSheet("color:blue;font-size:20px;font-family: 'Microsoft YaHei'");

    _pWorkFlowStatusLabel = new QLabel();

    _pProcessBar->setMaximumHeight(100);
    _pWorkFlowStatusLabel->setMaximumHeight(100);


    QGridLayout* playout = new QGridLayout(this);
    for(int i = 0; i < 5; i++)
    {
        playout->setColumnStretch(i,1);
    }

    playout->addWidget(_pNameLabel,0,0,1,2);
    playout->addWidget(_pWorkFlowStatusLabel,0,2,1,2);

    playout->addWidget(_pProcessBar,0,2,1,2);

    playout->addWidget(_pStartProcessButton,1,1,1,1);

    playout->addWidget(_pShowDetailButton,1,3,1,1);
}


/******************************************
* 功能:开始执行任务流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::onStartProcessTask()
{

    if(_pStartProcessButton->text() == M_START_PROCESS_TEXT)//开始
    {
        if(_pTaskProcessThread->checkThreadRunning())
        {
            QMessageBox::warning(this,"提示","已有任务正在执行，请等待其他任务完成后再执行本任务");
            return;
        }

        if(_checkRunConditionFun!= nullptr)//check 本流程运行的前提条件是否满足  需用户实现具体检测内容
        {
            if(!_checkRunConditionFun(_pWorkflowlist->m_sWorkflowName))//不满足条件则不运行
            {
                return;
            }
        }


        _pTaskProcessThread->setWorkflowlist(_pWorkflowlist);//设置要执行的流程
        _pTaskProcessThread->start();


        _pProcessBar->setHidden(false);
        _pProcessBar->setPersent(0);

        _pWorkFlowStatusLabel->setHidden(true);

        _pStartProcessButton->setText(M_STOP_PROCESS_TEXT);


        emit showlogInfo(_pWorkflowlist->m_sWorkflowName +" 开始执行...");

        emit startProcess(_pWorkflowlist->m_sWorkflowName);

        qInfo()<<"开始执行 "+_pWorkflowlist->m_sWorkflowName;

    }else if(_pStartProcessButton->text() == M_STOP_PROCESS_TEXT)//停止
    {
        emit showlogInfo("正在执行终止流程，请稍等...",true);
        _pTaskProcessThread->runTaskListStop();//如果停止流程前 需要执行流程则进行修改

        qInfo()<<"人为终止 "+_pWorkflowlist->m_sWorkflowName;
    }


}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::onUpdateStatus()
{

    QString sToolTip = "";

    if(_pWorkflowlist == nullptr)
    {
        return;
    }
    //显示执行状态
    if(_pWorkflowlist->m_nWorkflowRel == M_TASK_STATUS_UNRUN)
    {
        _pStatusImage->load(":/png/unrun.png");
        sToolTip = "未执行";
    }
    else if(_pWorkflowlist->m_nWorkflowRel == M_TASK_STATUS_RUNNING)
    {
        _pStatusImage->load(":/png/running");
        sToolTip = "正在执行";
    }
    else if(_pWorkflowlist->m_nWorkflowRel == M_TASK_STATUS_FAILD)
    {
        _pStatusImage->load(":/png/failed.png");
        sToolTip = "执行失败";
    }
    else if(_pWorkflowlist->m_nWorkflowRel == M_TASK_STATUS_SUCCESS)
    {
        _pStatusImage->load(":/png/ok.png");
        sToolTip = "执行成功";
    }

    _pWorkFlowStatusLabel->setPixmap(QPixmap::fromImage(*_pStatusImage));//状态

    _pWorkFlowStatusLabel->setToolTip(sToolTip);


    if(_pWorkflowlist == nullptr)
    {
        return;
    }

    //显示执行进度
    int nTotalNum = _pWorkflowlist->getAllTaskNum();
    int nFinishedNum = 0;

    if(_pTaskProcessThread->isRunning())
    {
        foreach (Tasklist* pTasklistItem, _pWorkflowlist->m_tWorkflow)
        {
            foreach (TaskInfo* item, pTasklistItem->m_tTaskList)
            {
                if(item->m_nTaskStatus != M_TASK_STATUS_UNRUN)
                {
                    nFinishedNum++;
                }
            }

        }
    }

    if(nTotalNum != 0)
    {
        _pProcessBar->setPersent(nFinishedNum*100/nTotalNum);
    }

}

/******************************************
* 功能:任务执行结束
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::onTaskFinished()
{
    if(_pWorkflowlist == nullptr)
    {
        return;
    }

    if(_pTaskProcessThread->m_sCurrentWorkflowName != _pWorkflowlist->m_sWorkflowName)
    {
        return;
    }


    _pProcessBar->setHidden(true);//隐藏
    _pStartProcessButton->setText(M_START_PROCESS_TEXT);

    emit showlogInfo(_pWorkflowlist->m_sWorkflowName +" 任务结束!");
    _pWorkFlowStatusLabel->setHidden(false);//显示当前的状态
    onUpdateStatus();

    emit finishedProcess(_pWorkflowlist->m_sWorkflowName,_pWorkflowlist->m_nWorkflowRel);//对外输出 结束消息

    qInfo()<<_pWorkflowlist->m_sWorkflowName+"执行结束,执行结果:"<<_pWorkflowlist->m_nWorkflowRel;

}

/******************************************
* 功能:显示任务执行详细信息
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::onShowDetailInfo()
{
    if(_pWorkFlowDetailWidget != nullptr)
    {
        _pWorkFlowDetailWidget->close();
        delete _pWorkFlowDetailWidget;
    }

    _pWorkFlowDetailWidget = new QWorkflowDetailWidget();
    _pWorkFlowDetailWidget->setWindowTitle(_pWorkflowlist->m_sWorkflowName);

    _pWorkFlowDetailWidget->setAttribute(Qt::WA_ShowModal);

    _pWorkFlowDetailWidget->resize(800,600);
    _pWorkFlowDetailWidget->show();

    _pWorkFlowDetailWidget->setWorkflow(_pWorkflowlist);//设置所要显示的流程信息
}

void QWorkflowWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/******************************************
* 功能:设置暂停时所要执行的流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::setSuspendWorkflow(vector<TaskInfo*> pTaskList)
{
    _pSuspendTasklist = pTaskList;
}

/******************************************
* 功能:设置停止时所要执行的流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::setStopWorkflow(vector<TaskInfo*> pTaskList)
{
    _pStopTasklist = pTaskList;
}

/******************************************
* 功能:设置继续时所要执行的流程
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowWidget::setContinueWorkflow(vector<TaskInfo*> pTaskList)
{
    _pContinueTasklist = pTaskList;
}
