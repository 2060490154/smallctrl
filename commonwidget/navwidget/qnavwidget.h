#ifndef QNAVWIDGET_H
#define QNAVWIDGET_H

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

#include <QWidget>
#include <qlistwidget.h>
#include <qstackedwidget.h>
#include <QMessageBox>
using namespace std;


struct Item
{
    int nItemCnt;//分组总个数
    int nItemIndex;//分组索引
    bool bExpand;
};


typedef struct _groupInfo
{
    QString sGroupName;
    QString sGroupICon;


    vector<QString> tItemWidgetName;
    vector<QString> tItemWIdgetIcon;
    vector<QWidget*> tItemWidget;

}tGroupInfo;


class QNavWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QNavWidget(QWidget *parent = 0);

signals:

public slots:
    void onExpandItem();
    void onCheckListItem();

public:

    int addGroup(QString sGroupName,QString sICon="");//添加组信息

    bool addItemWidget(int nGroupID,QString sItemName,QWidget* pwidget,QString sIcon="");

private:

    void updateStackedWidget();

    void updateNavList();

    void InitUI();
    void showNavListItem();
    void selectItem(int nIndex);

public:
        QListWidget* m_pNavList;//导航栏

        QList<struct Item> m_tItemList;//导航分组

        QStackedWidget* m_pUserViews;//视图切换

        vector<tGroupInfo> m_tGroupInfoList;
};

#endif // QNAVWIDGET_H
