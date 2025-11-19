/**************************************************
*文件说明:工作流程详细展示页面，并实时刷新工作流程的状态
*创建人:
*维护记录:
*
******************************************************/
#include "workflowdetailwidget.h"


#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

QWorkflowDetailWidget::QWorkflowDetailWidget(QWidget *parent) : QWidget(parent)
{


    _pWorkflow = nullptr;

    this->setWindowModality(Qt::NonModal);//非阻塞窗口

    _pTableWidget = new QTableWidget();

    InitTableWidget();

    QGridLayout* playout = new QGridLayout(this);

    playout->addWidget(_pTableWidget,0,0,1,1);

    //刷新状态
    _pUpdateStatusTimer = new QTimer();
    connect(_pUpdateStatusTimer,&QTimer::timeout,this,&QWorkflowDetailWidget::onUpDateStauts);
    _pUpdateStatusTimer->start(1000);

}


/******************************************
* 功能:设置Tasklist 用于状态显示等
* 输入:tTaskList  任务列表
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowDetailWidget::setWorkflow(Workflow* pWorkflow)
{
    _pWorkflow = pWorkflow;

    //初始化显示数据
    _pTableWidget->clearContents();

    int nTotalNum = _pWorkflow->getAllTaskNum();
    _pTableWidget->setRowCount(nTotalNum);

    int nRowIndex = 0;


    for(size_t i32Index = 0; i32Index < _pWorkflow->m_tWorkflow.size(); i32Index++)
    {
        Tasklist* _pTaskList = _pWorkflow->m_tWorkflow[i32Index];

        if(_pTaskList->m_tTaskList.size() > 1)//多个子任务时 合并单元格
        {
             _pTableWidget->setSpan(nRowIndex,0,_pTaskList->m_tTaskList.size(),1);
        }

        _pTableWidget->setItem(nRowIndex,0,new QTableWidgetItem(QString("%1").arg(_pTaskList->m_sTaskListName)));


        for(size_t i = 0; i < _pTaskList->m_tTaskList.size();i++)
        {
            TaskInfo taskItem = *_pTaskList->m_tTaskList[i];

            _pTableWidget->setItem(nRowIndex,1,new QTableWidgetItem(QString("%1").arg(taskItem.m_sTaskName)));

            QTableWidgetItem* pItem = new QTableWidgetItem(QString("%1").arg("未执行"));

            pItem->setTextColor(QColor(200,200,200));
            _pTableWidget->setItem(nRowIndex,2,pItem);

            nRowIndex++;
        }

    }





}

/******************************************
* 功能:实时刷新任务执行的状态显示
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowDetailWidget::onUpDateStauts()
{
    if(_pWorkflow==nullptr || _pWorkflow->m_tWorkflow.size() == 0)
    {
        return;
    }

    //刷新显示的内容
    int nRunningIndex = -1;//查找正在执行任务的最大行  设置滚动条位置
    int nRowIndex = 0;
    for(size_t i32Index = 0; i32Index < _pWorkflow->m_tWorkflow.size(); i32Index++)
    {
        Tasklist* _pTaskList = _pWorkflow->m_tWorkflow[i32Index];

        for(size_t i = 0; i < _pTaskList->m_tTaskList.size();i++)
        {

            TaskInfo taskItem = *_pTaskList->m_tTaskList[i];

            QTableWidgetItem* pItem = _pTableWidget->item(nRowIndex,2);

            if(pItem != nullptr)
            {
                pItem->setText(taskItem.getTaskStatusText());
                pItem->setTextColor(taskItem.getTaskStatusColor());
            }
            if(taskItem.m_nTaskStatus == M_TASK_STATUS_RUNNING)
            {
                nRunningIndex = nRowIndex;
            }

            nRowIndex++;
        }

    }

//    if(nRunningIndex != -1)//设置自动滑动
//    {
//        int min = _pTableWidget->verticalScrollBar()->minimum();
//        int max = _pTableWidget->verticalScrollBar()->maximum();
//        int value = (max-min)*((float)nRunningIndex/_pTableWidget->rowCount())+min;
//        _pTableWidget->verticalScrollBar()->setSliderPosition(value);
//    }
}


/******************************************
* 功能:设置表头信息
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QWorkflowDetailWidget::InitTableWidget()
{

    QStringList l_headerInfo;
    l_headerInfo.append("名称");
    l_headerInfo.append("详细步骤");
    l_headerInfo.append("状态");

    _pTableWidget->clear();
    _pTableWidget->verticalHeader()->setHidden(true);//垂直表头
    _pTableWidget->horizontalHeader()->setHidden(false);//水平表头
    _pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    _pTableWidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满

    _pTableWidget->setColumnCount(l_headerInfo.size());

    _pTableWidget->setHorizontalHeaderLabels(l_headerInfo);

    _pTableWidget->setColumnWidth(0,100);
    _pTableWidget->setColumnWidth(1,400);



}
