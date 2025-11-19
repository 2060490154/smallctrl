#ifndef QREPORTWIDET_H
#define QREPORTWIDET_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QDialog>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QProcess>
#include "dbdataprocess.h"
#include "expinfodefine.h"
#include "platformtaskdefine.h"


typedef struct _reldata
{

    QString sSampleName;
    int nWavelengthType;
    float lfArea;//光斑面积
    float lfPlusWidth;//脉宽
    float lfTargetPlus;//图纸要求脉冲宽度

    float lfFluxThreshold;//损伤阈值
    float lfFluxConvThreshold;//折算后的损伤阈值

    float lfTemp;
    float lfHum;


}st_RelData;


class QReportWidget : public QDialog
{
    Q_OBJECT
public:
    explicit QReportWidget(CDbDataProcess* pDbProcess,QWidget *parent = 0);
    ~QReportWidget();

    void InitUI();

    void setWidgetStyleSheet(QDialog* pWidget);

    void showExpRelInfo(QString sTaskNo);

signals:

public slots:

    void onGernReport();



private:
    QComboBox* _pReportTypeComBox;
    QPushButton* _pOkBtn;
    QPushButton* _pCancleBtn;

    QTableWidget* _pExpRelTablewidget;


    QString _sTaskNo;
    CDbDataProcess* _pDbProcess;

    vector<st_RelData> _TaskRelDatalist;
};

#endif // QReportWidget_H
