#ifndef QPOINTAREAWIDGET_H
#define QPOINTAREAWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QLabel>
#include "qcustomplot.h"



const int M_SIZETYPE_RECT  = 1;//矩形
const int M_SIZETYPE_CIRCL  = 2;//圆形

typedef struct  _QPointInfo
{
    float xPos;
    float yPos;
    int nStatus;//0--未打 1--正在  2--失败 3-成功 4-有损伤

}QPointInfo;

enum pointStatus {

    M_STATUS_UNUSE = 0,//未打过
    M_STATUS_RUNNING = 1,//正在打
    M_STATUS_FAILED =2,//执行失败或有损伤
    M_STATUS_UNDEFECT =3,//无损伤

};


class QPointAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QPointAreaWidget(float lfMaxWidth,float lfMaxheight,int nType,float lfStep,QWidget *parent = 0);

    QList<QPointInfo>* getPointlist();
signals:


public slots:

    void onUpdatePointUI(int nSizeType,float lfStep,float lfMaxWidth,float lfMaxHeigh,float dRadius=0.0);

    void onUpdatePointStatus();



public:

    float m_lfMaxHigh;
    float m_lfMaxWidth;

    float m_lfRadius;

    int m_nSizeType;//尺寸类型
    float m_lfStep;//步长

    int m_nValidPointNum;//实际使用的点个数


private:

    void InitUI();

    void showPointInfo();

    void showPoint();

    void showSizePoint();//画物体的外边框

    void setWidgetStyleSheet(QWidget* pWidget);


    QCustomPlot* _pCustomPlot;
    QLabel* _pPointInfoLabel;

    QTimer* _pUpdatePointTimer;

    QList<QPointInfo> _tPointList;
};

#endif // QPOINTAREAWIDGET_H
