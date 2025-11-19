#include "platformconfig.h"

QPlatformConfig::QPlatformConfig(QObject *parent) : QObject(parent)
{

}
//获取平台配置信息
bool QPlatformConfig::loadPlatformConfig(QString sfilePath)
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

    _sConfigfilePath = sfilePath;


    QDomElement root = configDoc.documentElement();

    QDomNodeList nodelist = root.childNodes();
    for(int i =0;i < nodelist.count();i++)
    {
        QDomNode subitem = nodelist.at(i);

        if(subitem.nodeName() == "Status")
        {
            m_tPlatformConfig.m_nCurrentWaveLength =  subitem.attributes().namedItem("WaveLengthType").nodeValue().toInt();
        }


        if(subitem.nodeName() == "LaserParam")
        {
            int nIndex =  subitem.attributes().namedItem("WaveLengthType").nodeValue().toInt();
            m_tPlatformConfig.m_st_laserParam[nIndex].m_lfK = subitem.attributes().namedItem("Coeff_k").nodeValue().toFloat();
            m_tPlatformConfig.m_st_laserParam[nIndex].m_lfPlusWidth = subitem.attributes().namedItem("pluswidth").nodeValue().toFloat();
            m_tPlatformConfig.m_st_laserParam[nIndex].m_lfArea = subitem.attributes().namedItem("Area").nodeValue().toFloat();
            m_tPlatformConfig.m_st_laserParam[nIndex].m_sLaserName = subitem.attributes().namedItem("Name").nodeValue();
            m_tPlatformConfig.m_st_laserParam[nIndex].m_lfM = subitem.attributes().namedItem("M").nodeValue().toFloat();

        }

        if(subitem.nodeName() == "Motor")
        {
            tWheelGear l_tWheelGear;

            l_tWheelGear.m_nDevIndex =  subitem.attributes().namedItem("DevIndex").nodeValue().toInt();
            l_tWheelGear.m_nChannelIndex =  subitem.attributes().namedItem("ChannelIndex").nodeValue().toInt();
            QDomNodeList motornodelist = subitem.childNodes();
            for (int j =0;j < motornodelist.count();j++)
            {

                QDomNode motornode = motornodelist.at(j);

                tMotorGear l_tMotorGear;

                l_tMotorGear.m_lfMaxEnergy = motornode.attributes().namedItem("MaxEnergy").nodeValue().toFloat();
                l_tMotorGear.m_lfminEnergy = motornode.attributes().namedItem("MinEnergy").nodeValue().toFloat();
                l_tMotorGear.m_nGearIndex = motornode.attributes().namedItem("GrearIndex").nodeValue().toFloat();

                l_tWheelGear.m_tMotorGearlist.push_back(l_tMotorGear);
            }

            m_tPlatformConfig.m_tWheelGear.push_back(l_tWheelGear);

         }

    }

    file.close();
    return true;

}

bool QPlatformConfig::onUpdatePlatConfigfile()
{
    qDebug()<<"更新配置文件";

    emit signal_updateshowParam();

    QDomDocument configDoc;
    QFile file(_sConfigfilePath);
    if(!file.open(QIODevice::ReadWrite))
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

        if(subitem.nodeName() == "Status")
        {
            subitem.attributes().namedItem("WaveLengthType").setNodeValue(QString("%1").arg(m_tPlatformConfig.m_nCurrentWaveLength));
        }
        if(subitem.nodeName() == "LaserParam")
        {
            int nIndex =  subitem.attributes().namedItem("WaveLengthType").nodeValue().toInt();

            subitem.attributes().namedItem("Coeff_k").setNodeValue(QString("%1").arg(m_tPlatformConfig.m_st_laserParam[nIndex].m_lfK));
            subitem.attributes().namedItem("pluswidth").setNodeValue(QString("%1").arg(m_tPlatformConfig.m_st_laserParam[nIndex].m_lfPlusWidth));
            subitem.attributes().namedItem("Area").setNodeValue(QString("%1").arg(m_tPlatformConfig.m_st_laserParam[nIndex].m_lfArea));
            subitem.attributes().namedItem("M").setNodeValue(QString("%1").arg(m_tPlatformConfig.m_st_laserParam[nIndex].m_lfM));

        }
    }


//保存数据
    file.close();

    QFile savefile(_sConfigfilePath);
    if(!savefile.open(QFile::ReadWrite | QFile::Truncate))
    {
        return false;
    }
    QTextStream stream(&savefile);

    stream.reset();
    stream.setCodec("utf-8");
    configDoc.save(stream,4,QDomNode::EncodingFromTextStream);
    file.close();



    return true;
}
