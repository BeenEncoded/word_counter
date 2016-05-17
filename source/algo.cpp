#include <utility>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <QString>
#include <locale>
#include <QMessageBox>

#include "algo.hpp"

namespace
{
	using pos_type = long;
	constexpr decltype(std::ios::beg) beg_dir{std::ios::beg};
	constexpr decltype(std::ios::end) end_dir{std::ios::end};

	template<typename stream_t, typename string_t>  string_t next_word(stream_t&);
	template<typename string_t>                     string_t lcase(const string_t&, const std::locale&);
	template<typename istream_t>                    pos_type begin_pos(istream_t&);
	template<typename istream_t, typename char_t>   char_t   rget(istream_t&);
	template<typename istream_t, typename char_t>   char_t   rpeek(istream_t&);
	template<typename istream_t, typename string_t> string_t extract_words(istream_t&, const unsigned int&, const std::ios_base::seekdir&);
	template<typename stream_t, typename string_t>  string_t getword(stream_t&);
	template<typename stream_t, typename string_t>  string_t rgetword(stream_t&);



	template<typename string_t>
	inline string_t lcase(const string_t& s, const std::locale& loc)
	{
		string_t temps{s};
		for(auto it = temps.begin(); it != temps.end(); ++it)
		{
			(*it) = std::tolower(*it, loc);
		}
		return temps;
	}

	template<typename stream_t, typename string_t>
	inline string_t next_word(stream_t& in)
	{
		string_t word;

		in.peek();
		while(in.good() && !std::isalpha(in.peek(), in.getloc())) in.get(); //eliminate trailing non-words
		while(in.good() && std::isalpha(in.peek(), in.getloc())) word += in.get();
		return word;
	}

	/*
	Behavior should be identical to that of getline, except 
	that instead of using a delimiter to denote the end of a read,
	it uses a word to denote the end of a read.
	*/
	template<typename stream_t, typename string_t>
	inline string_t getword(stream_t& in)
	{
		string_t temps;
		while(in.good() && !std::isalpha(in.peek(), in.getloc())) temps += in.get();
		while(in.good() && std::isalpha(in.peek(), in.getloc())) temps += in.get();
		return temps;
	}

	template<typename stream_t, typename string_t>
	inline string_t rgetword(stream_t& in)
	{
		string_t temps;
		wchar_t tempc;
		while(in.good() && !std::isalpha(rpeek<stream_t, wchar_t>(in), in.getloc()))
		{
			tempc = rget<stream_t, wchar_t>(in);
			if(!in.eof()) temps.insert(temps.begin(), tempc);
		}
		while(in.good() && std::isalpha(rpeek<stream_t, wchar_t>(in), in.getloc()))
		{
			tempc = rget<stream_t, wchar_t>(in);
			if(!in.eof()) temps.insert(temps.begin(), tempc);
		}
		return temps;
	}

	template<typename istream_t>
	inline pos_type begin_pos(istream_t& in)
	{
		pos_type current{in.tellg()};
		in.seekg(0, istream_t::beg);
		pos_type begin{in.tellg()};
		in.seekg(current);
		return begin;
	}

	/*
	Returns the previous character.  If at the beginning, sets eof and
	returns the first character in the stream.
	*/
	template<typename istream_t, typename char_t>
	inline char_t rget(istream_t& in)
	{
		pos_type p{in.tellg()};
		in.seekg((p > -1) ? (--p) : p);
		char_t c{(char_t)in.get()};
		in.seekg(p);
		if((pos_type)in.tellg() < 0) in.setstate(in.rdstate() | std::ios::eofbit);
		return c;
	}

	template<typename istream_t, typename char_t>
	inline char_t rpeek(istream_t& in)
	{
		pos_type pos{in.tellg()};
		char_t c{rget<istream_t, char_t>(in)};
		in.seekg(pos);
		return c;
	}

	template<typename istream_t, typename string_t>
	inline string_t extract_words(istream_t& in, const unsigned int& count, const std::ios_base::seekdir& dir)
	{
		string_t words;
		pos_type p{in.tellg()};
		std::ios::iostate original_state{in.rdstate()};

		if(dir == std::ios_base::end)
		{
			for(unsigned int x{0}; ((x < count) && in.good()); ++x)
			{
				words += getword<istream_t, string_t>(in);
			}
		}
		else if(dir == std::ios_base::beg)
		{
			for(unsigned int x{ 0 }; ((x < count) && in.good()); ++x)
			{
				words = (rgetword<istream_t, string_t>(in) + words);
			}
			if(in.eof() && !in.bad() && !in.fail())
			{
				std::ios::iostate state{in.rdstate()};
				in.clear();
				in.setstate(state & ~std::ios::eofbit);
			}
		}

		/*if a failure occured, return without clearing the state: */
		if(in.bad() || in.eof())
		{
			return words;
		}
		in.clear();
		in.setstate(original_state);
		in.seekg(p);
		return words;
	}


}

//functions exposed to the user:
namespace algo
{
	/**
	A flexible word counting algorithm that can count words in different
	languages using the locale of the stream that was passed as the argument.
	Note: it is case insensitive.  All results are lowercase.
	*/
	template<typename string_type, typename stream_type>
	std::map<string_type, unsigned long> count_words(stream_type& in)
	{
		std::map<string_type, unsigned long> count;

		in.peek();
		while (in.good()) ++count[lcase(next_word<stream_type, string_type>(in), in.getloc())];
		return count;
	}

	/**
	Seperates the file into context items that pertain to a particular word, given the number
	of words to pad the target word with.
	*/
	template<typename stream_t, typename string_t>
	std::vector<string_t> contexts(stream_t& in, const string_t& word, const unsigned int& num)
	{
		std::vector<string_t> results;
		pos_type current_pos;

		while(in.good())
		{
			if(lcase(next_word<stream_t, string_t>(in), in.getloc()) == word)
			{
				current_pos = in.tellg();
				results.push_back(extract_words<stream_t, string_t>(in, (num + 1), beg_dir));
				results.back() += extract_words<stream_t, string_t>(in, num, end_dir);
				in.seekg(current_pos);
			}
		}
		return results;
	}


	//explicit template instabtiations:

	template std::map<std::wstring, unsigned long> count_words<std::wstring, std::wistream>(std::wistream&);
	template std::map<std::string, unsigned long> count_words<std::string, std::istream>(std::istream&);

	template std::vector<std::wstring> contexts<std::wistream, std::wstring>(std::wistream&, const std::wstring&, const unsigned int&);
	template std::vector<std::string> contexts<std::istream, std::string>(std::istream&, const std::string&, const unsigned int&);
}