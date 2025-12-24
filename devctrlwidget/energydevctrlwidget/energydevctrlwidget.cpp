#include "energydevctrlwidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QEnergyDevCtrlWidget::QEnergyDevCtrlWidget(vector<tEnergdevconfig> pDevConfig,QWidget *parent) : QWidget(parent)
{
    _pEnergyDevCtrllist =pDevConfig;

    QGridLayout* playout = new QGridLayout(this);

    playout->setRowStretch(0,1);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,4);

    for(tEnergdevconfig devItem:_pEnergyDevCtrllist)
    {
        devItem.pEnergyDevCtrl->openDev();//先打开串口

        for(int i = 0; i < devItem.sChannelNamelist.size(); i++)
        {
            QString sTemp = devItem.sChannelNolist[i];
            int nPort = sTemp.toInt();
            QEnergyDevCtrlItemWidget* pDevItemWidget = new QEnergyDevCtrlItemWidget(devItem.pEnergyDevCtrl,nPort,devItem.sChannelNamelist[i],this);
            playout->addWidget(pDevItemWidget,i/2,i%2,1,1);//2行2列排列
        }
    }
}

/*******************************************************************
**功能：子组件
**输入：
**输出：
**返回值：
*******************************************************************/
QEnergyDevCtrlItemWidget::QEnergyDevCtrlItemWidget(QEnergyDevCtrl* pCtrl,int nChannelNo,QString sDevName,QWidget *parent) : QGroupBox(parent)
{
    _MeasureRangeVaulelist = QList<QString>()<<"3.0mJ"<<"10.0mJ"<<"30.0mJ"<<"100.0mJ"<<"300.0mJ"<<"1J"<<"3J";//测量范围 单位为mj
    _i32MeasureRangeStartIndex = 19;
    _MeasureWaveLengthlist = QList<QString>()<<"1064nm"<<"355nm";

    _pEnergyDevCtrl = pCtrl;
    _sDevName = sDevName;
    _nChannelNo = nChannelNo;

    InitUI();

    foreach (QString sItem, _MeasureRangeVaulelist)
    {
        _pMeasureRangeComBox->addItem(sItem);
    }

    foreach (QString sItem, _MeasureWaveLengthlist)
    {
        _pMeasureWaveLengthComBox->addItem(sItem);
    }



    connect(_pOpenButton,&QPushButton::clicked,this,&QEnergyDevCtrlItemWidget::onConnectDev);
    connect(_pRefreshStatusButton,&QPushButton::clicked,this,&QEnergyDevCtrlItemWidget::onRefreshParam);
    connect(_pCheckMeasureValueButton,&QPushButton::clicked,this,&QEnergyDevCtrlItemWidget::onCheckMeasureValue);
    connect(_pSetParamButton,&QPushButton::clicked,this,&QEnergyDevCtrlItemWidget::onSetParam);




    setWidgetStyleSheet(this);

    //刷新状态

    _pEnergyDevCtrl->setChannelStopCap(_nChannelNo);
    delayTime(50);

//    _pEnergyDevCtrl->setChannelTriggerLevel(_nChannelNo,5);
//    delayTime(100);

    _pEnergyDevCtrl->getDevParam(_nChannelNo);
    delayTime(50);

    _pEnergyDevCtrl->getDevConnectStatus(_nChannelNo);
    delayTime(50);

    _pEnergyDevCtrl->setChannelStartCap(_nChannelNo);//开始采集数据

    m_showDataTimer.setInterval(500);
    connect(&m_showDataTimer,&QTimer::timeout,this,&QEnergyDevCtrlItemWidget::onShowMeasureVaule);
    m_showDataTimer.start();

    showDevParam();

}

/*******************************************************************
**功能：初始化界面
**输入：
**输出：
**返回值：
*******************************************************************/
void QEnergyDevCtrlItemWidget::InitUI()
{
    _pTriggerLevelEdit = new QTextEdit();
    _pTriggerLevelEdit->setMaximumHeight(30);
 //   _pTriggerLevelEdit->setMaximumSize(QSize(150,30));

    _pMeasureRangeComBox = new QComboBox();
  //  _pMeasureRangeComBox->setMaximumSize(QSize(150,40));
    _pMeasureRangeComBox->setMinimumHeight(30);

    _pTriggerModeComBox = new QComboBox();
  //  _pTriggerModeComBox->setMaximumSize(QSize(150,40));
    _pTriggerModeComBox->setMinimumHeight(30);

    _pTriggerModeComBox->addItem("实时采集");
    _pTriggerModeComBox->addItem("外触发采集");


    _pMeasureWaveLengthComBox = new QComboBox();
//    _pMeasureWaveLengthComBox->setMaximumSize(QSize(150,40));
    _pMeasureWaveLengthComBox->setMinimumHeight(30);

    _pDevStatusLabel = new QLabel();

    QLabel* pDevName= new QLabel(this);
    pDevName->setText(_sDevName);
    pDevName->setStyleSheet("color:blue;font-size:16px;");


//    QLabel* pDevIP= new QLabel(this);
//    pDevIP->setText(_pEnergyDevCtrl->m_tDevParam.sDevIP+":通道 "+QString::number(_nChannelNo));
//    pDevIP->setMaximumSize(QSize(150,30));

    _pMeasureValueEdit = new QTextEdit("0.0");
    _pMeasureValueEdit->setMaximumHeight(30);
   // _pMeasureValueEdit->setMaximumSize(QSize(150,30));


    _pCheckMeasureValueButton = new QPushButton("查询测量值");
    _pOpenButton = new QPushButton("打开");
    _pRefreshStatusButton = new QPushButton("刷新参数");
    _pSetParamButton = new QPushButton("设置参数");


    _pCheckMeasureValueButton->setMaximumWidth(100);
    _pCheckMeasureValueButton->setMinimumHeight(30);

    _pOpenButton->setMaximumWidth(100);
    _pOpenButton->setMinimumHeight(30);

    _pRefreshStatusButton->setMaximumWidth(100);
    _pRefreshStatusButton->setMinimumHeight(30);

    _pSetParamButton->setMaximumWidth(100);
    _pSetParamButton->setMinimumHeight(30);


    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(pDevName,0,0,1,1);
    playout->addWidget(_pDevStatusLabel,0,1,1,1);

//    playout->addWidget(new QLabel("设备地址:"),1,0,1,1);
//    playout->addWidget(pDevIP,1,1,1,1);

    playout->addWidget(new QLabel("测量范围:"),1,0,1,1);
    playout->addWidget(_pMeasureRangeComBox,1,1,1,1);

    playout->addWidget(new QLabel("测量波长:"),2,0,1,1);
    playout->addWidget(_pMeasureWaveLengthComBox,2,1,1,1);

    playout->addWidget(new QLabel("触发模式"),3,0,1,1);
    playout->addWidget(_pTriggerModeComBox,3,1,1,1);


    playout->addWidget(new QLabel("触发门限(%):"),4,0,1,1);
    playout->addWidget(_pTriggerLevelEdit,4,1,1,1);

    playout->addWidget(new QLabel("当前测量值(mJ):"),5,0,1,1);
    playout->addWidget(_pMeasureValueEdit,5,1,1,1);



//    playout->addWidget(_pOpenButton,7,0,1,1);
    playout->addWidget(_pSetParamButton,7,0,1,1);
//    playout->addWidget(_pCheckMeasureValueButton,8,0,1,1);
    playout->addWidget(_pRefreshStatusButton,7,1,1,1);





}

/******************************************
* 功能:显示状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrlItemWidget::showDevParam()
{
    if(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].bConnected)
    {
        QPixmap l_pixmap(":/energypng/connect.png");
        _pDevStatusLabel->setPixmap(l_pixmap);
        _pDevStatusLabel->setToolTip("已连接");
    }
    else
    {
        QPixmap l_pixmap(":/energypng/disconnect.png");
        _pDevStatusLabel->setPixmap(l_pixmap);
        _pDevStatusLabel->setToolTip("未连接");
    }

    //测量范围
    _pMeasureRangeComBox->setCurrentIndex(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].i32CurrentRange - _i32MeasureRangeStartIndex);

    //测量波长
    int nIndex = -1;
    for(int i = 0; i < _pMeasureWaveLengthComBox->count();i++)
    {
        QString sWavelength = _pMeasureWaveLengthComBox->itemText(i);
        QString sCurrentLen = QString("%1nm").arg(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].i32CurrentWaveLength);

        if(sWavelength == sCurrentLen)
        {
            nIndex = i;
            break;
        }
    }
    _pMeasureWaveLengthComBox->setCurrentIndex(nIndex);

    _pTriggerLevelEdit->setText(QString("%1").arg(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].lfTriggerLevel));

    if(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].bConnected)
    {
        _pOpenButton->setText("断开");
    }
    else
    {
        _pOpenButton->setText("连接");
    }

    //触发模式
    int nTriggerModeIndex = _pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].i32ExTriggerMode==0?0:1;
    _pTriggerModeComBox->setCurrentIndex(nTriggerModeIndex);


}


/******************************************
* 功能:查询参数
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrlItemWidget:: onRefreshParam()
{
    if(_pEnergyDevCtrl->openDev() != E_SENDCMD_OK || _pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].bConnected == false)
    {
        QMessageBox::warning(this,"提示","连接设备失败!");
        return;
    }

    _pEnergyDevCtrl->setChannelStopCap(_nChannelNo);//先暂停采集

    if(_pEnergyDevCtrl->getDevParam(_nChannelNo) != E_SENDCMD_OK)
    {
        QMessageBox::warning(this,"提示","查询参数失败!");
        return;
    }

    _pEnergyDevCtrl->setChannelStartCap(_nChannelNo);//开始采集

    showDevParam();

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrlItemWidget::onConnectDev()
{
    if(_pOpenButton->text()=="打开")
    {
        if(_pEnergyDevCtrl->openDev() != E_SENDCMD_OK)
        {
            QMessageBox::warning(this,"提示","连接设备失败!");
            return;
        }

        _pOpenButton->setText("断开");

    }
    else if(_pOpenButton->text()=="断开")
    {
        _pEnergyDevCtrl->closeDev();
        _pOpenButton->setText("打开");
    }

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrlItemWidget::onCheckMeasureValue()
{
    if(_pEnergyDevCtrl->openDev() != E_SENDCMD_OK)
    {
        QMessageBox::warning(this,"提示","连接设备失败!");
        return;
    }
    if(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].bConnected == false)
    {
        QMessageBox::warning(this,"提示","探头未连接!");
        return;
    }


    _pMeasureValueEdit->setText(QString("%1").arg(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].lfMeasureValue));

}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrlItemWidget:: onSetParam()
{

    _pEnergyDevCtrl->setChannelStopCap(_nChannelNo);//先暂停采集

    int i32MeasuerRange = _pMeasureRangeComBox->currentIndex()+_i32MeasureRangeStartIndex;
    if(_pEnergyDevCtrl->setChannelMeauserRange(_nChannelNo,i32MeasuerRange) != E_SENDCMD_OK)
    {
         QMessageBox::warning(this,"提示","设置测量范围失败!");
    }

    delayTime(100);



    int nWavelength = _pMeasureWaveLengthComBox->currentIndex()==0?1064:355;
    if( nWavelength != 0)
    {
        if(_pEnergyDevCtrl->setChannelWaveLength(_nChannelNo,nWavelength) != E_SENDCMD_OK)
        {
             QMessageBox::warning(this,"提示","设置测量波长失败!");
        }
    }
    else
    {
        QMessageBox::warning(this,"提示","测量波长参数错误!");
    }

    delayTime(100);

    float lfTiggerLevel = _pTriggerLevelEdit->toPlainText().toFloat();

    if(_pEnergyDevCtrl->setChannelTriggerLevel(_nChannelNo,lfTiggerLevel) != E_SENDCMD_OK)
    {
         QMessageBox::warning(this,"提示","设置触发门限失败!");
    }

    delayTime(100);

    //设置触发模式
    bool bExtTrigger = _pTriggerModeComBox->currentIndex()==1?true:false;
    if(_pEnergyDevCtrl->setChannelExTrigger(_nChannelNo,bExtTrigger) != E_SENDCMD_OK)
    {
         QMessageBox::warning(this,"提示","设置触发模式失败!");
    }
    delayTime(100);



    onRefreshParam();

    _pEnergyDevCtrl->setChannelStartCap(_nChannelNo);//开始采集

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrlItemWidget::onShowMeasureVaule()
{
   _pMeasureValueEdit->setText(QString("%1").arg(_pEnergyDevCtrl->m_tDevParam.channelParam[_nChannelNo].lfMeasureValue));

}


void QEnergyDevCtrlItemWidget::delayTime(int time)
{
    QTime l_time;
    l_time.start();
    while(l_time.elapsed() < time)
    {

    }
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QEnergyDevCtrlItemWidget::setWidgetStyleSheet(QWidget* pWidget)
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
