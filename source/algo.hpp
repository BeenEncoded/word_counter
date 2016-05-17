/**

Various algorithms used in this program.

*/


#ifndef ALGO_HPP_INCLUDED
#define ALGO_HPP_INCLUDED
#include <map>

namespace algo
{
	template<typename string_type, typename stream_type> std::map<string_type, unsigned long> count_words(stream_type&);
	template<typename stream_t, typename string_t>       std::vector<string_t>                contexts(stream_t&, const string_t&, const unsigned int&);

}

#endif