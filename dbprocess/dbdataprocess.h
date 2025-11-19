#ifndef CDBDATAPROCESS_H
#define CDBDATAPROCESS_H

#include <QObject>
#include <QDateTime>
#include <QDir>
#include <QImage>
#include "dbbase.h"


class CDbDataProcess : public QObject,public CDbBase
{
    Q_OBJECT
public:
    explicit CDbDataProcess(QObject *parent = 0);

signals:

    void signal_showErrorInfo(QString str,bool bWarining=false);

public slots:


public:

    bool onSaveExpInfo(QString sExpNo,QString sSampleName,int nMeasureType,int nWavelengthType,float lfCoeff,
                       float lfArea,float lfAngle,float lfPlusWidth,float lfM,
                       QString sTaskNo="",float lfExpTemp=0.0,float lfExphum=0.0,float lfExpdirty=0.0);
    bool onSaveEnergyData(QString sExpNo,int nDevNo,int nChannelNo,int nPointNo,int nShotNo,float lfEnergySection,float lfMeasureEnergy);
    bool onSaveCCDData(QString sExpNo,int nwavelength,QByteArray imgData);

    bool onSaveLightParamResult(QString sExpNo,int nwavelength,int nMeasureType,float lfMeasureParam);

    bool onSaveDefectInfo(QString sExpNo,int nPointNo,int nShotNo,float nEnergySection,bool bdefect);


    bool getAllCCDImageToFile(QString sExpNo,QString sFolderName);

    bool saveTaskRelData(QString staskNo,QString sSampleName,int nWaveType,float lfArea,float fluxthreshold,float convfluxthreshold,float pluswidth,float lfTemp,float lfhum,float TargetPluswidth,float TargetThreshold=0);

    QStringList getHistoryAreaData(int nwavelengthType);
    QStringList getAreaExpNolist(int nwavelengthType);

    QStringList getHistoryCoeffData(int nwavelengthType);

    QStringList getHistoryPlusWidthData(int nwavelengthType);

    QStringList onGetHistoryExpInfo(int nMeasureType,int nWavelengthType,QDate startDate,QDate endDate);

    bool getDefectCheckImage(QString sExpNo,int nPointNo,int nShortNo,int nImageType,QImage& img1,bool bOrigImage = false);

    bool getShotdefectInfo(QString sExpNo,int nPointNo,int nShotNo,bool& bdefect);



};

#endif // CDBDATAPROCESS_H
