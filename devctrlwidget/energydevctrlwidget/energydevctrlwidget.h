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
#include <QtSerialPort/QSerialPortInfo>
#include <vector>

#include "energydevctrl.h"

// 设备配置结构体（确保名字与 cpp 使用一致）
typedef struct _energdevconfig
{
    QEnergyDevCtrl* pEnergyDevCtrl;
    QStringList  sChannelNamelist;
    QStringList sChannelNolist;
} tEnergdevconfig;

class QEnergyDevCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QEnergyDevCtrlWidget(const std::vector<tEnergdevconfig>& pDevConfig, QWidget* parent = nullptr);

signals:

public slots:

public:
    std::vector<tEnergdevconfig> _pEnergyDevCtrllist;
};


class QEnergyDevCtrlItemWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit QEnergyDevCtrlItemWidget(QEnergyDevCtrl* pCtrl, int nPort, QString sDevName, QWidget* parent = nullptr);

signals:

public slots:
    void onConnectDev();//连接设备
    void onRefreshParam();//刷新参数 (保留旧名)
    void onCheckMeasureValue();//查询测量值
    void onSetParam();//设置参数
    void onShowMeasureVaule();

    // 对外可调用的刷新全部参数接口
    void refreshAllParams();

public:
    void InitUI();
    void showDevParam();

    void delayTime(int time);

    void setWidgetStyleSheet(QWidget* pWidget);

private:
    QEnergyDevCtrl* _pEnergyDevCtrl;

    QString _sDevName;
    int _nChannelNo;

    bool _bDevConnected;//设备已连接

    // 新增控件成员（与 cpp 中使用的名字保持一致）
    QComboBox* _pPortCombo;
    QPushButton* _pPortRefreshButton;

    QLabel* _pDetectorNameLabel;
    QLabel* _pRangeLabel;
    QLabel* _pWaveLengthLabel;
    QLabel* _pTriggerLevelLabel;
    QLabel* _pGainLabel;
    QLabel* _pOffsetLabel;
    QLabel* _pSamplingRateLabel;
    QLabel* _pSamplingPeriodLabel;
    QLabel* _pTotalDurationLabel;

    QTextEdit* _pLogTextEdit;

    // 现有控件（保持原有）
    QComboBox* _pMeasureRangeComBox;
    QComboBox* _pTriggerModeComBox;
    QComboBox* _pMeasureWaveLengthComBox;
    QLabel* _pDevStatusLabel;
    QTextEdit* _pMeasureValueEdit;
    QTextEdit* _pTriggerLevelEdit;
    QPushButton* _pCheckMeasureValueButton;
    QPushButton* _pOpenButton;
    QPushButton* _pRefreshStatusButton;
    QPushButton* _pSetParamButton;

    QList<QString> _MeasureRangeVaulelist;
    QList<QString> _MeasureWaveLengthlist;
    int _i32MeasureRangeStartIndex;

    QTimer m_showDataTimer;
};

#endif // QEnergyDevCtrlWidget_H