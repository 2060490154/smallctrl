#ifndef QSHUTTERCTRLWIDGET_H
#define QSHUTTERCTRLWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QComboBox>

#include "shutterdevctrl.h"

class QShutterCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QShutterCtrlWidget(QList<QShutterDevCtrl*> pCtrllist, QWidget* parent = nullptr);

signals:

public slots:

private:

};


class QShutterCtrlItemWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit QShutterCtrlItemWidget(QShutterDevCtrl* pCtrl, QWidget* parent = nullptr);

signals:

public slots:

    void onOpenShutter();//打开 不吸合
    void onCloseShutter();//吸合
    void onCheckStatus();//查询
    void onSetLimitData();//设置门限值

    void onSetProcessStatus();//设置流程状态

    // 新增：UI 操作槽
    void onRefreshDevices();
    void onConnectDevice();
    void onDisconnectDevice();

public:
    void InitUI();

    void showStatus();

    void setWidgetStyleSheet(QWidget* pWidget);

public:
    //界面元素
    QLabel* m_pStatus;

    QTextEdit* m_psetPlusCntEdit;

    QPushButton* m_pOpenButton;
    QPushButton* m_pCloseButton;
    QPushButton* m_pRefreshButton;
    QPushButton* m_pSetLimitDataButton;
    QPushButton* m_psetProcessStatusButton;//设置流程模式

    // 新增：设备选择、连接、日志
    QComboBox* m_pDeviceCombo;
    QPushButton* m_pRefreshDevicesButton;
    QPushButton* m_pConnectButton;
    QPushButton* m_pDisconnectButton;
    QTextEdit* m_pLogEdit;

private:
    QShutterDevCtrl* _pShutterCtrl;
    int _nIndex;
};

#endif // QSHUTTERCTRLWIDGET_H