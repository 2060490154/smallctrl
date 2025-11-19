#include "systemdatawidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QSystemDataWidget::QSystemDataWidget(CDbDataProcess* pDbProcess,QWidget *parent) : QWidget(parent)
{
    _pDbProcess = pDbProcess;

    _nWaveType = 0;

    _pWaveTypeCombox = new QComboBox();
    _pWaveTypeCombox->addItem("1064nm");
    _pWaveTypeCombox->addItem("355nm");
    _pWaveTypeCombox->setCurrentIndex(_nWaveType);
    _pWaveTypeCombox->setMaximumHeight(25);

    _pInfoLabel = new QLabel(this);
    _pInfoLabel->hide();

    _pAreaPlot = new QCustomPlot();
    _pCoeffPlot = new QCustomPlot();
    _pPlusWidthPlot = new QCustomPlot();

    _pRefreshBtn = new QPushButton("刷新数据",this);
    _pRefreshBtn->setMaximumWidth(100);


    setPlotUI(_pAreaPlot,"时间","数据(J/cm²)","光斑面积数据变化趋势");
    setPlotUI(_pCoeffPlot,"时间","数据","分光比数据变化趋势");
    setPlotUI(_pPlusWidthPlot,"时间","数据","脉宽数据变化趋势");


    _pAreaPlot->setObjectName("area");
    _pCoeffPlot->setObjectName("coeff");
    _pPlusWidthPlot->setObjectName("pluswidth");

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(new QLabel("光路选择"),0,0,1,1);
    playout->addWidget(_pWaveTypeCombox,0,1,1,1);
    playout->addWidget(_pRefreshBtn,0,2,1,1);
    playout->addWidget(_pAreaPlot,1,0,1,6);
    playout->addWidget(_pCoeffPlot,2,0,1,3);
    playout->addWidget(_pPlusWidthPlot,2,3,1,3);

    connect(_pAreaPlot,&QCustomPlot::selectionChangedByUser,this,&QSystemDataWidget::onSelectData);
    connect(_pCoeffPlot,&QCustomPlot::selectionChangedByUser,this,&QSystemDataWidget::onSelectData);
    connect(_pPlusWidthPlot,&QCustomPlot::selectionChangedByUser,this,&QSystemDataWidget::onSelectData);

    connect(_pRefreshBtn,&QPushButton::clicked,this,&QSystemDataWidget::onRefreshData);

    void(QComboBox::*fp)(int)=&QComboBox::currentIndexChanged;
    connect(_pWaveTypeCombox,fp,this,&QSystemDataWidget::onWaveTypeSelect);


    onRefreshData();
}

/******************************************
* 功能:初始化显示数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QSystemDataWidget::setPlotUI(QCustomPlot* plot,QString XText,QString YText,QString sTitle)
{
    QBrush qBrush(QColor(255,255,255));//设置背景色

    QPen pen;
    pen.setColor(QColor(36, 160, 230, 200));
    pen.setStyle(Qt::DashDotLine);
    pen.setWidthF(2);


    //边框阴影效果
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(6);
    effect->setColor(QColor(150,150,150));
    effect->setOffset(5,5);
    plot->setGraphicsEffect(effect);


    //设置网格线样式
     QPen xGridPen = plot->xAxis->grid()->pen();
     xGridPen.setStyle(Qt::SolidLine);
     plot->xAxis->grid()->setPen(xGridPen);
     plot->yAxis->grid()->setPen(xGridPen);
     //设置x轴日期显示
    QSharedPointer<QCPAxisTickerDateTime> dateTick(new QCPAxisTickerDateTime);
    dateTick->setDateTimeFormat("yyyy/MM/dd");
    //dateTick->setTickCount(12);
    dateTick->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    plot->xAxis->setTicker(dateTick);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);

    plot->addGraph();
    plot->graph(0)->setLineStyle(QCPGraph::lsLine);//连接线为直线
    plot->graph(0)->setPen(pen);
    plot->graph(0)->setSelectable(QCP::stNone);//不可选
    //plot->graph(0)->setBrush(QBrush(QColor(176,221,255)));//设置面积颜色
    plot->addGraph(); // 数据节点

    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(36, 160, 230, 200));
    pen.setWidthF(4);
    plot->graph(1)->setPen(pen);
    plot->graph(1)->setLineStyle(QCPGraph::lsNone);
    plot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    plot->graph(1)->setSelectable(QCP::stSingleData);

    plot->setBackground(qBrush);
    plot->xAxis->setLabel(XText);
    plot->yAxis->setLabel(YText);

    //标题
    plot->plotLayout()->insertRow(0);
    QCPTextElement* pTextElement = new QCPTextElement(plot, sTitle,QFont("微软雅黑", 14));
    pTextElement->setTextColor(QColor(0,0,255));
    plot->plotLayout()->addElement(0, 0,pTextElement);

}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QSystemDataWidget::showAreaData()
{
    QStringList l_datalist = _pDbProcess->getHistoryAreaData(_nWaveType);
    if(l_datalist.size() == 0)
    {
        return;
    }



    showPlotData(_pAreaPlot,l_datalist,&_stAreaData);
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QSystemDataWidget::showCoeffData()
{
    QStringList l_datalist = _pDbProcess->getHistoryCoeffData(_nWaveType);
    if(l_datalist.size() == 0)
    {
        return;
    }

    showPlotData(_pCoeffPlot,l_datalist,&_stCoeffData);
}
/******************************************
* 功能:历史脉宽数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QSystemDataWidget::showPlusWidthData()
{
    QStringList l_datalist = _pDbProcess->getHistoryPlusWidthData(_nWaveType);
    if(l_datalist.size() == 0)
    {
        return;
    }

    showPlotData(_pPlusWidthPlot,l_datalist,&_stpluswidthData);
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void QSystemDataWidget::showPlotData(QCustomPlot* plot,QStringList l_datalist,st_plotdata* plotdata)
 {
     plotdata->timedata.clear();
     plotdata->valuedata.clear();

     QStringList l_splitdata = l_datalist[0].split('|');
     plotdata->lfTimeMin = QDateTime::fromString(l_splitdata[0],"yyyyMMddhhmmss").toTime_t();
     plotdata->lfTimeMax = plotdata->lfTimeMin;

     plotdata->lfValueMax = l_splitdata[1].toDouble();
     plotdata->lfValueMin = plotdata->lfValueMax;

     for(QString itemdata:l_datalist)
     {
         QStringList l_splitdata = itemdata.split('|');
         if(l_splitdata.size() != 2)
         {
             continue;
         }

         double temptime =static_cast<double>(QDateTime::fromString(l_splitdata[0],"yyyyMMddhhmmss").toTime_t());
         double datavalue = l_splitdata[1].toDouble();

         plotdata->timedata.push_back(temptime);
         plotdata->valuedata.push_back(datavalue);

         plotdata->lfTimeMin = plotdata->lfTimeMin > temptime?temptime:plotdata->lfTimeMin;
         plotdata->lfTimeMax = plotdata->lfTimeMax > temptime?plotdata->lfTimeMax:temptime;

         plotdata->lfValueMin = plotdata->lfValueMin > datavalue?datavalue:plotdata->lfValueMin;
         plotdata->lfValueMax = plotdata->lfValueMax > datavalue?plotdata->lfValueMax:datavalue;
     }
    // plot->graph(0)->data().data()->clear();
     plot->graph(0)->setData(plotdata->timedata,plotdata->valuedata);
     plot->graph(0)->rescaleValueAxis();

     plot->xAxis->setRange(plotdata->lfTimeMin*0.999,plotdata->lfTimeMax*1.01);
     plot->yAxis->setRange(plotdata->lfValueMin*0.995,plotdata->lfValueMax*1.005);

     //plot->graph(1)->data().data()->clear();
     plot->graph(1)->setData(plotdata->timedata,plotdata->valuedata);

     plot->replot();

 }

 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 void QSystemDataWidget::onWaveTypeSelect(int nWaveType)
 {
     _nWaveType = nWaveType;

     showAreaData();
     showCoeffData();
     showPlusWidthData();

 }

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QSystemDataWidget::onSelectData()
{
    QCustomPlot* pPlot = (QCustomPlot*)sender();

    st_plotdata l_stdata;

    if(pPlot->objectName() == "area")
    {
        l_stdata = _stAreaData;
    }

    if(pPlot->objectName() == "coeff")
    {
        l_stdata = _stCoeffData;
    }

    if(pPlot->objectName() == "pluswidth")
    {
        l_stdata = _stpluswidthData;
    }

    QString str;

    int nSelectNum = pPlot->graph(1)->selection().dataRanges().count();
    if(nSelectNum != 1)
    {
        _pInfoLabel->hide();
        return;
    }

    QCPDataRange selectPoint = pPlot->graph(1)->selection().dataRanges()[0];

    int nIndex = selectPoint.begin();

    QString l_time = QDateTime::fromTime_t(l_stdata.timedata[nIndex]).toString("yyyy-MM-dd hh:mm:ss");

    str = QString("时间:%1\n数据:%2").arg(l_time).arg(l_stdata.valuedata[nIndex]);


    _pInfoLabel->setText(str);
    _pInfoLabel->setWindowFlags(Qt::ToolTip);
    _pInfoLabel->move(cursor().pos().x()+10,cursor().pos().y()-30);
    _pInfoLabel->show();

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QSystemDataWidget::onRefreshData()
{
    showAreaData();

    showCoeffData();

    showPlusWidthData();
}
