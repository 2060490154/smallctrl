#ifndef PLATFORMDEVCONFIGWIDGET_H
#define PLATFORMDEVCONFIGWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>
#include "devconfig.h"

class QDevConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDevConfigWidget(QDevConfig* pDevConfig,QWidget *parent = nullptr);

signals:

public slots:

    void onUpdateDevConfig();

public:

    void initUI();

    void showDevInfo();

    void showEnergyCoeffInfo();




    QTableWidget* m_pDevInfoTablewidget;//设备参数

    QTableWidget* m_pEnergyCoeffTablewdiget;//能量计系数维护

private:
    QDevConfig* _pDevConfig;

    QPushButton* _pUpdateDevConfigButton;
};

#endif // PLATFORMDEVCONFIGWIDGET_H
