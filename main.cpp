#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>
#include <QTextStream>
#include <QFile>
#include <QtGlobal>

using namespace std;



#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif

//dump文件 只能在windows平台且是MSVC版本才能下使用
#if defined(RUN_ON_MSVC)
#include <windows.h>
#include <dbghelp.h>
////异常捕获函数
long ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
        // 在程序exe的同级目录中创建dmp文件夹
        QDir *dmp = new QDir;
        bool exist = dmp->exists("./dmp/");
        if(exist == false)
        {
            dmp->mkdir("./dmp/");
        }
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy_MM_dd_hh_mm_ss");
        QString time =  current_date + ".dmp";
          EXCEPTION_RECORD *record = pException->ExceptionRecord;
          QString errCode(QString::number(record->ExceptionCode, 16));
          QString errAddr(QString::number((uint)record->ExceptionAddress, 16));
          QString errFlag(QString::number(record->ExceptionFlags, 16));
          QString errPara(QString::number(record->NumberParameters, 16));
          qDebug()<<"errCode: "<<errCode;
          qDebug()<<"errAddr: "<<errAddr;
          qDebug()<<"errFlag: "<<errFlag;
          qDebug()<<"errPara: "<<errPara;

          qDebug()<<QString("./dmp/" + time);
          HANDLE hDumpFile = CreateFile((LPCWSTR)QString("./dmp/" + time).utf16(),
                   GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
          if(hDumpFile != INVALID_HANDLE_VALUE) {
              MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
              dumpInfo.ExceptionPointers = pException;
              dumpInfo.ThreadId = GetCurrentThreadId();
              dumpInfo.ClientPointers = TRUE;
              MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
              CloseHandle(hDumpFile);
          }
          else{
              qDebug()<<"hDumpFile == null";
          }
          return EXCEPTION_EXECUTE_HANDLER;
}
#endif


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QDir *logFolderDir= new QDir;
    bool exist = logFolderDir->exists("./log/");
    if(exist == false)
    {
        logFolderDir->mkdir("./log/");
    }
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy_MM_dd");
    QString sLogfileName =  current_date + ".txt";

    QString currentTime = current_date_time.toString("hh:mm:ss");

    QString logMessage = "";

    const char *function = context.function ? context.function : "";

    switch (type) {
    case QtDebugMsg:
        logMessage = QString("%1 <调试>: %2 (%3:%4)\r\n").arg(currentTime).arg(msg).arg( QString::number(context.line)).arg(QString(function));
        break;
    case QtInfoMsg:
         logMessage = QString("%1 <提示>: %2 (%3:%4)\r\n").arg(currentTime).arg(msg).arg( QString::number(context.line)).arg(QString(function));
        break;
    case QtWarningMsg:
        logMessage = QString("%1 <警告>: %2 (%3:%4)\r\n").arg(currentTime).arg(msg).arg( QString::number(context.line)).arg(QString(function));
         break;
    case QtCriticalMsg:
        logMessage = QString("%1 <严重>: %2 (%3:%4)\r\n").arg(currentTime).arg(msg).arg( QString::number(context.line)).arg(QString(function));
        break;
    case QtFatalMsg:
        logMessage = QString("%1 <错误>: %2 (%3:%4)\r\n").arg(currentTime).arg(msg).arg( QString::number(context.line)).arg(QString(function));
        break;
    }

//    if(type != QtDebugMsg)
//    {
        // 输出信息至文件中（读写、追加形式）
        QString slogfileFullPath = "./log/"+sLogfileName;
        QFile logfile(slogfileFullPath);
        logfile.open(QIODevice::ReadWrite | QIODevice::Append);
        logfile.write(logMessage.toUtf8());
        logfile.close();
//    }


#ifdef Q_CC_MSVC
    printf(msg.toUtf8().constData());
#else
    printf(msg.toUtf8().constData());
#endif

    fflush(stdout);

    mutex.unlock();
}



int main(int argc, char *argv[])
{    

#if defined(RUN_ON_MSVC)
    //  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

#if defined(RUN_ON_MSVC)
    //设置句柄 出错时执行创建dump文件  msvc版本中才能使用
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif

//#if QT_NO_DEBUG //release版本下输入log信息
    qInstallMessageHandler(myMessageOutput);
//#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    qDebug() << "Available SQL drivers:" << QSqlDatabase::drivers();


    int l_reason = a.exec();
    if(l_reason == 0x5555)//自动重启
    {
         QProcess::startDetached(qApp->applicationFilePath(), QStringList());
         return 0;
    }

    return l_reason;

}
