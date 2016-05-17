#include <QApplication>

#include "gui/MainWindow.cpp"

int main(int c, char** v)
{
	QApplication app{c, v};
	MainWindow w;
	w.show();
	return app.exec();
}