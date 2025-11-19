#ifndef QMOTORCTRLWIDGET_H
#define QMOTORCTRLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <qlistwidget.h>
#include <qcombobox.h>
#include <QtConcurrent>
#include <QGraphicsDropShadowEffect>
#include "motorctrlwidgetdefine.h"
#include "motordevdefine.h"



//单个电机的控制模块
class QMotorCtrlItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMotorCtrlItemWidget(const tMotorConfigInfo* pMotorConfigInfo,QWidget *parent = 0);
    void InitUI();
    void showLimitStatus(bool isLeftLimit,bool isRightLimit);
    void setlogInfo(QString slog);

    void createGearUI();
    void createNoGearUI();

    void setWidgetStyleSheet(QWidget* pWidget);


signals:

public slots:
    void onUpdateMotorInfo(int nMotorIndex,QVariant varStatus);

    void onMoveLeft();
    void onMoveRight();
   // void OnStopMove();
    void OnMove2Zero();

    void onStartMove();//开始运动
    void onStopMove();//停止运动




public:

    //控制信息
    QMotorDevCtrlBasic* m_pMotorCtrl;
    const  tMotorConfigInfo* m_pMotorConfigInfo;
    int m_nMotorIndex;//电机编号



    //不带档位信息的UI
    QTextEdit* m_pPostionEdit;
    QPushButton* m_pMoveLeftBtn;
    QPushButton* m_pMoveRightBtn;
    QPushButton* m_pImmidStopBtn;
    QPushButton* m_pReturnZeroBtn;

    QLabel* m_pLeftLimitStatusLabel;//左限位状态显示
    QLabel* m_pRightLimitStatusLabel;//由限位状态显示
    QLabel* m_pMotorMoveStatus;
    QLabel* m_pMotorCurrentStepLabel;

    //配置档位信息的Ui
    QComboBox* m_pPostionCombox;
    QPushButton* m_pStartMoveBtn;


    QListWidget* m_pLogList;


};

class QMotorCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMotorCtrlWidget(vector<tMotorConfigInfo> pConfigList,QWidget *parent = 0);
    void InitUI();
    //void getDevHandle(QMotorDevManager* pMotorDevManager);

signals:

public slots:

public:


    vector<tMotorConfigInfo> m_pMotorConfigInfoList;

};






#endif // QMOTORCTRLWIDGET_H
