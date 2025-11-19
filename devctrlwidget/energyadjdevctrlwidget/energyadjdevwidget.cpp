/***************************************************************************
**                                                                                               **
**QEnergyAdjDevWidget为电机控制组件，包含多个QEnergyAdjItemWidget，每个
** QEnergyAdjItemWidget对应一个电机
** 提供的功能函数为：
**
**
**                                                                                                 **
****************************************************************************
**创建人：李刚
** 创建时间：2018.05.21
** 修改记录：
**
****************************************************************************/
#include "energyadjdevwidget.h"
#include "qmessagebox.h"
#include <QTime>
#include <QDebug>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QEnergyAdjDevWidget::QEnergyAdjDevWidget(vector<tEnergyAdjDevConfig> pConfigList,QWidget *parent) : QWidget(parent)
{
    m_pEnergyAdjDevConfigInfoList = pConfigList;


    InitUI();

}

void QEnergyAdjDevWidget::InitUI()
{
    size_t i = 0;

    QGridLayout* playout = new QGridLayout(this);
    playout->setRowStretch(0,1);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,1);
    for(size_t i = 0; i < m_pEnergyAdjDevConfigInfoList.size() ; i++)
    {
        QEnergyAdjItemWidget* pEnergyAdjItemWidget = new QEnergyAdjItemWidget(&m_pEnergyAdjDevConfigInfoList.at(i),this);
        playout->addWidget(pEnergyAdjItemWidget,i/4,i%4,1,1);
    }


    QLabel* pInfoLabel= new QLabel(this);
    pInfoLabel->setText("能量调节大于1%（输入能量大于400mJ)时，可能会损坏科学CCD，请注意！");
    pInfoLabel->setStyleSheet("color:red;font-size:30px;font-family: 'Microsoft YaHei'");
    pInfoLabel->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

    playout->addWidget(pInfoLabel,i/4+1,0,1,2);

}


//单个电机的控制模块
QEnergyAdjItemWidget::QEnergyAdjItemWidget(const tEnergyAdjDevConfig* pMotorConfigInfo,QWidget *parent) : QWidget(parent)
{
    m_pMotorConfigInfo = pMotorConfigInfo;

    m_pEnergyAdjDevCtrl = (QEnergyAdjustDevCtrl*)pMotorConfigInfo->pEnergyAdjDevCtrl;

    InitUI();

    showLimitStatus(false,false);
    if(m_pEnergyAdjDevCtrl != NULL)
    {
        connect(m_pEnergyAdjDevCtrl,&QEnergyAdjustDevCtrl::signal_updateEnergyAdjDevStatus,this,&QEnergyAdjItemWidget::onUpdateMotorInfo);
    }

    setWidgetStyleSheet(this);

    m_pLogList->setFrameShape(QListWidget::NoFrame);

}

//UI初始化
void QEnergyAdjItemWidget::InitUI()
{
  createDevItem();
  connect(m_pMoveBtn,&QPushButton::clicked,this,&QEnergyAdjItemWidget::onStartMove);
  connect(m_pImmidStopBtn,&QPushButton::clicked,this,&QEnergyAdjItemWidget::onStopMove);
  connect(m_pReturnZeroBtn,&QPushButton::clicked,this,&QEnergyAdjItemWidget::OnMove2Zero);

  m_pImmidStopBtn->setHidden(true);

  setlogInfo("第一次设置，请先进行归零操作再设置衰减");

}

//创建不带档位信息的UI
void QEnergyAdjItemWidget::createDevItem()
{
    QLabel* pDevName= new QLabel(this);
    pDevName->setText(m_pMotorConfigInfo->sDevName);
    pDevName->setStyleSheet("color:blue;font-size:16px;");

    m_pLogList = new QListWidget();
    m_pPostionEdit = new QTextEdit();
    m_pMoveBtn = new QPushButton("");
    m_pImmidStopBtn = new QPushButton("");
    m_pReturnZeroBtn = new QPushButton("");
    m_pLeftLimitStatusLabel = new QLabel();
    m_pRightLimitStatusLabel = new QLabel();
    m_pMotorMoveStatus = new QLabel("未知");
    m_pMotorMoveStatus->setStyleSheet("color:rgb(200,200,200)");

    m_pMotorCurrentStepLabel = new QLabel("0%");



    //设置按钮图标
    m_pMoveBtn -> setIcon(QIcon(":/motorpng/start.png")); //将图片设置到按钮上
    m_pMoveBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pMoveBtn->setFlat(true);
    m_pMoveBtn->setToolTip("开始运动");

    m_pImmidStopBtn -> setIcon(QIcon(":/motorpng/stop.png")); //将图片设置到按钮上
    m_pImmidStopBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pImmidStopBtn->setFlat(true);
    m_pImmidStopBtn->setToolTip("立即停止运动");

    m_pReturnZeroBtn -> setIcon(QIcon(":/motorpng/zero.png")); //将图片设置到按钮上
    m_pReturnZeroBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pReturnZeroBtn->setFlat(true);
    m_pReturnZeroBtn->setToolTip("归零");


    m_pLeftLimitStatusLabel->setToolTip("左限位");
    m_pRightLimitStatusLabel->setToolTip("右限位");

    m_pPostionEdit->setMaximumHeight(30);

    QGridLayout* playout = new QGridLayout(this);



    playout->addWidget(pDevName,0,0,1,3);

    playout->addWidget( new QLabel("能量设置"),1,0,1,1);
    playout->addWidget(m_pPostionEdit,1,1,1,4);
    playout->addWidget(new QLabel("%"),1,5,1,1);


    playout->addWidget(new QLabel("限位状态"),2,0,1,1);
    playout->addWidget(m_pLeftLimitStatusLabel,2,2,1,1);
    playout->addWidget(m_pRightLimitStatusLabel,2,3,1,1);

    playout->addWidget(new QLabel("运动状态"),3,0,1,1);
    playout->addWidget(m_pMotorMoveStatus,3,1,1,2);

    playout->addWidget(new QLabel("当前位置"),4,0,1,1);
    playout->addWidget(m_pMotorCurrentStepLabel,4,1,1,2);


    playout->addWidget(m_pMoveBtn,5,1,1,1);
    playout->addWidget(m_pImmidStopBtn,5,2,1,1);
    playout->addWidget(m_pReturnZeroBtn,5,3,1,1);

    playout->addWidget(m_pLogList,6,0,1,6);

}


/*******************************************************************
**功能：显示限位信息
**输入：nLeftStatus--左限位状态 nRightStatus--右限位状态
**输出：
**返回值：
*******************************************************************/
void QEnergyAdjItemWidget::showLimitStatus(bool isLeftLimit,bool isRightLimit)
{
    QString sLimitStatus[2] = {":/motorpng/ok.png",":/motorpng/limit.png"};//处于左限位状态  0--未处于 1--处于限位状态

    int nLeftStatus = isLeftLimit==true?1:0;
    int nRightStatus = isRightLimit == true?1:0;


    m_pLeftLimitStatusLabel->setText("\
                    <table width=\"100%\">\
                    <tbody>\
                    <tr>\
                    <td><img src=\""+sLimitStatus[nLeftStatus]+"\"></td>\
                    </tr>\
                    </tbody>\
                    </table>");

    m_pRightLimitStatusLabel->setText("\
                    <table width=\"100%\">\
                    <tbody>\
                    <tr>\
                    <td><img src=\""+sLimitStatus[nRightStatus]+"\" ></td>\
                    </tr>\
                    </tbody>\
                    </table>");

}

/*******************************************************************
**功能：更新状态显示信息
**输入：bShowMoveStatus=true为全部都更新 bShowMoveStatus=false只更新限位和当前位置信息
**输出：
**返回值：
*******************************************************************/
void QEnergyAdjItemWidget::onUpdateMotorInfo(QVariant varStatus,bool bShowMoveStatus)
{

    energyAdjustDevStatus tCurrentStatus = varStatus.value<energyAdjustDevStatus>();

    showLimitStatus(tCurrentStatus.isLeftLimit,tCurrentStatus.isRightLimit);//更新限位信息

    m_pMotorCurrentStepLabel->setText(QString("%1 %").arg(tCurrentStatus.lfcurrentpresent)); //显示当前位置

    if(!bShowMoveStatus)
    {
        return;
    }

    if(tCurrentStatus.nStatus != 0x0 && tCurrentStatus.nStatus != 0x01)
    {
        setlogInfo("调节结束!");
    }

    if(tCurrentStatus.nStatus == 0x0)//未运动
    {
        m_pMotorMoveStatus->setStyleSheet("color:rgb(200,200,200)");
        m_pMotorMoveStatus->setText("未运动");
    }
    else if(tCurrentStatus.nStatus == 0x1)//正在运动
    {
         m_pMotorMoveStatus->setStyleSheet("color:rgb(0,0,255)");
         m_pMotorMoveStatus->setText("正在运动");
    }
    else if(tCurrentStatus.nStatus == 0x2)//运动成功
    {
        m_pMotorMoveStatus->setStyleSheet("color:rgb(34,177,76)");
        m_pMotorMoveStatus->setText("调节到位");
    }
    else if(tCurrentStatus.nStatus == 0x03)//运动失败
    {
         m_pMotorMoveStatus->setStyleSheet("color:rgb(255,0,0)");
         m_pMotorMoveStatus->setText(tCurrentStatus.sErroInfo);
    }



}



/*******************************************************************
**功能：开始运动
**输入：百分比
**输出：百分比
**返回值：无
*******************************************************************/
void QEnergyAdjItemWidget::onStartMove()
{
    bool bOk = false;
    float lfPrecent = m_pPostionEdit->toPlainText().toFloat(&bOk);
    if(lfPrecent > 100 || lfPrecent < 0 || bOk == false)
    {
        setlogInfo("输入信息有误,请重新输入");
        return;
    }

    if(!m_pEnergyAdjDevCtrl->checkMotorRunning())
    {
        setlogInfo(QString("开始调节能量至:%1 %").arg(m_pPostionEdit->toPlainText()));
    //    m_pEnergyAdjDevCtrl->MoveToPrecent(lfPrecent);
        QFuture<bool> future = QtConcurrent::run(m_pEnergyAdjDevCtrl,&QEnergyAdjustDevCtrl::MoveToPrecent,lfPrecent);
    }
    else
    {
        QMessageBox::warning(this,"警告","当前设备正在调节运动，无法执行该操作!");
    }


}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QEnergyAdjItemWidget::OnMove2Zero()
{
    if(!m_pEnergyAdjDevCtrl->checkMotorRunning())
    {
        setlogInfo("开始回零...");
        //m_pEnergyAdjDevCtrl->moveToHome();
        QtConcurrent::run(m_pEnergyAdjDevCtrl,&QEnergyAdjustDevCtrl::moveToHome);
    }
    else
    {
        QMessageBox::warning(this,"警告","当前电机正在运动，无法执行当前操作!");
    }
}
/*******************************************************************
**功能：停止当前电机的运动
**输入：无
**输出：无
**返回值：无
*******************************************************************/
void QEnergyAdjItemWidget::onStopMove()
{

    setlogInfo("正在停止运动...");
    QtConcurrent::run(m_pEnergyAdjDevCtrl,&QEnergyAdjustDevCtrl::stopMove);

}

/*******************************************************************
**功能：打印提示信息
**输入：slog 提示信息内容
**输出：
**返回值：
*******************************************************************/
void QEnergyAdjItemWidget::setlogInfo(QString slog)
{
    QTime time = QTime::currentTime();

    QString str = time.toString("hh:mm:ss") +"    "+ slog;


    m_pLogList->addItem(str);
    m_pLogList->setCurrentRow(m_pLogList->count()-1);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyAdjItemWidget::setWidgetStyleSheet(QWidget* pWidget)
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
