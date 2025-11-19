#ifndef QWorkflowProcessThread_H
#define QWorkflowProcessThread_H

#include <QObject>
#include <qmutex.h>
#include <QWaitCondition>
#include <QTime>
#include <QThreadPool>
#include "workflow.h"
#include "devctrlmanager.h"
#include "runtaskthread.h"
using namespace std;


#define M_TASK_MAX_TIME  (60*1000) //任务最长时间

#define M_SUSPEND_TASK_MAX_TIME  (10*1000) //执行暂停或停止流程的最长时间

class QWorkflowProcessThread : public QThread
{
    Q_OBJECT

public:

    static QWorkflowProcessThread* getInstance(QDevCtrlManager* pDevCtrlManager);

    ~QWorkflowProcessThread();

    bool checkThreadRunning();//检查当前线程是否正在运行

    void setWorkflowlist(Workflow* pworkflow);

    bool runTaskListStop();//立即执行 终止流程执行  先终止流程 再执行设置终止时执行的流程


    QString m_sCurrentWorkflowName;//当前流程的名称 唯一ID

private:

    QWorkflowProcessThread(QDevCtrlManager* pDevCtrlManager);

    void run();

     bool processTaskList(vector<TaskInfo>& pTaskList);

     _Taskstatus waitForTaskListRel();

     void InitTaskListStatus();//初始化流程执行状态

     void terminateRunningTask();//终止正在执行的task 强制终止

    static QWorkflowProcessThread *_pInstance;

    Workflow* _pWorkflow;

    QDevCtrlManager* _pDevCtrlManager;

    QThreadPool _RunTaskThreadPool;//线程池

    int _i32CurrentTaskCnt;//已经执行任务数量

    bool _bManualStopTask;//人为终止流程

    vector<TaskInfo*> _pManualStopTaskList;

     //析构单例
private:

     class Cleaner
     {
      public:
           ~Cleaner()
           {
               if(QWorkflowProcessThread::_pInstance != nullptr)
                {
                    delete QWorkflowProcessThread::_pInstance;
                    QWorkflowProcessThread::_pInstance = nullptr;
                 }
           }
      };


     static Cleaner _cleaner;

};

#endif // QWorkflowProcessThread_H
