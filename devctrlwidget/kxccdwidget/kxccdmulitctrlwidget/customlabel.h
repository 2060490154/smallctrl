#ifndef QCUSTOMLABEL_H
#define QCUSTOMLABEL_H
#include <qlabel.h>

class QCustomLabel:public QLabel
{
    Q_OBJECT
public:
    explicit  QCustomLabel(QString sTitle,int nIndex);
    ~QCustomLabel();

    void setClickedStyle();//选中样式
    void clearStyle();//清除样式

    void clearFrameCnt();//清除显示的帧计数


    void showImage(QImage imgeBuffer);

    void setLabelTitle(QString sTitle);

     //void paintEvent(QPaintEvent *event);//显示图像

    //帧计数
    int m_nFrameCnt;

signals:
    void clicked(int nIndex);
    void doubleclicked(int nIndex);
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

protected:
    int m_nIndex;//当前索引
    QString m_sLabelTitle;
    QPixmap m_pixMap;
};

#endif // QCUSTOMLABEL_H
