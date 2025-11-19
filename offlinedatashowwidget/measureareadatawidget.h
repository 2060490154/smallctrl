#ifndef MEASUREAREADATAWIDGET_H
#define MEASUREAREADATAWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <QGridLayout>
#include <qpushbutton.h>
#include <QHeaderView>
#include "qimagelabel.h"
#include "dbdataprocess.h"


class QMeasureAreaDataWidget : public QWidget
{
        Q_OBJECT
public:
    QMeasureAreaDataWidget(CDbDataProcess* pDbProcess);


signals:

public slots:


    void onWaveTypeSelect(int nIndex);

    void ondoubleClickExplist(QModelIndex index);

    void onExportImageTofolder();

    void onRefreshData();

public:
    void InitUI();

    void showExpNolist();




private:

    //界面相关
    QComboBox* _pWavelenTypeCombox;//光路选择
    QTableWidget* _pExpNolistWidget;//实验信息列表
    QLabel* _pAreaDataLabel;//光斑面积显示

    QImageLabel* _pImageLabel[10];//图像数据显示

    QPushButton* _pExportImageDataBtn;//导出按钮
    QPushButton* _pRefreshDataBtn;//刷新数据按钮

    QString _sExpNo;







    CDbDataProcess* _pDbProcess;
};

#endif // MEASUREAREADATAWIDGET_H
