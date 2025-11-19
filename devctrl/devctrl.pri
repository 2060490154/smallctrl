INCLUDEPATH += $$PWD

#INCLUDEPATH +=$$PWD QCommonDevCtrlManager
#include (./QCommonDevCtrlManager/QCommonDevCtrlManager.pri)

INCLUDEPATH +=$$PWD subdevctrl
include (./subdevctrl/subdevctrl.pri)

#INCLUDEPATH +=$$PWD QPlatformDevCtrl
#include (./QPlatformDevCtrl/QPlatformDevCtrl.pri)

INCLUDEPATH +=$$PWD devctrlmanager
include (./devctrlmanager/devctrlmanager.pri)

