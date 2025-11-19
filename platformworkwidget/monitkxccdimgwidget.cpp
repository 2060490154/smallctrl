/***************************************************************************
**                                                                        **
**  监视近场CCD图像 用于执行过程中的显示
** 本文件依赖qimagelabel类 见通用widget文件夹
**                                                                        **
****************************************************************************
**创建人：李刚
** 创建时间：2020-03-25
** 修改记录：
**
****************************************************************************/
#include "monitkxccdimgwidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

QMonitKxccdImgWidget::QMonitKxccdImgWidget(QWidget *parent) : QWidget(parent)
{
    InitUI();

    setWidgetStyleSheet(this);
}

/******************************************
* 功能:初始化显示
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMonitKxccdImgWidget::InitUI()
{
    for(int i = 0; i < M_IMAGE_CNT; i++)
    {
        m_pImageLabel[i] = new QImageLabel("");
    }

    QGridLayout* playout = new QGridLayout(this);

    for(int i = 0 ; i < M_IMAGE_CNT; i++)
    {
        playout->addWidget(m_pImageLabel[i],i/2,i%2);
    }
}

/******************************************
* 功能:清除所有显示
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMonitKxccdImgWidget::clearAllImage()
{
    for(int i = 0; i < M_IMAGE_CNT; i++)
    {
        m_pImageLabel[i]->m_pImagelabel->clear();
        if(m_pImageLabel[i]->m_pImageExpandWidget != nullptr)
        {
            m_pImageLabel[i]->m_pImageExpandWidget->close();
            delete m_pImageLabel[i]->m_pImageExpandWidget;
            m_pImageLabel[i]->m_pImageExpandWidget = nullptr;
        }
    }

    for (int i = 0; i < 256; i++)
    {
        m_vcolorTable.append(qRgb(i, i, i));
    }

}
/******************************************
* 功能:显示图片
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMonitKxccdImgWidget::onUpdateImage(int devType,int ndevIndex,QVariant var)
{
    if(devType == M_DEV_NEARFIELD_CCD || (ndevIndex == E_MEASUREPARAM_AREA && devType == M_DEV_DATA_PROCESS))
    {
        st_ccdimage l_st_ccd_image = var.value<st_ccdimage>();

        QImage l_imageBuff(l_st_ccd_image.sImageFilePath);  //封装QImage
    //    l_imageBuff.setColorTable(m_vcolorTable); //设置颜色表
    //    l_imageBuff.mirrored(false,true);

        QPixmap TempPixmap = QPixmap::fromImage(l_imageBuff);

        if(l_st_ccd_image.i32ImageIndex < M_IMAGE_CNT)
        {
            m_pImageLabel[l_st_ccd_image.i32ImageIndex]->showImage(TempPixmap);
        }

    }

}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMonitKxccdImgWidget::setWidgetStyleSheet(QWidget* pWidget)
{
    QPalette pal(pWidget->palette());
    pal.setColor(QPalette::Background, QColor(255,255,255));
    pWidget->setAutoFillBackground(true);
    pWidget->setPalette(pal);

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);// 阴影偏移
    shadowEffect->setColor(QColor(180,180,180));// 阴影颜色;
    shadowEffect->setBlurRadius(10);// 阴影半径;
    pWidget->setGraphicsEffect(shadowEffect);// 给窗口设置上当前的阴影效果;

}
