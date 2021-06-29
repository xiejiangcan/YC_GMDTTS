QT += gui widgets xml script

TEMPLATE = lib
DEFINES += TOOLFORUDS_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    model/canmessagemodel.cpp \
    scanmessagewidgt.cpp \
    sexportplugin.cpp \
    slistwidgetforlog.cpp \
    sudswidget.cpp \
    susbcanwidget.cpp \
    tool/abstracthandle.cpp \
    tool/udsservertree.cpp \
    tool/usbinterface.cpp \
    tool/ycanhandle.cpp

HEADERS += \
    ToolForUDS_global.h \
    model/canmessagemodel.h \
    scanmessagewidgt.h \
    sexportplugin.h \
    slistwidgetforlog.h \
    sudswidget.h \
    susbcanwidget.h \
    tool/abstracthandle.h \
    tool/udsservertree.h \
    tool/usbinterface.h \
    tool/ycanhandle.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Core/release/ -lSCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Core/debug/ -lSCore
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../third/ECANVCI/libs/lib_64 -lECanVci64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../third/ECANVCI/libs/lib_64 -lECanVci64

LIBS += -luser32 -lsetupapi

INCLUDEPATH += $$PWD/../Core \
            $$PWD/../third/ECANVCI/include
DEPENDPATH += $$PWD/../Core \
            $$PWD/../third/ECANVCI/include

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
