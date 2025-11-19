#include "devconfig.h"

QDevConfig::QDevConfig(QObject *parent) : QObject(parent)
{
    _mapNodeNameToDevType.insert("KxCCD",M_DEV_NEARFIELD_CCD);
    _mapNodeNameToDevType.insert("ZzCCD",M_DEV_FARFIELD_CCD);
    _mapNodeNameToDevType.insert("OSCII",M_DEV_OSCII);
    _mapNodeNameToDevType.insert("SHUTTER",M_DEV_SHUTTER);
    _mapNodeNameToDevType.insert("ENERGYMETER",M_DEV_ENERGY);
    _mapNodeNameToDevType.insert("ENERGYADJDEV",M_DEV_ENERGY_ADJ);
    _mapNodeNameToDevType.insert("MOTOR",M_DEV_MOTOR);
    _mapNodeNameToDevType.insert("DEFECTCHECKSYS",M_DEV_DEFECTCHECK);
}

/******************************************
* 功能:加载配置文件
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
 bool QDevConfig::loadConfigFile(QString sfilePath)
 {
     QDomDocument configDoc;
     QFile file(sfilePath);
     if(!file.open(QIODevice::ReadOnly))
     {
         return false;
     }

     if(!configDoc.setContent(&file))
     {
         file.close();
         return false;
     }


     QDomElement root = configDoc.documentElement();

     QDomNodeList nodelist = root.childNodes();
     for(int i =0;i < nodelist.count();i++)
     {
         QDomNode subitem = nodelist.at(i);

         QString sNodeName = subitem.nodeName();
         QString sDevTypeName = subitem.attributes().namedItem("Name").nodeValue();

         if(_mapNodeNameToDevType.contains(sNodeName))
         {
             tDevInfo l_devInfo;
             l_devInfo.devType = _mapNodeNameToDevType.value(sNodeName);
             l_devInfo.sdevTypeName = sDevTypeName;
             parseDevInfo(subitem,&l_devInfo);
             m_tDevInfoList.push_back(l_devInfo);
         }
     }
     file.close();


     _sfilePath = sfilePath;

     mapChannelNameToInfo();//映射通道名至通道信息

     return true;

 }

 /******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDevConfig::parseDevInfo(QDomNode item,tDevInfo* pDevInfo)
{
     if(item.hasChildNodes())
     {
         QDomNodeList l_nodelist = item.childNodes();

         for(int k =0;k < l_nodelist.count();k++)
         {
             QDomNode nodeitem = l_nodelist.at(k);
             tDevItem l_Item;
             l_Item.nIndex = nodeitem.attributes().namedItem("Index").nodeValue().toInt();
             l_Item.sDevName = nodeitem.attributes().namedItem("Name").nodeValue();
             l_Item.sDevIPAddress = nodeitem.attributes().namedItem("DevIP").nodeValue();
             l_Item.nPort = nodeitem.attributes().namedItem("Port").nodeValue().toInt();

             if(nodeitem.hasChildNodes())
             {
                 //添加通道信息
                QDomNodeList l_subnodelist = nodeitem.childNodes();
                for(int nsubIndex =0;nsubIndex < l_subnodelist.count();nsubIndex++)
                {
                    QDomNode subnodeitem = l_subnodelist.at(nsubIndex);
                    if(subnodeitem.nodeName() == "ChannelItem")
                    {
                        tChannelInfo l_channelInfo;
                        l_channelInfo.nChannelNo = subnodeitem.attributes().namedItem("ChannelNo").nodeValue().toInt();
                        l_channelInfo.sChannelName = subnodeitem.attributes().namedItem("ChannelName").nodeValue();
                        l_channelInfo.lfCoeff = subnodeitem.attributes().namedItem("Coeff").nodeValue().toFloat();
                        l_channelInfo.sUnit = subnodeitem.attributes().namedItem("Unit").nodeValue();

                        if(subnodeitem.attributes().namedItem("GearName").nodeValue() != "")
                        {
                           l_channelInfo.sGearNamelist = subnodeitem.attributes().namedItem("GearName").nodeValue().split("|");
                           l_channelInfo.sGearIndexlist = subnodeitem.attributes().namedItem("GearIndex").nodeValue().split("|");
                        }

                        assert(l_channelInfo.sGearNamelist.size() == l_channelInfo.sGearIndexlist.size());

                        l_Item.st_ChannelInfo.push_back(l_channelInfo);
                    }

                }
             }

             pDevInfo->devItemlist.push_back(l_Item);
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
QString QDevConfig::getDevIP(QString sDevType,int nDevIndex)
{
    QString sIPAddress  = "";
    std::vector<tDevItem> l_devItemlist;

    foreach (tDevInfo tdevInfo, m_tDevInfoList)
    {
        if(tdevInfo.devType == _mapNodeNameToDevType.value(sDevType))
        {
            l_devItemlist = tdevInfo.devItemlist;
            break;
        }
    }


    foreach (tDevItem tdevItem, l_devItemlist) {

        if(tdevItem.nIndex == nDevIndex)
        {
            sIPAddress = tdevItem.sDevIPAddress;
            break;
        }

   }

    return sIPAddress;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QDevConfig::saveConfigfile()
{
    QDomDocument configDoc;
    QFile file(_sfilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    if(!configDoc.setContent(&file))
    {
        file.close();
        return false;
    }


    QDomElement root = configDoc.documentElement();

    QDomNodeList nodelist = root.childNodes();
    for(int i =0;i < nodelist.count();i++)
    {
        QDomNode subitem = nodelist.at(i);

        //更新能量系数
        if(subitem.nodeName() == _mapNodeNameToDevType.key(M_DEV_ENERGY))
        {
             QDomNodeList l_nodelist = subitem.childNodes().at(0).childNodes();//channel

             for(int k =0;k < l_nodelist.count();k++)
             {
                 QDomNode nodeitem = l_nodelist.at(k);
                 int nIndex = nodeitem.attributes().namedItem("ChannelNo").nodeValue().toInt();
                 nodeitem.attributes().namedItem("Coeff").setNodeValue(QString::number(getChannelCoeff(M_DEV_ENERGY,0x0,nIndex),'g'));
             }
        }

        //更新IP
        if(subitem.hasChildNodes())
        {
            QDomNodeList l_nodelist = subitem.childNodes();

            for(int k =0;k < l_nodelist.count();k++)
            {
                QDomNode nodeitem = l_nodelist.at(k);
                int nIndex = nodeitem.attributes().namedItem("Index").nodeValue().toInt();
                nodeitem.attributes().namedItem("DevIP").setNodeValue(getDevIP(subitem.nodeName(),nIndex));
            }
        }

    }
    file.close();

    QFile savefile(_sfilePath);
    if(!savefile.open(QFile::ReadWrite | QFile::Truncate))
    {
        return false;
    }
    QTextStream stream(&savefile);

    stream.reset();
    stream.setCodec("utf-8");
    configDoc.save(stream,4,QDomNode::EncodingFromTextStream);
    savefile.close();

    return true;
}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QDevConfig::mapChannelNameToInfo()
{
    foreach (tDevInfo devInfo, m_tDevInfoList)
    {
        foreach (tDevItem devItem, devInfo.devItemlist)
        {
            for(size_t i = 0; i < devItem.st_ChannelInfo.size();i++)
            {
                m_mapChannelNameToInfo.insert(devItem.st_ChannelInfo[i].sChannelName,devItem.st_ChannelInfo[i]);
            }
        }
    }

}

/******************************************
* 功能:获取通道系数属性
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
float QDevConfig::getChannelCoeff(int nDevType,int nDevNo,int nChannelIndex)
{
    for (tDevInfo devInfo:m_tDevInfoList)
    {
        if(devInfo.devType != nDevType)
        {
            continue;
        }

        foreach (tDevItem devItem, devInfo.devItemlist){
            if(devItem.nIndex != nDevNo)
            {
                continue;
            }

            foreach (tChannelInfo channelInfo, devItem.st_ChannelInfo) {
                if(channelInfo.nChannelNo == nChannelIndex){
                    return channelInfo.lfCoeff;
                }
            }
        }
    }

    return 0.0;
}
