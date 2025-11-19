#-------------------------------------------------
#
# Project created by QtCreator 2020-01-16T09:47:00
#
#-------------------------------------------------

QT       += core gui sql network xml axcontainer printsupport serialport concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = SmallBorePlatformCtrlApp
TEMPLATE = app


RC_ICONS =app.ico
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +=$$PWD/devctrl
include (./devctrl/devctrl.pri)

INCLUDEPATH +=$$PWD/commonfile
include (./commonfile/commonfile.pri)

INCLUDEPATH +=$$PWD/workflowctrl
include (./workflowprocess/workflowprocess.pri)

INCLUDEPATH +=$$PWD/workflowwidget
include (./workflowwidget/workflowwidget.pri)

INCLUDEPATH +=$$PWD/commonwidget
include (./commonwidget/commonwidget.pri)


INCLUDEPATH +=$$PWD/devconfig
include (./devconfig/devconfig.pri)

INCLUDEPATH +=$$PWD/devctrlwidget
include (./devctrlwidget/devctrlwidget.pri)

INCLUDEPATH +=$$PWD/dbprocess
include (./dbprocess/dbprocess.pri)

INCLUDEPATH +=$$PWD/offlinedatashowwidget
include (./offlinedatashowwidget/offlinedatashowwidget.pri)

INCLUDEPATH +=$$PWD/paltformworkwidget
include (./platformworkwidget/platformworkwidget.pri)

INCLUDEPATH +=$$PWD/reportwidget
include (./reportwidget/reportwidget.pri)


SOURCES += \
    beamprowidget.cpp \
        main.cpp \
        mainwindow.cpp \



HEADERS += \
    beamprowidget.h \
        mainwindow.h \
    setdebugnew.h \



FORMS += \
        beamprowidget.ui \
        mainwindow.ui

RESOURCES += \
    resource/res.qrc





#realse loginfo
DEFINES += QT_MESSAGELOGCONTEXT

#for dump file
DEFINES += RUN_ON_MSVC #msvc dump
if(contains(DEFINES,RUN_ON_MSVC)){
    LIBS += -lDbgHelp
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
    QMAKE_LFLAGS_RELEASE += /INCREMENTAL:NO /DEBUG
    QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
}
#end



CONFIG += C++11
CONFIG += debug_and_release


#opencv
INCLUDEPATH += $$PWD/opencv/include
INCLUDEPATH += $$PWD/opencv/include/opencv2

win32:CONFIG(release, debug|release):{
DESTDIR = ../../bin/SmallBorePlatformCtrlApp/release

LIBS += -L$$PWD/opencv/lib/ -lopencv_core412\
                            -lopencv_highgui412\
                            -lopencv_imgproc412\
                            -lopencv_imgcodecs412

}
else:win32:CONFIG(debug, debug|release):{
DESTDIR = ../../bin/SmallBorePlatformCtrlApp/debug
LIBS += -L$$PWD/opencv/lib/ -lopencv_core412d\
                            -lopencv_highgui412d\
                            -lopencv_imgproc412d\
                            -lopencv_imgcodecs412d

}
