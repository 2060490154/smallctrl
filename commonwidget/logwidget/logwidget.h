#ifndef QLOGWIDGET_H
#define QLOGWIDGET_H

#include <QTableWidget>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QTime>


class QLogWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit QLogWidget(QWidget *parent = 0);


signals:


public slots:
   void onShowlog(QString str,bool bWarning=false);

public:

   void clear();


};

#endif // QLOGWIDGET_H
