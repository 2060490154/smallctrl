/***************************************************************************
**                                                                        **
** 文件描述：本文件定义了导航栏的操作功能
**                                                                        **
****************************************************************************
** 创建人：李刚
** 创建时间：2018-05-16
** 修改记录：
**
****************************************************************************/
#include "qnavwidget.h"
#include <QGridLayout>
#include <qfile.h>
#include <QDebug>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif



QNavWidget::QNavWidget(QWidget *parent) : QWidget(parent)
{


    InitUI();

    //应用样式
    QFile file(":navres/navui.qss");
    file.open(QFile::ReadOnly);
    QString stylesheet = file.readAll();


    this->setStyleSheet(stylesheet);

}
/*******************************************************************
**功能：
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::InitUI()
{

    m_pUserViews = new QStackedWidget(this);

    m_pNavList = new QListWidget(this);
    m_pNavList->setObjectName("NavList");//设置ID

    connect(m_pNavList, &QListWidget::itemDoubleClicked, this, &QNavWidget::onExpandItem);//展开或收缩
    connect(m_pNavList, &QListWidget::itemClicked, this, &QNavWidget::onCheckListItem);//展开或收缩

    //导航栏设置
    m_pNavList->setFocusPolicy(Qt::NoFocus);


    //布局
    QGridLayout* pGridlayout = new QGridLayout();

    pGridlayout->setColumnStretch(0, 1);
    pGridlayout->setColumnStretch(1, 4);


    QWidget* l_space = new QWidget(this);
    l_space->setObjectName("SpaceWidget");
    l_space->setMinimumHeight(30);

    pGridlayout->addWidget(l_space, 0, 0, 1, 1);
    pGridlayout->addWidget(m_pNavList, 1, 0, 1, 1);
    pGridlayout->addWidget(m_pUserViews, 0, 1, 2, 1);
    pGridlayout->setSpacing(0);
    pGridlayout->setMargin(0);

    this->setLayout(pGridlayout);
}


void QNavWidget::showNavListItem()
{
    for (int i = 0; i < m_tItemList.size(); i++)
    {
        for (int j = m_tItemList[i].nItemIndex + 1; j < m_tItemList[i].nItemIndex + 1 + m_tItemList[i].nItemCnt; j++)
        {
            m_pNavList->item(j)->setHidden(!m_tItemList[i].bExpand);
        }

    }
}

/*******************************************************************
**功能：导航栏选择
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::onExpandItem()
{

    int nRowIndex = m_pNavList->currentRow();

    for (int i = 0; i < m_tItemList.size(); i++)
    {
        if (nRowIndex == m_tItemList[i].nItemIndex)
        {
            m_tItemList[i].bExpand = !m_tItemList[i].bExpand;
        }
    }

    showNavListItem();

}

/*******************************************************************
**功能：单击事件
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::onCheckListItem()
{
    int nRowIndex = m_pNavList->currentRow();

    int nIndex = -1;
    foreach (tGroupInfo groupItem, m_tGroupInfoList)
    {
        nIndex++;
        for(size_t i = 0; i < groupItem.tItemWidget.size(); i++ )
        {
            QWidget* pWidget = groupItem.tItemWidget[i];
            nIndex++;
            if(nRowIndex == nIndex)
            {
                m_pUserViews->setCurrentWidget(pWidget);
            }
        }

    }


}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:组ID
* 维护记录:
******************************************/
int  QNavWidget::addGroup(QString sGroupName,QString sICon)
{
    tGroupInfo l_groupInfo;

    l_groupInfo.sGroupName = sGroupName;
    l_groupInfo.sGroupICon = sICon;

    m_tGroupInfoList.push_back(l_groupInfo);

    return m_tGroupInfoList.size()-1;

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QNavWidget::addItemWidget(int nGroupID,QString sItemName,QWidget* pwidget,QString sIcon)
{
    if(static_cast<size_t>(nGroupID) >= m_tGroupInfoList.size())
    {
        return false;
    }

    m_tGroupInfoList[nGroupID].tItemWidgetName.push_back(sItemName);
    m_tGroupInfoList[nGroupID].tItemWidget.push_back(pwidget);
    m_tGroupInfoList[nGroupID].tItemWIdgetIcon.push_back(sIcon);

    updateStackedWidget();
    updateNavList();
    selectItem(1);


    if(m_tItemList.size() >= 1)//default expand 1st group
    {
        m_tItemList[0].bExpand = true;
    }


    showNavListItem();

    return true;

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QNavWidget::updateStackedWidget()
{
    //清除
    int nIndex = m_pUserViews->count()-1;
    while(nIndex >= 0)
    {
        m_pUserViews->removeWidget(m_pUserViews->widget(nIndex));
        nIndex--;
    }


    //重新添加
    foreach (tGroupInfo groupItem, m_tGroupInfoList)
    {
        foreach (QWidget* pWidget, groupItem.tItemWidget)
        {
            m_pUserViews->addWidget(pWidget);
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
void QNavWidget::updateNavList()
{
    m_pNavList->clear();
    m_tItemList.clear();

    QFont font = m_pNavList->font();




    foreach (tGroupInfo groupItem, m_tGroupInfoList)
    {


        font.setBold(true);
        font.setPixelSize(22);
        int nRowCnt = m_pNavList->count();
        m_pNavList->addItem(groupItem.sGroupName);

        m_pNavList->item(nRowCnt)->setSizeHint(QSize(60, 60));
        m_pNavList->item(nRowCnt)->setFont(font);
        if(groupItem.sGroupICon != "")
        {
            m_pNavList->item(nRowCnt)->setIcon(QIcon(groupItem.sGroupICon));
        }

        struct Item Temp;
        Temp.bExpand = false;
        Temp.nItemIndex = nRowCnt;
        Temp.nItemCnt = groupItem.tItemWidgetName.size();
        m_tItemList.append(Temp);//添加到分组信息中

        for(size_t i = 0; i < groupItem.tItemWidgetName.size(); i++ )
        {
            font.setPixelSize(16);
            font.setBold(false);
            m_pNavList->addItem(groupItem.tItemWidgetName[i]);
            m_pNavList->item(nRowCnt+i+1)->setFont(font);
            //m_pNavList->item(nRowCnt+i+1)->setTextAlignment(Qt::AlignCenter);
            if(groupItem.tItemWIdgetIcon[i] != "")
            {
                m_pNavList->item(nRowCnt+i+1)->setIcon(QIcon(groupItem.tItemWIdgetIcon[i]));
            }
        }
    }
}

/*******************************************************************
**功能：新增分组
**输入：sGroupName分组 SubItemlist子项 iconlist
**输出：
**返回值：
*******************************************************************/
//void QNavWidget::SetNavItem(QString sGroupName, QStringList SubItemlist,QStringList iconlist, QString sIcon)
//{


//	int nRowCnt = m_pNavList->count();
//    m_pNavList->addItem(sGroupName);
//    m_pNavList->item(nRowCnt)->setSizeHint(QSize(60, 80));
//    m_pNavList->item(nRowCnt)->setFont(font);
//    if(sIcon != "")
//    {
//        m_pNavList->item(nRowCnt)->setIcon(QIcon(sIcon));

//    }


//	struct Item Temp;
//	Temp.bExpand = true;
//	Temp.nItemIndex = nRowCnt;
//	Temp.nItemCnt = SubItemlist.size();
//	m_tItemList.append(Temp);//添加到分组信息中


//	for (int i = 0; i < SubItemlist.size(); i++)
//    {
//        font.setPixelSize(18);
//        font.setBold(false);
//        m_pNavList->addItem(SubItemlist[i]);
//        m_pNavList->item(nRowCnt+i+1)->setFont(font);
//        if(i < iconlist.size())
//        {
//            m_pNavList->item(nRowCnt+i+1)->setIcon(QIcon(iconlist.at(i)));
//        }
//	}


//}




/*******************************************************************
**功能：切换视图
**输入：
**输出：
**返回值：
*******************************************************************/
void QNavWidget::selectItem(int nIndex)
{
    m_pNavList->setCurrentRow(nIndex);
    onCheckListItem();
}
