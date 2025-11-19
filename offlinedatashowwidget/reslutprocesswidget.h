#ifndef QMEASURERELPROCESSWIDGET_H
#define QMEASURERELPROCESSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QComboBox>
#include <QHash>
#include <QtConcurrent>
#include "qcustomplot.h"
#include "platformtaskdefine.h"
#include "dbdataprocess.h"
#include "defectcheckimagewidget.h"

using namespace std;


class QMeasureRelProcessWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMeasureRelProcessWidget(CDbDataProcess* pDb,QWidget *parent = nullptr);

    void InitUI();

signals:

public slots:

    void ondoubleClickDefectData(QModelIndex index);

    void onExpDataProcess();



    void onExportData();//导出数据

public:

    void clearAlldata();

    void show1On1Data(vector<st_damageInfo> damageInfolist, float lfMaxDamageFlux,double lfa,double lfb);
    void showROn1Data(vector<st_damageInfo> damageInfolist, float lfMaxDamageFlux);

    void showRelData(float lfMaxDamageFlux,float lfMaxDamageFlux1);

    bool getExpData(QString sExpNo,st_expDataInfo& expdataInfo);//获取实验信息

    bool getExplistData(QStringList sExpNolist);


    bool cacl1On1Data(vector<st_expDataInfo> expdataInfolist);
    bool caclROn1Data(vector<st_expDataInfo> expdataInfolist);

    void saveBluePrintParam();//保存图纸参数

    void saveTaskRel();//保存处理结果至数据库中

    void linearFit(double &a, double &b, QVector<double> xdata, QVector<double> ydata);

    QWidget* createDefectDataWidget();//创建损伤数据显示界面
    QWidget* createMeasureRelWidget();

    void exportdata();


    bool checkExpInfolist();//判断当前合并处理的数据是否合适 taskid相同 samplename相同



//    void saveDataToCSV(QString sExpNo,vector<st_pointdataInfo> l_st_pointdataInfolist);



    void showExpData(vector<st_expDataInfo> expdataInfolist);

    void caclTransferdata();//计算折算后的值


private:
    QWidget* createParamWidget();//创建数据处理参数组件

public:
    QPushButton* m_pProcessBtn;
    //QPushButton* m_pSaveParamBtn;
    QPushButton* m_pExportDataBtn;
    QCustomPlot m_customPlot;

    QTableWidget m_defectDataTableWidget;//显示每个发次的损伤情况


    QTextEdit* m_pEnergyStepEdit;//能量台阶
    QComboBox* m_pTargetTypeCombox;//元件类型
    QTextEdit* m_pTargetPulsewidthEdit;//图纸要求的测试脉宽
    QTextEdit* m_pTargetAngleEdit;//图纸要求的测试角度
    QTextEdit* m_pTestAngleEdit;//测试角度
    QLabel* m_pResultLabel;//显示处理结果

    //custom plot
    QCPItemTracer* m_groupTracer;
    QCPItemText* m_groupTracerText;
    QCPItemCurve* m_groupTracerArrow;

    QMap<QString,float> m_TargetType;



    //数据处理相关内容
    vector<st_expDataInfo> m_ExpdataInfolist;

    vector<st_damageInfo> m_damageInfolist;
    float m_lfMaxDamageFlux;//损伤阈值
    float m_lfTansferMaxDamageFlux;//折算后的损伤阈值

    float m_lfFluxStep;//能量台阶
    float m_lfTansferCoeff;//根据材料对应的折算系数
    float m_lfTargetAngle;//图纸要求的角度
    float m_lfTargetPlus;//图纸要求的脉宽
    float m_lfTestAngle;
    int m_nMeasureType;//测试类型


    //曲线拟合结果
    double m_lfa;
    double m_lfb;

private:

    QLabel* _pExpParamLabel;//实现参数显示

    CDbDataProcess* _pDbProcess;
    QDefectCheckImageWidget _defectImagewidget;




};

#endif // QMEASURERELPROCESSWIDGET_H
