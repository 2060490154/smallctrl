#include "defectcheckimagewidget.h"

QDefectCheckImageWidget::QDefectCheckImageWidget(QDialog *parent) : QDialog(parent)
{
    this->resize(400,500);

    InitUI();

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectCheckImageWidget::InitUI()
{

    m_preImageLabel = new QImageLabel("打前图像");
    m_backImageLabel = new QImageLabel("打后图像");


    QGridLayout* playout = new QGridLayout(this);


    playout->addWidget(m_preImageLabel,0,0);
    playout->addWidget(m_backImageLabel,1,0);

}


/******************************************
* 功能:显示图像
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDefectCheckImageWidget::onshowImage(int nMeasureType,int imagetype,QImage img)
{
    if(nMeasureType == M_MEASURETYPE_ROn1  || nMeasureType == M_MEASURETYPE_1On1)
    {
        if(imagetype == 0x0 && !img.isNull())//打前
        {
            m_preImageLabel->showImage(QPixmap::fromImage(img));
            m_backImageLabel->clearImage();
        }
        else if(imagetype == 0x1 && !img.isNull())//打后
        {
            m_backImageLabel->showImage(QPixmap::fromImage(img));
        }
    }

}
