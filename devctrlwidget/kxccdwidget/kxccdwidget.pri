INCLUDEPATH += $$PWD

INCLUDEPATH += $$PWD/kxccdwidget

INCLUDEPATH += $$PWD/kxccdmulitctrlwidget

HEADERS += \
    $$PWD/kxccdwidget/customsliderwidget.h \
    $$PWD/kxccdwidget/kxccdctrlwidget.h \
    $$PWD/kxccdwidget/measurewidget.h \
    $$PWD/kxccdwidget/searchdevwidget.h \
    $$PWD/kxccdwidget/autosaveimagewidget.h \
    $$PWD/kxccdmulitctrlwidget/mulitctrlwidget.h \
    $$PWD/kxccdmulitctrlwidget/customlabel.h

SOURCES += \
    $$PWD/kxccdwidget/searchdevwidget.cpp \
    $$PWD/kxccdwidget/measurewidget.cpp \
    $$PWD/kxccdwidget/kxccdctrlwidget.cpp \
    $$PWD/kxccdwidget/customsliderwidget.cpp \
    $$PWD/kxccdwidget/autosaveimagewidget.cpp \
	$$PWD/kxccdmulitctrlwidget/mulitctrlwidget.cpp \
    $$PWD/kxccdmulitctrlwidget/customlabel.cpp

RESOURCES += \
    $$PWD/kxccdres.qrc
