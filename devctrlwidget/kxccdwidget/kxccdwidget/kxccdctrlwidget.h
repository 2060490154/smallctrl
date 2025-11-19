#ifndef QKXCCD_WIDGET_H
#define QKXCCD_WIDGET_H

#include "kxccddefine.h"
#include "kxccdctrl.h"
#include "customsliderwidget.h"
#include "searchdevwidget.h"
#include "measurewidget.h"
#include "autosaveimagewidget.h"

class QKxccdctrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QKxccdctrlWidget(CKxccdCtrl* pCtrl,QWidget *parent = 0);
    ~QKxccdctrlWidget();

signals:
    void signal_doubleClick_Image(int nIndex);

public slots:

    void onStopCap();


    void onConnectDev();
    void onDisConnectDev();
    void onSetInContinueMode();//连续触发采集
    void onSetOutContinueMode();
    void onSetRecapMode();
     void onModfiyIPAddress();

    void onShowImage(int nDevIndex,uchar* pbuff,int nwidth,int nhigh);//显示图像

    void onOpenImageFile();//打开图片
    void onSaveAsImageFile();//保存图片

    void onRecapImag();//重传

    //参数设置和查询
    void onGetDevParam();
    void onSaveDevParam();

    //slider相关
    void onUpdateWindowPos(int position);//窗位移动
    void onUpdateWindowWidth(int position);//窗宽移动
    void onUpdateParamvalue(int position);//更新参数

    void onParamSelectChange(int nIndex);//参数选择改变

    //显示位数选择
    void onSet10Bit();
    void onSet12Bit();
    void onSet16Bit();

    //搜索相机
    void onSearchDev();

    void onAutoSave();//设置自动保存

    //图像测量相关
    void onAreaMeasure();
    void onLineMeasure();
    //void onLineMeasure();
    void onChangeLineType(int nType);


public:
    bool checkIPValid();//检查设备Ip是否配置
    void initUI();
    void initDock();
    void showScrollArea(bool bShow);//设置是否显示图像区 默认显示该区域
    QGroupBox* createWorkModeGroupBox();
    QGroupBox* createOperationGroupBox();
    QWidget* createImageProcessGroup();
    QWidget* createSetParamGroup();
    void setButtonICon(QPushButton* pButton,QString sPngName);

    void setWorkModeCheckBoxStatus(QCheckBox* pBox,bool bCheck);
    void setWorkModeCheckBoxEnable(bool benable);
    void showLogInfo(QString slog);
    void showDevStatus();//显示当前设备状态信息

    void updateShowBit(int nBitType);//更新显示位数

    void paintEvent(QPaintEvent *event);//显示图像
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);//滚轮事件
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent* event);//双击事件

    void paintLine();
    void setPointValid(QPoint* tPoint);


    void setDevName(QString sName);

    void setDevPort(int nPortNum);

public:
    //工作模式
    QCheckBox* m_pContinueActiveCheckBox;
    QCheckBox* m_pHardwareSynActiveCheckBox;
    QCheckBox* m_pHardwareSynNativeCheckBox;

    bool m_bStopCap;//暂停是否成功



private:
    QDockWidget* m_pdockWidget;
    QLabel* m_pStatusLabel;

    QScrollArea* m_pScrollArea;//显示滚动条
    QLabel* m_pImageLabel;//显示图片
    QListWidget* m_plogList;
    QTabWidget* m_pTabWidget;



    //参数设置
    QComboBox* m_pParamSelectComBox;//参数名称选择
    QCustomSliderWidget* m_pParamValueSliderWidget;

    QCustomSliderWidget* m_pWindowWidthSliderWidget;//窗宽
    QCustomSliderWidget* m_pWindowPosSliderWidget;//窗位

    //显示位数
    QRadioButton* m_pCheckBox[3];//0-8位 1-12位 2-16位

    QLineEdit* m_pIPAddressEdit;

    //button 按钮
    QPushButton* m_pConnectButton;
    QPushButton* m_pDisConnectButton;
    QPushButton* m_pOpenImageButton;
    QPushButton* m_pSaveImageButton;
    QPushButton* m_pRecapImageButton;
    QPushButton* m_pModifyIPAddressButton;
    QPushButton* m_pGetDevParamButton;
    QPushButton* m_pSaveDevParamButton;
    QPushButton* m_pSearchDevButton;//搜索相机
    QPushButton* m_pMeausreAreaButton;//面积测量按钮
    QPushButton* m_pMeausreLineButton;//测量按钮
    QPushButton* m_pStopCapButton;//暂停捕获
    QPushButton* m_pAutoSaveButton;//设置自动保存


    QLabel* m_pShowGrayValueLabel;


    QVector<QRgb> m_vcolorTable; //生成灰度颜色表
    QImage m_ImageBuff;
    QImage m_ImageTempBuff;
    QPixmap m_Pixmap;


    CKxccdCtrl* m_pKxccdCtrl;

    QSearchDevWidget* m_pSearchDevWidget;//搜索相机窗口


    //窗位 窗宽
    int m_nWindowPos;
    int m_nWindowWidth;


    int m_nRcvFrameCnt;
    QString m_sCurrentGray;//当前鼠标坐标 以及当前的灰度值

    float m_lfScal;
    int m_nOrginalWidth;//图像原始宽度
    int m_nOrigalHeight;//图像原始高度

    //绘图相关
    QPoint m_tStartPoint;//起始点
    QPoint m_tEndPoint;//终止点
    QPoint m_ptDown;//拖动时记录的拖动点
    int    m_nLineType;//线的类型  1->X直线 2->Y直线 3->矩形

    bool m_bTracking;//鼠标单击是否在矩形框内
    bool m_bDrawOrg;
    bool m_bDrawEnd;
    bool m_bPaint;

    QMeasureWidget* m_pMeasureWidget;


    //相机的IP
    QString m_sIPAdd;
    int m_nPort;
};

#endif // QKxccdctrlWidget_H
