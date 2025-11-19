#ifndef CDBBASE_H
#define CDBBASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>


#include "dbdefine.h"

class CDbBase
{

public:
    explicit CDbBase();


public:

    QString m_sLastErrorTxt;


    void setDbInfo(QString sHostName,int i32PortNum,QString sUserName,QString sPassword,QString sDatabaseName,DbType i32DbType);//设置数据库信息

    bool connectDB();//连接数据库
    bool disConnectDB();//断开数据库

    bool queryTableData(QString sSql,QStringList*pDatalist );//查询数据库

    bool insertDataToTable(QString sTbaleName,QStringList FieldNameArray,QVariantList ValueArray);

    bool updateItem(QString sSql);

    int getSeqNextID(QString seq, int &id);

    bool insertDataToTableBySql(QString sTbaleName,QStringList FieldNameArray,QVariantList ValueArray);

    int queryTableDataTotalCount(QString sSql);

    bool queryTableDataToByteArray(QString sSql,int nItemIndex,QByteArray& dataarray);

    QSqlDatabase _hdatabase;

private:


    st_dbInfo _dbInfo;//数据库信息



};

#endif // CDbBase_H
