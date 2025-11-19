#include "offlinedatawidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QOfflineDataWidget::QOfflineDataWidget(CDbDataProcess* pdb,QWidget *parent) : QWidget(parent)
{
    _pDbProcess = pdb;

    InitUI();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QOfflineDataWidget::InitUI()
{
    _pTabwidget = new QTabWidget();

    _pTabwidget->addTab(new QDefectDataWidget(_pDbProcess),"损伤阈值测量数据");
    _pTabwidget->addTab(new QSystemDataWidget(_pDbProcess),"系统性能参数看板");
    _pTabwidget->addTab(new QMeasureAreaDataWidget(_pDbProcess),"光斑面积测量数据");

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(_pTabwidget);

}



