#include "devconfigwidget.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QDevConfigWidget::QDevConfigWidget(QDevConfig* pDevConfig,QWidget *parent) : QWidget(parent)
{
    _pDevConfig = pDevConfig;

    initUI();

    connect(_pUpdateDevConfigButton,&QPushButton::clicked,this,&QDevConfigWidget::onUpdateDevConfig);

    showDevInfo();
    showEnergyCoeffInfo();
}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDevConfigWidget::initUI()
{
    QStringList sHorizontalHeader;
    QStringList sVerticalHeader;
    sHorizontalHeader<<"设备类型"<<"设备名"<<"设备IP";

    m_pDevInfoTablewidget = new QTableWidget();

    m_pDevInfoTablewidget->horizontalHeader()->setStretchLastSection(true);//最后一列充满

    m_pDevInfoTablewidget->setColumnCount(sHorizontalHeader.size());

    m_pDevInfoTablewidget->setHorizontalHeaderLabels(sHorizontalHeader);



    sHorizontalHeader.clear();
    sHorizontalHeader<<"能量计编号"<<"能量计名称"<<"校准系数";
    m_pEnergyCoeffTablewdiget = new QTableWidget();

    m_pEnergyCoeffTablewdiget->horizontalHeader()->setStretchLastSection(true);//最后一列充满

    m_pEnergyCoeffTablewdiget->setColumnCount(sHorizontalHeader.size());

    m_pEnergyCoeffTablewdiget->setHorizontalHeaderLabels(sHorizontalHeader);



    _pUpdateDevConfigButton = new QPushButton("更新设备信息");
    _pUpdateDevConfigButton->setMaximumWidth(200);



    QGridLayout*playout = new QGridLayout(this);

    playout->setRowStretch(0,5);
    playout->setRowStretch(1,1);
    playout->setRowStretch(2,1);
    playout->setRowStretch(3,5);

    playout->addWidget(m_pDevInfoTablewidget,0,0,1,3);

    playout->addWidget(m_pEnergyCoeffTablewdiget,0,3,1,3);
    playout->addWidget(_pUpdateDevConfigButton,2,2,1,1);

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDevConfigWidget::showDevInfo()
{
    foreach (tDevInfo devTypeInfo, _pDevConfig->m_tDevInfoList) {

        foreach (tDevItem devItem, devTypeInfo.devItemlist) {

            m_pDevInfoTablewidget->insertRow(m_pDevInfoTablewidget->rowCount());

            m_pDevInfoTablewidget->setItem(m_pDevInfoTablewidget->rowCount()-1,0,new QTableWidgetItem(devTypeInfo.sdevTypeName));
            m_pDevInfoTablewidget->setItem(m_pDevInfoTablewidget->rowCount()-1,1,new QTableWidgetItem(devItem.sDevName));
            m_pDevInfoTablewidget->setItem(m_pDevInfoTablewidget->rowCount()-1,2,new QTableWidgetItem(devItem.sDevIPAddress));
        }
    }

    //设置不可编辑
    for(int i = 0; i < m_pDevInfoTablewidget->rowCount(); i++)
    {
        QTableWidgetItem* pItem = m_pDevInfoTablewidget->item(i,0);
        pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));

        pItem = m_pDevInfoTablewidget->item(i,1);
        pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
    }

    //合并
    int nRowIndex = 0;
    foreach (tDevInfo devTypeInfo, _pDevConfig->m_tDevInfoList)
    {
        if(devTypeInfo.devItemlist.size() > 1)
        {
            m_pDevInfoTablewidget->setSpan(nRowIndex,0,devTypeInfo.devItemlist.size(),1);
        }

        nRowIndex = nRowIndex + devTypeInfo.devItemlist.size();
    }




}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDevConfigWidget::showEnergyCoeffInfo()
{
    foreach (tDevInfo devTypeInfo, _pDevConfig->m_tDevInfoList)
    {

        if(devTypeInfo.devType == M_DEV_ENERGY)
        {
            foreach (tDevItem devItem, devTypeInfo.devItemlist)
            {
                for(tChannelInfo channel:devItem.st_ChannelInfo)
                {
                    m_pEnergyCoeffTablewdiget->insertRow(m_pEnergyCoeffTablewdiget->rowCount());

                    m_pEnergyCoeffTablewdiget->setItem(m_pEnergyCoeffTablewdiget->rowCount()-1,0,new QTableWidgetItem(QString::number(channel.nChannelNo)));
                    m_pEnergyCoeffTablewdiget->setItem(m_pEnergyCoeffTablewdiget->rowCount()-1,1,new QTableWidgetItem(channel.sChannelName));
                    m_pEnergyCoeffTablewdiget->setItem(m_pEnergyCoeffTablewdiget->rowCount()-1,2,new QTableWidgetItem(QString::number(channel.lfCoeff,'g')));
                }

              }

        }


    }

    //设置不可编辑
    for(int i = 0; i < m_pEnergyCoeffTablewdiget->rowCount(); i++)
    {
        QTableWidgetItem* pItem = m_pEnergyCoeffTablewdiget->item(i,0);
        pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));

        pItem = m_pEnergyCoeffTablewdiget->item(i,1);
        pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
    }

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDevConfigWidget::onUpdateDevConfig()
{
    bool bModify = false;
    int nRowIndex = 0;

    for(size_t i = 0; i < _pDevConfig->m_tDevInfoList.size(); i++)
    {
        for(size_t j = 0; j < _pDevConfig->m_tDevInfoList[i].devItemlist.size(); j++)
        {
            //IP信息
            QTableWidgetItem* pItem = m_pDevInfoTablewidget->item(nRowIndex,2);
            QString sIP = pItem->text();
            if(sIP != _pDevConfig->m_tDevInfoList[i].devItemlist[j].sDevIPAddress)
            {
                _pDevConfig->m_tDevInfoList[i].devItemlist[j].sDevIPAddress = sIP;
                bModify = true;
            }
            nRowIndex++;
        }
    }

    for (tDevInfo &devTypeInfo:_pDevConfig->m_tDevInfoList)
    {
        if(devTypeInfo.devType == M_DEV_ENERGY)
        {
            for (tDevItem& devItem:devTypeInfo.devItemlist)
            {
                for(tChannelInfo& channel:devItem.st_ChannelInfo)
                {
                    for(int i = 0; i < m_pEnergyCoeffTablewdiget->rowCount(); i++)
                    {
                        int nIndex = m_pEnergyCoeffTablewdiget->item(i,0)->text().toInt();
                        float lfcoeff = m_pEnergyCoeffTablewdiget->item(i,2)->text().toInt();
                        if(nIndex == channel.nChannelNo)
                        {
                            if(lfcoeff != channel.lfCoeff)
                            {
                                channel.lfCoeff = lfcoeff;
                                bModify = true;
                            }
                        }
                    }

                }

            }
        }

    }


    if(bModify)
    {
        if(!_pDevConfig->saveConfigfile())
        {
            QMessageBox::warning(this,"提示","更新配置文件失败!");
            return;
        }

        if(QMessageBox::warning(this,"提示","更新配置文件成功，将自动重启，请确认无流程正在执行!",QMessageBox::Ok,QMessageBox::No) == QMessageBox::Ok)
        {
            qWarning()<<"配置文件修改，重启程序!";
            qApp->exit(0x5555);
        }
    }
    else
    {
        QMessageBox::warning(this,"提示","设备信息没有变化，无需更新!");
    }

}
