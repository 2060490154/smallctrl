#include "pointareawidget.h"
#include <qpainter.h>
#include <qmessagebox.h>
#include <qdebug.h>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

QPointAreaWidget::QPointAreaWidget(float lfMaxWidth,float lfMaxheight,int nType,float lfStep,QWidget *parent) : QWidget(parent)
{
    m_nSizeType = nType;//矩形
    m_lfStep = lfStep;
    m_lfMaxWidth = lfMaxWidth;
    m_lfMaxHigh = lfMaxheight;

    m_nValidPointNum = 0;


    InitUI();

    onUpdatePointUI(m_nSizeType,m_lfStep,lfMaxWidth,lfMaxheight,lfMaxWidth);

    _pUpdatePointTimer = new QTimer(this);
    connect(_pUpdatePointTimer,&QTimer::timeout,this,&QPointAreaWidget::onUpdatePointStatus);

    _pUpdatePointTimer->start(1000);


}

/*******************************************************************
**功能：初始化界面
**输入：
**输出：
**返回值：
*******************************************************************/
void QPointAreaWidget::InitUI()
{
    _pCustomPlot = new QCustomPlot(this);
    _pCustomPlot->sizePolicy().setHeightForWidth(true);
    _pCustomPlot->setMaximumSize(QSize(500,500));
    _pCustomPlot->setMinimumSize(QSize(500,500));//必须保证为正方形
    _pPointInfoLabel = new QLabel(this);


    _pCustomPlot->xAxis->setVisible(false);
    _pCustomPlot->yAxis->setVisible(false);

    _pCustomPlot->addGraph();//点--未执行
    _pCustomPlot->addGraph();//点--正在执行
    _pCustomPlot->addGraph();//点--执行失败
    _pCustomPlot->addGraph();//点-执行成功
    _pCustomPlot->addGraph();//点-有损伤

    _pCustomPlot->addGraph();//外围形状
    _pCustomPlot->addGraph();//外围形状

    QGridLayout* playout = new QGridLayout(this);
    playout->setMargin(0);
    playout->setSpacing(0);
    playout->setContentsMargins(0,0,0,0);

    for(int i = 0; i < 6; i++)
    {
        playout->setRowStretch(i,1);
    }

    playout->addWidget(_pCustomPlot,0,0,4,6);
    playout->addWidget(_pPointInfoLabel,4,1,1,1);

    setWidgetStyleSheet(this);

}



/******************************************
* 功能:
* 输入:nSizeType= 2时 半径值为lfMaxWidth=lfMaxHeigh=r
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QPointAreaWidget::onUpdatePointUI(int nSizeType,float lfStep,float MaxWidth,float MaxHeigh,float Radius)
{
    //更新参数
    m_nSizeType = nSizeType;
    m_lfStep = lfStep;
    m_lfMaxHigh = MaxHeigh;
    m_lfMaxWidth = MaxWidth;

    float l_lfMaxHigh=m_lfMaxHigh;
    float l_lfMaxWidth = m_lfMaxWidth;

    _tPointList.clear();

    if(m_nSizeType == M_SIZETYPE_CIRCL)
    {
        m_lfRadius = Radius;
        m_lfMaxHigh = Radius;
        m_lfMaxWidth = Radius;

        l_lfMaxHigh = m_lfRadius/1.414;
        l_lfMaxWidth = m_lfRadius/1.414;
    }

    //s型数据
    float lfsub1 = l_lfMaxHigh/m_lfStep - (int)(l_lfMaxHigh/m_lfStep);//计算起始点
    float lfStartPos_Heigt = m_lfStep - ((1.0-lfsub1)/2.0 )* m_lfStep;
    float lfsub = l_lfMaxWidth/m_lfStep - (int)(l_lfMaxWidth/m_lfStep);
    float lfStartPos_Width =m_lfStep - ((1.0-lfsub)/2.0) * m_lfStep;

    for(int j = 0; j < (int)(l_lfMaxHigh/m_lfStep);j ++)
    {
        //奇数行
        if((j) %2 == 1)
        {
            for(int i = (int)(l_lfMaxWidth/m_lfStep) ; i >= 0; i--)
            {
                QPointInfo PointInfo;
                PointInfo.nStatus = M_STATUS_UNUSE;
                PointInfo.xPos = i*lfStep-l_lfMaxWidth/2.0+lfStartPos_Width;
                PointInfo.yPos = j*lfStep-l_lfMaxHigh/2.0+lfStartPos_Heigt;

                if(PointInfo.xPos < l_lfMaxWidth/2.0 && PointInfo.yPos < l_lfMaxHigh/2.0&&PointInfo.xPos >- l_lfMaxWidth/2.0 && PointInfo.yPos >- l_lfMaxHigh/2.0)
                {
                    _tPointList.append(PointInfo);
                }

            }

        }
        else if((j) %2 == 0)//偶数行
        {
            for(int i = 0 ; i <= (int)(l_lfMaxWidth/m_lfStep); i ++)
            {
                QPointInfo PointInfo;
                PointInfo.nStatus = M_STATUS_UNUSE;
                PointInfo.xPos = i*lfStep-l_lfMaxWidth/2.0+lfStartPos_Width;
                PointInfo.yPos = j*lfStep-l_lfMaxHigh/2.0+lfStartPos_Heigt;

                if(PointInfo.xPos < l_lfMaxWidth/2.0 && PointInfo.yPos < l_lfMaxHigh/2.0&&PointInfo.xPos >- l_lfMaxWidth/2.0 && PointInfo.yPos >- l_lfMaxHigh/2.0)
                {
                    _tPointList.append(PointInfo);
                }

            }
        }

    }

    showSizePoint();//画物体外围边框
    onUpdatePointStatus();
}


//显示当前点信息
void QPointAreaWidget::showPointInfo()
{
    int nTotal = _tPointList.size();
    int ndestroyNum = 0;//损坏点
    int nfailedNum = 0;//失败点

    for(int i = 0; i < _tPointList.size();i++ )
    {
        if(_tPointList[i].nStatus == M_STATUS_FAILED)
        {
            nfailedNum++;
        }
        if(_tPointList[i].nStatus == 0x04)
        {
            ndestroyNum++;
        }
    }
    if(m_nValidPointNum == 0)
    {
        m_nValidPointNum = nTotal;
    }


    _pPointInfoLabel->setText("\
                    <table width=\"100%\">\
                    <tbody >\
                    <tr align=\"left\" valign=\"middle\">\
                    <td>"+QString("总扫描点数:%1").arg(nTotal)+"</td></tr>\
                    <tr align=\"left\" valign=\"middle\"><td></td></tr>\
                    <tr align=\"left\" valign=\"middle\">\
                    <td>"+QString("实际使用点数:%1").arg(m_nValidPointNum)+"</td></tr>\
                    <tr align=\"left\" valign=\"middle\"><td></td></tr>\
                    <tr align=\"left\" valign=\"middle\">\
                    <td>"+QString("损伤点数:%1").arg(ndestroyNum)+"</td></tr>\
                    </tbody>\
                    </table>");


}

void QPointAreaWidget::onUpdatePointStatus()
{
    QVector<double> l_yData[5];
    QVector<double> l_xData[5];//点数据

    // 设置画笔风格
    QPen drawPen;
    QColor l_color[5];
    l_color[0] = Qt::gray;
    l_color[1] = Qt::yellow;
    l_color[2] = Qt::red;
    l_color[3] = Qt::green;
    l_color[4] = Qt::blue;


    foreach (QPointInfo item, _tPointList)
    {
        l_xData[item.nStatus].push_back(item.xPos);
        l_yData[item.nStatus].push_back(item.yPos);
    }

    //绘制散点
    for(int i = 0; i < 5; i++)
    {
        drawPen.setColor(l_color[i]);
        drawPen.setWidth(8);

        _pCustomPlot->graph(i)->setPen(drawPen);
        _pCustomPlot->graph(i)->setLineStyle(QCPGraph::lsNone);
        _pCustomPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 2));
        _pCustomPlot->graph(i)->setData(l_xData[i], l_yData[i]);
    }

    float lfMax = m_lfMaxWidth > m_lfMaxHigh?m_lfMaxWidth:m_lfMaxHigh;

    _pCustomPlot->xAxis->setRange(-lfMax/2-lfMax*0.1,lfMax/2+lfMax*0.1);
    _pCustomPlot->yAxis->setRange(-lfMax/2-lfMax*0.1,lfMax/2+lfMax*0.1);
    _pCustomPlot->replot();

    showPointInfo();

}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QPointAreaWidget::showSizePoint()
{
    QVector<double> l_sizeXData;//外围数据
    QVector<double> l_sizeYData;
    QVector<double> l_sizeY1Data;



    if(m_nSizeType == M_SIZETYPE_RECT)//画矩形外框
    {
        QPen drawPen;
        drawPen.setColor(Qt::gray);
        drawPen.setWidth(4);

        l_sizeXData<<-m_lfMaxWidth/2.0<<-m_lfMaxWidth/2.0<<m_lfMaxWidth/2.0<<m_lfMaxWidth/2.0;
        l_sizeYData<<-m_lfMaxHigh/2.0<<m_lfMaxHigh/2.0<<m_lfMaxHigh/2.0<<-m_lfMaxHigh/2.0;

        _pCustomPlot->graph(5)->setData(l_sizeXData, l_sizeYData);
        _pCustomPlot->graph(5)->setPen(drawPen);

        l_sizeXData.clear();
        l_sizeYData.clear();
        l_sizeXData<<-m_lfMaxWidth/2.0<<m_lfMaxWidth/2.0;
        l_sizeYData<<-m_lfMaxHigh/2.0<<-m_lfMaxHigh/2.0;

        _pCustomPlot->graph(6)->setData(l_sizeXData, l_sizeYData);
        _pCustomPlot->graph(6)->setPen(drawPen);
    }

    if(m_nSizeType == M_SIZETYPE_CIRCL)//画圆形外框
    {

        float lfRadis = m_lfMaxWidth/2.0;
        float lfRadis2 = lfRadis*lfRadis;

        float i = -lfRadis;
        while(1)
        {
            if(i > lfRadis)
            {
                l_sizeXData<<lfRadis;
                l_sizeYData<<0.0;
                l_sizeY1Data<<0.0;
                break;
            }
            float lf_y = sqrt(lfRadis2-i*i);
            l_sizeXData<<i;
            l_sizeYData<<lf_y;
            l_sizeY1Data<<-lf_y;

            i = i + lfRadis*0.01;
        }

        QPen drawPen;
        drawPen.setColor(Qt::gray);
        drawPen.setWidth(4);

        _pCustomPlot->graph(5)->setData(l_sizeXData, l_sizeYData);
        _pCustomPlot->graph(5)->setPen(drawPen);

        _pCustomPlot->graph(6)->setData(l_sizeXData, l_sizeY1Data);
        _pCustomPlot->graph(6)->setPen(drawPen);
    }

    _pCustomPlot->replot();

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QList<QPointInfo>* QPointAreaWidget::getPointlist()
{
    return &_tPointList;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QPointAreaWidget::setWidgetStyleSheet(QWidget* pWidget)
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
