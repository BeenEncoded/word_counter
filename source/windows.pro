QT       += core gui widgets

CONFIG += C++14 debug

#LIBS += 

QMAKE_CXXFLAGS += /IC:\Users\include #additional includes

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = executable_name
TEMPLATE = app

#SOURCES += 

#HEADERS += 

#FORMS   += 

#RESOURCES += 

CONFIG(debug, debug|release) {
    message(BUILDING DEBUG...)
    DESTDIR = debug
}

CONFIG(release, debug|release) {
    message(BUILDING RELEASE...)
    DESTDIR = release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
