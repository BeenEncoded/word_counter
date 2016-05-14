QT       += core gui widgets

CONFIG += C++14 debug release

#release libs
CONFIG(release, debug|release) {
	LIBS += "C:\lib\x64\libboost_system-vc140-mt-1_60.lib" \
		"C:\lib\x64\libboost_filesystem-vc140-mt-1_60.lib"
}

#debug libs
CONFIG(debug, debug|release) {
	LIBS += "C:\lib\x64\libboost_system-vc140-mt-gd-1_60.lib" \
		"C:\lib\x64\libboost_filesystem-vc140-mt-gd-1_60.lib"
}

QMAKE_CXXFLAGS += /IC:\Users\include #additional includes

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = word_counter
TEMPLATE = app

SOURCES += main.cpp \
			gui/MainWindow.cpp \
			gui/widgets/WordCount.cpp \
			gui/dialogs/ListDialog.cpp

HEADERS += gui/MainWindow.hpp \
			gui/widgets/WordCount.hpp \
			gui/dialogs/ListDialog.hpp

FORMS   += gui/MainWindow.ui \
			gui/widgets/WordCount.ui \
			gui/dialogs/ListDialog.ui

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
