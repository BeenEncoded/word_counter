#include <QWidget>
#include <QMainWindow>

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "gui/widgets/WordCount.hpp"

MainWindow::MainWindow(QWidget* parent) : 
	QMainWindow{parent},
	ui{new Ui::MainWindow}
{
	this->ui->setupUi(this);
	this->setCentralWidget(new WordCount);
}

MainWindow::~MainWindow()
{
	delete this->ui;
}