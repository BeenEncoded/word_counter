#include <QWidget>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <exception>
#include <locale>
#include <stdexcept>
#include <vector>
#include <utility>
#include <QString>
#include <algorithm>
#include <QFileDialog>
#include <QDir>
#include <boost/filesystem.hpp>
#include <QMessageBox>
#include <sstream>

#include "WordCount.hpp"
#include "ui_WordCount.h"
#include "algo.hpp"
#include "gui/dialogs/ContextListDialog.hpp"
#include "algo.hpp"

namespace
{
	QString filter_tostr(const std::pair<QString, unsigned long>&);
	QString result_display(const std::pair<QString, unsigned long>&);
	unsigned int total_words(const std::vector<std::pair<QString, unsigned int> >&);

	template<typename string_type> bool                                            compare_result(const std::pair<string_type, unsigned long>&, const std::pair<string_type, unsigned long>&);
	template<typename type>        QString                                         to_qstring(const type&);
	template<typename string_type> std::vector<std::pair<QString, unsigned long> > sorted_list(const std::map<string_type, unsigned long>&);



	/**
	Generates a sorted list of results from the map produced by "count_words".
	*/
	template<typename string_type>
	inline std::vector<std::pair<QString, unsigned long> > sorted_list(const std::map<string_type, unsigned long>& results)
	{
		std::vector<std::pair<QString, unsigned long> > sorts;
		for (std::map<string_type, unsigned long>::const_iterator it{ results.begin() }; it != results.end(); ++it)
		{
			sorts.emplace_back(to_qstring(it->first), it->second);
		}
		std::sort(sorts.begin(), sorts.end(), compare_result<QString>);
		return sorts;
	}

	inline QString filter_tostr(const std::pair<QString, unsigned long>& el)
	{
		return el.first;
	}

	inline QString result_display(const std::pair<QString, unsigned long>& r)
	{
		return (QString::fromStdString(std::to_string(r.second)) + QString{ ": " } +r.first);
	}

	template<typename string_type>
	inline bool compare_result(const std::pair<string_type, unsigned long>& p1, const std::pair<string_type, unsigned long>& p2)
	{
		return (p1.second > p2.second);
	}

	/**
	Some template metaprogramming to convert arbitrary string
	types to qstring.
	*/
	template<typename type>
	inline QString to_qstring(const type& t)
	{
		throw std::runtime_error{ "Incorrect template instantiation!" };
	}

	template<>
	inline QString to_qstring<std::string>(const std::string& s)
	{
		return QString::fromStdString(s);
	}

	template<>
	inline QString to_qstring<std::wstring>(const std::wstring& s)
	{
		return QString::fromStdWString(s);
	}

	inline unsigned long total_words(const std::vector<std::pair<QString, unsigned long> >& words)
	{
		unsigned long t{0};
		for(auto it{words.begin()}; it != words.end(); ++it)
		{
			t += it->second;
		}
		return t;
	}


}

WordCount::WordCount(QWidget *parent) : 
	QWidget{parent},
	ui{new Ui::WordCount},
	file{},
	filter{std::vector<QString>{}, std::vector<QString>{}},
	words{}
{
	this->ui->setupUi(this);
}

WordCount::~WordCount()
{
	delete this->ui;
}

void WordCount::openFile()
{
	this->words.erase(this->words.begin(), this->words.end());
	this->filter = data::filter_data{std::vector<QString>{}, std::vector<QString>{}};
	this->file = "";

	this->file = QFileDialog::getOpenFileName(this, "Open: ", QDir::homePath(), "Text Files (*.txt)");
	if(!boost::filesystem::exists(this->file.toStdString())) return;

	std::wifstream in{this->file.toStdString().c_str(), std::ios::binary};
	in.imbue(std::locale(""));
	this->words = sorted_list<std::wstring>(algo::count_words<std::wstring, std::wistream>(in));
	in.close();

	this->ui->word_count->setText(QString::fromStdString(std::to_string(this->words.size())));
	this->ui->total->setText(QString::fromStdString(std::to_string(total_words(this->words))));
	this->refreshList();
	this->ui->result_box->setEnabled(true);
}

void WordCount::applyExplicitFilter()
{
	std::map<std::wstring, unsigned long> f{algo::count_words<std::wstring, std::wistream>(std::wistringstream{this->ui->include->text().toStdWString()})};
	this->filter.exp.erase(this->filter.exp.begin(), this->filter.exp.end());
	for(auto it = f.begin(); it != f.end(); ++it)
	{
		this->filter.exp.push_back(QString::fromStdWString(it->first));
	}
	this->refreshList();
}

void WordCount::applyExclusiveFilter()
{
	std::map<std::wstring, unsigned long> r{algo::count_words<std::wstring, std::wistream>(std::wistringstream{this->ui->exclude->text().toStdWString()})};
	this->filter.exc.erase(this->filter.exc.begin(), this->filter.exc.end());
	for(auto it = r.begin(); it != r.end(); ++it)
	{
		this->filter.exc.push_back(QString::fromStdWString(it->first));
	}
	this->refreshList();
}

void WordCount::refreshList()
{
	std::vector<std::pair<QString, unsigned long> > w{data::apply_filter(this->words, this->filter, filter_tostr)};
	this->ui->list->clear();
	for(unsigned int x{0}; x < w.size(); ++x)
	{
		this->ui->list->addItem(result_display(w[x]));
	}
}

void WordCount::displayContexts(QListWidgetItem* i)
{
	if(!this->ui->list->selectedItems().empty())
	{
		std::vector<std::wstring> con;
		std::wstring word{data::apply_filter(this->words, this->filter, filter_tostr)[this->ui->list->row(i)].first.toStdWString()};

		std::wifstream in{this->file.toStdString().c_str(), std::ios::binary};
		if(in.good())
		{
			con = algo::contexts<std::wistream, std::wstring>(in, word, this->ui->concount->value());
		}
		in.close();

		ContextListDialog diag{this, con};
		diag.setModal(true);
		diag.exec();
	}
}

