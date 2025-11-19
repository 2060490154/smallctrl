/**************************************************************
* 功能描述:在数据库基本操作的基础上，实现定制化功能
* 创建人:李刚
* 创建时间:2019-03-04
* 维护纪录:
*
**************************************************************/
#include "dbdataprocess.h"
#include <QFile>
#include <QApplication>
#include <QDir>
#include <QDirIterator>
#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif
CDbDataProcess::CDbDataProcess(QObject *parent) : QObject(parent)
{

}

/******************************************
* 功能:保存实验信息
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool CDbDataProcess::onSaveExpInfo(QString sExpNo,QString sSampleName,int nMeasureType,
                                   int nWavelengthType,float lfCoeff,float lfArea
                                   ,float lfAngle,float lfPlusWidth,float lfM,
                                   QString sTaskNo,float lfExpTemp,float lfExphum,float lfExpdirty)
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QStringList l_fieldNamelist;
    QVariantList l_valuelist;

    l_fieldNamelist.append("ExpNo");
    l_fieldNamelist.append("MeasureType");
    l_fieldNamelist.append("SampleName");
    l_fieldNamelist.append("WavelengthType");
    l_fieldNamelist.append("SampleCoeff");//分光比
    l_fieldNamelist.append("Area");//光斑面积
    l_fieldNamelist.append("Angle");//测试角度
    l_fieldNamelist.append("Pluswidth");//测试角度
    l_fieldNamelist.append("M");//调制度

    l_fieldNamelist.append("TaskNo");//调制度
    l_fieldNamelist.append("ExpTemp");//温度
    l_fieldNamelist.append("ExpHum");//湿度
    l_fieldNamelist.append("Expdirty");//洁净度

    l_fieldNamelist.append("Time");

    l_valuelist.append(sExpNo);
    l_valuelist.append(nMeasureType);
    l_valuelist.append(sSampleName);
    l_valuelist.append(nWavelengthType);
    l_valuelist.append(QString::number(lfCoeff,'g'));
    l_valuelist.append(QString::number(lfArea,'g'));
    l_valuelist.append(QString::number(lfAngle,'g'));
    l_valuelist.append(QString::number(lfPlusWidth,'g'));
    l_valuelist.append(QString::number(lfM,'g'));
    l_valuelist.append(sTaskNo);
    l_valuelist.append(QString::number(lfExpTemp,'g'));
    l_valuelist.append(QString::number(lfExphum,'g'));
    l_valuelist.append(QString::number(lfExpdirty,'g'));

    l_valuelist.append(currentTime);


    if(!insertDataToTableBySql("T_EXP_INFO",l_fieldNamelist,l_valuelist))
    {
        emit signal_showErrorInfo("创建实验基本信息失败,失败原因:"+m_sLastErrorTxt,true);
        return false;
    }
    return true;

}

/******************************************
* 功能:保存能量数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool CDbDataProcess::onSaveEnergyData(QString sExpNo,int nDevNo,int nChannelNo,int nPointNo,int nShotNo,float lfEnergySection,float lfMeasureEnergy)
{
    QDateTime currentTime = QDateTime::currentDateTime();

    QStringList l_fieldNamelist;
    QVariantList l_valuelist;

    l_fieldNamelist.append("ExpNo");
    l_fieldNamelist.append("PointNo");
    l_fieldNamelist.append("ShotNo");
    //l_fieldNamelist.append("FluxData");
    l_fieldNamelist.append("EnergySection");
    l_fieldNamelist.append("MeasureEnergy");
    l_fieldNamelist.append("DevNo");
    l_fieldNamelist.append("ChannelNo");
    l_fieldNamelist.append("Time");

    l_valuelist.append(sExpNo);
    l_valuelist.append(nPointNo);
    l_valuelist.append(nShotNo);
   // l_valuelist.append(QString("%1").arg(lfFluxData));
    l_valuelist.append(QString("%1").arg(lfEnergySection));
    l_valuelist.append(QString("%1").arg(lfMeasureEnergy));
    l_valuelist.append(nDevNo);
    l_valuelist.append(nChannelNo);
    l_valuelist.append(currentTime);



    if(!insertDataToTableBySql("T_ERNERGY_DATA",l_fieldNamelist,l_valuelist))
    {
        emit signal_showErrorInfo("保存能量数据失败,失败原因:"+m_sLastErrorTxt,true);
        return false;
    }

    return true;


}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool CDbDataProcess::onSaveCCDData(QString sExpNo,int nwavelength,QByteArray imgData)
{
    QDateTime currentTime = QDateTime::currentDateTime();

    QStringList l_fieldNamelist;
    QVariantList l_valuelist;

    l_fieldNamelist.append("ExpNo");
    l_fieldNamelist.append("ccdimage");
    l_fieldNamelist.append("wavelength");
    l_fieldNamelist.append("Time");


    l_valuelist.append(sExpNo);
    l_valuelist.append(imgData);
    l_valuelist.append(nwavelength);
    l_valuelist.append(currentTime);


    if(!insertDataToTableBySql("T_KXCCD_DATA",l_fieldNamelist,l_valuelist))
    {
        emit signal_showErrorInfo("保存能量数据失败,失败原因:"+m_sLastErrorTxt,true);
        return false;
    }

    return true;
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool CDbDataProcess::onSaveDefectInfo(QString sExpNo,int nPointNo,int nShotNo,float lfEnergySection,bool bdefect)
{
    QDateTime currentTime = QDateTime::currentDateTime();

    QStringList l_fieldNamelist;
    QVariantList l_valuelist;

    l_fieldNamelist.append("ExpNo");
    l_fieldNamelist.append("PointNo");
    l_fieldNamelist.append("ShotNo");
    l_fieldNamelist.append("EnergySection");
    l_fieldNamelist.append("Defect");
    l_fieldNamelist.append("Time");

    int nDefect = bdefect==true?1:0;

    l_valuelist.append(sExpNo);
    l_valuelist.append(nPointNo);
    l_valuelist.append(nShotNo);
    l_valuelist.append(QString("%1").arg(lfEnergySection));

    l_valuelist.append(nDefect);

    l_valuelist.append(currentTime);




    if(!insertDataToTableBySql("T_DEFECT_CHECK_DATA",l_fieldNamelist,l_valuelist))
    {
        emit signal_showErrorInfo("保存损伤检测结果数据失败,失败原因:"+m_sLastErrorTxt,true);
        return false;
    }

    return true;

}

/******************************************
* 功能:保存光路参数实验结果
* 输入:nWaveLength--0 1064 1-355
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool CDbDataProcess::onSaveLightParamResult(QString sExpNo,int nWaveLength,int nMeasureParamType,float lfMeasureParam)
{
    QDateTime currentTime = QDateTime::currentDateTime();

    QStringList l_fieldNamelist;
    QVariantList l_valuelist;

    l_fieldNamelist.append("ExpNo");
    l_fieldNamelist.append("MeasureParamType");
    l_fieldNamelist.append("wavelength");
    l_fieldNamelist.append("param");
    l_fieldNamelist.append("Time");


    l_valuelist.append(sExpNo);
    l_valuelist.append(nMeasureParamType);
    l_valuelist.append(nWaveLength);
    l_valuelist.append(QString::number(lfMeasureParam));
    l_valuelist.append(currentTime);


    if(!insertDataToTableBySql("T_LIGHTPARAM_DATA",l_fieldNamelist,l_valuelist))
    {
        emit signal_showErrorInfo("保存实验结果数据失败,失败原因:"+m_sLastErrorTxt,true);
        return false;
    }

    return true;
}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool CDbDataProcess::getAllCCDImageToFile(QString sExpNo,QString sFolderName)
{

    QString sSql = QString("select t.ccdimage from t_kxccd_data t where t.expno = %1").arg(sExpNo);

    if(_hdatabase.isOpen() == false)
    {
        if(!this->connectDB())
        {
            return false;

        }
    }

    QSqlQuery sqlQuery(_hdatabase);
    bool bOk = sqlQuery.exec(sSql);
    if(bOk == false)
    {
       return false;
    }




    QDir l_dir(sFolderName);
    if(!l_dir.exists())
    {
        l_dir.mkdir(sFolderName);
    }

    if(l_dir.exists(sFolderName))
    {
        QDirIterator DirsIterator(sFolderName, QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
        while(DirsIterator.hasNext())
        {
            if (!l_dir.remove(DirsIterator.next())) // 删除文件操作如果返回否，那它就是目录
            {
                QDir(DirsIterator.filePath()).removeRecursively(); // 删除目录本身以及它下属所有的文件及目录
            }
        }
    }


    int nCnt = 0;

    while(sqlQuery.next())
    {
        nCnt++;


        QByteArray l_byteArray;

        l_byteArray = sqlQuery.value(0).toByteArray();


        if(l_byteArray.size() > 0)
        {
            QFile l_file(QString("%1%2.bmp").arg(sFolderName).arg(nCnt));
            l_file.open(QFile::ReadWrite);
            l_file.write(l_byteArray);
            l_file.close();
        }


     }



    return true;
}



/******************************************
* 功能:获取所有的光斑面积历史数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QStringList CDbDataProcess:: getHistoryAreaData(int nwavelengthType)
{
    QString sSql = QString("select t.expno,t.param from T_LIGHTPARAM_DATA t where t.MeasureParamType = 3 and wavelength = %1 ").arg(nwavelengthType);



    QStringList l_datalist;
    queryTableData(sSql,&l_datalist);

    return l_datalist;
}


/******************************************
* 功能:获取所有的光斑面积历史数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QStringList CDbDataProcess:: getAreaExpNolist(int nwavelengthType)
{
    QString sSql = QString("select t.expno from T_LIGHTPARAM_DATA t where t.MeasureParamType = 3 and wavelength = %1 ").arg(nwavelengthType);



    QStringList l_datalist;
    queryTableData(sSql,&l_datalist);

    return l_datalist;
}
/******************************************
* 功能:获取所有的光分光比历史数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QStringList CDbDataProcess:: getHistoryCoeffData(int nwavelengthType)
{
    QString sSql = QString("select t.expno,t.param from T_LIGHTPARAM_DATA t where t.MeasureParamType = 4 and wavelength = %1 ").arg(nwavelengthType);



    QStringList l_datalist;
    queryTableData(sSql,&l_datalist);

    return l_datalist;
}

/******************************************
* 功能:获取所有的光分光比历史数据
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QStringList CDbDataProcess:: getHistoryPlusWidthData(int nwavelengthType)
{
    QString sSql = QString("select t.expno,t.param from T_LIGHTPARAM_DATA t where t.MeasureParamType = 5 and wavelength = %1 ").arg(nwavelengthType);



    QStringList l_datalist;
    queryTableData(sSql,&l_datalist);

    return l_datalist;
}
/******************************************
* 功能:获取实验信息
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QStringList CDbDataProcess::onGetHistoryExpInfo(int nMeasureType,int nWavelengthType,QDate startDate,QDate endDate)
{
    QString sSql = QString("select t.expno,t.samplename from T_EXP_INFO t where t.MeasureType = %1 and wavelengthType = %2 and CONVERT(CHAR(100), t.time, 23)  BETWEEN '%3' AND '%4'").arg(nMeasureType).arg(nWavelengthType).arg(startDate.toString(Qt::ISODate)).arg(endDate.toString(Qt::ISODate));

    QStringList l_datalist;
    queryTableData(sSql,&l_datalist);

    return l_datalist;

}

/******************************************
* 功能:
* 输入:
* 输出:nImageType=0时 img1为打靶前图像 nImageType=1 无损伤时 img1为打靶后的图像  img2为标记后的图像
* 打靶后图像 若出现损伤 默认查询输出的图像为标记后的图像 当bOrigImage=true时 输出为标记前的图像
* 返回值:
* 维护记录:
******************************************/
bool CDbDataProcess:: getDefectCheckImage(QString sExpNo,int nPointNo,int nShotNo,int nImageType,QImage& imgdata,bool bOrigImage)
{
    QString sSql = QString("select t.defect,t.imagedata,t.defectimage from t_defect_system_data t where t.expno = '%1' and pointno='%2' and shotno = '%3' and imagetype='%4'")\
            .arg(sExpNo).arg(nPointNo).arg(nShotNo).arg(nImageType);

    qDebug()<<sSql;

    if(_hdatabase.isOpen() == false)
    {
        if(!this->connectDB())
        {
            return false;

        }
    }


    QSqlQuery sqlQuery(_hdatabase);
    bool bOk = sqlQuery.exec(sSql);
    if(bOk == false)
    {
       return false;
    }

    int nCnt = 0;
    QByteArray l_byteArray;

    while(sqlQuery.next())
    {
        nCnt++;

        if(nImageType == 0x0)//打靶前图像
        {
            l_byteArray = sqlQuery.value(1).toByteArray();

            imgdata = QImage::fromData(l_byteArray);
            return true;
        }

        if(nImageType == 0x1)//打靶后图像
        {
            if(sqlQuery.value(0).value<int>() == 0x0||bOrigImage == true)//无损伤
            {
                l_byteArray = sqlQuery.value(1).toByteArray();
                imgdata = QImage::fromData(l_byteArray);
                return true;
            }
            else if(sqlQuery.value(0).value<int>() == 0x1 && bOrigImage == false)//有损伤
            {
                l_byteArray = sqlQuery.value(2).toByteArray();
                imgdata = QImage::fromData(l_byteArray);
                return true;
            }

        }
    }


    return false;
}

/******************************************
* 功能:查询该发次是否损伤
* 输入:
* 输出:
* 返回值:bdefect为是否损伤结果 函数返回结果为查询是否成功
* 维护记录:
******************************************/
bool CDbDataProcess::getShotdefectInfo(QString sExpNo, int nPointNo, int nShotNo, bool &bdefect)
{
    QStringList defectlist;
    bool bRel = queryTableData(QString("select defect from dbo.T_DEFECT_CHECK_DATA where ExpNo='%1'and PointNo=%2 and ShotNo=%3 order by PointNo, ShotNo").\
                                        arg(sExpNo).arg(nPointNo).arg(nShotNo),&defectlist);

    if(bRel == false)
    {
        return false;
    }

    if(defectlist.size() != 1)
    {
        qInfo()<<"损伤数据查询错误,不为1条["<<sExpNo<<nPointNo<<nShotNo<<"]";
        return false;
    }

    bdefect = defectlist[0].split('|')[0].toInt()==0?false:true;

    return true;
}


/******************************************
* 功能:保存处理结果信息
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool CDbDataProcess::saveTaskRelData(QString staskNo,QString sSampleName,int nWaveType,float lfArea,float fluxthreshold,float convfluxthreshold,float pluswidth,float lfTemp,float lfhum,float TargetPluswidth,float TargetThreshold)
{
    QStringList defectlist;
    bool bRel = queryTableData(QString("select * from dbo.T_TASK_REL_DATA where TaskNo ='%1'and SampleName ='%2' and WavelengthType=%3").\
                                        arg(staskNo).arg(sSampleName).arg(nWaveType),&defectlist);

    if(bRel == false)
    {
        return false;
    }


    if(defectlist.size() == 1)//更新
    {
        QString sSql = QString("update dbo.T_TASK_REL_DATA set Threshold = %1,ConvThreshold = %2,Pluswidth = '%3',TargetPluswidth = '%4',TargetThreshold = '%5' where TaskNo ='%6'and SampleName ='%7' and WavelengthType='%8' and ExpTemp='%9' and ExpHum = '10%'")\
                .arg(fluxthreshold).arg(TargetThreshold).arg(pluswidth)\
                .arg(TargetPluswidth).arg(TargetThreshold)\
                .arg(staskNo).arg(sSampleName).arg(nWaveType)\
                .arg(lfTemp).arg(lfhum);

        updateItem(sSql);
    }
    else if(defectlist.size() == 0)//插入
    {

        QDateTime currentTime = QDateTime::currentDateTime();

        QStringList l_fieldNamelist;
        QVariantList l_valuelist;

        l_fieldNamelist.append("TaskNo");
        l_fieldNamelist.append("SampleName");
        l_fieldNamelist.append("WavelengthType");
        l_fieldNamelist.append("Area");
        l_fieldNamelist.append("Threshold");
        l_fieldNamelist.append("ConvThreshold");
        l_fieldNamelist.append("Pluswidth");
        l_fieldNamelist.append("TargetPluswidth");
        l_fieldNamelist.append("TargetThreshold");
        l_fieldNamelist.append("ExpTemp");
        l_fieldNamelist.append("ExpHum");
        l_fieldNamelist.append("Time");

        l_valuelist.append(staskNo);
        l_valuelist.append(sSampleName);
        l_valuelist.append(nWaveType);
        l_valuelist.append(QString::number(lfArea,'g'));
        l_valuelist.append(QString::number(fluxthreshold,'g'));
        l_valuelist.append(QString::number(convfluxthreshold,'g'));
        l_valuelist.append(QString::number(pluswidth,'g'));
        l_valuelist.append(QString::number(TargetPluswidth,'g'));
        l_valuelist.append(QString::number(TargetThreshold,'g'));
        l_valuelist.append(QString::number(lfTemp,'g'));
        l_valuelist.append(QString::number(lfhum,'g'));
        l_valuelist.append(currentTime);

        if(!insertDataToTableBySql("T_TASK_REL_DATA",l_fieldNamelist,l_valuelist))
        {
            emit signal_showErrorInfo("保存处理数据失败,失败原因:"+m_sLastErrorTxt,true);
            return false;
        }
    }


    return true;
}

