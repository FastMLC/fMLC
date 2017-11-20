
#pragma once

//	undef macros
#undef min
#undef max

#include <string>

#ifdef BOOST
	#include <boost/container/vector.hpp>
	using namespace boost::container;
#else
	#include <vector>
	using namespace std;
#endif

vector<std::string> split(const char * srce, char delim);
vector<std::wstring> split(const wchar_t * srce, wchar_t delim);

template <class T>	void RemoveDuplicatesAndSort(vector<T> & p_List);
template <class T>	void Sort(vector<T> & p_List);
template <class T>	bool RemoveDuplicates(vector<T> & p_List);
template <class T>	size_t CountIntersection(const vector<T> & p_SortedList1, const vector<T> & p_SortedList2);
template <class T>	void RemoveFromList(vector<T> & p_SortedList1, const vector<T> & p_SortedList2);

template <class T>	void	OutputDebugVector(std::wostringstream & stream, const vector<T> & p_List);

