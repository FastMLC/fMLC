
#pragma once

//	undef macros
#undef min
#undef max

#include <algorithm> //	for vector, fabs()

#include <string>
#include <vector> //	for vector, fabs()

#include <cstdint>
//	*****	std extra functions	*****

//	japanese testing string L"日本語"

namespace std
{
// convert UTF-8 string to wstring
std::wstring utf8_to_wstring(const std::string & str);

// convert wstring to UTF-8 string
std::string wstring_to_utf8(const std::wstring & str);

// convert ascii to wstring
std::wstring ascii_to_wstring(const std::string & p_Srce);

// convert wstring to ascii
std::string wstring_to_ascii(const std::wstring & p_Srce);


std::vector<std::string> & split(const std::string & srce, char delim, std::vector<std::string> & elems);

void reverse(std::string & srce);


template <class T>
int64_t
strtoi64(const T * ptr, const T ** endptr)
{
	// '+' = 43; '-' = 45, '0-9' = 48 - 57
	static const uint32_t MapTable[] = {
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		32, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 43, 88, 45, 88, 88, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88
	};
	uint64_t ux = 0;
	int32_t Sign = 1; //	positive by default

	//	scan header
	for (; *ptr; ++ptr) {
		int32_t d = MapTable[static_cast<unsigned char>(* ptr)];
		if (d < 10) { //	valid number
			break ;
		}
		if (d == 45) { //	'-'
			Sign = - Sign ;
		}
		else if (d <= 43) { //	' ' and '+'
			//	do nothing
		}
		else { // invalid character
			return 0 ;
		}
	}
	//	scan values
	int64_t x;
	try {
		for (; *ptr; ++ptr) {
			int32_t d = MapTable[static_cast<unsigned char>(* ptr)];
			if (d < 10) { //	valid number
				ux *= 10 ;
				ux += d ;
			}
			else { // invalid character
				break ;
			}
		}
		x = ux * Sign ; //	may fail !
	}
	catch (...) { //	overflow
		if (Sign == 1) {
			x = INT64_MAX ;
		}
		else {
			x = INT64_MIN ;
		}
	}
	* endptr = ptr ; //	return the next char to read
	return x ;
}


template <class T>
int32_t
strtoi(const T * ptr, const T ** endptr)
{
	// '+' = 43; '-' = 45, '0-9' = 48 - 57
	static const uint32_t MapTable[] = {
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		32, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 43, 88, 45, 88, 88, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88
	};
	uint32_t ux = 0;
	int32_t Sign = 1; //	positive by default

	//	scan header
	for (; *ptr; ++ptr) {
		int32_t d = MapTable[static_cast<unsigned char>(* ptr)];
		if (d < 10) { //	valid number
			break ;
		}
		if (d == 45) { //	'-'
			Sign = - Sign ;
		}
		else if (d <= 43) { //	' ' and '+'
			//	do nothing
		}
		else { // invalid character
			return 0 ;
		}
	}
	//	scan values
	int32_t x;
	try {
		for (; *ptr; ++ptr) {
			int32_t d = MapTable[static_cast<unsigned char>(* ptr)];
			if (d < 10) { //	valid number
				ux *= 10 ;
				ux += d ;
			}
			else { // invalid character
				break ;
			}
		}
		x = ux * Sign ; //	may fail !
	}
	catch (...) { //	overflow
		if (Sign == 1) {
			x = INT32_MAX ;
		}
		else {
			x = INT32_MIN ;
		}
	}
	* endptr = ptr ; //	return the next char to read
	return x ;
}


//	***************************************	FillList	********************************

struct OnResultPr
{
	bool
	operator()(const std::pair<int32_t, int32_t> & a, const std::pair<int32_t, int32_t> & b) const
	{
		return (a.first < b.first) ;
	}
};


//	increment srce to point to "+-0123456789\0", ASCII 43, 45, 48-57
template <class T>
const T *
NextNum(const T * srce)
{
	const bool Invalid[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1};

	while (* srce >= 64 || Invalid[* srce]) {
		++srce ;
	}
	return (srce) ;
}

//	a record ID is an integer, also in the 64 bit version
typedef std::pair<int32_t, int32_t> range;

//	srce points to a list of integers representing ranges : "1, 5, 2, 10-15, 40-60, 17, 22-25, 30"
//	these values are added to the destination vector which is sorted according to p_Sort. Duplicates are removed if touching each other. See PRC #408
//	Call p_vRange.reserve() before calling this function
template <class T> //	T = char or wchar_t
void
GetRange(std::vector<range> & p_vRange, const T * p_Srce, bool p_Sort)
{
	if (! p_Srce) {
		return;
	}
	const T * ptr = p_Srce;
	const T * ptr1 = nullptr;
	const T * ptr2 = nullptr;
	while (* ptr) {
		ptr = NextNum(ptr) ;
		if (* ptr == 0) {
			break ;
		}
		int32_t a = std::strtoi(ptr, & ptr1);
		if (*ptr1 == '-') { //	that's a range
			int32_t b = std::strtoi(++ptr1, & ptr2);
			if (a > b) {
				std::swap(a, b) ;
			}
			p_vRange.push_back(range(a, b)) ;
			ptr = ptr2 ;
		}
		else { //	not a range
			if (p_vRange.size() > 0 && p_vRange.back().second + 1 == a) { //	extend the range, useful for a list like "5, 6, 7, 8, 9, 10" => "5 - 10"
				p_vRange.back().second = a ;
			}
			else { //	add a single value
				p_vRange.push_back(range(a, a)) ;
			}
			ptr = ptr1 ;
		}
	}

	if (p_Sort) {
		std::sort(p_vRange.begin(), p_vRange.end(), OnResultPr()) ; //	sort by "first"
	}

	//	in place removal of duplicates only if they touch each other, like "0, 1, 2, 3-12, 13" => "0-13"
	size_t j = 0; //	j is the destination
	for (size_t i = 1; i < p_vRange.size(); ++i) { //	i is the source
		range & dest = p_vRange[j];
		range & srce = p_vRange[i];
		//	check it the min or the max values of dest touches the srce range
		if (dest.second < srce.first - 1 || dest.first > srce.second + 1) { //	no merge
			p_vRange[++j] = p_vRange[i] ;
		}
		else { // merge
			dest.first = std::min(dest.first, srce.first) ;
			dest.second = std::max(dest.second, srce.second) ;
		}
	}
	p_vRange.resize(++j) ;
	return;
}

/*	explanation : (byte per byte)
	x		x			x-1	x-1		~x		~x
	0		0x00		255	0xFF		255	0xFF
	1		0x01		0		0x00		254	0xFE
	2		0x02		1		0x01		253	0xFD
	...
	127	0x7F		126	0x7E		128	0x80
	128	0x80		127	0x7F		127	0x7F
	129	0x81		128	0x80		126	0x7E
	...
	255	0xFF		254	0xFE		0		0x00

	(x - 1)			set the top bit to 1 for values [0, 129-255]
	~x					set the top bit to 1 for values [0-127]
	(x - 1) & ~x	set the top bit to 1 for value  [0]
	& 0x80808080		check if any top bit of every byte is 1
*/

inline bool
HasZeroByte(unsigned short x)
{
	return ((x - 0x0101) & ~x & 0x8080) != 0 ;
}


inline bool
HasZeroByte(uint32_t x)
{
	return ((x - 0x01010101) & ~x & 0x80808080) != 0 ;
}


inline bool
HasZeroByte(uint64_t x)
{
	return ((x - 0x0101010101010101) & ~x & 0x8080808080808080) != 0 ;
}

/*
template <class T, class U, class V>
class trio {
public :
	trio(T p_first, U p_second, V p_third)
		: m_first(p_first), m_second(p_second), m_third(p_third)
	{
	}

	T & first() const {
		return m_first ;
	}

	U & second() const {
		return m_scond ;
	}
	
	V & third() const {
		return m_third ;
	}
	
protected :
	T		m_first ;
	U		m_second ;
	V		m_third ;
} ;
*/

} //	end of namespace std

