
//#include "stdafx.h"

#include "double_vector.h"
//#include "mmsystem.h"	//	for timeGetTime()
#include "std_plugin.h"
#include "tostring.h"

#include <cmath>
#include <climits>
#include <fstream>      // std::ofstream
#include <iostream>
#include <random>

#include <boost/lexical_cast.hpp>
#include "toolbox_tl.h"


double_vector::double_vector()
	: std::vector<double>()
{
}


double_vector::double_vector(const double_vector & srce)
	: std::vector<double>(srce)
{
}


double_vector::double_vector(size_t p_Count)
	: std::vector<double>()
{
	resize(p_Count);
}


//	allow writing: double_vector pos = { 1.0, 2.0, 3.0 };
double_vector::double_vector(std::initializer_list<value_type> il, const allocator_type & a)
	:std::vector<double>(il, a)
{
}

double_vector::double_vector(size_t p_Count, double x)
	: std::vector<double>()
{
	resize(p_Count, x);
}


double_vector::double_vector(const double *p_First, const double *p_Last)
	:double_vector((size_t)(p_Last - p_First))
{
	for (const double * ptr = p_First; ptr <= p_Last; ++ptr) {
		push_back(*ptr);
	}
}

void
double_vector::operator *=(double p)
{
	for (double & x : *this) {
		x *= p;
	}
}


void
double_vector::operator /=(double p)
{
	for (double & x : *this) {
		x /= p;
	}
}


void
double_vector::operator +=(double p)
{
	for (double & x : *this) {
		x += p;
	}
}


void
double_vector::operator+=(const double_vector & p)
{
	if (size() != p.size()) {
		std::string errorMessage = std::string("Invalid vector size");
		throw std::out_of_range(errorMessage);
	}

	size_t i = 0;
	for (double & x : *this) {
		x += p[i];
		++i;
	}
}


void
double_vector::operator-=(const double_vector & p)
{
	if (size() != p.size()) {
		std::string errorMessage = std::string("Invalid vector size");
		throw std::out_of_range(errorMessage);
	}

	size_t i = 0;
	for (double & x : *this) {
		x -= p[i];
		++i;
	}
}


void
double_vector::operator -=(double p)
{
	for (double & x : *this) {
		x -= p;
	}
}



//	generate a random matrix
const double_vector &
double_vector::Random()
{
	////	see inverse matrix here :http://en.wikipedia.org/wiki/Gaussian_elimination

	std::default_random_engine generator;
    
    long t = static_cast<long int> (time(NULL));
    
	generator.seed(t);
	std::uniform_real_distribution<double> distribution(0.0, 1.0);
	for (double & x : *this) {
		x = distribution(generator);
		x = (int32_t)std::floor(20.0 * x - 9.5);	//	-10 to 10
	}
	return *this;
}


double_vector &
double_vector::PowMe(double p)
{
	for (double & x : *this) {
		x = std::pow(x, p);
	}
	return *this;
}


//	return this->Pow();
const double_vector
double_vector::Pow(double p) const
{
	double_vector mat(*this);
	return mat.PowMe(p);
}


double_vector &
double_vector::Pow2Me()
{
	for (double & x : *this) {
		x = x * x;
	}
	return *this;
}


//	return this->Pow2();
const double_vector
double_vector::Pow2() const
{
	double_vector vec(*this);
	return vec.Pow2Me();
}


double_vector &
double_vector::RootMe()
{
	for (double & x : *this) {
		x = x < 0.0 ? cNaN : std::sqrt(x);
	}
	return *this;
}


//	return this->Root();
const double_vector
double_vector::Root() const
{
	double_vector vec(*this);
	return vec.RootMe();
}

//	for 10 M items:
//	254 ms using		for (double x : *this)
//	254 ms using		for (const double & x : *this)
//	which makes sense, as both x have the same lenght of 64 bits
double
double_vector::Average() const
{
	double tot = static_cast<double>(size());	//	ok up to 51 bits...
	double avg = 0;
	for (const double & x : *this) {
		avg += x / tot;	//	avoid an overflow
	}
	return avg;
}

//	return the length of the vector
double
double_vector::Magnitude() const
{
	double s2 = 0;
	for (const double & x : *this) {
		s2 += x * x;	//	may quickly overflow, but used only for 2 and 3 D vectors
	}
	return std::sqrt(s2);
}


bool
double_vector::IsNull() const
{
	for (const double & x : *this) {
		if (x != 0.0) {
			return false;	//	not null
		}
	}
	return true;
}


double_vector &
double_vector::ComplementMe(double p)
{
	for (double & x : *this) {
		x = p - x;
	}
	return *this;
}


//	return this->Complement();
const double_vector
double_vector::Complement(double p) const
{
	double_vector vec(*this);
	return vec.ComplementMe(p);
}


//	two steps process, no other way.
double_vector &
double_vector::NormalizeMe()
{
	//	compute the vector length
	double len = 0.0;
	for (double x : *this) {
		len += x * x;
	}
	len = std::sqrt(len);

	if (len != 0.0 && round(len, 6) != 1.0) {			//	divide each element by the vector length, except if len = 0.0 or 1.0
		for (double & x : *this) {
			x /= len;
		}
	}
	return *this;
}


//	return this->Complement();
const double_vector
double_vector::Normalize() const
{
	double_vector vec(*this);
	return vec.NormalizeMe();
}


//	round this
double_vector &
double_vector::RoundMe(int32_t decimals)
{
	if (decimals >= 15) {//	double datatype have around a 15 decimal precision
		return *this;
	}
	if (decimals <= 0) {
		for (double & x : *this) {
			x = round(x);
		}
	}
	else {
		double mult = pow(10, decimals);
		for (double & x : *this) {
			x = round(x * mult) / mult;
		}
	}
	return *this;
}


//	return this->Round()
const double_vector
double_vector::Round(int32_t decimals) const
{
	double_vector mat(*this);
	return mat.RoundMe(decimals);
}


void
double_vector::Print() const
{
	std::wostringstream stream;
	Print(stream);
	//OutputDebugString(stream.str().c_str());
}


void
double_vector::Print(std::wstring & dest) const
{
	using boost::lexical_cast;
	using std::wstring;

	size_t cmax = size();

	std::wstringstream stream;
	stream << L"(" << cmax << L") vector:" << std::endl;
	dest.append(stream.str());

	static wchar_t spaces[][32] = {
		L"\0",
		L" \0",
		L"  \0",
		L"   \0",
		L"    \0",
		L"     \0",
		L"      \0",
		L"       \0",
		L"        \0",
		L"         \0",
		L"          \0",
		L"           \0",
		L"            \0",
		L"             \0",
		L"              \0",
		L"               \0",
		L"                \0",
		L"                 \0",
		L"                  \0",
		L"                   \0",
		L"                    \0",
		L"                     \0",
		L"                      \0",
		L"                       \0",
		L"                        \0"
	};

	const wchar_t separator[] = L" ";
	const size_t decimals = 4;			//	4 decimals

												// compute the largest width
	size_t maxWidth = 0;

	wchar_t buffer[512];

	for (double x : (*this)) {
		ToString(buffer, x, decimals);	//	CETIC optimized
		maxWidth = std::max(maxWidth, wcslen(buffer));
	}

	//	you can't reserve memory for a stream, so use a std::wstring here
	dest.reserve(dest.size() + cmax * (maxWidth + 2));

	bool first = true;

	for (double x : *this) {
		ToString(buffer, x, decimals);	//	CETIC optimized
		size_t width = wcslen(buffer);						//	0 to maxLen

		if (width < maxWidth) {
			dest.append(spaces[maxWidth - width]);		//	0 to maxWidth
		}
		if (first) {
			first = false;
		}
		else {
			dest.append(separator);
		}
		dest.append(buffer);
	}
	dest.append(L"\r\n");
	return;
}


std::wostringstream &
double_vector::Print(std::wostringstream & stream) const
{
	std::wstring dest;
	Print(dest);

	stream << dest;
	return stream;
}


//	****************************************************************************
//											Out of class functions								*
//	****************************************************************************

//	return the distance between these two points: sqrt( (a[0] - b[0])^2 + (a[1] - b[1])^2 + ... (a[i] - b[i])^2 )
double Distance(const double_vector & a, const double_vector & b)
{
	if (a.size() != b.size()) {
		std::string errorMessage = std::string("Invalid vector size");
		throw std::out_of_range(errorMessage);
	}

	double tot = 0.0;
	for (size_t r = 0, rmax = a.size(); r < rmax; ++r) {
		tot += (a[r] - b[r]) * (a[r] - b[r]);
	}
	return std::sqrt(tot);
}


//	compute the scalar product (dot product) of two vectors of same size : a[0] * b[0] + a[1] * b[1] + ...
double operator *(const double_vector & a, const double_vector & b)
{
	if (a.size() != b.size()) {
		std::string errorMessage = std::string("Invalid vector size");
		throw std::out_of_range(errorMessage);
	}
	double s = 0.0;
	for (double_vector::const_iterator ita = a.begin(), itb = b.begin(); ita != a.end(); ++ita, ++itb) {
		s += (*ita) * (*itb);
	}
	return s;
}


double_vector operator *(double p, const double_vector & a)
{
	double_vector vec(a);
	vec *= p;
	return vec;
}


double_vector operator +(const double_vector & a, const double_vector & b)
{
	double_vector vec(a);
	vec += b;
	return vec;
}


double_vector operator -(const double_vector & a, const double_vector & b)
{
	double_vector vec(a);
	vec -= b;
	return vec;
}


double_vector operator *(const double_vector & a, double p)
{
	double_vector vec(a);
	vec *= p;
	return vec;
}


std::wostringstream & operator << (std::wostringstream & stream, const double_vector & vec)
{
	return vec.Print(stream);
}


