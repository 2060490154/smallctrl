#ifndef PLATFORMDATAPROCESSCTRL_H
#define PLATFORMDATAPROCESSCTRL_H

#include <QObject>
#include <QApplication>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include "platformtaskdefine.h"
#include "devconfig.h"
#include "workflowdefine.h"
#include "taskinfo.h"
#include "platformtaskdefine.h"


class QPlatformDataProcessCtrl : public QObject
{
    Q_OBJECT
public:
    explicit QPlatformDataProcessCtrl(QObject *parent = nullptr);

signals:
    void signal_devdata(int ndevtype,int ndevIndex,QVariant data);//必须定义

public slots:

public:
    bool runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);//必须定义和实现的函数

    bool checkDevStatus();//设备是否连接
private:
    void caclEnergyCoeff(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);//计算能量系数

    bool caclAreaData(TaskInfo *pTask,tCommonInfo* pCommonInfo=nullptr);//计算光斑面积

    //二值化算法
    void SauvolaThresh(const cv::Mat& src, cv::Mat& dst, const float k, const cv::Size wndSize);


};

#endif // PLATFORMDATAPROCESSCTRL_H
