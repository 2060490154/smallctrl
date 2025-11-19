/***************************************************************************
**                                                                                               **
**QMotorCtrlWidget为电机控制组件，包含多个QMotorCtrlItemWidget，每个
** QMotorCtrlItemWidget对应一个电机
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
#include "motorctrlwidget.h"
#include "qmessagebox.h"
#include <QTime>
#include <QDebug>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

QMotorCtrlWidget::QMotorCtrlWidget(vector<tMotorConfigInfo> pConfigList,QWidget *parent) : QWidget(parent)
{
    m_pMotorConfigInfoList = pConfigList;


    InitUI();

}

void QMotorCtrlWidget::InitUI()
{
    QGridLayout* playout = new QGridLayout(this);
    playout->setRowStretch(0,1);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,1);
    for(size_t i = 0; i < m_pMotorConfigInfoList.size() ; i++)
    {
        //旋转台不提供归单独操作 线缆限制
        if(m_pMotorConfigInfoList[i].nMotorIndex == 0x02)
        {
           continue;
        }

        QMotorCtrlItemWidget* pMotorWidget = new QMotorCtrlItemWidget(&m_pMotorConfigInfoList.at(i),this);
        playout->addWidget(pMotorWidget,i/3,i%3,1,1);
    }

}


//单个电机的控制模块
QMotorCtrlItemWidget::QMotorCtrlItemWidget(const tMotorConfigInfo* pMotorConfigInfo,QWidget *parent) : QWidget(parent)
{
    m_pMotorConfigInfo = pMotorConfigInfo;

    m_nMotorIndex = m_pMotorConfigInfo->nMotorIndex;//电机编号

    m_pMotorCtrl = (QMotorDevCtrlBasic*)pMotorConfigInfo->pMotorCtrl;



    InitUI();


    showLimitStatus(false,false);
    if(m_pMotorCtrl != NULL)
    {
        connect(m_pMotorCtrl,&QMotorDevCtrlBasic::signal_updateMotorStatus,this,&QMotorCtrlItemWidget::onUpdateMotorInfo);
    }

    setWidgetStyleSheet(this);

    m_pLogList->setFrameShape(QListWidget::NoFrame);

}

//UI初始化
void QMotorCtrlItemWidget::InitUI()
{
  if(m_pMotorConfigInfo->tPostionConfig.size() == 0)
  {
      createNoGearUI();
      connect(m_pMoveLeftBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onMoveLeft);
      connect(m_pMoveRightBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onMoveRight);
      connect(m_pImmidStopBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onStopMove);
      connect(m_pReturnZeroBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::OnMove2Zero);
  }
  else
  {
      createGearUI();

      foreach (tMotorPostionInfo item, m_pMotorConfigInfo->tPostionConfig)
      {
          m_pPostionCombox->addItem(item.sPostionName);
      }
      connect(m_pStartMoveBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onStartMove);
      connect(m_pImmidStopBtn,&QPushButton::clicked,this,&QMotorCtrlItemWidget::onStopMove);

  }

}

//创建不带档位信息的UI
void QMotorCtrlItemWidget::createNoGearUI()
{
    QLabel* pDevName= new QLabel(this);
    pDevName->setText(m_pMotorConfigInfo->sMotorName);
    pDevName->setStyleSheet("color:blue;font-size:16px;");

    m_pLogList = new QListWidget();
    m_pPostionEdit = new QTextEdit();
    m_pMoveLeftBtn = new QPushButton("");
    m_pMoveRightBtn = new QPushButton("");
    m_pImmidStopBtn = new QPushButton("");
    m_pReturnZeroBtn = new QPushButton("");
    m_pLeftLimitStatusLabel = new QLabel();
    m_pRightLimitStatusLabel = new QLabel();
    m_pMotorMoveStatus = new QLabel("未知");
    m_pMotorMoveStatus->setStyleSheet("color:rgb(200,200,200)");

    m_pMotorCurrentStepLabel = new QLabel("0");



    //设置按钮图标
    m_pMoveLeftBtn -> setIcon(QIcon(":/motorpng/left.png")); //将图片设置到按钮上
    m_pMoveLeftBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pMoveLeftBtn->setFlat(true);
    m_pMoveLeftBtn->setToolTip("向左运动");

    m_pMoveRightBtn -> setIcon(QIcon(":/motorpng/right.png")); //将图片设置到按钮上
    m_pMoveRightBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pMoveRightBtn->setFlat(true);
    m_pMoveRightBtn->setToolTip("向右运动");

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

    playout->addWidget( new QLabel("运动位置"),1,0,1,1);
    playout->addWidget(m_pPostionEdit,1,1,1,4);
    playout->addWidget(new QLabel(m_pMotorConfigInfo->sUnit),1,5,1,1);


    playout->addWidget(new QLabel("限位状态"),2,0,1,1);
    playout->addWidget(m_pLeftLimitStatusLabel,2,2,1,1);
    playout->addWidget(m_pRightLimitStatusLabel,2,3,1,1);

    playout->addWidget(new QLabel("运动状态"),3,0,1,1);
    playout->addWidget(m_pMotorMoveStatus,3,1,1,2);

    playout->addWidget(new QLabel("当前位置"),4,0,1,1);
    playout->addWidget(m_pMotorCurrentStepLabel,4,1,1,2);


    playout->addWidget(m_pMoveLeftBtn,5,1,1,1);
    playout->addWidget(m_pImmidStopBtn,5,2,1,1);
    playout->addWidget(m_pMoveRightBtn,5,3,1,1);
    playout->addWidget(m_pReturnZeroBtn,5,4,1,1);

    playout->addWidget(m_pLogList,6,0,1,6);

}

/*******************************************************************
**功能：创建待档位的电机控制界面
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::createGearUI()
{
    QLabel* pDevName= new QLabel(this);
    pDevName->setText(m_pMotorConfigInfo->sMotorName);
    pDevName->setStyleSheet("color:blue;font-size:16px;");

    m_pLogList = new QListWidget();

    m_pPostionCombox = new QComboBox(this);
    m_pStartMoveBtn = new QPushButton("");
    m_pLeftLimitStatusLabel = new QLabel();
    m_pRightLimitStatusLabel = new QLabel();
    m_pMotorMoveStatus = new QLabel("未知");
    m_pImmidStopBtn = new QPushButton("");

    m_pMotorMoveStatus->setStyleSheet("color:rgb(200,200,200)");

    m_pMotorCurrentStepLabel = new QLabel("0");

    //设置按钮图标
    m_pStartMoveBtn -> setIcon(QIcon(":/motorpng/start.png")); //将图片设置到按钮上
    m_pStartMoveBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pStartMoveBtn->setFlat(true);
    m_pStartMoveBtn->setToolTip("运动");

    m_pImmidStopBtn -> setIcon(QIcon(":/motorpng/stop.png")); //将图片设置到按钮上
    m_pImmidStopBtn -> setIconSize(QSize(50,50));//根据实际调整图片大小
    m_pImmidStopBtn->setFlat(true);
    m_pImmidStopBtn->setToolTip("立即停止运动");


    m_pLeftLimitStatusLabel->setToolTip("左限位");
    m_pRightLimitStatusLabel->setToolTip("右限位");

    m_pPostionCombox->setMinimumHeight(30);


    QGridLayout* playout = new QGridLayout(this);


    playout->addWidget(pDevName,0,0,1,3);

    playout->addWidget( new QLabel("运动位置"),1,0,1,1);
    playout->addWidget(m_pPostionCombox,1,1,1,4);


    playout->addWidget(new QLabel("限位状态"),2,0,1,1);
    playout->addWidget(m_pLeftLimitStatusLabel,2,2,1,1);
    playout->addWidget(m_pRightLimitStatusLabel,2,3,1,1);

    playout->addWidget(new QLabel("运动状态"),3,0,1,1);
    playout->addWidget(m_pMotorMoveStatus,3,1,1,2);

    playout->addWidget(new QLabel("当前位置"),4,0,1,1);
    playout->addWidget(m_pMotorCurrentStepLabel,4,1,1,2);

    playout->addWidget(m_pStartMoveBtn,5,1,1,1);
    playout->addWidget(m_pImmidStopBtn,5,3,1,1);

    playout->addWidget(m_pLogList,6,0,1,6);

}

/*******************************************************************
**功能：显示限位信息
**输入：nLeftStatus--左限位状态 nRightStatus--右限位状态
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::showLimitStatus(bool isLeftLimit,bool isRightLimit)
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
**输入：控制箱编号(0-2)  电机编号(0-3)  状态数据
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::onUpdateMotorInfo(int nMotorIndex,QVariant varStatus)
{

    if(nMotorIndex != m_nMotorIndex)
    {
        return;
    }

    motorStatusdata tCurrentStatus = varStatus.value<motorStatusdata>();

    showLimitStatus(tCurrentStatus.isLeftLimit,tCurrentStatus.isRightLimit);//更新限位信息

    m_pMotorCurrentStepLabel->setText(QString("%1").arg(tCurrentStatus.nMotorCurrentPos)); //显示当前位置
    if(tCurrentStatus.nMotorStatus != 0x0 && tCurrentStatus.nMotorStatus != 0x01)
    {
        setlogInfo("运动结束!");
    }

    if(tCurrentStatus.nMotorStatus == 0x0)//未运动
    {
        m_pMotorMoveStatus->setStyleSheet("color:rgb(200,200,200)");
        m_pMotorMoveStatus->setText("未运动");
    }
    else if(tCurrentStatus.nMotorStatus == 0x1)//正在运动
    {
         m_pMotorMoveStatus->setStyleSheet("color:rgb(0,0,255)");
         m_pMotorMoveStatus->setText("正在运动");
    }
    else if(tCurrentStatus.nMotorStatus == 0x2)//运动成功
    {
        m_pMotorMoveStatus->setStyleSheet("color:rgb(34,177,76)");
        m_pMotorMoveStatus->setText("运动到位");
    }
    else if(tCurrentStatus.nMotorStatus == 0x03)//运动失败
    {
         m_pMotorMoveStatus->setStyleSheet("color:rgb(255,0,0)");
         m_pMotorMoveStatus->setText(tCurrentStatus.sErroInfo);
    }



}
/*******************************************************************
**功能：向左运动
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::onMoveLeft()
{
    float lfPos = m_pPostionEdit->toPlainText().toFloat();
    int nPlus = lfPos*m_pMotorConfigInfo->nMotorCoeff;//计算脉冲数

    if(!m_pMotorCtrl->checkMotorRunning(m_nMotorIndex))
    {
        setlogInfo("执行向左运动开始");
        QtConcurrent::run(m_pMotorCtrl,&QMotorDevCtrlBasic::moveToRelationPos,m_nMotorIndex,nPlus*(-1));
    }
    else
    {
        QMessageBox::warning(this,"警告","当前电机正在运动，无法执行当前操作!");
    }

}

/*******************************************************************
**功能：向右运动
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::onMoveRight()
{
    float lfPos = m_pPostionEdit->toPlainText().toFloat();
    int nPlus = lfPos*m_pMotorConfigInfo->nMotorCoeff;//计算脉冲数

    if(!m_pMotorCtrl->checkMotorRunning(m_nMotorIndex))
    {
        setlogInfo("执行向右运动开始");
        QtConcurrent::run(m_pMotorCtrl,&QMotorDevCtrlBasic::moveToRelationPos,m_nMotorIndex,nPlus);
    }
    else
    {
        QMessageBox::warning(this,"警告","当前电机正在运动，无法执行当前操作!");
    }
}

/*******************************************************************
**功能：开始运动 运动到指定档位
**输入：档位选择
**输出：当前档位
**返回值：无
*******************************************************************/
void QMotorCtrlItemWidget::onStartMove()
{
    int nCurrentSel = m_pPostionCombox->currentIndex();

    if(!m_pMotorCtrl->checkMotorRunning(m_nMotorIndex))
    {
        setlogInfo(QString("开始运动至档位:%1").arg(m_pPostionCombox->currentText()));
        QFuture<bool> future = QtConcurrent::run(m_pMotorCtrl,&QMotorDevCtrlBasic::moveToGear,m_nMotorIndex,m_pMotorConfigInfo->tPostionConfig[nCurrentSel].nGearIndex);
    }
    else
    {
        QMessageBox::warning(this,"警告","当前电机正在运动，无法执行当前操作!");
    }


}

/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::OnMove2Zero()
{
    if(!m_pMotorCtrl->checkMotorRunning(m_nMotorIndex))
    {
        setlogInfo("开始回零...");
        QtConcurrent::run(m_pMotorCtrl,&QMotorDevCtrlBasic::moveToHome,m_nMotorIndex);
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
void QMotorCtrlItemWidget::onStopMove()
{
      QtConcurrent::run(m_pMotorCtrl,&QMotorDevCtrlBasic::stopMove,m_nMotorIndex);
}

/*******************************************************************
**功能：打印提示信息
**输入：slog 提示信息内容
**输出：
**返回值：
*******************************************************************/
void QMotorCtrlItemWidget::setlogInfo(QString slog)
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
void QMotorCtrlItemWidget::setWidgetStyleSheet(QWidget* pWidget)
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
