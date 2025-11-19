#include "mulitctrlwidget.h"
#include <qgridlayout.h>
#include <qfile.h>
#include <QRegExpValidator>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QMulitCtrlWidget::QMulitCtrlWidget(vector<CKxccdCtrl*> pKxccdCtrllist,QWidget *parent) : QWidget(parent)
{


    m_tDevCtrllist = pKxccdCtrllist;

    m_nDevNum = m_tDevCtrllist.size();

    for (int i = 0; i < 256; i++)
    {
        m_vcolorTable.append(qRgb(i, i, i));
    }


    for(int i = 0; i < m_nDevNum;i++)
    {
        QString sName = QString("CCD %1").arg(i+1);

        connect(m_tDevCtrllist[i],&CKxccdCtrl::signal_showKxccdImag,this,&QMulitCtrlWidget::onShowDevImage);

        QKxccdctrlWidget* pKxccdCtrlWidget = new QKxccdctrlWidget(m_tDevCtrllist[i]);
        connect(pKxccdCtrlWidget,&QKxccdctrlWidget::signal_doubleClick_Image,this,&QMulitCtrlWidget::onDoubleClickImge);
        m_tDevCtrlWidgetlist.append(pKxccdCtrlWidget);
    }

    InitUI();

    //默认选中第一个
    m_tCustonLabelArray[0]->setClickedStyle();
    m_pCurrentDevCtrl = m_tDevCtrllist[0];

    //只有一个设备的时候
    if(m_nDevNum == 1)
    {
        onDoubleClickDevlabel(0);
     //   m_tCustonLabelArray[0]->hide();
    }

}

/******************************************
* 功能:析构
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QMulitCtrlWidget::~QMulitCtrlWidget()
{
//    for(int i = m_tDevCtrllist.size()-1; i >=0;i--)
//    {
//        if(m_tDevCtrllist[i] != nullptr)
//        {
//            m_tDevCtrllist[i]->closeDev();
//        }

//    }

}

void QMulitCtrlWidget::InitUI()
{

    int nColNum = 0;
    int nRow = 0;

    m_pCCDDevLabelWidget = new QWidget(this);



    //控制界面
    if(m_nDevNum <= 4)
    {
        nColNum = m_nDevNum/2+1;
        nRow = m_nDevNum/2;
    }
    else
    {
        nColNum = m_nDevNum/4+1;
        nRow = m_nDevNum/4;
    }



    QString sTemp;
    QGridLayout* pCCDDevLabelLayout = new QGridLayout(m_pCCDDevLabelWidget);

    for(size_t i = 0; i < m_tDevCtrllist.size();i++)
    {
        sTemp = QString("CCD%1").arg(i+1);
        QCustomLabel* pLabel = new QCustomLabel(sTemp,i);

        m_tCustonLabelArray.append(pLabel);

        connect(pLabel,&QCustomLabel::clicked,this,&QMulitCtrlWidget::onSelectDev);
        connect(pLabel,&QCustomLabel::doubleclicked,this,&QMulitCtrlWidget::onDoubleClickDevlabel);
        if(m_nDevNum<= 4)
        {
            pLabel->setMaximumSize(QSize(500,500));
            pCCDDevLabelLayout->addWidget(pLabel,i/2,(i%2),1,1);//2行2列排列
        }
        else
        {
            pLabel->setMaximumSize(QSize(300,300));
            pCCDDevLabelLayout->addWidget(pLabel,i/4,(i%4),1,1);
        }

    }


    m_pMainWidget  = new QWidget();
    QGridLayout* pLayout = new QGridLayout(m_pMainWidget);
    pLayout->setColumnStretch(0,8);
    pLayout->setColumnStretch(1,1);

    for(int i = 0; i < m_nDevNum;i++)
    {
       pLayout->addWidget(m_tDevCtrlWidgetlist[i],0,1,1,1);
       m_tDevCtrlWidgetlist[i]->showScrollArea(false);
       m_tDevCtrlWidgetlist[i]->hide();

       m_tCustonLabelArray[i]->setLabelTitle(m_tDevCtrllist[i]->m_sDevName);
    }
    pLayout->addWidget(m_pCCDDevLabelWidget,0,0,1,1);
    m_tDevCtrlWidgetlist[0]->show();

    QGridLayout* playout = new QGridLayout(this);
    playout->addWidget(m_pMainWidget);

}

void QMulitCtrlWidget::onSelectDev(int nIndex)
{
    //清除样式
    m_tDevCtrlWidgetlist[nIndex]->show();
    for(int i = 0; i < m_nDevNum;i++)
    {
        if(i != nIndex)
        {
            m_tCustonLabelArray[i]->clearStyle();
            m_tDevCtrlWidgetlist[i]->hide();
        }
    }

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMulitCtrlWidget::onDoubleClickDevlabel(int nIndex)
{
    for(int i = 0; i < m_nDevNum;i++)
    {
        m_tDevCtrlWidgetlist[i]->hide();
    }
    m_pCCDDevLabelWidget->hide();

    delete m_pMainWidget->layout();
    QGridLayout* pLayout = new QGridLayout(m_pMainWidget);
    pLayout->addWidget(m_tDevCtrlWidgetlist[nIndex],0,0,1,1);
    m_tDevCtrlWidgetlist[nIndex]->show();
    m_tDevCtrlWidgetlist[nIndex]->showScrollArea(true);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMulitCtrlWidget::onDoubleClickImge(int nIndex)
{
    if(m_nDevNum == 1)//只有一个设备的时候 不进行切换
    {
        return;
    }
    delete m_pMainWidget->layout();

    m_pCCDDevLabelWidget->show();


    QGridLayout* pLayout = new QGridLayout(m_pMainWidget);
    pLayout->setColumnStretch(0,8);
    pLayout->setColumnStretch(1,1);

    for(int i = 0; i < m_nDevNum;i++)
    {
       pLayout->addWidget(m_tDevCtrlWidgetlist[i],0,1,1,1);
       m_tDevCtrlWidgetlist[i]->showScrollArea(false);
       m_tDevCtrlWidgetlist[i]->hide();
    }
    pLayout->addWidget(m_pCCDDevLabelWidget,0,0,1,1);
    m_tDevCtrlWidgetlist[nIndex]->show();
}

//显示缩略图
void QMulitCtrlWidget::onShowDevImage(int nDevIndex,uchar* pBuff,int nWidth,int nHigh)
{
    if(nDevIndex >= m_tCustonLabelArray.size())
    {
        return;
    }

    m_tCustonLabelArray[nDevIndex]->m_nFrameCnt = m_tDevCtrllist[nDevIndex]->m_nRcvFrameCount;

    QImage OrigImageBuffer = QImage(pBuff, nWidth, nHigh, QImage::Format_Indexed8);  //封装QImage
    OrigImageBuffer.setColorTable(m_vcolorTable); //设置颜色表
    OrigImageBuffer = OrigImageBuffer.mirrored(false,true);//倒向
    m_tCustonLabelArray[nDevIndex]->showImage(OrigImageBuffer);
}



