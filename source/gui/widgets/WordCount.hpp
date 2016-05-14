#ifndef WIDGETS_WORDCOUNT_HPP_INCLUDED
#define WIDGETS_WORDCOUNT_HPP_INCLUDED
#include <QWidget>
#include <vector>
#include <utility>
#include <QString>
#include <algorithm>
#include <QListWidgetItem>

namespace Ui
{
	class WordCount;
}

namespace data
{
	struct filter_data;

	template<class list_t, class func_t> list_t apply_filter(const list_t&, const filter_data&, const func_t&);



	struct filter_data
	{
		std::vector<QString> exp, exc; //explicit and exclusive filter lists
	};

	/**
	Applies the filter given an original list, a filter, and a function that returns
	a QString given an element of list_t.  Can be implicitly instantiated for convenience.
	*/
	template<typename list_t, typename func_t>
	inline list_t apply_filter(const list_t& original, const filter_data& filter, const func_t& to_str)
	{
		list_t newlist;

		if(filter.exp.empty())
		{
			newlist = original;
		}
		for(list_t::const_iterator it{original.begin()}; it != original.end(); ++it)
		{
			if(std::find(filter.exp.begin(), filter.exp.end(), to_str(*it)) != filter.exp.end())
			{
				newlist.insert(newlist.end(), *it);
			}
		}
		for(list_t::iterator it{newlist.begin()}; it != newlist.end();)
		{
			if(std::find(filter.exc.begin(), filter.exc.end(), to_str(*it)) != filter.exc.end())
			{
				it = newlist.erase(it);
			}
			else ++it;
		}
		return newlist;
	}


}

class WordCount : public QWidget
{
	Q_OBJECT

public:
	explicit WordCount(QWidget* = nullptr);
	~WordCount();

public slots:
	void openFile();
	void applyExplicitFilter();
	void applyExclusiveFilter();
	void refreshList();
	void showSentences(QListWidgetItem*);

private:
	Ui::WordCount* ui;
	QString file;
	data::filter_data filter;
	std::vector<std::pair<QString, unsigned long> > words;
	std::vector<std::wstring> sentences;
};

#endif