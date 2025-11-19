#ifndef LIGHTPARAMWIDGET_H
#define LIGHTPARAMWIDGET_H

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
#include "monitkxccdimgwidget.h"
#include "monitenergycoeffwidget.h"

#include "platformkxccdctrl.h"
#include "platformenergydevctrl.h"
#include "platformdataprocessctrl.h"

//#include "measurereslutwidget.h"
#include "logwidget.h"

#define M_ENERGY_COEFF_CNT 10//采集10个发次



//标定流程界面
class LightParamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LightParamWidget(QDevCtrlManager* pPlatformDevCtrlManager,CDbDataProcess* pDBProcess,tPlatformConfig* pPlatformConfig,QHash<QString,tChannelInfo>channelNameToInfo,QWidget *parent = nullptr);

    ~LightParamWidget();
signals:

    void signal_updatePlatformFile();

public slots:


    void onFinishWorkflow(QString sworkflowName,_Taskstatus taskstatus);

    void onSaveParam();

    void showParam();

    void onMeasureLaserAreaCheck();

public:
    void initUI();

    QWidget* createParamWidget();//创建参数设置界面

    void createMeasureWorkflow();//创建任务流程

    vector<Tasklist*>  createStandByTasklist();//创建准备任务列表



    void setWidgetStyleSheet(QWidget* pWidget);

    virtual bool checkRunCondition(QString sworkflowName);

    void updateCommonInfo();//更新公共信息

    Workflow* m_pMeasureLightParamWorkflow;//光路参数测量流程

    tPlatformConfig* m_tPlatformConfig;//平台的配置信息

private:

    QWorkflowWidget* _pMeasureLightParamWorkflowWidget;//光路参数测量流程界面

    QWorkflowWidget* _pStandbyWorkflowWiget;//准备流程界面

    QLogWidget* _pLogWidget;//日志显示窗宽

    QDevCtrlManager* _pPlatformDevCtrlManager;//设备控制类



    tCommonInfo _tCommonInfo;//公共信息 TaskInfo中可获取该数据

    QHash<QString,tChannelInfo> _mapChannelNameToInfo;

    //参数设置
    //QComboBox* _pLightSelectComBox;//光路选择
    QLineEdit* _pEnergySectionEdit;//能量衰减比例
    QCheckBox* _pMeasureLaserAreaCheckBox;//光斑面积
    QCheckBox* _pMeasureLaserPlusCheckBox;//脉宽
    QCheckBox* _pMeasureLightCoeffCheckBox;//光路分光比系数

    QLabel* _pCurrentLaserAreaLabel;//当前光斑面积
    QLabel* _pCurrentLightCoeffLabel;//光路分光比
    QLabel* _pCurrentPlusWidthLabel;//当前脉宽

    QLabel* _pCurrentWaveLengthLabel;//当前光路

    QPushButton* _pRefreshParamBtn;//参数保存

    QTextEdit* _pLaserInputEnergyEdit;//激光器输出能量值


    //监视窗口

    QTabWidget* _pMonitWidgetTab;//监视窗口切换
    QMonitKxccdImgWidget* _pMonitKxccdImageWidget;//近场CCD监视画面
    QMonitEnergyCoeffWidget* _pMonitEnergyCoeffWidget;//监视能量计系数标定数据

    //当前光路选择
    int m_nWaveLengthType;

};

#endif // LIGHTPARAMWIDGET_H
