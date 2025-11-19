#include "customlabel.h"
#include <QPainter>
#include <QFont>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QCustomLabel::QCustomLabel(QString sTitle,int nIndex)
{
    this->setAlignment(Qt::AlignCenter);


    m_nFrameCnt = 0;
    m_nIndex = nIndex;
    this->setText(sTitle);
    clearStyle();

    m_sLabelTitle = sTitle;
}

QCustomLabel::~QCustomLabel()
{

}

void QCustomLabel::mousePressEvent(QMouseEvent* event)
{
    setClickedStyle();
    emit clicked(m_nIndex);
}

void QCustomLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleclicked(m_nIndex);
}


void QCustomLabel::setClickedStyle()//选中样式
{
    this->setStyleSheet("border: 2px solid #33FF00;");

}

void QCustomLabel::clearStyle()//清除样式
{
    this->setStyleSheet("border: 2px solid #DCDCDC;");
}

void QCustomLabel::showImage(QImage imgeBuffer)
{
    QString sText;
    QSize tSize = this->size();
    this->setFixedSize(tSize);

    sText = QString("%1 Cnt:%2").arg(m_sLabelTitle).arg(m_nFrameCnt);

    m_pixMap = QPixmap::fromImage(imgeBuffer);
    QPixmap pixMap = m_pixMap.scaled(tSize, Qt::KeepAspectRatio);


    //显示文字
    QPen pen;
    QPainter painter(&pixMap);
    QFont font;
    font.setFamily("宋体");
    font.setPointSize(12);
    painter.setFont(font);

    pen.setColor(QColor(170,170,170));
    painter.setPen(pen); //添加画笔



    painter.drawText(pixMap.rect(),Qt::AlignHCenter|Qt::AlignBottom,sText);
    this->setPixmap(pixMap);

}

void QCustomLabel::setLabelTitle(QString sTitle)
{
    this->setText(sTitle);
    m_sLabelTitle = sTitle;
}

void QCustomLabel::clearFrameCnt()
{
    QString sText;
    QSize tSize = this->size();
    this->setFixedSize(tSize);

    if(m_pixMap.isNull())
    {
        return;
    }

    QPixmap pixMap = m_pixMap.scaled(tSize, Qt::KeepAspectRatio);;

    m_nFrameCnt = 0;

    sText = QString("%1 Cnt:%2").arg(m_sLabelTitle).arg(m_nFrameCnt);


    //显示文字
    QPen pen;
    QPainter painter(&pixMap);
    QFont font;
    font.setFamily("宋体");
    font.setPointSize(12);
    painter.setFont(font);

    pen.setColor(QColor(170,170,170));
    painter.setPen(pen); //添加画笔



    painter.drawText(pixMap.rect(),Qt::AlignHCenter|Qt::AlignBottom,sText);
    this->setPixmap(pixMap);

}
