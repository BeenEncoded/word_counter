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

#include "WordCount.hpp"
#include "ui_WordCount.h"

namespace
{
	template<typename string_type, typename stream_type> std::map<string_type, unsigned long> count_words(stream_type&);
	template<typename type> bool flex_isalpha(type&);
	template<typename string_type> std::vector<std::pair<QString, unsigned long> > sorted_list(const std::map<string_type, unsigned long>&);
	template<typename type> QString to_qstring(const type&);
	template<typename string_type> bool compare_result(const std::pair<string_type, unsigned long>&, const std::pair<string_type, unsigned long>&);



	template<typename string_type>
	inline bool compare_result(const std::pair<string_type, unsigned long>& p1, const std::pair<string_type, unsigned long>& p2)
	{
		return (p1.second > p2.second);
	}

	/**
	A bit of template metaprogramming to use isalpha differently
	depending on the type of stream being used:
	*/
	template<typename type>
	inline bool flex_isalpha(type& in)
	{
		throw std::runtime_error{"Template did not resolve to one that's been instantiated!"};
	}

	template<>
	inline bool flex_isalpha<std::istream>(std::istream& in)
	{
		return std::isalpha(in.peek(), std::locale{""});
	}

	template<>
	inline bool flex_isalpha<std::wistream>(std::wistream& in)
	{
		return std::isalpha(in.peek(), in.getloc());
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
	*/
	template<typename string_type, typename stream_type>
	inline std::map<string_type, unsigned long> count_words(stream_type& in)
	{
		string_type word;
		std::map<string_type, unsigned long> count;

		in.peek();
		while(in.good())
		{
			if(flex_isalpha(in)) word += in.peek();
			else
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


}

WordCount::WordCount(QWidget *parent) : 
	QWidget{parent},
	ui{new Ui::WordCount},
	file{}
{
	this->ui->setupUi(this);
}

WordCount::~WordCount()
{
	delete this->ui;
}

void WordCount::openFile()
{
	this->file = QFileDialog::getOpenFileName(this, "Open: ", QDir::homePath(), "Text Files (*.txt)");
	if(!boost::filesystem::exists(this->file.toStdString())) return;

	std::wifstream in{this->file.toStdString().c_str(), std::ios::in};
	in.imbue(std::locale(""));
	auto results = sorted_list<std::wstring>(count_words<std::wstring, std::wistream>(in));
	in.close();

	this->ui->list->clear();
	for(unsigned int x{0}; x < results.size(); ++x)
	{
		this->ui->list->addItem(QString::fromStdString(std::to_string(results[x].second)) + QString{": "} + results[x].first);
	}
	this->ui->result_box->setEnabled(true);
}

