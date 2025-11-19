#ifndef OFFLINEDATAWIDGET_H
#define OFFLINEDATAWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QTabWidget>
#include <QGridLayout>
#include "systemdatawidget.h"
#include "measureareadatawidget.h"
#include "defectdatawidget.h"

class QOfflineDataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QOfflineDataWidget(CDbDataProcess* pdb, QWidget *parent = nullptr);

signals:

public slots:

public:

    void InitUI();



private:
    CDbDataProcess* _pDbProcess;//数据库操作

    QTabWidget* _pTabwidget;


    QSystemDataWidget* _pSystemDataWidget;

};

#endif // OFFLINEDATAWIDGET_H
