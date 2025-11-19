INCLUDEPATH +=$$PWD

HEADERS += \
    $$PWD/motordevctrl.h \
    $$PWD/motordevdefine.h \
    $$PWD/LTDMC.h \
    $$PWD/motordevctrlbasic.h

SOURCES += \
    $$PWD/motordevctrl.cpp \
    $$PWD/motordevctrlbasic.cpp

LIBS += -L$$PWD -lLTDMC


