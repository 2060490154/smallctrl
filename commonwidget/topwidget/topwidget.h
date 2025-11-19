#ifndef QTopWidget_H
#define QTopWidget_H

#include <QWidget>
#include <QLabel>
#include <qpushbutton.h>
#include <QFile>

class QTopWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QTopWidget(QString sAppName,QString sIcon="",QWidget *parent = 0);

 public:

    void paintEvent(QPaintEvent *);


signals:
	void closeWindow();//关闭窗口事件

public slots:
    void onMinButtonclicked();
    void onMaxButtonclicked();
    void onCloseButtonclicked();

public:
    QLabel* m_pIconLabel;
    QLabel* m_pTitleLabel;
    QLabel* m_pStatusLabel;
	QLabel*m_pVersionLabel;

    bool m_bMax;


    QPushButton* m_pMinButton;
    QPushButton* m_pMaxButton;
    QPushButton* m_pCloseButton;

    QWidget* m_pParentWidget;

};

#endif // QTopWidget_H
