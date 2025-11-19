#ifndef SYSTEMDATAWIDGET_H
#define SYSTEMDATAWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include "dbdataprocess.h"
#include "qcustomplot.h"

typedef struct plotdata
{
    double lfTimeMin;
    double lfTimeMax;
    double lfValueMin;
    double lfValueMax;
    QVector<double> timedata;
    QVector<double> valuedata;
}st_plotdata;

class QSystemDataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSystemDataWidget(CDbDataProcess* pDbProcess,QWidget *parent = nullptr);

signals:

public slots:

    void onSelectData();

    void onWaveTypeSelect(int nIndex);

    void onRefreshData();

public:
    void setPlotUI(QCustomPlot* plot,QString XText,QString YText,QString sTitle);

    void showAreaData();//显示历史光斑面积数据

    void showCoeffData();//显示历史分光比数据

    void showPlusWidthData();//显示历史脉宽数据

    void showPlotData(QCustomPlot* plot,QStringList data,st_plotdata* plotdata);



private:
    CDbDataProcess* _pDbProcess;//数据库操作

    //显示光斑面积和分光比参数
    QCustomPlot* _pAreaPlot;
    QCustomPlot* _pCoeffPlot;
    QCustomPlot* _pPlusWidthPlot;

    QComboBox* _pWaveTypeCombox;

    QPushButton* _pRefreshBtn;

    QLabel* _pInfoLabel;

    st_plotdata _stAreaData;
    st_plotdata _stCoeffData;
    st_plotdata _stpluswidthData;

    int _nWaveType;
};

#endif // SYSTEMDATAWIDGET_H
