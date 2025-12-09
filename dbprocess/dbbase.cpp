/*************************************
* 功能描述:本文件提供数据库基本操作
* 创建人:李刚
* 创建时间:2019-03-04
* 维护纪录:
*
**************************************/
#include "dbbase.h"
#include <qsqlerror.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qcoreapplication.h>
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
    // 如果已经 open 则直接返回
    if (_hdatabase.isOpen())
        return true;

    // 统一使用一个连接名，避免重复默认连接带来的警告
    const QString connName = QString("SmallCtrlConnection");

    // 如果存在同名连接，先确保关闭并移除，避免 duplicate connection 警告
    if (QSqlDatabase::contains(connName))
    {
        QSqlDatabase existing = QSqlDatabase::database(connName);
        if (existing.isOpen()) existing.close();
        QSqlDatabase::removeDatabase(connName);
    }

    QString driverName;
    if (_dbInfo.m_i32DbType == E_DB_ORACL)
    {
        driverName = "QOCI";
    }
    else if (_dbInfo.m_i32DbType == E_DB_SQLSERV)
    {
        driverName = "QODBC";
    }
    else
    {
        qDebug() << "Unsupported DbType:" << _dbInfo.m_i32DbType;
        return false;
    }

    // addDatabase with explicit connection name
    _hdatabase = QSqlDatabase::addDatabase(driverName, connName);

    // 对于 ODBC，使用 DSN-less 连接字符串更稳妥
    if (driverName == "QODBC")
    {
        // 请根据本机安装的 ODBC Driver 名称（17 或 18）调整 DRIVER 名称
        QString odbcDriver = "ODBC Driver 17 for SQL Server";
        QString connStr = QString("DRIVER={%1};Server=%2,%3;Database=%4;Uid=%5;Pwd=%6;")
            .arg(odbcDriver)
            .arg(_dbInfo.m_sHostName)
            .arg(_dbInfo.m_i32PortNum)
            .arg(_dbInfo.m_sDatabaseName)
            .arg(_dbInfo.m_sUserName)
            .arg(_dbInfo.m_sPassword);
        _hdatabase.setDatabaseName(connStr);
    }
    else
    {
        // OCI or other drivers: 尝试使用 setHostName/setPort/setDatabaseName
        _hdatabase.setHostName(_dbInfo.m_sHostName);
        _hdatabase.setPort(_dbInfo.m_i32PortNum);
        _hdatabase.setDatabaseName(_dbInfo.m_sDatabaseName);
        _hdatabase.setUserName(_dbInfo.m_sUserName);
        _hdatabase.setPassword(_dbInfo.m_sPassword);
    }

    bool ok = _hdatabase.open();
    if (!ok)
    {
        // 打印并记录详细错误
        QSqlError err = _hdatabase.lastError();
        qDebug() << "Failed to open DB. Host:" << _dbInfo.m_sHostName << "Port:" << _dbInfo.m_i32PortNum
            << "DBName:" << _dbInfo.m_sDatabaseName << "User:" << _dbInfo.m_sUserName;
        qDebug() << "QSqlDatabase drivers available:" << QSqlDatabase::drivers();
        qDebug() << "QSqlError.text():" << err.text();
        qDebug() << "QSqlError.driverText():" << err.driverText();
        // 追加到日志文件
        QFile f(QCoreApplication::applicationDirPath() + "/db_error.log");
        if (f.open(QIODevice::Append | QIODevice::Text))
        {
            QTextStream ts(&f);
            ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                << " Failed to open DB. Host:" << _dbInfo.m_sHostName << " Port:" << _dbInfo.m_i32PortNum
                << " DB:" << _dbInfo.m_sDatabaseName << " User:" << _dbInfo.m_sUserName << "\n";
            ts << "Drivers:" << QSqlDatabase::drivers().join(",") << "\n";
            ts << "Error.text:" << err.text() << "\n";
            ts << "Error.driverText:" << err.driverText() << "\n\n";
            f.close();
        }
    }
    return ok;
}//////////////////////////////////////////
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
// ---------- 替换 CDbBase::insertDataToTableBySql 的实现 ----------
bool CDbBase::insertDataToTableBySql(QString sTbaleName, QStringList FieldNameArray, QVariantList ValueArray)
{
    if (_hdatabase.isOpen() == false)
    {
        if (!this->connectDB())
        {
            m_sLastErrorTxt = "连接数据库失败";
            return false;
        }
    }

    if (FieldNameArray.size() != ValueArray.size())
    {
        m_sLastErrorTxt = "字段与值个数不一致";
        return false;
    }

    // 规范化表名：若未包含 schema，则默认加 dbo
    QString tableName = sTbaleName;
    if (!tableName.contains('.'))
        tableName = QString("dbo.%1").arg(sTbaleName);

    // 将 schema.table 转为 [schema].[table] 形式
    QStringList parts = tableName.split('.');
    for (int i = 0; i < parts.size(); ++i)
        parts[i] = QString("[%1]").arg(parts[i]);
    QString tableRef = parts.join(".");

    // 构造带方括号的字段名列表与占位符
    QString sKey;
    QString sPlaceholders;
    for (int i = 0; i < FieldNameArray.size(); ++i)
    {
        if (i > 0) { sKey += ","; sPlaceholders += ","; }
        sKey += QString("[%1]").arg(FieldNameArray[i]);
        sPlaceholders += "?";
    }

    QString sql = QString("INSERT INTO %1(%2) VALUES(%3)").arg(tableRef).arg(sKey).arg(sPlaceholders);

    QSqlQuery insertQuery(_hdatabase);
    if (!insertQuery.prepare(sql))
    {
        m_sLastErrorTxt = insertQuery.lastError().text();
        qCritical() << "Prepare failed:" << sql << " Error:" << insertQuery.lastError().text();
        return false;
    }

    for (int i = 0; i < ValueArray.size(); ++i)
    {
        insertQuery.addBindValue(ValueArray[i]);
    }

    bool bRel = insertQuery.exec();
    if (!bRel)
    {
        m_sLastErrorTxt = insertQuery.lastError().text();
        qCritical() << "Database insert failed. SQL:" << sql << " Error:" << insertQuery.lastError().text();
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
// ---------- 替换 CDbBase::insertDataToTable 的实现，确保使用方括号并检查连接 ----------
bool CDbBase::insertDataToTable(QString sTbaleName, QStringList FieldNameArray, QVariantList ValueArray)
{
    if (_hdatabase.isOpen() == false)
    {
        if (!this->connectDB())
        {
            return false;
        }
    }

    if (FieldNameArray.size() != ValueArray.size())
    {
        return false;
    }

    // 将表名做 schema+方括号处理
    QString tableName = sTbaleName;
    if (!tableName.contains('.'))
        tableName = QString("dbo.%1").arg(sTbaleName);
    QStringList parts = tableName.split('.');
    for (int i = 0; i < parts.size(); ++i)
        parts[i] = QString("[%1]").arg(parts[i]);
    QString tableRef = parts.join(".");

    QSqlTableModel model(nullptr, _hdatabase);
    model.setTable(tableRef);
    QSqlRecord record = model.record();
    for (int i = 0; i < FieldNameArray.size(); ++i)
    {
        record.setValue(FieldNameArray[i], ValueArray[i]);
    }

    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    bool bRel = model.insertRecord(-1, record);
    if (!bRel)
    {
        qCritical() << "insertDataToTable failed for table:" << tableRef << " error:" << model.lastError().text();
    }
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
