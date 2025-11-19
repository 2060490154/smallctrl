INCLUDEPATH += $$PWD

INCLUDEPATH +=$$PWD kxccdwidget
include (./kxccdwidget/kxccdwidget.pri)

INCLUDEPATH +=$$PWD oscictrlwidget
include (./oscictrlwidget/oscictrlwidget.pri)

INCLUDEPATH +=$$PWD shutterctrlwidget
include (./shutterctrlwidget/shutterctrlwidget.pri)

INCLUDEPATH +=$$PWD energydevctrlwidget
include (./energydevctrlwidget/energydevctrlwidget.pri)

INCLUDEPATH +=$$PWD motorctrlwidget
include (./motorctrlwidget/motorctrlwidget.pri)

INCLUDEPATH +=$$PWD energyadjdevctrlwidget
include (./energyadjdevctrlwidget/energyadjdevctrlwidget.pri)

INCLUDEPATH +=$$PWD beamproctrlwidget
include (./beamproctrlwidget/beamproctrlwidget.pri)
