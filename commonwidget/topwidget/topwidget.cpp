#include "topwidget.h"
#include <QGridLayout>
#include <QStyleOption>
#include <QPainter>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QTopWidget::QTopWidget(QString sAppName,QString sIcon,QWidget *parent) : QWidget(parent)
{

    this->setObjectName("topwidget");
    this->setMaximumHeight(60);

    m_pParentWidget = parent;

    this->setObjectName("topwidget");

    //软件名称显示
    QString sName = QString("<div><span style = 'font-weight:bold;font-family:Microsoft YaHei;font-size:28px; color:#ffffff;'>%2</span></div>").arg(sAppName);
    m_pTitleLabel = new QLabel(sName, this);
    m_pTitleLabel->setAlignment(Qt::AlignLeft);

    m_pIconLabel = new QLabel(this);
    QImage resultImg;
    resultImg.load(sIcon);
    resultImg = resultImg.scaled(m_pIconLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    m_pIconLabel->setPixmap(QPixmap::fromImage(resultImg));
    m_pIconLabel->setMaximumWidth(50);


    QWidget* pwidget1 = new QWidget();
    QGridLayout* playout1 = new QGridLayout(pwidget1);
    playout1->setContentsMargins(0, 0, 0, 0);

    playout1->setColumnStretch(0,1);
    playout1->setColumnStretch(1,40);

    playout1->addWidget(m_pIconLabel,0,0,1,1,Qt::AlignLeft);
    playout1->addWidget(m_pTitleLabel,0,1,1,1,Qt::AlignLeft);






    //最大化  最小化 关闭按钮
    m_pMinButton = new QPushButton();
    m_pMaxButton = new QPushButton();
    m_pCloseButton = new QPushButton();

    m_pMinButton->setIcon(QIcon(":res/titlemini.png"));
    m_pMinButton->setMinimumHeight(30);
    m_pMinButton->setMaximumWidth(30);
    m_pMinButton->setToolTip("最小化");

    m_pMaxButton->setIcon(QIcon(":res/titlemax.png"));
    m_pMaxButton->setMinimumHeight(30);
    m_pMaxButton->setMaximumWidth(30);
    m_pMaxButton->setToolTip("最大化");

    m_pCloseButton->setIcon(QIcon(":res/titleclose.png"));
    m_pCloseButton->setMinimumHeight(30);
    m_pCloseButton->setMaximumWidth(30);
    m_pCloseButton->setToolTip("关闭");






    QGridLayout* playout = new QGridLayout(this);


    playout->addWidget(pwidget1,0,0,1,1);
    playout->addWidget(m_pMinButton,0,1,1,1);
    playout->addWidget(m_pMaxButton,0,2,1,1);
    playout->addWidget(m_pCloseButton,0,3,1,1);

    this->setMinimumHeight(50);


    //绑定事件
    connect(m_pMinButton,&QPushButton::clicked,this,&QTopWidget::onMinButtonclicked);
    connect(m_pCloseButton,&QPushButton::clicked,this,&QTopWidget::onCloseButtonclicked);


    //应用样式
    QFile file(":res/topwidgetui.qss");
    file.open(QFile::ReadOnly);
    QString stylesheet = file.readAll();

    this->setStyleSheet(stylesheet);

}


/******************************************
* 功能:最小化功能
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QTopWidget::onMinButtonclicked()
{
    m_pParentWidget->showMinimized();
}
/******************************************
* 功能:关闭功能
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QTopWidget::onCloseButtonclicked()
{
    if(m_pParentWidget != nullptr)
    {
        m_pParentWidget->close();
    }

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QTopWidget::onMaxButtonclicked()
{
    m_pParentWidget->showMaximized();
}


void QTopWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
