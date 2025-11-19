#include "logwidget.h"
#include <QGridLayout>
#include <QStyleOption>
#include <QPainter>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QLogWidget::QLogWidget(QWidget *parent)
{
    QStringList l_headerInfo;
    l_headerInfo<<"时间"<<"信息";

    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, QColor(255,255,255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);// 阴影偏移
    shadowEffect->setColor(QColor(180,180,180));// 阴影颜色;
    shadowEffect->setBlurRadius(10);// 阴影半径;
    this->setGraphicsEffect(shadowEffect);// 给窗口设置上当前的阴影效果;

    this->verticalHeader()->setHidden(true);//垂直表头
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    this->horizontalHeader()->setStretchLastSection(true);//最后一列充满
    this->setColumnCount(l_headerInfo.size());
    this->setHorizontalHeaderLabels(l_headerInfo);
    this->setColumnWidth(0,200);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QLogWidget::onShowlog(QString slog,bool bWarning)
{
    QTime time = QTime::currentTime();

    int nRowIndex = this->rowCount();

    this->insertRow(nRowIndex);

    QTableWidgetItem* pTimeItem =  new QTableWidgetItem(time.toString("hh:mm:ss"));
    QTableWidgetItem* plogItem =  new QTableWidgetItem(slog);
    if(bWarning)
    {
       plogItem->setTextColor(QColor(255,0,0));
    }

    this->setItem(nRowIndex,0,pTimeItem);
    this->setItem(nRowIndex,1,plogItem);

    this->scrollToBottom();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QLogWidget::clear()
{
    this->clearContents();
    this->setRowCount(0);
}
