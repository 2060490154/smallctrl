#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QHash>
#include <QTimer>
#include "devctrlmanager.h"
#include "platformmotordevctrl.h"
#include "platformkxccdctrl.h"
#include "platformoscictrl.h"
#include "platformshutterctrl.h"
#include "platformenergydevctrl.h"
#include "platformenergyadjdevctrl.h"
#include "platformdefectcheckctrl.h"
#include "platformdataprocessctrl.h"


#include "topwidget.h"
#include "qnavwidget.h"
#include "mulitctrlwidget.h"
#include "oscictrlwidget.h"
#include "shutterctrlwidget.h"
#include "energydevctrlwidget.h"
#include "mulitctrlwidget.h"
#include "motorctrlwidget.h"
#include "energyadjdevwidget.h"

#include "defectMeasurewidget.h"
#include "lightparamwidget.h"
#include "offlinedatawidget.h"
#include "measureprereadywidget.h"


#include "devconfigwidget.h"

#include "dbdataprocess.h"

#include "devconfig.h"
#include "ishutterbackend.h"
#include "platformconfig.h"
using namespace std;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    void InitDevCtrl();//初始化控制类

    void InitDevCtrlWidget();//初始化设备控制界面

    void InitUI();//初始化界面

    void releaseDevCtrl();//释放所有的控制类

    void closeEvent(QCloseEvent *event);

public:
    QDevCtrlManager* m_pDevCtrlManager;

public slots:
    void checkDevStatus();//检查设备状态



private:
    Ui::MainWindow *ui;


private:

    QTopWidget* _pTopWidget;//标题栏
    QNavWidget* _pNavWidget;//侧边导航栏

    //子窗口
    vector<QWidget*> m_measureDevCtrlWidgetList;//测量设备

    vector<QWidget*> m_moveDevCtrlWidgetlist;//运动设备

    QDevConfigWidget* _pDevConfigWidget;//设备信息维护界面




    MeasurePreReadyWidget* _pMeasurePreWidget;//测试准备界面
    DefectMeasureWidget* _pDefectMeasureWidget;//流程界面
    LightParamWidget* _pLightParamWidget;//光路参数测量界面


    CDbDataProcess* _pDBProcess;

    QDevConfig* _pPlatformDevConfig;//设备控制信息

    QPlatformConfig* _pPlatformConfig;//平台配置信息


    //延迟检查

    QTimer _checkStatusTime;






};

#endif // MAINWINDOW_H
