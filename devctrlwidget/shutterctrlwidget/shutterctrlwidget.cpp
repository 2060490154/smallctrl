#include "shutterctrlwidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

QShutterCtrlWidget::QShutterCtrlWidget(QList<QShutterDevCtrl*> pCtrllist,QWidget *parent) : QWidget(parent)
{

    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0,1);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,4);


    for(int i = 0; i < pCtrllist.size();i++)
    {
         QShutterCtrlItemWidget* pShutterItemWidget = new QShutterCtrlItemWidget(pCtrllist[i],this);

        playout->addWidget(pShutterItemWidget,i/2,i%2,1,1);//2行2列排列
    }

}

/*******************************************************************
**功能：子组件
**输入：
**输出：
**返回值：
*******************************************************************/
QShutterCtrlItemWidget::QShutterCtrlItemWidget(QShutterDevCtrl* pCtrl,QWidget *parent) : QGroupBox(parent)
{
    _pShutterCtrl = pCtrl;

    InitUI();

    m_pSetLimitDataButton->setHidden(true);

    connect(m_pOpenButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onOpenShutter);
    connect(m_pCloseButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onCloseShutter);
    connect(m_pRefreshButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onCheckStatus);
    connect(m_pSetLimitDataButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onSetLimitData);
    connect(m_psetProcessStatusButton,&QPushButton::clicked,this,&QShutterCtrlItemWidget::onSetProcessStatus);

    setWidgetStyleSheet(this);

    onCloseShutter();//启动时自动关闭
}

/*******************************************************************
**功能：初始化界面
**输入：
**输出：
**返回值：
*******************************************************************/
void QShutterCtrlItemWidget::InitUI()
{

    QLabel* pDevName= new QLabel(this);
    pDevName->setText(_pShutterCtrl->m_sDevName);
    pDevName->setStyleSheet("color:blue;font-size:16px;");

    QLabel* plabel1= new QLabel(this);
    plabel1->setText("设备IP:");

    QLabel* plabel2= new QLabel(this);
    plabel2->setText(_pShutterCtrl->m_tShutterDevInfo.tHostAddress.toString());

    QLabel* plabel3= new QLabel(this);
    plabel3->setText("当前状态:");



    QLabel* plabel8= new QLabel(this);
    plabel8->setText("设置发次数");


    m_pStatus = new QLabel(this);

    m_psetPlusCntEdit = new QTextEdit(this);

    m_psetPlusCntEdit->setText("1");


    m_pCloseButton = new QPushButton("挡光");
    m_pOpenButton = new QPushButton("放光");
    m_pRefreshButton = new QPushButton("状态查询");
    m_pSetLimitDataButton = new QPushButton("设置电压门限");

    m_psetProcessStatusButton = new QPushButton("放N发次光");

    m_pCloseButton->setMaximumWidth(100);
    m_pCloseButton->setMinimumHeight(30);

    m_pOpenButton->setMaximumWidth(100);
    m_pOpenButton->setMinimumHeight(30);

    m_pSetLimitDataButton->setMaximumWidth(100);
    m_pSetLimitDataButton->setMinimumHeight(30);

    m_pRefreshButton->setMaximumWidth(100);
    m_pRefreshButton->setMinimumHeight(30);

    m_psetProcessStatusButton->setMaximumWidth(100);
    m_psetProcessStatusButton->setMinimumHeight(30);



    m_psetPlusCntEdit->setMaximumWidth(200);
    m_psetPlusCntEdit->setMaximumHeight(30);



    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(new QLabel("设备名:"),0,0,1,1);
    playout->addWidget(pDevName,0,1,1,1);
    playout->addWidget(plabel1,1,0,1,1);
    playout->addWidget(plabel2,1,1,1,1);
    playout->addWidget(plabel3,2,0,1,1);
    playout->addWidget(m_pStatus,2,1,1,1);


    playout->addWidget(plabel8,3,0,1,1);
    playout->addWidget(m_psetPlusCntEdit,3,1,1,1);

    playout->addWidget(m_pOpenButton,4,0,1,1);
    playout->addWidget(m_pCloseButton,4,1,1,1);
    playout->addWidget(m_pRefreshButton,5,0,1,1);
    playout->addWidget(m_pSetLimitDataButton,5,1,1,1);
    playout->addWidget(m_psetProcessStatusButton,5,1,1,1);

    showStatus();

}

void QShutterCtrlItemWidget::showStatus()
{
    if(_pShutterCtrl->m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_NONE)
    {
        m_pStatus->setText("未知状态");
    }

    if(_pShutterCtrl->m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_OPENED)
    {
        m_pStatus->setText("放光");
    }
    if(_pShutterCtrl->m_tShutterDevInfo.nCurrentStatus == M_SHUTTER_STATUS_CLOSED)
    {
        m_pStatus->setText("挡光");
    }


    QString scurrentdata = QString("%1").arg(_pShutterCtrl->m_tShutterDevInfo.nCurrentVoltData);
    //m_pshowCurrentDataLabel->setText(scurrentdata);


    QString sdata = QString("%1").arg(_pShutterCtrl->m_tShutterDevInfo.nSetLimitData);
  //  m_pSetLimitEdit->setText(sdata);



}

void QShutterCtrlItemWidget::onOpenShutter()
{
    bool bRel;

    bRel = _pShutterCtrl->SetDevStatus(true);

    if(bRel == false)
    {
        QMessageBox::warning(this,"提示","打开光闸失败");
        return;
    }

    showStatus();
}

void QShutterCtrlItemWidget::onCloseShutter()
{
    bool bRel;

    bRel = _pShutterCtrl->SetDevStatus(false);

    if(bRel == false)
    {
        QMessageBox::warning(this,"提示","关闸吸合失败");
        return;
    }

    showStatus();
}

//查询状态
void QShutterCtrlItemWidget:: onCheckStatus()
{
    bool bRel = _pShutterCtrl->checkDevStatus();
    if(bRel == false)
    {
        QMessageBox::warning(this,"提示","查询状态失败");
        return;
    }

    showStatus();

}

//设置门限值
void QShutterCtrlItemWidget:: onSetLimitData()
{
//    int nData = m_pSetLimitEdit->toPlainText().toInt();
//    if(nData <= 0)
//    {
//        QMessageBox::warning(this,"提示","门限设置必须大于0");
//        return;
//    }


//    bool bRel = _pShutterCtrl->SetDevLimit(nData);
//    if(bRel == false)
//    {
//        QMessageBox::warning(this,"提示","设置门限值失败");
//        return;
//    }

    showStatus();

}

//设置流程
void QShutterCtrlItemWidget::onSetProcessStatus()
{
    int nPlusCnt = m_psetPlusCntEdit->toPlainText().toInt();
    int nDelayTime = 0;

    if(nPlusCnt <= 0 )
    {
        QMessageBox::warning(this,"提示","脉冲数必须大于0");
        return;
    }

    bool bRel = _pShutterCtrl->setProcessStatus(nPlusCnt,nDelayTime);

    if(!bRel )
    {
        QMessageBox::warning(this,"错误","执行超时");
        return;
    }

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QShutterCtrlItemWidget::setWidgetStyleSheet(QWidget* pWidget)
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
