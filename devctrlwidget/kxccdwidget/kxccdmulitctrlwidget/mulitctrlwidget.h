#ifndef QMulitCtrlWidget_H
#define QMulitCtrlWidget_H

#include <QWidget>
#include <qlist.h>
#include <QStackedWidget>
#include <QDockWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <qlineedit.h>
#include "customlabel.h"
#include "kxccdctrlwidget.h"
#include "kxccdctrl.h"

#define M_BASE_PORT 4110
using namespace std;



class QMulitCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMulitCtrlWidget(vector<CKxccdCtrl*> pKxccdCtrllist,QWidget *parent = 0);
    ~QMulitCtrlWidget();

signals:

public slots:
    void onSelectDev(int nIndex);
    void onDoubleClickDevlabel(int nIndex);
    void onDoubleClickImge(int nIndex);
    void onShowDevImage(int nDevIndex,uchar* pBuff,int nWidth,int nHigh);



public:
    void InitUI();



public:
    //界面元素相关
    QList<QCustomLabel*>m_tCustonLabelArray;

    QWidget* m_pCCDDevLabelWidget;
    QWidget* m_pMainWidget;

    QVector<QRgb> m_vcolorTable; //生成灰度颜色表


    int m_nDevNum;//配置参数的设备数量






public:
    // 数据相关
    CKxccdCtrl*m_pCurrentDevCtrl;//当前选中的相机设备
    vector<CKxccdCtrl*> m_tDevCtrllist;//所有的相机设备
    QList<QKxccdctrlWidget*> m_tDevCtrlWidgetlist;//所有的相机设备









};

#endif // QMulitCtrlWidget_H
