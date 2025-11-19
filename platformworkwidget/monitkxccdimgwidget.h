#ifndef QMONITKXCCDIMGWIDGET_H
#define QMONITKXCCDIMGWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include "qimagelabel.h"
#include "platformtaskdefine.h"
#include "devconfig.h"

#define M_IMAGE_CNT 10


class QMonitKxccdImgWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMonitKxccdImgWidget(QWidget *parent = nullptr);

signals:

public slots:

    void onUpdateImage(int devType,int ndevIndex,QVariant var);//显示图片信息


public:

    void clearAllImage();//清除所有的显示

    QImageLabel* m_pImageLabel[M_IMAGE_CNT];//最多显示10张图片

    QVector<QRgb> m_vcolorTable; //生成颜色表

private:

    void InitUI();

    void setWidgetStyleSheet(QWidget* pWidget);

};

#endif // QMONITKXCCDIMGWIDGET_H
