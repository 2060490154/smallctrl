INCLUDEPATH +=$$PWD
include (./kxccdctrl/kxccdctrl.pri)
include (./oscidevctrl/oscidevctrl.pri)
include (./shutterdevctrl/shutterdevctrl.pri)
include (./energydevctrl/energydevctrl.pri)
include (./motordevctrl/motordevctrl.pri)
include (./energyadjustdevctrl/energyadjustdevctrl.pri)
include (./defectchecksysctrl/defectchecksysctrl.pri)

HEADERS += \
    $$PWD/platformkxccdctrl.h \
    $$PWD/platformmotordevctrl.h \
    $$PWD/platformoscictrl.h \
    $$PWD/platformshutterctrl.h \
    $$PWD/platformenergydevctrl.h \
    $$PWD/platformdataprocessctrl.h \
    $$PWD/platformtaskdefine.h \
    $$PWD/platformenergyadjdevctrl.h \
    $$PWD/platformdefectcheckctrl.h

SOURCES += \
    $$PWD/platformkxccdctrl.cpp \
    $$PWD/platformmotordevctrl.cpp \
    $$PWD/platformoscictrl.cpp \
    $$PWD/platformshutterctrl.cpp \
    $$PWD/platformenergydevctrl.cpp \
    $$PWD/platformdataprocessctrl.cpp \
    $$PWD/platformenergyadjdevctrl.cpp \
    $$PWD/platformdefectcheckctrl.cpp


