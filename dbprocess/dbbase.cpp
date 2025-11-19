/*************************************
* 功能描述:本文件提供数据库基本操作
* 创建人:李刚
* 创建时间:2019-03-04
* 维护纪录:
*
**************************************/
#include "dbbase.h"
#include <qsqlerror.h>

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

CDbBase::CDbBase()
{


}


//////////////////////////////////////////
//@功能:设置数据库参数
//@输入:
//@输出:
//@返回值:
//@维护纪录:
//
//////////////////////////////////////////
void CDbBase::setDbInfo(QString sHostName,int i32PortNum,QString sUserName,QString sPassword,QString sDatabaseName,DbType i32DbType)
{
    _dbInfo.m_sHostName = sHostName;
    _dbInfo.m_i32PortNum = i32PortNum;
    _dbInfo.m_sUserName = sUserName;
    _dbInfo.m_sPassword = sPassword;
    _dbInfo.m_sDatabaseName = sDatabaseName;
    _dbInfo.m_i32DbType = i32DbType;
}
//////////////////////////////////////////
//@功能:断开数据库
//@输入:无
//@输出:无
//@返回值:true断开成功 false为断开失败
//@维护纪录:
//
//////////////////////////////////////////

bool CDbBase::disConnectDB()
{
    if(_hdatabase.isOpen() == true)
    {
        _hdatabase.close();
    }

    return true;
}

//////////////////////////////////////////
//@功能:连接数据库
//@输入:无
//@输出:无
//@返回值:true连接成功 false为连接失败
//@维护纪录:
//
//////////////////////////////////////////
bool CDbBase::connectDB()
{
    if(_hdatabase.isOpen() == true)
    {
        return true;
    }

    if(_dbInfo.m_i32DbType == E_DB_ORACL)
    {
        _hdatabase = QSqlDatabase::addDatabase("QOCI");
    }

    if(_dbInfo.m_i32DbType == E_DB_SQLSERV)
    {
        _hdatabase = QSqlDatabase::addDatabase("QODBC");
    }


    _hdatabase.setHostName(_dbInfo.m_sHostName); 	//数据库主机名
    _hdatabase.setPort(_dbInfo.m_i32PortNum);
    _hdatabase.setUserName(_dbInfo.m_sUserName); 		//数据库用户名
    _hdatabase.setPassword(_dbInfo.m_sPassword); 		//数据库密码 .
    _hdatabase.setDatabaseName(_dbInfo.m_sDatabaseName); 	//数据库名


    return _hdatabase.open();//打开数据库连接
}
//////////////////////////////////////////
//@功能:查询数据,返回数据指针  sql语句为确定的某一个字段
//@输入:查询sql语句 nItemIndex第几条数据 从1开始  nColumIndex为列号 0开始
//@输出:dataarray为数据
//@返回值:查询成功或失败
//@维护纪录:
//2019-03-04  创建  by lg
//////////////////////////////////////////
bool CDbBase::queryTableDataToByteArray(QString sSql,int nItemIndex,QByteArray& dataarray)
{

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


    int itemcnt = 0;
    while(sqlQuery.next())
    {
        itemcnt ++;
        if(nItemIndex == itemcnt)
        {
             dataarray = sqlQuery.value(0).toByteArray();
             break;
        }
     }

    return true;

}
//////////////////////////////////////////
//@功能:查询数据,结果以stringlist形式返回
//@输入:查询sql语句
//@输出:pDatalist为查询获取的数据
//@返回值:查询成功或失败
//@维护纪录:
//2019-03-04  创建  by lg
//////////////////////////////////////////
bool CDbBase::queryTableData(QString sSql,QStringList*pDatalist )
{
    QString str;
    if(_hdatabase.isOpen() == false)
    {
        if(!this->connectDB())
        {
            return false;

        }
    }

    qDebug()<<sSql;
    QSqlQuery sqlQuery(_hdatabase);
    bool bOk = sqlQuery.exec(sSql);
    if(bOk == false)
    {
       return false;
    }


    while(sqlQuery.next())
    {
        str = "";
        for(int i = 0;i < sqlQuery.record().count(); i++)
        {
             str = str + sqlQuery.value(i).toString();
             if(i != sqlQuery.record().count() - 1)
             {
                 str = str + "|";
             }
        }
        pDatalist->push_back(str);
     }

    return true;
}


//////////////////////////////////////////
//@功能:查询数据的总个数
//@输入:查询sql语句
//@输出:pDatalist为查询获取的数据
//@返回值:查询成功或失败
//@维护纪录:
//2019-03-04  创建  by lg
//////////////////////////////////////////
int CDbBase::queryTableDataTotalCount(QString sSql)
{
    QString str;
    if(_hdatabase.isOpen() == false)
    {
        if(!this->connectDB())
        {
            return 0;

        }
    }

    QSqlQuery sqlQuery(_hdatabase);
    bool bOk = sqlQuery.exec(sSql);
    if(bOk == false)
    {
       return 0;
    }

    int nNum = 0;
    while(sqlQuery.next())
    {
        nNum++;
    }

    return nNum;
}
//////////////////////////////////////////
//@功能:插入数据
//@输入:sTbaleName为表名称 sFieldName为字段名称 以逗号隔开 sValue 值 以逗号隔开
//@输出:无
//@返回值:插入成功或失败
//@维护纪录:
//2019-03-04  创建  by lg
//////////////////////////////////////////
bool CDbBase::insertDataToTableBySql(QString sTbaleName,QStringList FieldNameArray,QVariantList ValueArray)
{
    if(_hdatabase.isOpen() == false)
    {
        if(!this->connectDB())
        {
            m_sLastErrorTxt="连接数据库失败";
            return false;

        }
    }

    if(FieldNameArray.size() != ValueArray.size())
    {
        m_sLastErrorTxt="字段与值个数不一致";
        return false;
    }

    QSqlQuery insertImgSql(_hdatabase);



    QString sKey,sValue;
    for(int i = 0; i < FieldNameArray.size();i++)
    {
        sKey = sKey + FieldNameArray[i];
        sValue = sValue +"?";
        if(i < FieldNameArray.size()-1)
        {
            sKey = sKey + ",";
            sValue = sValue +",";
        }
    }

    QString sql = QString("insert into %1(%2)values(%3)").arg(sTbaleName).arg(sKey).arg(sValue);

    insertImgSql.prepare(sql);

    for(int i = 0; i < ValueArray.size();i++)
    {
        insertImgSql.addBindValue(ValueArray[i]);
    }

    bool bRel = insertImgSql.exec();
    if(!bRel)
    {
        m_sLastErrorTxt = insertImgSql.lastError().text();

        QString str = "数据库插入失败:"+m_sLastErrorTxt;

        qCritical()<<str;
    }

    return bRel;
}
//////////////////////////////////////////
//@功能:插入数据
//@输入:sTbaleName为表名称 sFieldName为字段名称 以逗号隔开 sValue 值 以逗号隔开
//@输出:无
//@返回值:插入成功或失败
//@维护纪录:
//2019-03-04  创建  by lg
//////////////////////////////////////////
bool CDbBase::insertDataToTable(QString sTbaleName,QStringList FieldNameArray,QVariantList ValueArray)
{
    if(_hdatabase.isOpen() == false)
    {
        if(!this->connectDB())
        {
            return false;

        }
    }

    if(FieldNameArray.size() != ValueArray.size())
    {
        return false;
    }


    QSqlTableModel model;
    model.setTable(sTbaleName);

    QSqlRecord record = model.record();

    for(int i = 0; i < FieldNameArray.size();i++)
    {
        record.setValue(FieldNameArray[i],ValueArray[i]);
    }

    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    bool bRel =  model.insertRecord(-1,record);

    return bRel;
}

/////////////////////////////////////////////////////////////////////////
//@功能:更新数据
//@输入:sTbaleName为表名称 sFieldName为字段名称 以逗号隔开 sValue 值 以逗号隔开
//@输出:无
//@返回值:插入成功或失败
//@维护纪录:
//2019-03-04  创建  by lg  tobe
/////////////////////////////////////////////////////////////////////////
bool CDbBase::updateItem(QString sSql)
{

    if(_hdatabase.isOpen() == false)
    {
        if(!this->connectDB())
        {
            return false;

        }
    }

    qDebug()<<sSql;
    QSqlQuery sqlQuery(_hdatabase);
    bool bOk = sqlQuery.exec(sSql);
    if(bOk == false)
    {
        qDebug()<<sqlQuery.lastError().text();
       return false;
    }


    return true;


}

/******************************************
* 功能:获取序列ID
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int CDbBase::getSeqNextID(QString seq, int &id)
{
    QSqlQuery l_query = QSqlQuery(_hdatabase);
    int result = -1;


    //查询
    QString idsql = "select " + seq + ".nextval from dual";
    bool success = l_query.exec(idsql);

    if (l_query.next() && success)
    {
        result = l_query.value(0).toInt();
    }
    l_query.clear();
    id = result;
    return 1;
}
