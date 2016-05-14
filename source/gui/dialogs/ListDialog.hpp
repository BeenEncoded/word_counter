#pragma once
#ifndef LISTDIALOG_H
#define LISTDIALOG_H
#include <QDialog>
#include <string>
#include <QString>
#include <vector>


namespace Ui
{
	class ListDialog;
}

class ListDialog : public QDialog {
	Q_OBJECT

public:
	explicit ListDialog(QWidget*, const QString&, const std::vector<std::wstring*>&);
	~ListDialog();

private:
	Ui::ListDialog *ui;
};


#endif // LISTDIALOG_H
