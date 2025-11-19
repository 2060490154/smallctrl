#include "reportwidget.h"
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include "quiqss.h"
#include "qmsword.h"
#include <windows.h>
#include <QDateTime>
#include <QLineEdit>
#include <QApplication>
#include <QDebug>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif


QReportWidget::QReportWidget(CDbDataProcess* pDbProcess,QWidget *parent) : QDialog(parent)
{

    this->setWindowTitle("报表生成");
    this->resize(800,400);
    this->setModal(true);

    _pDbProcess = pDbProcess;

    InitUI();

    setWidgetStyleSheet(this);

    connect(_pCancleBtn,&QPushButton::clicked,this,[=](){this->hide();});
    connect(_pOkBtn,&QPushButton::clicked,this,&QReportWidget::onGernReport);

}

QReportWidget::~QReportWidget()
{

}

void QReportWidget::InitUI()
{
    _pReportTypeComBox = new QComboBox(this);
    _pOkBtn = new QPushButton("确定",this);
    _pCancleBtn = new QPushButton("取消",this);
    _pExpRelTablewidget = new QTableWidget(this);

    _pReportTypeComBox->addItem("检测报告");
    _pReportTypeComBox->addItem("检验报告");


    QStringList l_defect_headerInfo;
    l_defect_headerInfo<<"元件编号"<<"波长"<<"损伤阈值(J/cm²)"<<"折算后损伤阈值(J/cm²)";

    _pExpRelTablewidget->verticalHeader()->setHidden(true);//垂直表头
    _pExpRelTablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    _pExpRelTablewidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满
    _pExpRelTablewidget->setColumnCount(l_defect_headerInfo.size());
    _pExpRelTablewidget->setHorizontalHeaderLabels(l_defect_headerInfo);
    _pExpRelTablewidget->setColumnWidth(0,200);


    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(new QLabel("报告类型:"),0,0,1,1);
    playout->addWidget(_pReportTypeComBox,0,1,1,4);

    playout->addWidget(_pExpRelTablewidget,1,0,5,5);



    playout->addWidget(_pOkBtn,7,1,1,1);
    playout->addWidget(_pCancleBtn,7,3,1,1);

}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QReportWidget::showExpRelInfo(QString sTaskNo)
{
    _sTaskNo = sTaskNo;

    _pExpRelTablewidget->clearContents();
    _TaskRelDatalist.clear();

    QStringList defectlist;
    _pDbProcess->queryTableData(QString("select SampleName,WavelengthType,Area,Threshold,ConvThreshold,Pluswidth,TargetPluswidth,ExpTemp,ExpHum from dbo.T_TASK_REL_DATA where TaskNo ='%1' order by WavelengthType").arg(_sTaskNo)\
                                            ,&defectlist);

    for(QString str:defectlist)
    {
        st_RelData l_item;

        QStringList sItemArray = str.split('|');
        if(sItemArray.size() < 9)
        {
            continue;
        }

        l_item.sSampleName = sItemArray[0];
        l_item.nWavelengthType = sItemArray[1].toInt();
        l_item.lfArea = sItemArray[2].toFloat();
        l_item.lfFluxThreshold = sItemArray[3].toFloat();
        l_item.lfFluxConvThreshold = sItemArray[4].toFloat();
        l_item.lfPlusWidth = sItemArray[5].toFloat();
        l_item.lfTargetPlus = sItemArray[6].toFloat();
        l_item.lfTemp = sItemArray[7].toFloat();
        l_item.lfHum = sItemArray[8].toFloat();

        _TaskRelDatalist.emplace_back(l_item);
    }

    //显示数据
    _pExpRelTablewidget->setRowCount(_TaskRelDatalist.size());

    for(size_t i = 0;i < _TaskRelDatalist.size(); i++)
    {
        st_RelData item = _TaskRelDatalist[i];
        _pExpRelTablewidget->setItem(i,0,new QTableWidgetItem(item.sSampleName));
        QString sWavelength = item.nWavelengthType == 0?"1064nm":"355nm";
        _pExpRelTablewidget->setItem(i,1,new QTableWidgetItem(sWavelength));
        _pExpRelTablewidget->setItem(i,2,new QTableWidgetItem(QString::number(item.lfFluxThreshold,'g')));
        _pExpRelTablewidget->setItem(i,3,new QTableWidgetItem(QString::number(item.lfFluxConvThreshold,'g')));

    }

}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QReportWidget::onGernReport()
{
    QMSWord t_word;

    if(_pReportTypeComBox->currentIndex() == 0)
    {
       t_word.open(QApplication::applicationDirPath() + "/Doc/JCBG.dot",false);
    }
    else  if(_pReportTypeComBox->currentIndex() == 1)
    {
       t_word.open(QApplication::applicationDirPath() + "/Doc/JYBG.dot",false);
    }


    //添加检测报告基本信息
    t_word.setMarkContent("Number",_sTaskNo); //预留12个字符的位置，不够需要补全


    //添加检测结果信息
    t_word.addTableRow("RelTable",_TaskRelDatalist.size()-1,0);

    t_word.mergeCells(3,4,1,4+_TaskRelDatalist.size()-1,1);//合并元件名称表格
    t_word.mergeCells(3,4,2,4+_TaskRelDatalist.size()-1,2);//合并规格表格



    if(_TaskRelDatalist.size() > 0)
    {
        t_word.setCellText(3,3,5,QString::number(_TaskRelDatalist[0].lfPlusWidth,'g')+"ns");//实测脉宽
        t_word.setCellText(3,3,6,QString::number(_TaskRelDatalist[0].lfTargetPlus,'g')+"ns");//图纸脉宽

        t_word.setMarkContent("Temp",QString::number(_TaskRelDatalist[0].lfTemp,'g')); //取第一个温度和湿度
        t_word.setMarkContent("Hum",QString::number(_TaskRelDatalist[0].lfHum,'g'));

    }

    for (size_t i = 0; i< _TaskRelDatalist.size(); i++)
    {

        t_word.setCellText(3,i + 4,3,_TaskRelDatalist[i].sSampleName);

        QString sWavelength = _TaskRelDatalist[i].nWavelengthType == 0?"1064nm":"355nm";
        t_word.setCellText(3,i + 4,4,sWavelength);
        t_word.setCellBorderBottom(3,i + 4,4);


        t_word.setCellText(3,i + 4,5,QString::number(_TaskRelDatalist[i].lfFluxThreshold,'g'));
        t_word.setCellBorderBottom(3,i + 4,5);


        t_word.setCellText(3,i + 4,6,QString::number(_TaskRelDatalist[i].lfFluxConvThreshold,'g'));
        t_word.setCellBorderBottom(3,i + 4,6);

        if(_pReportTypeComBox->currentIndex() == 0)
        {
           t_word.setCellText(3,i + 4,7,"/");//检测报告
        }
        else  if(_pReportTypeComBox->currentIndex() == 1)
        {
           t_word.setCellText(3,i + 4,7,"");//检验报告
        }

        t_word.setCellBorderBottom(3,i + 4,7);



    }


    t_word.saveAs(QApplication::applicationDirPath() +"/Report/"+_sTaskNo+".doc");
    t_word.close();


    QMessageBox::warning(this,"提示","报告生成完毕！");
    this->hide();

    QString path = QApplication::applicationDirPath() +"/Report/";
    path = path.replace("/", "\\");
    QString cmd = QString("explorer.exe /e,/root,%1").arg(path);


    QProcess process;
    process.startDetached(cmd);




}



/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QReportWidget::setWidgetStyleSheet(QDialog* pWidget)
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
