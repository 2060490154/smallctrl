#ifndef DEVCONFIG_H
#define DEVCONFIG_H

#include <QObject>
#include <QtXml>
#include <QDomElement>
#include "devdefine.h"
#include <assert.h>
#include <QHash>



class QDevConfig : public QObject
{
    Q_OBJECT
public:
    explicit QDevConfig(QObject *parent = nullptr);

signals:

public slots:

public:

    bool loadConfigFile(QString sfilePath);

    bool saveConfigfile();

    float getChannelCoeff(int nDevType,int nDevNo,int nChannelIndex);


    vector<tDevInfo> m_tDevInfoList;

    QHash<QString,tChannelInfo> m_mapChannelNameToInfo;//通道名称映射为通道编号、系数

private:

     void parseDevInfo(QDomNode item,tDevInfo* pDevInfo);

     QString getDevIP(QString sDevType,int nDevIndex);

     void mapChannelNameToInfo();

     QString _sfilePath;

     QHash<QString,eDevType> _mapNodeNameToDevType;

};

#endif // PLATFORMCONFIG_H
