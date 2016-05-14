#include <vector>
#include <QString>
#include <string>

#include "ListDialog.hpp"
#include "ui_ListDialog.h"

ListDialog::ListDialog(QWidget* parent, const QString& word, const std::vector<std::wstring*>& l) : 
	QDialog{parent},
	ui{new Ui::ListDialog}
{
	ui->setupUi(this);
	this->ui->groupbox->setTitle(QString{"Sentences containing the word \""} + 
		word + QString{"\":"});
	for(auto it = l.begin(); it != l.end(); ++it)
	{
		this->ui->list->addItem(QString::fromStdWString(**it));
	}
	this->ui->count->setText(QString{"Number of sentences: "} + QString::fromStdString(std::to_string(l.size())));
}

ListDialog::~ListDialog()
{
	delete ui;
}

