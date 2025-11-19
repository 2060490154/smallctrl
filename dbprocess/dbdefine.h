#ifndef DBDEFINE_H
#define DBDEFINE_H

enum DbType
{
    E_DB_SQLSERV = 1,
    E_DB_ORACL = 2
};

typedef struct _dbInfo
{
    QString m_sHostName;
    QString m_sDatabaseName;
    QString m_sUserName;
    QString m_sPassword;
    int     m_i32PortNum;
    DbType  m_i32DbType;
}st_dbInfo;


#endif // DBDEFINE_H

