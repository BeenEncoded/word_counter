#include <QWidget>
#include <map>

#include "WordCount.hpp"
#include "ui_WordCount.h"

namespace
{

}

WordCount::WordCount(QWidget *parent) : 
	QWidget{parent},
	ui{new Ui::WordCount}
{
	this->ui->setupUi(this);
}

WordCount::~WordCount()
{
	delete this->ui;
}

