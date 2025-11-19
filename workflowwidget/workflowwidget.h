#ifndef WORKFLOWWIDGET_H
#define WORKFLOWWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QFile>
#include <functional>
#include "ringsprogressbar.h"
#include "workflowprocessthread.h"
#include "devctrlmanager.h"
#include "workflow.h"
#include "workflowdetailwidget.h"
#include "quiqss.h"

using namespace std;

#define M_START_PROCESS_TEXT  ("开始")
#define M_SUSPEND_PROCESS_TEXT  ("暂停")
#define M_CONTINUE_PROCESS_TEXT  ("继续")
#define M_STOP_PROCESS_TEXT  ("停止")


using namespace std;
typedef std::function<bool(QString )> checkRunConditionFun;

#define bindCheckConditionFun(funName,inistanceClass) std::bind(&funName,inistanceClass,placeholders::_1)


class QWorkflowWidget:public QWidget
{
     Q_OBJECT
public:
    QWorkflowWidget(QDevCtrlManager* pPlatformDevCtrlManager,checkRunConditionFun pcheckRunConditionFun=nullptr);
    ~QWorkflowWidget();


signals:

    void showlogInfo(QString str,bool bWarning=false);
    void startProcess(QString sworkflowName);//开始启动信号
    void finishedProcess(QString sworkflowName,_Taskstatus nTaskstatus);//结束信号

public slots:

    void onStartProcessTask();//开始/停止流程

    void onTaskFinished();//任务执行结束

    void onUpdateStatus();//刷新状态

    void onShowDetailInfo();//显示任务执行详细信息



public:
    void setWorkflow(Workflow* tworkflowlist);


    void paintEvent(QPaintEvent *event);


private:
    void initWidget();//初始化界面

    //暂时无此功能
    void setSuspendWorkflow(vector<TaskInfo*> pTaskList);//设置暂停时所要执行的流程

    void setContinueWorkflow(vector<TaskInfo*> pTaskList);//设置继续时所要执行的流程

    void setStopWorkflow(vector<TaskInfo*> pTaskList);//设置

    checkRunConditionFun _checkRunConditionFun;

private:

    QPushButton* _pStartProcessButton;//开始、停止按钮
    QPushButton* _pShowDetailButton;//显示流程执行详细信息的按钮
    RingsProgressbar* _pProcessBar;//进度条
    QLabel* _pNameLabel;
    QLabel* _pWorkFlowStatusLabel;//任务执行状态

private:

    Workflow* _pWorkflowlist;//工作流程

    QWorkflowProcessThread* _pTaskProcessThread;//任务处理线程

    QDevCtrlManager* _pPlatformDevCtrlManager;//设备控制

    QImage* _pStatusImage;//状态图片

    QTimer* _pUpdateStatusTimer;//状态更新定时器

    QWorkflowDetailWidget* _pWorkFlowDetailWidget;//工作流程 处理详细显示

    vector<TaskInfo*> _pSuspendTasklist;//暂停时需要执行的流程

    vector<TaskInfo*> _pContinueTasklist;//继续时需要执行的流程

    vector<TaskInfo*> _pStopTasklist;//停止时需要执行的流程

};

#endif // WORKFLOWWIDGET_H
