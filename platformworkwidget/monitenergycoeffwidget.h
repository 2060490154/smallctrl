#ifndef MONITENERGYCOEFFWIDGET_H
#define MONITENERGYCOEFFWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QTableWidget>
#include <QGridLayout>
#include <QTextEdit>
#include <QLabel>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include "devconfig.h"
#include "workflowdefine.h"
#include "platformtaskdefine.h"

class QMonitEnergyCoeffWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMonitEnergyCoeffWidget(QWidget *parent = nullptr);

signals:

public slots:

    void onUpdateEnergyData(int nDevType,int nChannelNo,QVariant var);//更新显示数据

public:

    void clearAllData();

private:

    void InitUI();
    void setWidgetStyleSheet(QWidget* pWidget);

    vector<float> _EnergyDevData[2];//1-监视能量计数据 0-主能量计
    float _lfCoeffData;//能量系数


    QTableWidget* _pTableWidget;//显示能量数据

    QTextEdit* _pEnergyCoeffEdit;//能量系数显示



};

#endif // MONITENERGYCOEFFWIDGET_H
