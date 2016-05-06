QT       += core gui widgets

CONFIG += C++14 debug

#LIBS += 

QMAKE_CXXFLAGS += /IC:\Users\include #additional includes

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = word_counter
TEMPLATE = app

SOURCES += main.cpp \
			gui/MainWindow.cpp \
			gui/widgets/WordCount.cpp

HEADERS += gui/MainWindow.hpp \
			gui/widgets/WordCount.hpp

FORMS   += gui/MainWindow.ui \
			gui/widgets/WordCount.ui

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
