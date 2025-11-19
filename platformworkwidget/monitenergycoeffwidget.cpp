#include "monitenergycoeffwidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

QMonitEnergyCoeffWidget::QMonitEnergyCoeffWidget(QWidget *parent) : QWidget(parent)
{
    InitUI();
    setWidgetStyleSheet(this);
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMonitEnergyCoeffWidget::InitUI()
{
    QStringList sHorizontalHeader;
    sHorizontalHeader<<"主能量计数据"<<"监视能量计数据";
    _pTableWidget = new QTableWidget(this);
    _pTableWidget->horizontalHeader()->show();
    _pTableWidget->verticalHeader()->show();
    _pTableWidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满
    _pTableWidget->setColumnCount(sHorizontalHeader.size());
    _pTableWidget->setHorizontalHeaderLabels(sHorizontalHeader);


    _pEnergyCoeffEdit = new QTextEdit("0.0",this);
    _pEnergyCoeffEdit->setMaximumHeight(30);

    QGridLayout* playout = new QGridLayout(this);
    playout->addWidget(_pTableWidget,0,0,1,6);

    playout->addWidget(new QLabel("分光比"),1,0,1,1);
    playout->addWidget(_pEnergyCoeffEdit,1,1,1,2);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMonitEnergyCoeffWidget::clearAllData()
{
    _pTableWidget->clearContents();

    _EnergyDevData[0].clear();
    _EnergyDevData[1].clear();

    _lfCoeffData= 0.0;

}

/******************************************
* 功能:显示能量数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMonitEnergyCoeffWidget::onUpdateEnergyData(int nDevType, int nDevNo, QVariant var)
{
    tTaskData l_taskdata = var.value<tTaskData>();

    if(nDevType == M_DEV_ENERGY)
    {
        float lfData = l_taskdata.vardata.value<float>();
        if(l_taskdata.i32ChannelNo < 2)//0-主能量计 1-监视能量计 通道号 代替设备编号
        {
            _EnergyDevData[l_taskdata.i32ChannelNo].push_back(lfData);
        }
    }

    if(nDevType == M_DEV_DATA_PROCESS && nDevNo == E_MEASUREPARAM_COEFF)
    {
        _lfCoeffData = l_taskdata.vardata.value<float>();
        _pEnergyCoeffEdit->setText(QString("%1").arg(_lfCoeffData));
    }

    //更新显示
     _pTableWidget->clearContents();
     int nRowMax = _EnergyDevData[0].size() > _EnergyDevData[1].size()?_EnergyDevData[0].size():_EnergyDevData[1].size();
     _pTableWidget->setRowCount(nRowMax);

     for(int i = 0; i < nRowMax; i++)
     {
         if( i < _EnergyDevData[0].size())
         {
             _pTableWidget->setItem(i,0,new QTableWidgetItem(QString("%1").arg(_EnergyDevData[0].at(i))));
         }
         if( i < _EnergyDevData[1].size())
         {
             _pTableWidget->setItem(i,1,new QTableWidgetItem(QString("%1").arg(_EnergyDevData[1].at(i))));
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
void QMonitEnergyCoeffWidget::setWidgetStyleSheet(QWidget* pWidget)
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

