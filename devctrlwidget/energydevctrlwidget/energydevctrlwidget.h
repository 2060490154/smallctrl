#ifndef QEnergyDevCtrlWidget_H
#define QEnergyDevCtrlWidget_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QComboBox>
#include <QGraphicsDropShadowEffect>
#include <QStringList>
#include <QTimer>
#include <QThread>

#include "energydevctrl.h"

using namespace std;

typedef struct _energdevconfig
{
    QEnergyDevCtrl* pEnergyDevCtrl;
    QStringList  sChannelNamelist;
    QStringList sChannelNolist;
}tEnergdevconfig;


class QEnergyDevCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QEnergyDevCtrlWidget( vector<tEnergdevconfig> pDevConfig,QWidget *parent = nullptr);

signals:

public slots:


public:
    vector<tEnergdevconfig> _pEnergyDevCtrllist;

};


class QEnergyDevCtrlItemWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit QEnergyDevCtrlItemWidget( QEnergyDevCtrl* pCtrl,int nPort,QString sDevName,QWidget *parent = nullptr);

signals:

public slots:

    void onConnectDev();//连接设备

    void onRefreshParam();//刷新参数

    void onCheckMeasureValue();//查询测量值

    void onSetParam();//设置参数



    void onShowMeasureVaule();


    void delayTime(int time);

public:
    void InitUI();

    void showDevParam();


    void setWidgetStyleSheet(QWidget* pWidget);

    QTimer m_showDataTimer;





private:

    QLabel* _pDevStatusLabel;//设备状态
    QComboBox* _pMeasureRangeComBox;//测量范围
    QComboBox* _pMeasureWaveLengthComBox;//测量波长范围
    QTextEdit* _pTriggerLevelEdit;//触发门限
    QTextEdit* _pMeasureValueEdit;//测量值
    QComboBox* _pTriggerModeComBox;//触发模式

    QPushButton* _pOpenButton;//连接设备
    QPushButton* _pRefreshStatusButton;//刷新状态
    QPushButton* _pSetParamButton;//参数设置
    QPushButton* _pCheckMeasureValueButton;//查询测量值




    QList<QString> _MeasureRangeVaulelist;//测量范围
    QList<QString> _MeasureWaveLengthlist;//测量波长
    int _i32MeasureRangeStartIndex;//测量范围的起始值 与每个型号相关






//    QLabel* m_pStatus;
//    QTextEdit* m_pSetLimitEdit;

//    QTextEdit* m_psetPlusCntEdit;
//    QTextEdit* m_psetDelayEdit;

//    QLabel* m_pshowCurrentDataLabel;
//    QPushButton* m_pOpenButton;
//    QPushButton*m_pCloseButton;
//    QPushButton*m_pRefreshButton;
//    QPushButton*m_pSetLimitDataButton;
//    QPushButton*m_psetProcessStatusButton;//设置流程模式




private:
    QEnergyDevCtrl*_pEnergyDevCtrl;

    QString _sDevName;
    int _nChannelNo;

    bool _bDevConnected;//设备已连接
};

#endif // QEnergyDevCtrlWidget_H
