#include "measureareadatawidget.h"
#include <QFileDialog>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QMeasureAreaDataWidget::QMeasureAreaDataWidget(CDbDataProcess* pDbProcess)
{
    _pDbProcess = pDbProcess;

    InitUI();

    showExpNolist();

    void(QComboBox::*fp)(int)=&QComboBox::currentIndexChanged;
    connect(_pWavelenTypeCombox,fp,this,&QMeasureAreaDataWidget::onWaveTypeSelect);


    connect(_pExpNolistWidget,&QTableWidget::doubleClicked,this,&QMeasureAreaDataWidget::ondoubleClickExplist);

    connect(_pExportImageDataBtn,&QPushButton::clicked,this,&QMeasureAreaDataWidget::onExportImageTofolder);


}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureAreaDataWidget::InitUI()
{
    _pWavelenTypeCombox = new QComboBox(this);
    _pExpNolistWidget = new QTableWidget(this);
    _pAreaDataLabel = new QLabel("0.0",this);
    _pExportImageDataBtn = new QPushButton("导出数据",this);
    _pRefreshDataBtn = new QPushButton("刷新",this);

    QStringList l_headerInfo;
    l_headerInfo<<"实验编号";
    _pExpNolistWidget->verticalHeader()->setHidden(true);//垂直表头
    _pExpNolistWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    _pExpNolistWidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满
    _pExpNolistWidget->setColumnCount(l_headerInfo.size());
    _pExpNolistWidget->setHorizontalHeaderLabels(l_headerInfo);

    _pWavelenTypeCombox->addItem("1064nm");
    _pWavelenTypeCombox->addItem("355nm");
    _pWavelenTypeCombox->setCurrentIndex(0);

    for(int i = 0; i < 10; i++)
    {
        _pImageLabel[i] = new QImageLabel("");
    }

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(new QLabel("光路选择"),0,0,1,1);
    playout->addWidget(_pWavelenTypeCombox,0,1,1,1);
    playout->addWidget(_pRefreshDataBtn,0,2,1,1);


    playout->addWidget(new QLabel("光斑面积"),0,3,1,1);
    playout->addWidget(_pAreaDataLabel,0,4,1,1);
    playout->addWidget(_pExportImageDataBtn,0,5,1,1);


    for(int i = 0; i < 10; i++)
    {
        int nRow = 1+i/4;
        int nCol = 3*(i%4)+3;
       playout->addWidget(_pImageLabel[i],nRow,nCol,1,3);
    }

    playout->addWidget(_pExpNolistWidget,1,0,6,3);
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureAreaDataWidget:: showExpNolist()
{
    int nWaveType = _pWavelenTypeCombox->currentIndex();
    QStringList l_datalist = _pDbProcess->getAreaExpNolist(nWaveType);

    _pExpNolistWidget->clearContents();
    _pExpNolistWidget->setRowCount(0);

    for(QString sexpNo:l_datalist)
    {
        int nRowIndex = _pExpNolistWidget->rowCount();
        _pExpNolistWidget->insertRow(nRowIndex);
        _pExpNolistWidget->setItem(nRowIndex,0,new QTableWidgetItem(sexpNo));
    }

    for(int i = 0; i < 10; i++)
    {
        _pImageLabel[i]->m_pImagelabel->clear();
    }


}

/******************************************
* 功能:光路选择
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureAreaDataWidget:: onWaveTypeSelect(int nIndex)
{
    showExpNolist();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureAreaDataWidget::ondoubleClickExplist(QModelIndex index)
{
    int nWaveType = _pWavelenTypeCombox->currentIndex();
    //查询 显示图像数据
    _sExpNo = _pExpNolistWidget->item(index.row(),index.column())->text();

    QString sSql = QString("select t.ccdimage from t_kxccd_data t where t.expno = %1").arg(_sExpNo);

    if(_pDbProcess->_hdatabase.isOpen() == false)
    {
        if(!_pDbProcess->connectDB())
        {
            return;
        }
    }

    QSqlQuery sqlQuery(_pDbProcess->_hdatabase);
    if(sqlQuery.exec(sSql) == false)
    {
       return;
    }

    int nCnt = 0;
    for(int i = 0; i < 10; i++)
    {
        _pImageLabel[i]->m_pImagelabel->clear();
    }

    while(sqlQuery.next())
    {
        QByteArray l_byteArray;

        l_byteArray = sqlQuery.value(0).toByteArray();

        if(nCnt< 10 )
        {
            QImage l_image = QImage::fromData(l_byteArray);
            QPixmap l_pixmap = QPixmap::fromImage(l_image);
            _pImageLabel[nCnt]->showImage(l_pixmap);
        }

        nCnt++;
     }


    //查询 显示 光斑面积数据
    QStringList l_areadatalist =  _pDbProcess->getHistoryAreaData(nWaveType);
    for(QString itemdata:l_areadatalist)
    {
        QStringList l_splitdata = itemdata.split('|');
        if(l_splitdata.size() != 2)
        {
            continue;
        }
        QString l_sExpNo = l_splitdata[0];
        if(l_sExpNo == _sExpNo)
        {
            _pAreaDataLabel->setText(l_splitdata[1]);
            break;
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
void QMeasureAreaDataWidget::onRefreshData()
{
    showExpNolist();
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMeasureAreaDataWidget::onExportImageTofolder()
{
    //选择文件夹
    QString sSelectFolderPath = QFileDialog::getExistingDirectory(this,"选择","C:/",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);

    if(sSelectFolderPath.isEmpty())
    {
        return;
    }


    //查询 显示图像数据

    QString sSql = QString("select t.ccdimage from t_kxccd_data t where t.expno = %1").arg(_sExpNo);

    if(_pDbProcess->_hdatabase.isOpen() == false)
    {
        if(!_pDbProcess->connectDB())
        {
            return;
        }
    }

    QSqlQuery sqlQuery(_pDbProcess->_hdatabase);
    if(sqlQuery.exec(sSql) == false)
    {
       return;
    }

    int nCnt = 0;
    while(sqlQuery.next())
    {
        QByteArray l_byteArray;

        l_byteArray = sqlQuery.value(0).toByteArray();

        if(nCnt< 10 )
        {
            QString sfilepath = sSelectFolderPath+"/"+QString::number(nCnt)+".bmp";
            QImage l_image = QImage::fromData(l_byteArray);
            l_image.save(sfilepath);
        }

        nCnt++;
     }


}
