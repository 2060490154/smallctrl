#ifndef BASICWIDGET_H
#define BASICWIDGET_H

#include <QWidget>

class QBasicWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QBasicWidget(QWidget *parent = nullptr);

signals:

public slots:

public:
    QString m_sDevTypeName;
};

#endif // BASICWIDGET_H
