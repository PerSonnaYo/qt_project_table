QT       += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    closed.cpp \
    imagedelegate.cpp \
    lo.cpp \
    main.cpp \
    mainwindow.cpp \
    photo.cpp \
    sql_block.cpp \
    util1.cpp

HEADERS += \
    closed.h \
    imagedelegate.h \
    lo.h \
    mainwindow.h \
    photo.h

FORMS += \
    closed.ui \
    lo.ui \
    mainwindow.ui \
    photo.ui

TRANSLATIONS += \
    Table_coin_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#INCLUDEPATH+=c:\Program Files\PostgreSQL\14\include
#LIBS+=c:\Program Files\PostgreSQL\14\lib\libpq.lib

#DISTFILES += \
#    vk_api.py

CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/release
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug

copy_to_build.path = $$DESTDIR
copy_to_build.files = copy_to_build_dir/*

INSTALLS += \
    copy_to_build
