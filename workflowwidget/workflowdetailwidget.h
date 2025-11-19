#ifndef WORKFLOWITEMWIDGET_H
#define WORKFLOWITEMWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QGridLayout>
#include <QTimer>
#include <QHeaderView>
#include <QScrollBar>
#include "workflow.h"
using namespace std;

class QWorkflowDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QWorkflowDetailWidget(QWidget *parent = nullptr);

signals:

public slots:

    void onUpDateStauts();//刷新状态  定时器调用


public:
    void setWorkflow(Workflow* pWorkflow);


private:
    void InitTableWidget();

private:

    Workflow* _pWorkflow;


    QTableWidget* _pTableWidget;
    QTimer* _pUpdateStatusTimer;
};

#endif // WORKFLOWITEMWIDGET_H
