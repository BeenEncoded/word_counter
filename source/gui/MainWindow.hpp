#ifndef GUI_MAINWINDOW_HPP_INCLUDED
#define GUI_MAINWINDOW_HPP_INCLUDED
#include <QMainWindow>
#include <QWidget>

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* = nullptr);
	~MainWindow();

private:
	Ui::MainWindow *ui;
};

#endif