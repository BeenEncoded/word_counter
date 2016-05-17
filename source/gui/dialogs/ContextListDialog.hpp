#pragma once
#ifndef CONTEXTLISTDIALOG_H
#define CONTEXTLISTDIALOG_H
#include <QDialog>
#include <vector>
#include <string>


namespace Ui
{
	class ContextListDialog;
}

class ContextListDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ContextListDialog(QWidget*, const std::vector<std::wstring>&);
	~ContextListDialog();

private:
	Ui::ContextListDialog *ui;

};


#endif // CONTEXTLISTDIALOG_H
