#ifndef TASKINFO_H
#define TASKINFO_H
#include <QVariant>
#include <QColor>
#include <initializer_list>

using namespace std;


//执行结果
enum _Taskstatus {
    M_TASK_STATUS_UNRUN = 0,//未执行
    M_TASK_STATUS_RUNNING = 0x01,//正在执行
    M_TASK_STATUS_FAILD = 0x02,//执行失败
    M_TASK_STATUS_SUCCESS = 0x03,//执行成功
    M_TASK_RELATION_FAILED=0x04,//关联任务失败 本任务无法执行

};

class TaskInfo
{

public:
    TaskInfo(QString sName,int nDevType,int nDevNo,int nAction,QVariant paramdata=0,int i32waitForTime = 0,initializer_list<int> relationTaskIndexlist={-1});

    QColor  getTaskStatusColor();
    QString getTaskStatusText();


public:
    QString m_sTaskName;//步骤名称
    int    m_nDevType;//设备类型
    int    m_nDevNo;//设备编号  同一个类型可能有多个设备编号
    int    m_nAction;//需要执行的动作
    QVariant m_tParamData;//相关参数数据 用户自定义  自定义结构体需要 Q_DECLARE_METATYPE

    vector<int> m_i32RelationTaskIndex;//等待关联步骤执行完成 -1 则无关联步骤 不等待

    int m_i32waitForTime;//本任务执行前 延迟时间 单位为ms

    _Taskstatus    m_nTaskStatus;//步骤状态  _Taskstatus

    QString m_sTaskErrorInfo;

    bool m_bTerimalTask;//强制终止任务 具体任务需要对本参数进行处理 方便强制退出任务

    bool m_bDefect;//是否损伤


private:




};


//任务列表
class Tasklist
{

public:
    Tasklist(QString sTaskListName)
    {
        m_sTaskListName = sTaskListName;
        m_nPonitIndex = -1;
        m_nTasklistStatus = M_TASK_STATUS_UNRUN;
    }
    ~Tasklist()
    {
        for(int i = m_tTaskList.size() -1; i >= 0; i--)
        {
            delete m_tTaskList[i];
        }
        m_tTaskList.clear();
    }

//    int addTask(TaskInfo* pTaskInfo)//返回的是taskid
//    {
//        m_tTaskList.push_back(pTaskInfo);

//        return m_tTaskList.size()-1;
//    }

    int addTask(QString sName,int nDevType,int nDevNo,int nAction,QVariant paramdata=0,int i32waitForTime = 0,initializer_list<int> realtionTaskIndelist={-1})//返回的是taskid
    {
        TaskInfo* pTaskInfo = new TaskInfo(sName,nDevType,nDevNo,nAction,paramdata,i32waitForTime,realtionTaskIndelist);
        m_tTaskList.push_back(pTaskInfo);

        return m_tTaskList.size()-1;
    }

public:
    vector<TaskInfo*> m_tTaskList;
    QString m_sTaskListName;

    _Taskstatus m_nTasklistStatus;//tasklist的执行结果

    int m_nPonitIndex;//点的索引 定制内容

};




#endif // TASKINFO_H
