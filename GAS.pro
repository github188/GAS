#-------------------------------------------------
#
# Project created by QtCreator 2014-08-15T14:34:50
#
#-------------------------------------------------

QT       += core gui network sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport multimedia

TARGET = GAS
TEMPLATE = app

include($$PWD/api/api.pri)
include($$PWD/qemail/qemail.pri)
include($$PWD/inputnew/inputnew.pri)
include($$PWD/usercontrol/usercontrol.pri)
include($$PWD/qextserialport/qextserialport.pri)

SOURCES += main.cpp\
    frmmain.cpp \
    frmvisitor.cpp \
    frmalarm.cpp \
    frmdefencelot.cpp \    
    frmabout.cpp \
    frmselect.cpp \
    frmgateway.cpp \
    frmconfig.cpp

HEADERS  += frmmain.h \
    frmvisitor.h \
    frmalarm.h \
    frmdefencelot.h \
    frmabout.h \
    frmselect.h \
    frmgateway.h \
    frmconfig.h

FORMS    += frmmain.ui \      
    frmvisitor.ui \
    frmalarm.ui \
    frmdefencelot.ui \
    frmabout.ui \
    frmselect.ui \
    frmgateway.ui \
    frmconfig.ui

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/usercontrol

MOC_DIR             = temp/moc
RCC_DIR             = temp/rcc
UI_DIR              = temp/ui
OBJECTS_DIR         = temp/obj
DESTDIR             = bin
win32:RC_FILE       = other/main.rc
PRECOMPILED_HEADER  = api/myhelper.h

RESOURCES += \
    other/rc.qrc

CONFIG += qt warn_off release
