#ifndef DefectMeasureWidget_H
#define DefectMeasureWidget_H

#include <QWidget>
#include <QGridLayout>
#include <qlistwidget.h>
#include <QTabWidget>
#include "workflowwidget.h"
#include "devctrlmanager.h"
#include "dbdataprocess.h"
#include "pointareawidget.h"
#include "paramwidget.h"
#include "platformtaskdefine.h"
#include "platformconfig.h"
#include "devconfig.h"
#include "reslutprocesswidget.h"
#include "logwidget.h"
#include "defectcheckimagewidget.h"
#include "platformdefectcheckctrl.h"




//标定流程界面
class DefectMeasureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefectMeasureWidget(QDevCtrlManager* pPlatformDevCtrlManager,CDbDataProcess* pDBProcess,tPlatformConfig* pPlatformConfig,QDevConfig* pDevConfig,QWidget *parent = nullptr);

    ~DefectMeasureWidget();
signals:

    void signal_updatePlatformFile();

public slots:

    void onUpdateWorkflow(QVariant var);

    void onUpdatePointStatus();

    void onFinishWorkflow(QString sworkflowName,_Taskstatus taskstatus);

    void showParam();

public:
    void initUI();


    void create1On1workflow();

    void createROn1workflow();


    void setWidgetStyleSheet(QWidget* pWidget);

    virtual bool checkRunCondition(QString sworkflowName);

    void updateCommonInfo();//更新公共信息

    Workflow* m_p1On1workflow;//1On1流程

    Workflow* m_pROn1workflow;//ROn1流程

    tPlatformConfig* m_tPlatformConfig;//平台的配置信息

private:

    QWorkflowWidget* _pMeasureWorkflowWiget;//1On1、 ROn1流程界面

    ParamWidget* _pParamWidget;//参数设置界面

    tWorkflowParam _tWorkflowParam;//保存参数设置界面的参数

    QPointAreaWidget* _pPointAreaWidget;//点状态显示界面

    QLogWidget* _pLogWidget;//日志显示窗宽

    QDevCtrlManager* _pPlatformDevCtrlManager;//设备控制类

    QDevConfig* _pDevConfig;//设备配置信息

    tCommonInfo _tCommonInfo;//公共信息 TaskInfo中可获取该数据

    QHash<QString,tChannelInfo> _mapChannelNameToInfo;

    QList<QPointInfo>* _pPointList;//点列表

    QTimer* _pUpdatePointStatusTimer;//刷新点状态的定时器

    QMeasureRelProcessWidget* _pMeasureReslutWidget;//测试结果


    QDefectCheckImageWidget*_pDefectCheckImageWidget;//损伤检测系统给出的图像


    QTabWidget* _pTabWidget;



};

#endif // DefectMeasureWidget_H
