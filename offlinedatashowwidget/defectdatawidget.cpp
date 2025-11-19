#include "defectdatawidget.h"
#include <QFileDialog>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QDefectDataWidget::QDefectDataWidget(CDbDataProcess* pDbProcess)
{
    _pDbProcess = pDbProcess;

    _pReportWidget = new QReportWidget(pDbProcess,this);
    _pReportWidget->hide();

    InitUI();

    //设置初始日期
    QDate l_date;
    _pStartDateEdit->setDate(l_date.currentDate().addDays(-30));
    _pEndDateEdit->setDate(l_date.currentDate());

    onShowExpNolist();


    QAction* pAction = new QAction("合并处理", this);
    connect(pAction, SIGNAL(triggered()), this, SLOT(onMulitDataProcess()));
    m_mulitProcessMenu.addAction(pAction);

    connect(_pExpNolistWidget,&QTableWidget::customContextMenuRequested,this,&QDefectDataWidget::onShowMenu);


    //双击事件
    connect(_pExpNolistWidget,&QTableWidget::doubleClicked,this,&QDefectDataWidget::ondoubleClickExplist);

    //按钮状态变化
    connect(_pRefreshDataBtn,&QPushButton::clicked,this,&QDefectDataWidget::onShowExpNolist);
    connect(_pStartDateEdit,&QDateTimeEdit::dateChanged,this,&QDefectDataWidget::onDateChange);
    connect(_pEndDateEdit,&QDateTimeEdit::dateChanged,this,&QDefectDataWidget::onDateChange);

    void(QComboBox::*fp)(int)=&QComboBox::currentIndexChanged;
    connect(_pWavelenTypeCombox,fp,this,&QDefectDataWidget::onComboxSelectChange);
    connect(_pMeasureTypeCombox,fp,this,&QDefectDataWidget::onComboxSelectChange);


    connect(_pExpTaskRefreshBtn,&QPushButton::clicked,this,&QDefectDataWidget::onQueryByTaskNo);
    connect(_pReportBtn,&QPushButton::clicked,this,&QDefectDataWidget::onGerenReport);


  }

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QDefectDataWidget::~QDefectDataWidget()
{
    if(_pReportWidget!= nullptr)
    {
        _pReportWidget->close();
    }
}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget::InitUI()
{
    _pStartDateEdit = new QDateEdit(this);
    _pEndDateEdit = new QDateEdit(this);

    _pExpTaskEdit = new QTextEdit(this);

    _pWavelenTypeCombox = new QComboBox(this);
    _pMeasureTypeCombox = new QComboBox(this);
    _pExpNolistWidget = new QTableWidget(this);
    _pRefreshDataBtn = new QPushButton("查询",this);
    _pExpTaskRefreshBtn = new QPushButton("任务查询",this);

    _pReportBtn = new QPushButton("生成报告",this);



    _pMeasureRelProcessWidget = new QMeasureRelProcessWidget(_pDbProcess);





    _pStartDateEdit->setMinimumHeight(30);
    _pEndDateEdit->setMinimumHeight(30);
    _pExpTaskEdit->setMaximumHeight(25);

    _pMeasureTypeCombox->setMinimumHeight(25);
    _pWavelenTypeCombox->setMinimumHeight(25);

    QStringList l_headerInfo;
    l_headerInfo<<"选择"<<"实验编号"<<"元件编号";
    _pExpNolistWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    _pExpNolistWidget->verticalHeader()->setHidden(true);//垂直表头
    _pExpNolistWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    _pExpNolistWidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满
    _pExpNolistWidget->setColumnCount(l_headerInfo.size());
    _pExpNolistWidget->setHorizontalHeaderLabels(l_headerInfo);
    _pExpNolistWidget->setColumnWidth(0,50);
    _pExpNolistWidget->setColumnWidth(1,200);

    _pWavelenTypeCombox->addItem("1064nm");
    _pWavelenTypeCombox->addItem("355nm");
    _pWavelenTypeCombox->setCurrentIndex(0);

    _pMeasureTypeCombox->addItem("1On1");
    _pMeasureTypeCombox->addItem("ROn1");
    _pMeasureTypeCombox->setCurrentIndex(0);

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(new QLabel("日期选择"),0,0,1,1);
    playout->addWidget(_pStartDateEdit,0,1,1,1);
    playout->addWidget(new QLabel("至"),0,2,1,1);
    playout->addWidget(_pEndDateEdit,0,3,1,1);


    playout->addWidget(new QLabel("波长选择"),1,0,1,1);
    playout->addWidget(_pWavelenTypeCombox,1,1,1,1);

    playout->addWidget(new QLabel("测试类型"),1,2,1,1);
    playout->addWidget(_pMeasureTypeCombox,1,3,1,1);

    playout->addWidget(_pRefreshDataBtn,2,3,1,1);

    playout->addWidget(new QLabel("任务编号"),3,0,1,1);

    playout->addWidget(_pExpTaskEdit,3,1,1,1);

    playout->addWidget(_pExpTaskRefreshBtn,3,2,1,1);
    playout->addWidget(_pReportBtn,3,3,1,1);



    playout->addWidget(_pMeasureRelProcessWidget,0,4,12,8);// 数据处理

    playout->addWidget(_pExpNolistWidget,4,0,8,4);



}



/******************************************
* 功能:显示实验编号和元件编号等信息
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget:: onShowExpNolist()
{
    QDate l_startDate = _pStartDateEdit->date();
    QDate l_endDate = _pEndDateEdit->date();
    int l_currentMeasureType = _pMeasureTypeCombox->currentIndex()+1;
    int l_wavelengthType = _pWavelenTypeCombox->currentIndex();

    QStringList l_datalist =_pDbProcess->onGetHistoryExpInfo(l_currentMeasureType,l_wavelengthType,l_startDate,l_endDate);

    _pExpNolistWidget->clearContents();
    _pExpNolistWidget->setRowCount(0);

    for(int i = 0; i < l_datalist.size(); i++)
    {
        QStringList l_dataItem = l_datalist[i].split('|');
        int nRowIndex = _pExpNolistWidget->rowCount();
        _pExpNolistWidget->insertRow(nRowIndex);
        _pExpNolistWidget->setItem(nRowIndex,1,new QTableWidgetItem(l_dataItem[0]));
        _pExpNolistWidget->setItem(nRowIndex,2,new QTableWidgetItem(l_dataItem[1]));

        QTableWidgetItem *checkBox = new QTableWidgetItem();
        checkBox->setCheckState(Qt::Unchecked);
        _pExpNolistWidget->setItem(nRowIndex, 0, checkBox);

    }

}

/******************************************
* 功能:光路选择
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget:: onWaveTypeSelect(int nIndex)
{
    onShowExpNolist();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/

void QDefectDataWidget::onQueryByTaskNo()
{
    QString sTaskNo = _pExpTaskEdit->toPlainText();

    QString sSql = QString("select t.expno,t.samplename from T_EXP_INFO t where t.TaskNo = '%1'").arg(sTaskNo);

    QStringList l_datalist;
    _pDbProcess->queryTableData(sSql,&l_datalist);

    _pExpNolistWidget->clearContents();
    _pExpNolistWidget->setRowCount(0);

    for(int i = 0; i < l_datalist.size(); i++)
    {
        QStringList l_dataItem = l_datalist[i].split('|');
        int nRowIndex = _pExpNolistWidget->rowCount();
        _pExpNolistWidget->insertRow(nRowIndex);
        _pExpNolistWidget->setItem(nRowIndex,1,new QTableWidgetItem(l_dataItem[0]));
        _pExpNolistWidget->setItem(nRowIndex,2,new QTableWidgetItem(l_dataItem[1]));

        QTableWidgetItem *checkBox = new QTableWidgetItem();
        checkBox->setCheckState(Qt::Unchecked);
        _pExpNolistWidget->setItem(nRowIndex, 0, checkBox);

    }


}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget::ondoubleClickExplist(QModelIndex index)
{
    QString sExpNo = _pExpNolistWidget->item(index.row(),1)->text();

    QStringList l_expNolist;
    l_expNolist.push_back(sExpNo);

    _pMeasureRelProcessWidget->clearAlldata();

    if(!_pMeasureRelProcessWidget->getExplistData(l_expNolist))
    {
        return;
    }

    _pMeasureRelProcessWidget->showExpData(_pMeasureRelProcessWidget->m_ExpdataInfolist);

    _pMeasureRelProcessWidget->onExpDataProcess();
}




/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget::onShowMenu(QPoint pos)
{

    int nCnt = 0;

    for(int i = 0; i < _pExpNolistWidget->rowCount(); i++)
    {
        QTableWidgetItem *checkBox = _pExpNolistWidget->item(i,0);
        if(checkBox->checkState() == Qt::Checked)
        {
            nCnt++;
        }
    }

    if(nCnt >=2)
    {
        m_mulitProcessMenu.move(cursor().pos());
        m_mulitProcessMenu.show();
    }

}

/******************************************
* 功能:数据处理
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget::onMulitDataProcess()
{
    QStringList l_expNolist;

    for(int i = 0; i < _pExpNolistWidget->rowCount(); i++)
    {
        QTableWidgetItem *checkBox = _pExpNolistWidget->item(i,0);
        if(checkBox->checkState() == Qt::Checked)
        {
            l_expNolist.append(_pExpNolistWidget->item(i,1)->text());
        }
    }

    _pMeasureRelProcessWidget->getExplistData(l_expNolist);
    if(!_pMeasureRelProcessWidget->checkExpInfolist())
    {
        QMessageBox::warning(this,"提示","数据中任务编号或元件编号不一致,无法进行合并处理!");
        return;
    }

    _pMeasureRelProcessWidget->showExpData(_pMeasureRelProcessWidget->m_ExpdataInfolist);

    _pMeasureRelProcessWidget->onExpDataProcess();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget::onDateChange(QDate date)
{
    onShowExpNolist();
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget::onComboxSelectChange(int nIndex)
{
    onShowExpNolist();
}


/******************************************
* 功能:生成报表
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectDataWidget::onGerenReport()
{

    QString sTaskNo = _pExpTaskEdit->toPlainText();

    if(sTaskNo.isEmpty())
    {
        QMessageBox::warning(this,"提示","请输入任务单编号");
        return;
    }

    _pReportWidget->showExpRelInfo(sTaskNo);

    _pReportWidget->show();
}



/******************************************
* 功能:获取原始数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
//bool QDefectDataWidget::getOrigEnergydata(QString sExpNo,int nChannelNo,vector<st_pointdataInfo>& output_st_pointdataInfolist)
//{
//    vector<st_pointdataInfo> l_st_pointdataInfolist;//内部数据
//   //查询能量数据
//    QStringList energyDatalist;

//    bool bRel = _pDbProcess->queryTableData(QString("select* from dbo.T_ERNERGY_DATA where ExpNo='%1' and  ChannelNo = '%2' order by PointNo, shotNo").arg(sExpNo).arg(nChannelNo),&energyDatalist);
//    if(bRel == false)
//    {
//        return false;
//    }

//    for (QString itemData : energyDatalist)
//    {
//        QStringList strArray = itemData.split('|');
//        if (strArray.size() < 9)
//        {
//            return false;
//        }

//        st_pointdataInfo l_pointInfo;
//        l_pointInfo.nPointNo = strArray[4].toInt();//点号
//        l_pointInfo.nShotNo = strArray[5].toInt();//发次
//        l_pointInfo.i32EnergySection = static_cast<int>(strArray[6].toFloat()*1000.0);//能量段 转换为整数
//        l_pointInfo.lfEnergydata = strArray[7].toFloat();//采集的能量数据 已换算为测量能量的数据
//        //l_pointInfo.lfFluxdata = strArray[8].toFloat();//通量数据

//        l_st_pointdataInfolist.push_back(l_pointInfo);
//    }


//    //查询损伤信息
//    for(st_pointdataInfo& l_pointInfo:l_st_pointdataInfolist)
//    {
//        QStringList defectlist;
//        bool bRel = _pDbProcess->queryTableData(QString("select* from dbo.T_DEFECT_CHECK_DATA where ExpNo='%1'and PointNo=%2 and ShotNo=%3 order by PointNo, ShotNo").\
//                                            arg(sExpNo).arg(l_pointInfo.nPointNo).arg(l_pointInfo.nShotNo),&defectlist);

//        if(bRel == false)
//        {
//            return false;
//        }

//        if(defectlist.size() != 1)
//        {
//            qInfo()<<"损伤数据查询错误,不为1条";
//            l_pointInfo.bDefect = false;
//            continue;
//        }

//        l_pointInfo.bDefect = defectlist[0].split('|')[5].toInt()==0?false:true;
//    }

//    for(st_pointdataInfo l_pointInfo:l_st_pointdataInfolist)
//    {
//        output_st_pointdataInfolist.push_back(l_pointInfo);
//    }


//    return true;

//}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
//void QDefectDataWidget::saveDataToCSV(QString sExpNo, vector<st_pointdataInfo> l_st_pointdataInfolist)
//{
//    //保存文件
//    QDir l_dir( QApplication::applicationDirPath()+"/data");
//    if (!l_dir.exists())
//    {
//        l_dir.mkdir(QApplication::applicationDirPath()+"/data");
//    }
//    QString sPath = QApplication::applicationDirPath()+QString("/data/%1").arg(sExpNo);
//    QDir l_dir0(sPath);
//    if (!l_dir0.exists())
//    {
//        l_dir0.mkdir(sPath);
//    }

//    QFile csvfile(sPath+QString("/%1.csv").arg(sExpNo));
//    if(csvfile.exists())
//    {
//        csvfile.remove();
//    }

//    if(!csvfile.open(QIODevice::ReadWrite|QIODevice::Text))
//    {
//        return;
//    }

//    QString sHeader = "能量段,点号,发次,能量数据,通量数据,是否损伤\r";

//    QString str = sHeader;

//    for(st_pointdataInfo& l_pointInfo:l_st_pointdataInfolist)
//    {
//        str = str + QString("%1,%2,%3,%4,%5,%6\r").arg((float)l_pointInfo.i32EnergySection/1000.0).arg(l_pointInfo.nPointNo).arg(l_pointInfo.nShotNo).arg(l_pointInfo.lfEnergydata).arg(l_pointInfo.lfFluxdata).arg(l_pointInfo.bDefect?"是":"否");
//    }

//    csvfile.write(str.toLocal8Bit());

//    csvfile.close();

//}




