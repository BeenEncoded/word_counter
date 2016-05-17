#include <vector>
#include <string>
#include <QString>

#include "ContextListDialog.hpp"
#include "ui_contextlistdialog.h"

ContextListDialog::ContextListDialog(QWidget* parent, const std::vector<std::wstring>& l) : 
	QDialog{parent},
	ui{new Ui::ContextListDialog}
{
	this->ui->setupUi(this);
	for(unsigned int x{0}; x < l.size(); ++x)
	{
		this->ui->list->addItem(QString::fromStdWString(l[x]));
	}
	this->ui->count->setText(QString::fromStdString(std::to_string(l.size())));
}

ContextListDialog::~ContextListDialog()
{
	delete this->ui;
}

