#ifndef QSEARCHDEVWIDGET_H
#define QSEARCHDEVWIDGET_H


#include <QWidget>
#include <QPushButton>
#include <qlistwidget.h>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include "kxccddefine.h"
#include "kxccdctrl.h"


class QSearchDevWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSearchDevWidget(CKxccdCtrl* pKxccdCtrl,QWidget *parent = nullptr);

signals:

public:
    void closeEvent(QCloseEvent *event);

public slots:
    void onSearchTimer();

    void onRefreshFun();

    void onCloseWindow();


private:
    QListWidget* m_pListWidget;
    QPushButton* m_pRefreshButton;//刷新按钮
    QPushButton* m_pCloseButton;

    QLabel* m_pshowInfoLabel;

    QTimer* m_pSearchTimer;

    int m_nSearchTime;


private:
    CKxccdCtrl* _pKxccdCtrl;
};

#endif // QSEARCHDEVWIDGET_H
