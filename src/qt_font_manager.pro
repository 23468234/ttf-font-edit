#-------------------------------------------------
#
# Project created by QtCreator 2016-07-05T10:57:59
#
#-------------------------------------------------

QT       += core gui
QT       += xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_font_manager
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    bmpparse.cpp \
    fontwidget.cpp \
    config.cpp \
    aboutwin.cpp \
    guicfgparse.cpp \
    tsparse.cpp \
    characterstatistics.cpp \
    ttfmodify.cpp \
    building.cpp \
    ttftable.cpp

HEADERS += mainwindow.h \
    common.h \
    binfont_parse.h \
    node_list.h \
    bmpparse.h \
    fontwidget.h \
    config.h \
    aboutwin.h \
    guicfgparse.h \
    tsparse.h \
    characterstatistics.h \
    ttfmodify.h \
    ttftable.h \
    unicodetogb2312tab.h \
    unicodetogbktab.h \
    building.h

FORMS += mainwindow.ui \
    aboutwin.ui \
    building.ui

TRANSLATIONS += \
    fontManager_zh_CN.ts \
    fontManager_en_US.ts \
    fontManager_ko_Kp.ts\

DISTFILES += \
    fontManager_en_US.qm \
    fontManager_zh_CN.qm \
    fontManager_en_US.ts \
    fontManager_zh_CN.ts \
    fontManager_ko_Kp.qm \
    fontManager_ko_Kp.ts \
    qtFontConfig.bat \
    lib/libbinfont_parse.a \
    lib/libbinfont_parsed.a \
    lib/libnode_list.a \
    lib/libnode_listd.a

RESOURCES += \
    resource.qrc


win32:CONFIG(release, debug|release): LIBS += -L../src/lib -lbinfont_parse -lnode_list
else:win32:CONFIG(debug, debug|release): LIBS += -L../src/lib -lbinfont_parsed -lnode_listd

