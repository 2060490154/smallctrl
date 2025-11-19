#ifndef ParamWidget_H
#define ParamWidget_H

#include <QWidget>
#include <qlabel.h>
#include <qgridlayout.h>
#include <qimagereader.h>
#include <qpushbutton.h>
#include <QGroupBox>
#include <QTextEdit>
#include <QTextLine>
#include <QComboBox>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <qdatetime.h>
#include <QDir>
#include <QtMath>
#include <QDebug>
#include "platformtaskdefine.h"



class ParamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParamWidget(QWidget *parent = 0);

signals:
    void updatePointUI(int nSizeType,float nStep,float dRealWidth,float dRealHigh,float dRadius=0.0);
    void updateWorkFlow(QVariant var);//更新流程

public slots:
    void onUpdateSetParam();//保存设置参数
    void onChangeSizeType(int nIndex);
    void onChangeMeasureType(int nIndex);

    void onChangeWaveLength(int nIndex);


public:

    void showParam();

    bool checkParamSaved();

    void setAreaData(float lfArea);//更新光斑面积
    void setEnergyCoeffData(float lfCoeff);//更新分光比参数

    tWorkflowParam m_tworkflowParam;


private:
    void InitUI();

    void InitParam();

    void setWidgetStyleSheet(QWidget* pWidget);

    QComboBox* m_pDevSizeTypeCombox;//

    QComboBox* m_pMeasureTypeCombox;//测试类型选择

    QLabel* m_pWidthLabel;//长/半径显示
    QLabel* m_pHightLabel;//长/半径显示

    QLabel* m_pEnergyLabel;//能量发次/能量台阶数


    QTextEdit* m_pMeasureStepEdit;//实验步长
    QTextEdit* m_pSampleName;   //元件名称
    QTextEdit* m_pRectColCntEdit;//矩形宽
    QTextEdit* m_pRectRowCntEdit;//矩形高
    QTextEdit* m_pRadiusEdit;//圆形半径
    QTextEdit* m_pEnergyIncreamEdit;//能量阶梯
    QTextEdit* m_pMinimumEnergyEdit;//最低能量
    QTextEdit* m_pMaximumEnergyEdit;//最高能量
    QTextEdit* m_pMinEnergyDeg; //波片最小能量度数
    QTextEdit* m_pMaxEnergyDeg; //波片最大能量度数
    QTextEdit* m_pEnergySectionNumEdit;//能量段数
    QTextEdit* m_p1On1CntEdit;//每个能量阶梯对应的点数

    QPushButton* m_pRefreshParamBtn;//参数保存

    QTextEdit* m_pEnergyCoeffEdit;//能量系数

    QTextEdit* m_pRatioEdit;//旋转台角度信息
    QTextEdit* m_pPlusWidthEdit;//脉宽数据
    QTextEdit* m_pManualAreaEdit;//光斑面积

    QLabel* m_pCurrentWaveLengthLabel;//当前光路


    QTextEdit* m_pTaskNoEdit;//实验单任务编号
    QTextEdit* m_pExpTempeEdit;//实验温度
    QTextEdit* m_pExphumityEdit;//实验湿度
    QTextEdit* m_pExpdirtyEdit;//实验洁净度
    QTextEdit* m_pLaserInputEnergyEdit;//激光器输出能量值

    QList<QWidget*> m_pwidgetlist;



};

#endif // ParamWidget_H
