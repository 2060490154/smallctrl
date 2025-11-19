#ifndef BASICWORKFLOWWIDGET_H
#define BASICWORKFLOWWIDGET_H

#include <QWidget>

class QBasicWorkflowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QBasicWorkflowWidget(QWidget *parent = nullptr);

signals:

public slots:

public:

    virtual bool checkRunCondition(QString sworkflowName)=0;//检测运行的前提条件是否满足
};

#endif // BASICWORKFLOWWIDGET_H
