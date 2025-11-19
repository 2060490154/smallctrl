#ifndef QDEVCTRLMANAGER_H
#define QDEVCTRLMANAGER_H

#include <QObject>
#include <functional>
#include "workflow.h"
#include "devdefine.h"

using namespace std;

using runTaskFun = std::function<bool(TaskInfo* ,tCommonInfo* )>;
using checkDevStatusFun = std::function<bool()>;
//typedef std::function<bool(TaskInfo* ,tCommonInfo* )> runTaskFun;
//typedef std::function<bool()> checkDevStatusFun;

#define bindRunTask(className,inistanceClass) static_cast<void*>(inistanceClass),std::bind(&className::runTask,inistanceClass,placeholders::_1,placeholders::_2)\
                                               ,std::bind(&className::checkDevStatus,inistanceClass)

typedef struct  _DevCtrlInfo
{
    int m_nDevType;//设备类型
    int m_nDevIndex;//设备编号
    void* m_pDevCtrl;//设备控制
    runTaskFun devRunTaskFun;
    checkDevStatusFun devStatusCheckFun;//设备连接状态检查函数
}st_DevCtrlInfo;


class QDevCtrlManager : public QObject
{
    Q_OBJECT
public:
    explicit QDevCtrlManager(QObject *parent = nullptr);
    ~QDevCtrlManager();
signals:

    void showErrorMsg(QString serrMsg);//对外输出错误信息 方便界面显示

public slots:

public:


    void* getDevCtrl(int nDevType,int nDevIndex);

    int runDevCtrlTask(TaskInfo* pTask,tCommonInfo* pCommonInfo);

    void registerDevInfo(int nDevType,int nDevIndex,void* devCtrl,runTaskFun devRunTaskFun=nullptr,checkDevStatusFun devstatusCheckFun=nullptr);

    bool getDevStatus(int nDevType,int nDevIndex);

private:

    runTaskFun getDevRunTaskFun(int nDevType,int nDevIndex);

    QList<st_DevCtrlInfo> _tDevCtrlManagerlist;//所有的设备控制
};

#endif // QDEVCTRLMANAGER_H
