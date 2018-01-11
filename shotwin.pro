CONFIG += c++14

QT += widgets sql quickwidgets

DEFINES += "GIT_VERSION=\\\"lol\\\""

SOURCES = main.cpp \
          mainwindow.cpp \
          models/datefilteredeventmodel.cpp \
          models/eventitem.cpp \
          models/eventmodel.cpp \
          models/eventortagfilteredphotomodel.cpp \
          models/photoitem.cpp \
          models/photomodel.cpp \
          models/tagmodel.cpp \
          models/treeproxymodel.cpp \
          settingsdialog.cpp \
          shotwin.cpp \
          thumbnailprovider.cpp

INCLUDEPATH += models

RESOURCES = resources.qrc

FORMS = mainwindow.ui \
        settingsdialog.ui

HEADERS = mainwindow.h \
          models/datefilteredeventmodel.h \
          models/eventitem.h \
          models/eventmodel.h \
          models/eventortagfilteredphotomodel.h \
          models/filterflattenproxymodel.h \
          models/photoitem.h \
          models/photomodel.h \
          models/tagmodel.h \
          models/treeproxymodel.h \
          settingsdialog.h \
          shotwin.h \
          thumbnailprovider.h \
          workaroundquickwidget.h
