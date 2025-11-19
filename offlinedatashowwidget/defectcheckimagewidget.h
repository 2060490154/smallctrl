#ifndef QDEFECTCHECKIMAGEWIDGET_H
#define QDEFECTCHECKIMAGEWIDGET_H

#include <QDialog>
#include "qimagelabel.h"
#include "platformtaskdefine.h"

class QDefectCheckImageWidget : public QDialog
{
    Q_OBJECT
public:
    explicit QDefectCheckImageWidget(QDialog *parent = nullptr);

signals:

public slots:

    void onshowImage(int nMeasureType,int imagetype,QImage img);

public:

    void InitUI();



     QImageLabel* m_preImageLabel;//打之前的图像
     QImageLabel* m_backImageLabel;//打之后的图像





};

#endif // QDEFECTCHECKIMAGEWIDGET_H
