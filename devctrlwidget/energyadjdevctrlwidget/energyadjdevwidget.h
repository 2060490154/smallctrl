#ifndef QEnergyAdjDevWidget_H
#define QEnergyAdjDevWidget_H

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
#include "energyadjdevwidgetdefine.h"
#include "energyadjustdevctrl/energyadjustdevctrl.h"



//单个能量调节模块
class QEnergyAdjItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QEnergyAdjItemWidget(const tEnergyAdjDevConfig* pEnergyAdjDevConfig,QWidget *parent = 0);
    void InitUI();
    void showLimitStatus(bool isLeftLimit,bool isRightLimit);
    void setlogInfo(QString slog);

    void createDevItem();

    void setWidgetStyleSheet(QWidget* pWidget);


signals:

public slots:
    void onUpdateMotorInfo(QVariant varStatus,bool bShowMoveStatus = true);

    void onStartMove();
    void OnMove2Zero();

    void onStopMove();//停止运动




public:

    //控制信息
    QEnergyAdjustDevCtrl* m_pEnergyAdjDevCtrl;
    const  tEnergyAdjDevConfig* m_pMotorConfigInfo;
    int m_nMotorIndex;//电机编号



    //不带档位信息的UI
    QTextEdit* m_pPostionEdit;
    QPushButton* m_pMoveBtn;
    QPushButton* m_pImmidStopBtn;
    QPushButton* m_pReturnZeroBtn;

    QLabel* m_pLeftLimitStatusLabel;//左限位状态显示
    QLabel* m_pRightLimitStatusLabel;//由限位状态显示
    QLabel* m_pMotorMoveStatus;
    QLabel* m_pMotorCurrentStepLabel;



    QListWidget* m_pLogList;


};

class QEnergyAdjDevWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QEnergyAdjDevWidget(vector<tEnergyAdjDevConfig> pConfigList,QWidget *parent = 0);
    void InitUI();

signals:

public slots:

public:

    vector<tEnergyAdjDevConfig> m_pEnergyAdjDevConfigInfoList;

};






#endif // QEnergyAdjDevWidget_H
