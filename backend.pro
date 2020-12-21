TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        connect.cpp \
        main.cpp

HEADERS += \
    QString.h \
    codegen.h \
    defParser.h \
    defStru.h \
    help.h \
    lefParser.h \
    lefStru.h \
    optional.h \
    pos.h \
    rect.h \
    router.h
