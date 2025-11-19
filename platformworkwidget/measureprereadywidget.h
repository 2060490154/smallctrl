#ifndef MeasurePreReadyWidget_H
#define MeasurePreReadyWidget_H

#include <QWidget>
#include <QGridLayout>
#include <qlistwidget.h>
#include <QTabWidget>
#include <QButtonGroup>
#include <QtConcurrent>
#include "workflowwidget.h"
#include "devctrlmanager.h"
#include "motordevctrlbasic.h"
#include "dbdataprocess.h"
#include "pointareawidget.h"
#include "paramwidget.h"
#include "platformtaskdefine.h"
#include "platformconfig.h"
#include "monitkxccdimgwidget.h"
#include "monitenergycoeffwidget.h"
#include "motordevdefine.h"

#include "platformkxccdctrl.h"
#include "platformenergydevctrl.h"
#include "platformdataprocessctrl.h"
#include "logwidget.h"
//#include "measurereslutwidget.h"

#define M_ENERGY_COEFF_CNT 10//采集10个发次



//标定流程界面
class MeasurePreReadyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MeasurePreReadyWidget(QDevCtrlManager* pPlatformDevCtrlManager,CDbDataProcess* pDBProcess,tPlatformConfig* pPlatformConfig,QDevConfig* pDevConfig,QWidget *parent = nullptr);

    ~MeasurePreReadyWidget();

signals:

    void signal_updatePlatformFile();

public slots:

    void onFinishWorkflow(QString sworkflowName,_Taskstatus taskstatus);

    void onSaveParam();

    void onMeasureTypeSelect(int nIndex);

    void onMoveButtonClick(int nButtonId);

    void onShowMotorStatus(int nMotorIndex,QVariant varStatus);

    void showParam();

public:
    void initUI();

    QWidget* createParamWidget();//创建参数设置界面
    QWidget* createMotorWidget();//电机操作界面

    void createWaveLengthChangeWorkflow();//创建光路切换流程

    void createMaxEergyConfirmWorkflow();//创建最大能量测量流程




    void setWidgetStyleSheet(QWidget* pWidget);

    virtual bool checkRunCondition(QString sworkflowName);

    void updateCommonInfo();//更新公共信息

    Workflow* m_pWaveLengthChangeWorkflow;//光路切换流程
    Workflow* m_pMaxEnergyConfirmWorkflow;//最大能量测量流程

    tPlatformConfig* m_tPlatformConfig;//平台的配置信息

private:

    QWorkflowWidget* _pWaveLengthChangeWorkflowWidget;//光路切换流程
    QWorkflowWidget* _pMaxEnergyConfirmWidget;//最大能量测量流程

    QLogWidget* _pLogWidget;//日志显示窗宽

    QDevCtrlManager* _pPlatformDevCtrlManager;//设备控制类



    tCommonInfo _tCommonInfo;//公共信息 TaskInfo中可获取该数据

    QHash<QString,tChannelInfo> _mapChannelNameToInfo;

    //参数设置
    QComboBox* _pWaveLengthSelectComBox;//光路选择
    QComboBox* _pMeasureTypeSelectComBox;//测量模式选择

    QLineEdit* _pEnergyMinEdit;//最小能量设置
    QLabel* _pEnergyMaxlabel;//最大能量设置
    QLineEdit* _pEnergyIncreamEdit;//能量递增台阶设置
    QLineEdit* _pEnergyIncreamCntEdit;//能量递增台阶数设置
    QLineEdit* _pRatioEdit;//旋转台
    QLabel* _pEnergyMinLabel;//1on1为能量设置 ron1为最小能量设置
    QLabel* _pCurrentWaveLengthLabel;//当前光路
    QPushButton* _pSaveParamButton;

    //X轴
    QPushButton* _pLeftMoveButton;
    QPushButton* _pRightMoveButton;
    //Y轴
    QPushButton* _pUpMoveButton;
    QPushButton* _pDownMoveButton;

    QPushButton* _pRatioMoveButton;

    QButtonGroup* _pMoveButtonGroup;

    QLineEdit* _pMoveDistanceEdit;


    QDevConfig* _pDevConfig;//设备配置信息





    int m_nWaveLengthType; //当前光路选择
    int m_nMeasureType;
    float m_lfMinEnergy;
    float m_lfEnergyIncream;
    int m_nEnergyIncreamCnt;

    float m_lfMaxEnergy;//测量出的最大能量
    QHash<int,QString> _WaveTypeHash;

};

#endif // MeasurePreReadyWidget_H
