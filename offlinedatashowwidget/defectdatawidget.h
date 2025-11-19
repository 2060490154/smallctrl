#ifndef DEFECTDATAWIDGET_H
#define DEFECTDATAWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <QGridLayout>
#include <qpushbutton.h>
#include <QHeaderView>
#include <QDateEdit>
#include <qtablewidget.h>
#include <QTabWidget>
#include <qmath.h>
#include <QHash>
#include <QVector>
#include <QMessageBox>
#include "qimagelabel.h"
#include "dbdataprocess.h"
#include "platformtaskdefine.h"
#include "reslutprocesswidget.h"
#include "reportwidget.h"

using namespace std;


class QDefectDataWidget : public QWidget
{
        Q_OBJECT
public:
    QDefectDataWidget(CDbDataProcess* pDbProcess);
    ~QDefectDataWidget();


signals:

public slots:


    void onWaveTypeSelect(int nIndex);

    void onQueryByTaskNo();

    void ondoubleClickExplist(QModelIndex index);

    void onComboxSelectChange(int nIndex);

    void onDateChange(QDate date);

    void onShowExpNolist();

    void onShowMenu(QPoint pos);

    void onMulitDataProcess();

    void onGerenReport();



public:
    void InitUI();









private:

    //界面相关
    QPushButton* _pRefreshDataBtn;//刷新数据按钮

    QComboBox* _pWavelenTypeCombox;//波长选择
    QComboBox* _pMeasureTypeCombox;//测试类型选择

    CDbDataProcess* _pDbProcess;
    //日期选择
    QDateTimeEdit* _pStartDateEdit;
    QDateTimeEdit* _pEndDateEdit;

    QTableWidget* _pExpNolistWidget;//实验信息列表

    QMenu m_mulitProcessMenu;

    QMeasureRelProcessWidget* _pMeasureRelProcessWidget;

    QTextEdit* _pExpTaskEdit;//实验任务编号
    QPushButton* _pExpTaskRefreshBtn;

    QPushButton* _pReportBtn;//生成报告按钮

    QReportWidget* _pReportWidget;

};

#endif // DEFECTDATAWIDGET_H
