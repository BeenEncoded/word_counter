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
#include "gui/dialogs/ListDialog.hpp"

namespace
{
	template<typename string_type, typename stream_type> std::map<string_type, unsigned long> count_words(stream_type&);
	template<typename string_type> std::vector<std::pair<QString, unsigned long> > sorted_list(const std::map<string_type, unsigned long>&);
	template<typename type> QString to_qstring(const type&);
	template<typename string_type> bool compare_result(const std::pair<string_type, unsigned long>&, const std::pair<string_type, unsigned long>&);
	template<typename string_type, typename stream_type> std::vector<string_type> tokenize_into_sentences(stream_type&);

	QString result_display(const std::pair<QString, unsigned long>&);
	QString filter_tostr(const std::pair<QString, unsigned long>&);



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
		throw std::runtime_error{"Incorrect template instantiation!"};
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

	/**
	A flexible word counting algorithm that can count words in different
	languages using the locale of the stream that was passed as the argument.
	Note: it is case insensitive.
	*/
	template<typename string_type, typename stream_type>
	inline std::map<string_type, unsigned long> count_words(stream_type& in)
	{
		string_type word;
		std::map<string_type, unsigned long> count;

		in.peek();
		while(in.good())
		{
			if(std::isalpha(in.peek(), in.getloc())) word += std::tolower(in.peek(), in.getloc());
			else if(!word.empty())
			{
				++count[word];
				word.erase();
			}
			in.get();
		}
		return count;
	}

	/**
	Generates a sorted list of results from the map produced by "count_words".
	*/
	template<typename string_type>
	inline std::vector<std::pair<QString, unsigned long> > sorted_list(const std::map<string_type, unsigned long>& results)
	{
		std::vector<std::pair<QString, unsigned long> > sorts;
		for(std::map<string_type, unsigned long>::const_iterator it{results.begin()}; it != results.end(); ++it)
		{
			sorts.emplace_back(to_qstring(it->first), it->second);
		}
		std::sort(sorts.begin(), sorts.end(), compare_result<QString>);
		return sorts;
	}

	template<typename string_type, typename stream_type>
	inline std::vector<string_type> tokenize_into_sentences(stream_type& in)
	{
		std::vector<string_type> s;
		string_type temps;

		in.peek();
		while(in.good())
		{
			if((in.peek() != '.') && !in.eof()) temps += in.peek();
			else if(in.peek() == '.')
			{
				temps += in.peek();
				s.emplace_back(std::move(temps));
				temps.erase();
				in.get();
				while(in.good() && std::isspace(in.peek(), in.getloc())) in.get(); //skip whitespace
				continue;
			}
			else if(!temps.empty() && in.eof())
			{
				s.emplace_back(std::move(temps));
				temps.erase();
			}
			in.get();
		}
		return s;
	}

	inline QString result_display(const std::pair<QString, unsigned long>& r)
	{
		return (QString::fromStdString(std::to_string(r.second)) + QString{": "} + r.first);
	}

	inline QString filter_tostr(const std::pair<QString, unsigned long>& el)
	{
		return el.first;
	}


}

WordCount::WordCount(QWidget *parent) : 
	QWidget{parent},
	ui{new Ui::WordCount},
	file{},
	filter{std::vector<QString>{}, std::vector<QString>{}},
	words{},
	sentences{}
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
	this->sentences.erase(this->sentences.begin(), this->sentences.end());
	this->filter = data::filter_data{std::vector<QString>{}, std::vector<QString>{}};

	this->file = QFileDialog::getOpenFileName(this, "Open: ", QDir::homePath(), "Text Files (*.txt)");
	if(!boost::filesystem::exists(this->file.toStdString())) return;

	std::wifstream in{this->file.toStdString().c_str(), std::ios::in};
	in.imbue(std::locale(""));
	this->words = sorted_list<std::wstring>(count_words<std::wstring, std::wistream>(in));
	in.close();
	in.open(this->file.toStdString().c_str(), std::ios::in);
	if(in.good()) this->sentences = tokenize_into_sentences<std::wstring, std::wistream>(in);
	in.close();

	this->refreshList();
	this->ui->result_box->setEnabled(true);
}

void WordCount::applyExplicitFilter()
{
	std::map<std::wstring, unsigned long> f = count_words<std::wstring, std::wistream>(std::wistringstream{this->ui->include->text().toStdWString()});
	this->filter.exp.erase(this->filter.exp.begin(), this->filter.exp.end());
	for(auto it = f.begin(); it != f.end(); ++it)
	{
		this->filter.exp.push_back(QString::fromStdWString(it->first));
	}
	this->refreshList();
}

void WordCount::applyExclusiveFilter()
{
	std::map<std::wstring, unsigned long> r{count_words<std::wstring, std::wistream>(std::wistringstream{this->ui->exclude->text().toStdWString()})};
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

void WordCount::showSentences(QListWidgetItem* i)
{
	if(!this->ui->list->selectedItems().empty())
	{
		QString word{data::apply_filter(this->words, this->filter, filter_tostr)[this->ui->list->row(i)].first};
		std::vector<std::wstring*> displayed;

		{
			std::map<std::wstring, unsigned long> tempmap;

			for(std::vector<std::wstring>::iterator it{this->sentences.begin()}; it != this->sentences.end(); ++it)
			{
				tempmap = count_words<std::wstring, std::wistream>(std::wstringstream{*it});
				if(tempmap.find(word.toStdWString()) != tempmap.end())
				{
					displayed.emplace_back(&*it);
				}
			}
		}

		ListDialog dialog{this, word, displayed};
		dialog.setModal(true);
		dialog.exec();
	}
}

