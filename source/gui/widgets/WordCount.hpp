#ifndef WIDGETS_WORDCOUNT_HPP_INCLUDED
#define WIDGETS_WORDCOUNT_HPP_INCLUDED
#include <QWidget>

namespace Ui
{
	class WordCount;
}

class WordCount : public QWidget
{
	Q_OBJECT

public:
	explicit WordCount(QWidget* = nullptr);
	~WordCount();

public slots:
	void openFile();

private:
	Ui::WordCount* ui;
	QString file;
};

#endif