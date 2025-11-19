#include "reslutprocesswidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QMeasureRelProcessWidget::QMeasureRelProcessWidget(CDbDataProcess* pDb,QWidget *parent) : QWidget(parent)
{
    _pDbProcess = pDb;
    m_nMeasureType = 0;

    m_TargetType.insert("材料与裸面",0.5);
    m_TargetType.insert("AR膜",0.35);
    m_TargetType.insert("HR膜",0);
    m_TargetType.insert("偏振片",0.18);


    InitUI();

    connect(&m_defectDataTableWidget,&QTableWidget::doubleClicked,this,&QMeasureRelProcessWidget::ondoubleClickDefectData);

    connect(m_pProcessBtn,&QPushButton::clicked,this,&QMeasureRelProcessWidget::onExpDataProcess);
    //connect(m_pSaveParamBtn,&QPushButton::clicked,this,&QMeasureRelProcessWidget::onSaveParam);
    connect(m_pExportDataBtn,&QPushButton::clicked,this,&QMeasureRelProcessWidget::onExportData);


}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::InitUI()
{

    _pExpParamLabel = new QLabel(this);
    _pExpParamLabel->setFixedWidth(600);

    _pExpParamLabel->setText("实验编号:    光斑面积:0 cm²    分光比: 0.0 脉宽:0 ns\n");
    _pExpParamLabel->setStyleSheet("color:blue;font-size:16px;");


    QTabWidget* l_pTabwidget = new QTabWidget();


    l_pTabwidget->addTab(createMeasureRelWidget(),"损伤阈值计算");
    l_pTabwidget->addTab(createDefectDataWidget(),"实验数据");

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(_pExpParamLabel,0,4,1,6);// 详细信息

    playout->addWidget(l_pTabwidget,1,4,9,8);// 详细信息



}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QWidget* QMeasureRelProcessWidget:: createParamWidget()
{
    QWidget* pParamWidget = new QWidget(this);

    m_pEnergyStepEdit = new QTextEdit("3");
    m_pTargetTypeCombox = new QComboBox();
    m_pTargetPulsewidthEdit = new QTextEdit("0");
    m_pTargetAngleEdit = new QTextEdit("0");

    m_pTestAngleEdit = new QTextEdit("0");


    QList<QString> skeys = m_TargetType.keys();

    for(QString str:skeys)
    {
        m_pTargetTypeCombox->addItem(str);
    }



    m_pResultLabel = new QLabel("损伤阈值:0.0 J/cm² \n\n折算后损伤阈值:0.0 J/cm²\n");
    m_pResultLabel->setStyleSheet("color:blue;font-size:16px;");


    m_pEnergyStepEdit->setMaximumHeight(30);
    m_pTargetPulsewidthEdit->setMaximumHeight(30);
    m_pTargetAngleEdit->setMaximumHeight(30);
    m_pTargetTypeCombox->setMinimumHeight(25);
    m_pTestAngleEdit->setMaximumHeight(30);

    QGridLayout* playout  = new QGridLayout(pParamWidget);

    playout->addWidget(new QLabel("能量台阶(J/cm²)"),0,0,1,1);
    playout->addWidget(m_pEnergyStepEdit,0,1,1,1);

    playout->addWidget(new QLabel("测试样品类型"),0,2,1,1);
    playout->addWidget(m_pTargetTypeCombox,0,3,1,1);

    playout->addWidget(new QLabel("图纸要求脉冲宽度(ns)"),1,0,1,1);
    playout->addWidget(m_pTargetPulsewidthEdit,1,1,1,1);

    playout->addWidget(new QLabel("图纸要求测试角度(°)"),1,2,1,1);
    playout->addWidget(m_pTargetAngleEdit,1,3,1,1);
    playout->addWidget(new QLabel("实际测试角度(°)"),2,0,1,1);
    playout->addWidget(m_pTestAngleEdit,2,1,1,1);


    playout->addWidget(m_pResultLabel,3,0,1,2);


    return pParamWidget;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QWidget* QMeasureRelProcessWidget:: createMeasureRelWidget()
{

    QWidget* pMeasureRelWidget = new QWidget(this);


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
    m_customPlot.setGraphicsEffect(effect);


    //设置网格线样式
    QPen xGridPen = m_customPlot.xAxis->grid()->pen();
    xGridPen.setStyle(Qt::SolidLine);
    m_customPlot.xAxis->grid()->setPen(xGridPen);
    m_customPlot.yAxis->grid()->setPen(xGridPen);

    m_customPlot.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);

    m_customPlot.addGraph();
    m_customPlot.addGraph(); // 数据节点
    m_customPlot.graph(1)->setLineStyle(QCPGraph::lsLine);//连接线为直线
    m_customPlot.graph(1)->setPen(pen);
    m_customPlot.graph(1)->setSelectable(QCP::stNone);//不可选

    m_groupTracer = new QCPItemTracer(&m_customPlot);
    m_groupTracer->setInterpolating(true);
    m_groupTracer->setStyle(QCPItemTracer::tsCircle);
    m_groupTracer->setPen(QPen(Qt::red));
    m_groupTracer->setBrush(Qt::red);
    m_groupTracer->setSize(10);

    m_groupTracerText = new QCPItemText(&m_customPlot);
    m_groupTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    m_groupTracerText->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    m_groupTracerText->position->setCoords(0.5, 0.1); // lower right corner of axis rect


    m_groupTracerArrow = new QCPItemCurve(&m_customPlot);
    m_groupTracerArrow->start->setParentAnchor(m_groupTracerText->left);
    m_groupTracerArrow->startDir->setParentAnchor(m_groupTracerArrow->start);
    m_groupTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
    m_groupTracerArrow->endDir->setParentAnchor(m_groupTracerArrow->end);
    m_groupTracerArrow->endDir->setCoords(0, -40);
    m_groupTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
    m_groupTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (m_groupTracerText->bottom->pixelPosition().y()-m_groupTracerText->top->pixelPosition().y())*0.85));

    m_groupTracer->setVisible(false);
    m_groupTracerText->setVisible(false);
    m_groupTracerArrow->setVisible(false);

    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(0, 0, 255, 200));
    pen.setWidthF(2);
    m_customPlot.graph(0)->setPen(pen);
    m_customPlot.graph(0)->setLineStyle(QCPGraph::lsNone);
    m_customPlot.graph(0)->setScatterStyle(QCPScatterStyle::ssCross);
    m_customPlot.graph(0)->setSelectable(QCP::stSingleData);

    m_customPlot.setBackground(qBrush);


    //标题
    m_customPlot.plotLayout()->insertRow(0);
    QCPTextElement* pTextElement = new QCPTextElement(&m_customPlot, "损伤阈值曲线",QFont("微软雅黑", 14));
    pTextElement->setTextColor(QColor(0,0,255));
    m_customPlot.plotLayout()->addElement(0, 0,pTextElement);


    m_customPlot.xAxis->setLabel("平均通量(J/cm²)");
    m_customPlot.yAxis->setLabel("损伤概率");


    m_pProcessBtn = new QPushButton("处理测试数据");
    //m_pSaveParamBtn = new QPushButton("保存图纸参数");
    m_pExportDataBtn = new QPushButton("导出数据");

    QWidget* pParamWidget = createParamWidget();

    QGridLayout* playout  = new QGridLayout(pMeasureRelWidget);

    for(int i = 0; i < 5; i++)
    {
        playout->setRowStretch(i,1);
    }

    playout->addWidget(pParamWidget,0,0,1,5);

    playout->addWidget(m_pProcessBtn,1,1,1,1);
    //playout->addWidget(m_pSaveParamBtn,1,2,1,1);
    playout->addWidget(m_pExportDataBtn,1,3,1,1);


    playout->addWidget(&m_customPlot,2,0,3,5);

    m_pExportDataBtn->setEnabled(false);

    return pMeasureRelWidget;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QWidget* QMeasureRelProcessWidget::createDefectDataWidget()
{
    QStringList l_defect_headerInfo;
    l_defect_headerInfo<<"实验编号"<<"点号"<<"发次号"<<"能量(mJ)"<<"通量(J/cm²)"<<"是否损伤";

    m_defectDataTableWidget.verticalHeader()->setHidden(true);//垂直表头
    m_defectDataTableWidget.setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    m_defectDataTableWidget.horizontalHeader()->setStretchLastSection(true);//最后一列充满
    m_defectDataTableWidget.setColumnCount(l_defect_headerInfo.size());
    m_defectDataTableWidget.setHorizontalHeaderLabels(l_defect_headerInfo);
    m_defectDataTableWidget.setColumnWidth(0,200);

    QWidget* pwidget = new QWidget(this);


    QGridLayout* playout = new QGridLayout(pwidget);

    playout->addWidget(&m_defectDataTableWidget,0,0,10,1);

    return pwidget;

}

/******************************************
* 功能:显示损伤前后的图片
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/

void QMeasureRelProcessWidget::ondoubleClickDefectData(QModelIndex index)
{

    if(!_defectImagewidget.isHidden())
    {
        _defectImagewidget.hide();
    }

    QImage l_preImage,l_backImage;
    QString sExpNo = m_defectDataTableWidget.item(index.row(),0)->text();
    int nPointNo = m_defectDataTableWidget.item(index.row(),1)->text().toInt();
    int nShotNo = m_defectDataTableWidget.item(index.row(),2)->text().toInt();

    _pDbProcess->getDefectCheckImage(sExpNo,nPointNo,nShotNo,0x0,l_preImage);
    _pDbProcess->getDefectCheckImage(sExpNo,nPointNo,nShotNo,0x1,l_backImage);


    _defectImagewidget.onshowImage(m_nMeasureType,0x0,l_preImage);
    _defectImagewidget.onshowImage(m_nMeasureType,0x01,l_backImage);
    _defectImagewidget.setModal(true);
    _defectImagewidget.setWindowTitle("测试前后图像数据");
    _defectImagewidget.resize(1000,600);
    _defectImagewidget.show();
}
/******************************************
* 功能:保存图纸参数
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::saveBluePrintParam()
{
    m_lfTansferCoeff = m_TargetType.value(m_pTargetTypeCombox->currentText());
    m_lfTargetAngle = m_pTargetAngleEdit->toPlainText().toFloat();
    m_lfTargetPlus = m_pTargetPulsewidthEdit->toPlainText().toFloat();
    m_lfTestAngle = m_pTestAngleEdit->toPlainText().toFloat();

    for(st_expDataInfo item:m_ExpdataInfolist)
    {
        QString sSql = QString("update dbo.T_EXP_INFO set targetplus = %1,targetangle = %2,targettypename = '%3',angle = '%4' where ExpNo=%5")\
                .arg(m_lfTargetPlus).arg(m_lfTargetAngle).arg(m_pTargetTypeCombox->currentText()).arg(m_lfTestAngle).arg(item.sExpNo);

        _pDbProcess->updateItem(sSql);
    }


}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::show1On1Data(vector<st_damageInfo> damageInfolist, float lfMaxDamageFlux,double lfa,double lfb)
{
    QVector<double> xdata;
    QVector<double> ydata;

    m_customPlot.graph(1)->data().data()->clear();
    m_customPlot.graph(0)->data().data()->clear();

    if(damageInfolist.size() == 0)
    {
        return;
    }


    float lfMaxFlux = damageInfolist[0].lfFlux;
    float lfMinFlux = damageInfolist[0].lfFlux;

    for(size_t i = 0; i < damageInfolist.size(); i++)
    {
        st_damageInfo l_damageInfo = damageInfolist[i];

        lfMaxFlux = std::max(lfMaxFlux,l_damageInfo.lfFlux);
        lfMinFlux = std::min(lfMinFlux,l_damageInfo.lfFlux);

        xdata.push_back(l_damageInfo.lfFlux);
        ydata.push_back(l_damageInfo.lfDamagePro);
    }


    m_customPlot.graph(0)->setData(xdata, ydata);
    m_customPlot.xAxis->setRange(0, lfMaxFlux+0.3*lfMaxFlux);//设置范围
    m_customPlot.yAxis->setRange(-0.01, 1.2);
    m_customPlot.replot();


    QVector<double> xfit;
    QVector<double> yfit;

    xfit.append(-lfb/lfa);
    yfit.append(0.0);

    xfit.append((1.0-lfb)/lfa);
    yfit.append(1.0);


    m_customPlot.graph(1)->setData(xfit,yfit);

    // m_groupTracer->setVisible(true);
    m_groupTracerText->setVisible(true);
    m_groupTracerArrow->setVisible(true);


    m_groupTracer->setGraphKey(lfMaxDamageFlux);
    m_groupTracer->setGraph(m_customPlot.graph(0));

    m_groupTracerText->setText(QString(" 损伤阈值:")+QString::number(lfMaxDamageFlux,'g',4)+QString("J/cm²"));

    m_groupTracerArrow->end->setCoords(lfMaxDamageFlux, 0);

    m_customPlot.replot();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::showROn1Data(vector<st_damageInfo> damageInfolist, float lfMaxDamageFlux)
{
    bool bexist = false;//是否有损伤阈值
    m_customPlot.graph(1)->data().data()->clear();
    m_customPlot.graph(0)->data().data()->clear();

    QVector<double> xdata;
    QVector<double> ydata;
    if(damageInfolist.size() == 0)
    {
        return;
    }

    float lfMaxFlux = damageInfolist[0].lfFlux;
    float lfMinFlux = damageInfolist[0].lfFlux;

    for(size_t i = 0; i < damageInfolist.size(); i++)
    {
        st_damageInfo l_damageInfo = damageInfolist[i];

        lfMaxFlux = std::max(lfMaxFlux,l_damageInfo.lfFlux);
        lfMinFlux = std::min(lfMinFlux,l_damageInfo.lfFlux);

        xdata.push_back(l_damageInfo.lfFlux);
        ydata.push_back(l_damageInfo.lfDamagePro);

        if(lfMaxDamageFlux == l_damageInfo.lfFlux)
        {
            bexist = true;
        }
    }

    m_customPlot.graph(0)->setData(xdata, ydata);
    m_customPlot.xAxis->setRange(0, lfMaxFlux+0.3*lfMaxFlux);//设置范围
    m_customPlot.yAxis->setRange(-0.01, 1.2);


    if(bexist)
    {
        m_groupTracer->setGraphKey(lfMaxDamageFlux);
        m_groupTracer->setGraph(m_customPlot.graph(0));
        m_groupTracer->setVisible(true);
    }
    else
    {
        m_groupTracer->setVisible(false);
    }



    m_groupTracerText->setVisible(true);
    m_groupTracerArrow->setVisible(true);

    m_groupTracerText->setText(QString(" 损伤阈值:")+QString::number(lfMaxDamageFlux,'g',4)+QString("J/cm²"));

    m_groupTracerArrow->end->setCoords(lfMaxDamageFlux, 0);

    m_customPlot.replot();
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::showRelData(float lfMaxDamageFlux,float lfTansferMaxDamageFlux1)
{
    QString sRel = "";

    sRel = QString("损伤阈值:%1 J/cm² \n\n折算后损伤阈值:%2 J/cm²\n").arg(lfMaxDamageFlux).arg(lfTansferMaxDamageFlux1);

    m_pResultLabel->setText(sRel);
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::showExpData(vector<st_expDataInfo> expdataInfolist)
{
    m_defectDataTableWidget.clearContents();
    m_defectDataTableWidget.setRowCount(0);

    QString sExpParam = "";
    for(st_expDataInfo l_ExpdataInfo:expdataInfolist)
    {
        if(expdataInfolist.size() > 1 && !sExpParam.isEmpty())
        {
           sExpParam +="\n\n";
        }
        sExpParam = sExpParam + QString("实验编号:%1  光斑面积:%2 cm² 调制度:%3 分光比:%4 脉宽:%5 ns").arg(l_ExpdataInfo.sExpNo).arg(l_ExpdataInfo.lfArea)\
                .arg(l_ExpdataInfo.lfM).arg(l_ExpdataInfo.lfSampleCoeff).arg(l_ExpdataInfo.lfPlusWidth);

        //图纸参数显示
        m_pTargetAngleEdit->setText(QString::number(l_ExpdataInfo.lfTargetAngle,'g'));
        m_pTargetPulsewidthEdit->setText(QString::number(l_ExpdataInfo.lfTargetPlus,'g'));
        m_pTargetTypeCombox->setCurrentText(l_ExpdataInfo.sTargettypename);
        m_pTestAngleEdit->setText(QString::number(l_ExpdataInfo.lfAngle,'g'));

        for(st_pointdataInfo pointItem:l_ExpdataInfo.pointdatalist)
        {
            for(st_shotdataInfo shotItem:pointItem.shotdatalist)
            {
                int nRowIndex = m_defectDataTableWidget.rowCount();
                m_defectDataTableWidget.insertRow(nRowIndex);


                m_defectDataTableWidget.setItem(nRowIndex,0,new QTableWidgetItem(l_ExpdataInfo.sExpNo));
                m_defectDataTableWidget.setItem(nRowIndex,1,new QTableWidgetItem(QString("%1").arg(pointItem.nPointNo)));
                m_defectDataTableWidget.setItem(nRowIndex,2,new QTableWidgetItem(QString("%1").arg(shotItem.nShotNo)));
                m_defectDataTableWidget.setItem(nRowIndex,3,new QTableWidgetItem(QString("%1").arg(shotItem.lfEnergydata)));
                m_defectDataTableWidget.setItem(nRowIndex,4,new QTableWidgetItem(QString("%1").arg(shotItem.lfFluxdata)));

                QString str = shotItem.bDefect?"是":"否";
                m_defectDataTableWidget.setItem(nRowIndex,5,new QTableWidgetItem(str));

            }

        }

    }

    //显示实验数据
    _pExpParamLabel->setText(sExpParam);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QMeasureRelProcessWidget::getExplistData(QStringList sExpNolist)
{
    m_pExportDataBtn->setEnabled(false);

    m_ExpdataInfolist.clear();

    for(QString sExpNo:sExpNolist)
    {
        st_expDataInfo l_ExpdataInfo;

        if(!getExpData(sExpNo,l_ExpdataInfo))
        {
            QMessageBox::warning(this,"提示",QString("获取实验%1数据失败").arg(sExpNo));
            return false;
        }
        m_ExpdataInfolist.push_back(l_ExpdataInfo);
    }

    return true;

}

/******************************************
* 功能:获取原始数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QMeasureRelProcessWidget::getExpData(QString sExpNo,st_expDataInfo& expdataInfo)
{
    m_pExportDataBtn->setEnabled(false);

    //查询实验基本数据
     QStringList expbaseInfolist;
    if(!_pDbProcess->queryTableData(QString("select expno,samplecoeff,area,angle,pluswidth,measuretype,targetplus,targetangle,targettypename,M,ExpTemp,ExpHum,Expdirty,TaskNo,SampleName,WavelengthType from dbo.T_EXP_INFO where ExpNo='%1'").arg(sExpNo),&expbaseInfolist))
    {
        return false;
    }

    if(expbaseInfolist.size() != 1)
    {
        return false;
    }


    QStringList strArray = expbaseInfolist[0].split('|');

    expdataInfo.sExpNo =  strArray[0];
    expdataInfo.lfSampleCoeff = strArray[1].toFloat();
    expdataInfo.lfArea = strArray[2].toFloat();
    expdataInfo.lfAngle = strArray[3].toFloat();
    expdataInfo.lfPlusWidth = strArray[4].toFloat();
    expdataInfo.nMeasureType = strArray[5].toInt();

    expdataInfo.lfTargetPlus = strArray[6].toFloat();
    expdataInfo.lfTargetAngle = strArray[7].toFloat();
    expdataInfo.sTargettypename = strArray[8];

    expdataInfo.lfM = strArray[9].toFloat();
    expdataInfo.lfExpTemp = strArray[10].toFloat();
    expdataInfo.lfExphum = strArray[11].toFloat();
    expdataInfo.lfExpdirty = strArray[12].toFloat();

    expdataInfo.sTaskNo = strArray[13];
    expdataInfo.sSampleName = strArray[14];
    expdataInfo.nWavelengthType = strArray[15].toInt();



    m_nMeasureType = expdataInfo.nMeasureType;
    //查询能量数据
     QStringList energyDatalist;

     bool bRel = _pDbProcess->queryTableData(QString("select* from dbo.T_ERNERGY_DATA where ExpNo='%1' and  ChannelNo = '%2' order by PointNo, shotNo").arg(sExpNo).arg(0x01),&energyDatalist);
     if(bRel == false)
     {
         return false;
     }

     for (QString itemData : energyDatalist)
     {
         QStringList strArray = itemData.split('|');
         if (strArray.size() < 9)
         {
             return false;
         }


         int nPointNo = strArray[4].toInt();//点号
         st_shotdataInfo l_shotdata;
         l_shotdata.nShotNo = strArray[5].toInt();//发次
         //还原为主能量数据
         l_shotdata.lfEnergydata =  strArray[7].toFloat()*expdataInfo.lfSampleCoeff;
         l_shotdata.lfEnergySection = strArray[6].toFloat();//能量段信息 暂时没有使用
         //计算通量数据
         l_shotdata.lfFluxdata = l_shotdata.lfEnergydata*0.001*cos(expdataInfo.lfAngle/180.0*3.14159)/expdataInfo.lfArea;

         //查询是否损伤
         if(!_pDbProcess->getShotdefectInfo(sExpNo,nPointNo,l_shotdata.nShotNo,l_shotdata.bDefect))
         {
             continue;
         //    return false;
         }


         st_pointdataInfo* pPointInfo = nullptr;
         bool bexist = false;


         for(size_t i = 0; i < expdataInfo.pointdatalist.size(); i++)
         {
             if(expdataInfo.pointdatalist[i].nPointNo == nPointNo)
             {
                 pPointInfo = &expdataInfo.pointdatalist[i];
                 bexist = true;
                 break;
             }
         }

         if(pPointInfo == nullptr)
         {
             pPointInfo = new st_pointdataInfo();
         }

         pPointInfo->nPointNo = nPointNo;

         //统计最大通量
         pPointInfo->lfMaxFlux = max(pPointInfo->lfMaxFlux,l_shotdata.lfFluxdata);//?pPointInfo->lfMaxFlux:l_shotdata.lfFluxdata;
         pPointInfo->lfMinFlux = min(pPointInfo->lfMinFlux,l_shotdata.lfFluxdata);
         pPointInfo->shotdatalist.push_back(l_shotdata);

         if(bexist == false)
         {
             expdataInfo.pointdatalist.push_back(*pPointInfo);//加入列表中
         }

     }


    return true;
}







/******************************************
* 功能:对数据进行处理 计算损伤阈值
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::onExpDataProcess()
{
    if(m_pTargetTypeCombox->currentText() == "")
    {
        QMessageBox::warning(this,"提示","请选择测试元件类型");
        return;
    }


    //保存图纸参数
    saveBluePrintParam();


    m_lfFluxStep = m_pEnergyStepEdit->toPlainText().toFloat();
    m_lfTansferCoeff = m_TargetType.value(m_pTargetTypeCombox->currentText());
    m_lfTargetAngle = m_pTargetAngleEdit->toPlainText().toFloat();
    m_lfTargetPlus = m_pTargetPulsewidthEdit->toPlainText().toFloat();
    if(m_lfFluxStep <= 0.0)
    {
        QMessageBox::warning(this,"提示","能量台阶参数设置错误，请重新设置");
        return;
    }

    if(m_nMeasureType == M_MEASURETYPE_1On1)//1on1
    {
        cacl1On1Data(m_ExpdataInfolist);
        //画图显示
        show1On1Data(m_damageInfolist,m_lfMaxDamageFlux,m_lfa,m_lfb);
    }

    if(m_nMeasureType == M_MEASURETYPE_ROn1)//Ron1
    {
        caclROn1Data(m_ExpdataInfolist);
        //画图显示
        showROn1Data(m_damageInfolist,m_lfMaxDamageFlux);

    }

    caclTransferdata();//计算折算后的阈值

    //显示阈值数据
    showRelData(m_lfMaxDamageFlux,m_lfTansferMaxDamageFlux);

    m_pExportDataBtn->setEnabled(true);

    //保存处理结果
    saveTaskRel();


}

/******************************************
* 功能:计算1On1数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QMeasureRelProcessWidget::cacl1On1Data(vector<st_expDataInfo> expdataInfolist)
{
    m_damageInfolist.clear();
    m_lfMaxDamageFlux = 0.0;

    //转换为点--发次数据结构
    float l_lfMinFlux = 65535.0;//所有发次中最小通量
    float l_lfMaxFlux = 0.0;//所有发次中最大通量

    QVector<double> l_x_Flux;
    QVector<double> l_y_pro;
    vector<st_pointdataInfo> l_pointdataInfo;

    for(st_expDataInfo expinfo:expdataInfolist)
    {
        for(st_pointdataInfo pointinfo:expinfo.pointdatalist)
        {
            l_pointdataInfo.push_back(pointinfo);
            l_lfMaxFlux = max(l_lfMaxFlux,pointinfo.lfMaxFlux);
            l_lfMinFlux = min(l_lfMinFlux,pointinfo.lfMinFlux);
        }
    }

    int nNum =  ceil((l_lfMaxFlux-l_lfMinFlux)/m_lfFluxStep);

    for(int i = 0; i <= nNum; i++ )
    {

        float lfSumFlux = 0.0;
        int nDefectNum = 0;
        int nPointNum = 0;//实际试验时进行的发次数 计算平均通量时使用
        float lfStart =l_lfMinFlux + i*m_lfFluxStep;

        for(st_pointdataInfo pointitem:l_pointdataInfo)
        {
            for(st_shotdataInfo item:pointitem.shotdatalist)
            {
                if(item.lfFluxdata >= lfStart &&item.lfFluxdata < lfStart + m_lfFluxStep )
                {
                    lfSumFlux += item.lfFluxdata;
                    nPointNum++;
                    if(item.bDefect == true)
                    {
                        nDefectNum++;
                    }
                }
            }

        }

        if(nPointNum != 0)
        {
            //存储当前信息
            st_damageInfo l_damageInfo;
            l_damageInfo.lfDamagePro = (float)nDefectNum/(float)nPointNum;//计算损伤概率
            l_damageInfo.lfFlux = lfSumFlux/nPointNum;//计算平均通量
            l_damageInfo.lfStartFlux = lfStart;
            l_damageInfo.lfEndFlux = lfStart + m_lfFluxStep;
            m_damageInfolist.push_back(l_damageInfo);

            l_x_Flux.push_back(l_damageInfo.lfFlux);
            l_y_pro.push_back(l_damageInfo.lfDamagePro);
        }

    }

    //曲线拟合
    linearFit(m_lfa,m_lfb,l_x_Flux,l_y_pro);
    if(m_lfa != 0.0)
    {
        m_lfMaxDamageFlux = -m_lfb/m_lfa;
    }



    return true;
}


/*****************************************************************
* 功能:计算ROn1数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
*
* 备注：
* 损伤点个数 统计时需要注意 会出现该能量段对某个点已经超过了其损伤的值，
* 因此该能量段对应的实际发次其实是没有打的 但是在统计时,需要将该发次统计进去
*****************************************************************/
bool QMeasureRelProcessWidget::caclROn1Data(vector<st_expDataInfo> expdataInfolist)
{
    m_damageInfolist.clear();
    m_lfMaxDamageFlux = 0.0;

    //转换为点--发次数据结构
    float l_lfMinFlux = 65535.0;//所有发次中最小通量
    float l_lfMaxFlux = 0.0;//所有发次中最大通量
    vector<st_pointdataInfo> l_pointdataInfo;

    for(st_expDataInfo expinfo:expdataInfolist)
    {
        for(st_pointdataInfo pointinfo:expinfo.pointdatalist)
        {
            l_pointdataInfo.push_back(pointinfo);
            l_lfMaxFlux = max(l_lfMaxFlux,pointinfo.lfMaxFlux);
            l_lfMinFlux = min(l_lfMinFlux,pointinfo.lfMinFlux);
        }
    }


    int nPointNum = l_pointdataInfo.size();//总点数

    int nNum = (l_lfMaxFlux-l_lfMinFlux)/m_lfFluxStep;

    for(int i = 0; i <= nNum; i++ )
    {

        float lfSumFlux = 0.0;
        int nDefectNum = 0;
        int nRealNum = 0;//实际试验时进行的发次数 计算平均通量时使用
        float lfStart = l_lfMinFlux + i*m_lfFluxStep;

        for(st_pointdataInfo pointitem:l_pointdataInfo)
        {
            //该能量段超出了该发次试验的能量段 则按照最后一发的状态进行统计
            if(pointitem.lfMaxFlux <= lfStart && pointitem.lfMaxFlux > 0.0)
            {
                if(pointitem.bMaxfluxdefect == true)
                {
                   nDefectNum++;
                }
                continue;
            }
            //没有超出该发次的最大能量段
            for(st_shotdataInfo item:pointitem.shotdatalist)
            {
                if(item.lfFluxdata >= lfStart &&item.lfFluxdata < lfStart + m_lfFluxStep )
                {
                    lfSumFlux += item.lfFluxdata;
                    nRealNum++;
                    if(item.bDefect == true)
                    {
                        nDefectNum++;
                    }
                }
            }

        }

        if(nRealNum != 0)
        {
            //存储当前信息
            st_damageInfo l_damageInfo;
            l_damageInfo.lfDamagePro = (float)nDefectNum/(float)nPointNum;//计算损伤概率

            l_damageInfo.lfFlux = lfSumFlux/nRealNum;//计算平均通量
            l_damageInfo.lfStartFlux = lfStart;
            l_damageInfo.lfEndFlux = lfStart + m_lfFluxStep;
            m_damageInfolist.push_back(l_damageInfo);

            if(l_damageInfo.lfDamagePro < 1e-6)//未出现损伤
            {
                m_lfMaxDamageFlux = max(m_lfMaxDamageFlux ,l_damageInfo.lfFlux);
            }
        }
    }


    return true;
}

/******************************************
* 功能:计算折算后的参数
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::caclTransferdata()
{
    m_lfTansferMaxDamageFlux = 0.0;

    if(m_ExpdataInfolist.size() == 0 || m_lfTargetPlus == 0)
    {
        return;
    }

    float transfer_pulsewidth = m_ExpdataInfolist[0].lfPlusWidth/m_lfTargetPlus;

    transfer_pulsewidth = pow(transfer_pulsewidth,m_lfTansferCoeff);

    float transfer_angle = cos(m_lfTargetAngle/180.0*3.14159)/cos(m_lfTestAngle/180.0*3.14159);

    m_lfTansferMaxDamageFlux = m_lfMaxDamageFlux/transfer_pulsewidth*transfer_angle*m_ExpdataInfolist[0].lfM;

}
/***************************************************************
*功能:线性拟合
*输入:
*输出:
*返回值:
*修改记录:
* 保留最后一个0 和第一个1
******************************************************************/
void QMeasureRelProcessWidget::linearFit(double &a, double &b, QVector<double> xdata, QVector<double> ydata)
{
    a = 0.0;
    b = 0.0;

    if(xdata.size() != ydata.size() || ydata.size() == 0)
    {
        return;
    }

    QHash<int,float> l_zero_fuxdata;
    l_zero_fuxdata.insert(0,0.0);
    l_zero_fuxdata.insert(1,65535.0);

    //查找出最大的0  和最小的1
    for(int i = 0; i < ydata.size(); i++)
    {
//        qDebug()<<"原始数据"<<ydata[i]<<xdata[i];
        if(ydata[i] < 1e-6 && l_zero_fuxdata[0] < xdata[i])//概率为0
        {
            l_zero_fuxdata[0] = xdata[i];//概率为0时 最大的y值
        }
        else if(ydata[i] > 0.9999 && l_zero_fuxdata[1] > xdata[i])//概率为1
        {
           l_zero_fuxdata[1] = xdata[i];//概率为1时 最小的y值
        }
    }


    int nsize = ydata.size()-1;

    while (nsize>=0)
    {
        if((ydata[nsize]> 0.9999 && xdata[nsize] > l_zero_fuxdata[1])||\
           (ydata[nsize] < 1e-6 && xdata[nsize] < l_zero_fuxdata[0]))
        {
            ydata.remove(nsize);
            xdata.remove(nsize);
        }

        nsize--;
    }



    float xdataSquareSum = 0.0;//x平方和
    float xdatasum = 0.0;//x的和
    float ydatasum = 0.0;//y的和
    float lfxMultysum = 0.0;//x*y
    for(int i = 0; i < ydata.size(); i++)
    {
//        qDebug()<<"剔除后数据"<<ydata[i]<<xdata[i];

        xdatasum = xdatasum + xdata[i];
        ydatasum = ydatasum + ydata[i];

        lfxMultysum = lfxMultysum + xdata[i]*ydata[i];
        xdataSquareSum = xdataSquareSum + xdata[i]*xdata[i];
    }

    a = (lfxMultysum*xdata.size() - xdatasum*ydatasum)/(xdata.size()*xdataSquareSum-xdatasum*xdatasum);

    b = (xdataSquareSum*ydatasum-xdatasum*lfxMultysum)/(xdata.size()*xdataSquareSum-xdatasum*xdatasum);

}

/******************************************
* 功能:导出数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::onExportData()
{

    QFuture<void> l_fu = QtConcurrent::run(this,&QMeasureRelProcessWidget::exportdata);

    while(1)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
        if(l_fu.isFinished())
        {
            break;
        }
    }


    QMessageBox::information(this,"提示","数据导出完成");
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/

 void QMeasureRelProcessWidget::exportdata()
 {


     //保存文件
     QDir l_dir( QApplication::applicationDirPath()+"/data");
     if (!l_dir.exists())
     {
         l_dir.mkdir(QApplication::applicationDirPath()+"/data");
     }


     //csv文件 和图像文件
     for(st_expDataInfo expdata: m_ExpdataInfolist)
     {
         //图像
         QImage l_preImage,l_backImage,l_label_backImage;

         QString sPath = QApplication::applicationDirPath()+QString("/data/%1").arg(expdata.sExpNo);
         QDir l_dir0(sPath);
         if (!l_dir0.exists())
         {
             l_dir0.mkdir(sPath);
         }

         l_dir0.mkdir(sPath+"/image/");

         QFile csvfile(sPath+QString("/%1.csv").arg(expdata.sExpNo));
         if(csvfile.exists())
         {
             csvfile.remove();
         }

         if(!csvfile.open(QIODevice::ReadWrite|QIODevice::Text))
         {
             return;
         }

         QString str = QString("\r实验编号:,%1\r光斑面积(cm²):,%2\r脉宽(ns):,%3\r测试角度(°):,%4\r实验温度:,%5\r实验湿度:,%6\r洁净度:,%5\r\r\r")\
                 .arg(expdata.sExpNo).arg(expdata.lfArea).arg(expdata.lfPlusWidth).arg(expdata.lfAngle)\
                 .arg(expdata.lfExpTemp).arg(expdata.lfExphum).arg(expdata.lfExpdirty);


         QString sHeader = "点号,发次,能量数据(mJ),通量数据(J/cm²),是否损伤\r";


         str = str + sHeader;


         for(st_pointdataInfo pointItem:expdata.pointdatalist)
         {
             for(st_shotdataInfo shotItem:pointItem.shotdatalist)
             {

                 str = str + QString("%1,%2,%3,%4,%5\r")\
                         .arg(pointItem.nPointNo).arg(shotItem.nShotNo)\
                         .arg(shotItem.lfEnergydata).arg(shotItem.lfFluxdata)\
                         .arg(shotItem.bDefect?"是":"否");


                 _pDbProcess->getDefectCheckImage(expdata.sExpNo,pointItem.nPointNo,shotItem.nShotNo,0x0,l_preImage);
                 _pDbProcess->getDefectCheckImage(expdata.sExpNo,pointItem.nPointNo,shotItem.nShotNo,0x1,l_backImage,true);

                 //原始图像
                 if(!l_preImage.isNull())
                 {
                     l_preImage.save(sPath+QString("/image/%1_%2_before.jpg").arg(pointItem.nPointNo).arg(shotItem.nShotNo));
                 }

                 if(!l_backImage.isNull())
                 {
                     l_backImage.save(sPath+QString("/image/%1_%2_after.jpg").arg(pointItem.nPointNo).arg(shotItem.nShotNo));
                 }


                 //标记图像
                 if(shotItem.bDefect)
                 {
                     _pDbProcess->getDefectCheckImage(expdata.sExpNo,pointItem.nPointNo,shotItem.nShotNo,0x1,l_label_backImage,false);//标记后的图像

                     if(!l_label_backImage.isNull())
                     {
                         l_label_backImage.save(sPath+QString("/image/%1_%2_after_defect.jpg").arg(pointItem.nPointNo).arg(shotItem.nShotNo));
                     }
                 }





             }
         }

         csvfile.write(str.toLocal8Bit());
         csvfile.close();

         m_customPlot.saveJpg(sPath+"/损伤阈值.png");
     }

 }

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::clearAlldata()
{

    m_customPlot.graph(1)->data().data()->clear();
    m_customPlot.graph(0)->data().data()->clear();
    m_defectDataTableWidget.clearContents();
    m_defectDataTableWidget.setRowCount(0);

    m_groupTracer->setVisible(false);
    m_groupTracerText->setVisible(false);
    m_groupTracerArrow->setVisible(false);


}

/******************************************
* 功能:判断当前合并处理的数据是否合适 taskid相同 samplename相同
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QMeasureRelProcessWidget::checkExpInfolist()
{
    if(m_ExpdataInfolist.size() <= 1)
    {
        return true;
    }

    st_expDataInfo l_temp = m_ExpdataInfolist[0];

     vector<st_expDataInfo> m_ExpdataInfolist;

     for(st_expDataInfo l_item:m_ExpdataInfolist)
     {
         if(l_item.sTaskNo != l_temp.sTaskNo || l_item.sSampleName != l_temp.sSampleName)
         {
             return false;
         }
     }
     return true;
}

/******************************************
* 功能:保存处理结果至数据库中 供后续生成报表使用
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureRelProcessWidget::saveTaskRel()
{
    if(m_ExpdataInfolist.size() == 0)
    {
        return;
    }

    st_expDataInfo l_temp = m_ExpdataInfolist[0];

    _pDbProcess->saveTaskRelData(l_temp.sTaskNo,l_temp.sSampleName,l_temp.nWavelengthType,l_temp.lfArea,\
                                 m_lfMaxDamageFlux,m_lfTansferMaxDamageFlux,l_temp.lfExpTemp,l_temp.lfExphum,l_temp.lfPlusWidth,m_lfTargetPlus);

}
