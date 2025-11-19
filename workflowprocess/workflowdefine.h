/**************************************************
*文件说明:本文件为工作流的自定义文件
*创建人: 李刚
*维护记录:
*
******************************************************/
#ifndef WORKFLOWDEFINE_H
#define WORKFLOWDEFINE_H

#include <QObject>
#include <QMutex>
#include "dbprocess/dbdataprocess.h"

//任务类型  可根据实际需要进行扩展
enum ActionType {
    M_SETPARAM_ACTION = 1,//执行参数设置
    M_PROCESS_ACTION  = 2,//执行动作 如运动、采集、处理等
};

//任务执行过程中产生的清除缓存数据  该结构体根据具体使用进行修改
typedef struct  _taskdata
{
    int i32DevType;//哪个设备产生的
    int i32DevNo;//编号
    int i32DataIndex;//数据编号
    int i32ChannelNo;//通道编号--能量计时使用
    QVariant vardata;//数据内容
    //...可扩展

}tTaskData;

Q_DECLARE_METATYPE(tTaskData)
//根据需要可扩展该结构内容
class  tCommonInfo
{
public:

    QString sExpNo;//实验编号
    QString sSampleName;//被测元件名称
    int nMeasureType;//测量模式
    int nwavelengthType;//光路类型 0-1064nm 1-355nm

    float lfCoeff;//分光比
    float lfArea;//光斑面积 单位为cm^2
    float lfAngle;//测试角度
    float lfPlusWidth;//脉宽
    float lfM;//调制度

    //实验环境
    QString sTaskNo;//实验任务
    float lfExpTemp;//温度
    float lfExpdirty;//洁净度
    float lfExpHum;//湿度

    CDbDataProcess* _pDBProcess;//数据库操作相关内容

    QList<tTaskData> tTaskDatalist;//清除缓存数据 使用逻辑需要用户自己维护

    void addTaskData(tTaskData taskdata)
    {
        locker.lock();
        tTaskDatalist.append(taskdata);
        locker.unlock();
    }

private:

    QMutex locker;



};
//Q_DECLARE_METATYPE(tCommonInfo)



#endif //  WORKFLOWDEFINE_H
