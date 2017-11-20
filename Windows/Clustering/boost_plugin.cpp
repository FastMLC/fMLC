
#include "stdafx.h"

#include <sstream>
#include <boost_plugin.h>
#include <algorithm>

#include <unordered_set>

//	*****	boost extra functions	*****

vector<std::string> split(const char * srce, char delim)
{
	vector<std::string> elems;

	std::stringstream ss(srce);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


vector<std::wstring> split(const wchar_t * srce, wchar_t delim)
{
	vector<std::wstring> elems;

	std::wstringstream ss(srce);
	std::wstring item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

//	in place sorting and removing of duplicates
template<class T>
void RemoveDuplicatesAndSort(vector<T> & p_List)
{
	size_t smax = p_List.size();
	if (smax <= 1) {
		return;
	}
	std::sort(p_List.begin(), p_List.end());	//	sort ascending
//remove duplicate
	size_t d = 0;
	for (size_t s = 1; s < smax; ++s) { //	s = source, d = dest
		if (p_List[s] > p_List[d]) { //	keep it
			p_List[++d] = p_List[s];
		}
	}
	p_List.resize(d + 1);
}

//	force compilation
template void RemoveDuplicatesAndSort<uint32_t>(vector<uint32_t> &);
template void RemoveDuplicatesAndSort<uint64_t>(vector<uint64_t> &);
template void RemoveDuplicatesAndSort<double>(vector<double> &);

//	in place sorting and removing of duplicates
template<class T>
void Sort(vector<T> & p_List)
{
	size_t smax = p_List.size();
	if (smax <= 1) {
		return;
	}
	std::sort(p_List.begin(), p_List.end());	//	sort ascending					
}

//	force compilation
template void Sort<uint32_t>(vector<uint32_t> &);
template void Sort<uint64_t>(vector<uint64_t> &);
template void Sort<double>(vector<double> &);


//	removing of duplicates, no sorting.
//	the first duplicated value remains, if any
//	return true if a change occured
template<class T>
bool RemoveDuplicates(vector<T> & p_List)
{
	size_t imax = p_List.size();
	if (imax <= 1) {
		return false;	// no change
	}

	std::unordered_set<T>  idSet;				//	a set containing all values, no duplicates of course (it's a set)
	vector<size_t>		idxToRemove;		//	the list of the indexes of all duplicated values, not including the first occurence

	for (size_t i = 0; i < imax; ++i) {
		const T & id = p_List[i];
		if (idSet.find(id) != idSet.end()) {	//	a duplicates
			idxToRemove.push_back(i);				//	save the index of the value, not the value
		}
		else {
			idSet.emplace(id);
		}
	}
	if (idxToRemove.size() == 0) {
		return false;	//	no change
	}

	vector<size_t>::const_iterator rit = idxToRemove.begin();	//	an iterator on the list of duplicates indexes
	size_t d = *rit;	//	 d = the destination index, start at the first duplicate position

	for (size_t s = d; s < imax; ++s) { //	s = source, d = dest
		if (rit < idxToRemove.end() && s == *rit) {	//	this is a duplicate, skip copy and increment the duplicates iterator
			++rit;
		}
		else {	//	continue copying
			p_List[d] = p_List[s];
			++d;
		}
	}
	p_List.resize(d);
	return true;	// change
}


//	force compilation
template bool RemoveDuplicates<uint32_t>(vector<uint32_t> &);
template bool RemoveDuplicates<uint64_t>(vector<uint64_t> &);


//	count how many items are in common between these two sorted lists
template<class T>
size_t CountIntersection(const vector<T> & p_SortedList1, const vector<T> & p_SortedList2)
{
	size_t len1 = p_SortedList1.size();
	size_t len2 = p_SortedList2.size();

	if (len1 == 0 || len2 == 0) {
		return 0;
	}
	if (p_SortedList1.front() > p_SortedList2.back() || p_SortedList2.front() > p_SortedList1.back()) {	//	no overlap between each list
		return 0;
	}

	size_t n = 0;	//	counter

	T x = p_SortedList1.front();
	T y = p_SortedList2.front();

	size_t i = 0, j = 0, imax = p_SortedList1.size(), jmax = p_SortedList2.size();
	for (; ; ) {
		if (x == y) {
			++n;
			if (++i >= imax) {
				break;
			}
			if (++j >= jmax) {
				break;
			}
			x = p_SortedList1[i];
			y = p_SortedList2[j];
		}
		else if (x < y) {
			if (++i >= imax) {
				break;
			}
			x = p_SortedList1[i];
		}
		else {
			if (++j >= jmax) {
				break;
			}
			y = p_SortedList2[j];
		}
	}
	return n;
}

template size_t CountIntersection<uint32_t>(const vector<uint32_t> & p_SortedList1, const vector<uint32_t> & p_SortedList2);
template size_t CountIntersection<uint64_t>(const vector<uint64_t> & p_SortedList1, const vector<uint64_t> & p_SortedList2);


//	all items found in list 2 are removed from list 1. Note that if list contains value x, x is removed only once from list 1.
//	both lists must already be sorted, ascending
template<class T>
void RemoveFromList(vector<T> & p_SortedList1, const vector<T> & p_SortedList2)
{
	size_t len1 = p_SortedList1.size();
	size_t len2 = p_SortedList2.size();

	if (len1 == 0 || len2 == 0) {
		return;
	}
	if (p_SortedList1.front() > p_SortedList2.back() || p_SortedList2.front() > p_SortedList1.back()) {	//	no overlap between each list
		return;
	}

	size_t s = 0, smax = p_SortedList1.size();
	size_t d = 0;
	for (size_t i = 0, imax = p_SortedList2.size(); s < smax && i < imax; ) { //	s = source, d = dest, i = index in p_SortedList2
		T idx = p_SortedList1[s];
		T centroid = p_SortedList2[i];
		if (idx < centroid) { //	keep it, most common case first
			p_SortedList1[d] = p_SortedList1[s];
			++d;
			++s;
		}
		else if (idx == centroid) { //	remove it, second most common case
			++i;
			++s;
		}
		else {	//	idx > centroid
			++i;
		}
	}

	//		copy the remaining items in case we left due to (i < imax)
	for (; s < smax; ++s, ++d) {
		p_SortedList1[d] = p_SortedList1[s];
	}
	p_SortedList1.resize(d);	//	d is the index of the next item to add (= new size)
}

template void RemoveFromList<uint32_t>(vector<uint32_t> & p_SortedList1, const vector<uint32_t> & p_SortedList2);
template void RemoveFromList<uint64_t>(vector<uint64_t> & p_SortedList1, const vector<uint64_t> & p_SortedList2);


template <class T>
void
OutputDebugVector(std::wostringstream & stream, const vector<T> & p_List)
{
	for (const T & c : p_List) {
		stream << c << L", ";
	}
	stream << L"\r\n";
}


template <class T>
void
OutputDebugVector(std::ostringstream & stream, const vector<T> & p_List)
{
	for (const T & c : p_List) {
		stream << c << ", ";
	}
	stream << "\r\n";
}

template void OutputDebugVector<uint32_t>(std::wostringstream & stream, const vector<uint32_t> & p_List);
template void OutputDebugVector<uint64_t>(std::wostringstream & stream, const vector<uint64_t> & p_List);
template void OutputDebugVector<double>(std::wostringstream & stream, const vector<double> & p_List);

template void OutputDebugVector<uint32_t>(std::ostringstream & stream, const vector<uint32_t> & p_List);
template void OutputDebugVector<uint64_t>(std::ostringstream & stream, const vector<uint64_t> & p_List);
template void OutputDebugVector<double>(std::ostringstream & stream, const vector<double> & p_List);

