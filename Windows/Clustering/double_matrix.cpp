
#include "stdafx.h"

#include "double_matrix.h"
#include "matrix_context.h"
#include "std_plugin.h"
#include "Toolbox.h"
#include "ToString.h"

#include <cmath>

#include <fstream>      // std::ofstream
#include <mutex>
#include <random>
#include <thread>

#include <sysinfoapi.h>
#include <debugapi.h>
#include <algorithm>
#include <boost/lexical_cast.hpp>

extern uint32_t g_MaxThread ;	//	depend in the number of cores and processors, set in the DLL initialization


double_matrix::double_matrix()
	: TMatrixTpl<double, double_vector> ()
{
}


double_matrix::double_matrix(const double_matrix & srce)
	: TMatrixTpl<double, double_vector>(srce)
{
}


void
	double_matrix::operator =(const char * srce)
{
	size_t rmax = RowCount() ;
	size_t cmax = ColCount() ;
	if(rmax == 0 || cmax == 0) {
		return;
	}
	//	read the given string and fill us row by row
	std::string numStr ;

	size_t r = 0;
	size_t c = 0;
	
	//	start reading
	for(const char * ptr = srce; *ptr != 0; ++ptr) {
		 //	jump over the separators
		while(*ptr > 0 && *ptr <= ' ') {
			++ptr;
		}
		if(*ptr == 0) {
			return;
		}
		//	read a number
		double x = strtod_(ptr, & ptr);
		m_Values[r][c] = x;
		if(++c >= cmax) {
			c = 0;
			if(++r >= rmax) {
				return;	//	we are full
			}
		}
	}
}



double_matrix::double_matrix(size_t p_RowNo, size_t p_ColNo)
	: TMatrixTpl<double, double_vector> (p_RowNo, p_ColNo)
{
}


double_matrix::double_matrix(size_t p_RowNo, size_t p_ColNo, double x)
	: TMatrixTpl<double, double_vector> (p_RowNo, p_ColNo, x)
{
}


double_matrix::double_matrix(std::initializer_list<value_type> il, const allocator_type & a)
	: TMatrixTpl<double, double_vector>(il, a)
{
}


//	Load a UTF-8 file
bool double_matrix::Load(const wchar_t * p_FullFileName)
{
	std::ifstream file(p_FullFileName, std::ofstream::binary) ;
	if(file.bad()) {
		return true ;	//	error
	}
	std::string	Line ;
	Line.reserve(2000000) ;
	
	//	clear current matrix
	clear() ;
	//	initialize variables
	bool IsError = false ;

	bool RowNameFirst = false ;	//	will be set to true if the first cell is empty, which means that the first row and column contain names instead of values
	bool FirstLineLoaded = false ;

	std::vector<std::string> Cells ;

	size_t colNo = ColCount() ;

	//	parse file
	while(file.good()) {
		//	read line
		Line.clear() ;
		std::getline(file, Line) ;
		if(Line.empty()) {
			continue ;
		}
		//	a non-empty line is loaded
		if(FirstLineLoaded == false) {	//	check for the utf-8 header
			if(Line.size() >= 3 && Line[0] == cUtf8Header[0] && Line[1] == cUtf8Header[1] && Line[2] == cUtf8Header[2]) {
				//	a pure utf-8 file, remove the utf-8 BOM. If not, it may be an ascii file. Load it anyway.
				Line.erase(Line.begin(), Line.begin() + 3) ;
			}
			FirstLineLoaded = true ;
			if(Line.empty()) {
				continue ;
			}
		}
		//	clear cells
		Cells.clear() ;
		
		if(m_Values.size() == 0 && Line[0] < 32) {	//	first cell is empty => first row and column contains the row and column names 
			////	read the column names
			//split(Line, '\t', Cells) ;
			////	reset our column number
			//colNo = Cells.size() ;
			//
			////	scan the column names
			//m_vColNames.clear() ;
			//for(std::string value : Cells) {
			//	m_vColNames.push_back(utf8_to_wstring(value)) ;
			//}
			////	now we know the number of column from the column names
			//m_Values.reserve(colNo) ;	//	assuming the matrix is square
			
			RowNameFirst = true ;	//	read the first cell or each row as a row name
			continue ;
		}
		//	allocate a new row
		double_vector row ;
		row.reserve(colNo) ;

		//	split row
		split(Line, '\t', Cells) ;

		bool IsFirst = true ;	//	is first cell being read
		for(std::string value : Cells) {
			if(IsFirst && RowNameFirst) {
				//	read row name
				//m_vRowNames.push_back(utf8_to_wstring(value)) ;
				IsFirst = false ;
			}
			else {
				char * EndPtr ;
				double x = strtod(value.c_str(), & EndPtr) ;
				row.push_back(x) ;
			}
		}
		//	a few checking about the row length
		if(RowCount() == 0) {
			m_Values.push_back(row) ;
		}
		else if(ColCount() != row.size()) {
			IsError = true ;
			row.resize(ColCount()) ;
			m_Values.push_back(row) ;
		}
	}	//	next row

	file.close() ;
	return(IsError) ;	
}


////	Load a Unicode file
//bool double_matrix::LoadW(const wchar_t * p_FullFileName)
//{
//	//	max 2MB per line
//	const int32_t cMaxLineSize = 2000000 ;
//	wchar_t	Separator[] = L"\t" ;
//	wchar_t	Cell[512] ;
//
//	FILE * pStream = NULL ;
//	_wfopen_s(& pStream, p_FullFileName, L"rb") ;
//	if(pStream == NULL) {	//	File does not exist
//		return(true) ;
//	}
//	//	allocate line memory
//	wchar_t * Line = new wchar_t[cMaxLineSize + 8] ;
//
//	//	clear current matrix
//	clear() ;
//	//	initialize variables
//	bool IsError = false ;
//	WString String(cMaxLineSize + 8) ;
//	wchar_t * EndPtr = NULL ;
//	bool SkipFirst = false ;	//	will be set to true if the first cell is empty, which means that the first row and column contain names instead of values
//
//	//	parse file
//	for( ; ; ) {
//		//	read line
//		wchar_t * pLine = fgetws(Line, cMaxLineSize, pStream) ;
//		if(! pLine) {	//	end of file reached
//			break ;
//		}
//		String = pLine ;
//		//	read columns
//		double_vector vLineValues ;
//		vLineValues.reserve(m_ColCount == 0 ? 1024 : m_ColCount) ;
//		WString RowName ;
//
//		if(m_RowCount == 0 && * pLine < 32) {	//	first cell is empty => first row and column contains the row and column names 
//			//	TODO : read the column names
//			SkipFirst = true ;
//			continue ;
//		}
//		for(int32_t c = 0; ; ++c) {
//			const wchar_t * ptr = String.strtok(Separator, Cell, 512) ;
//			if(! ptr) {
//				break ;
//			}
//			if(c == 0) {
//				RowName = Cell ;
//				if(SkipFirst) {
//					continue ;
//				}
//			}
//			double X = wcstod(Cell, & EndPtr) ;
//			vLineValues.push_back(X) ;
//		}	//	next cell in row
//		
//		//	now we know the number of column of the first numerical line
//		if(m_RowCount == 0) {
//			m_ColCount = vLineValues.size() ;
//			m_Values.reserve(m_ColCount) ;	//	assuming the matrix is square...
//		}
//		m_Values.push_back(vLineValues) ;		
//		m_vRowNames.push_back(RowName) ;
//		++m_RowCount ;
//	}	//	next row
//	
//	m_RowCount = m_Values.size() ;
//	m_NMax = m_RowCount * m_ColCount ;
//
//	//	free line memory
//	delete[] Line ;
//	Line = NULL ;
//	
//	//	check that all row vectors have the same size
//	for(std::vector<double_vector >::iterator rit = m_Values.begin(); rit < m_Values.end(); ++rit) {
//		double_vector & Row = * rit ;
//		Row.resize(m_ColCount) ;
//	}
//	fclose(pStream) ;
//	return(IsError) ;	
//}


//	Save into a UTF-8 file
bool double_matrix::Save(const wchar_t * p_FullFileName) const
{
	std::ofstream file(p_FullFileName, std::ofstream::binary) ;
	if(file.fail()) {
		return true ;	//	error
	}
	//	mark the file as utf-8
	file << cUtf8Header ;

	////	save the columns names
	//for(std::wstring & ColName : m_vColNames) {
	//	file << "\t" ;
	//	file << wstring_to_utf8(ColName) ;
	//}
	//	save all values
	for(size_t r = 0, rmax = RowCount(); r < rmax && ! file.fail(); ++r) {
		//file << "\r\n" ;
		//file << wstring_to_utf8(m_vRowNames[r]) ;
		bool bNext = false;
		for(double x : m_Values[r]) {
			if(bNext) {
				file << "\t";
			}
			else {
				bNext = true;
			}
			file << std::to_string(x) ;
		}
		file << "\r\n" ;
	}
	bool IsError = (file.goodbit != 0) ;
	file.close() ;
	return(IsError) ;
}


////	Save into a Windows UTF-16 file
//bool double_matrix::SaveW(const wchar_t * p_FullFileName)
//{
//	FILE * pStream = NULL ;
//	_wfopen_s(& pStream, p_FullFileName, L"w+b, ccs=UTF-16LE") ;
//	if(! pStream) {
//		return(true) ;	//	error
//	}
//	wchar_t wStringC[512] ;
//	WString Line ;
//	Line.reserve(64 + m_ColCount * 10) ;
//
//	//	mark the file as unicode
//	Line += cUtf16Header ;
//	bool IsError = Line.fwrite(pStream) ;
//
//	//	save the columns names
//	for(size_t c = 0; c < m_ColCount && ! IsError; ++c) {
//		Line.Set(L"\t", m_vColNames[c].c_str()) ;
//		IsError = Line.fwrite(pStream) ;
//	}
//	//	save all rows
//	for(size_t r = 0; r < m_RowCount && ! IsError; ++r) {
//		Line.Set(L"\r\n", m_vRowNames[r].c_str()) ;
//		const double_vector vLine = m_Values[r] ;
//		
//		for(double_vector::const_iterator it = vLine.begin(); it < vLine.end(); ++it) {
//			Line.append('\t') ;
//			Line.append(ToString(wStringC, *it)) ;
//		}
//		IsError = Line.fwrite(pStream) ;
//	}
//	fclose(pStream) ;
//	return(IsError) ;
//}


double_vector &
	double_matrix::operator [](size_t r)
{
	return m_Values[r] ;
}


const double_vector & 
	double_matrix::operator [](size_t r) const
{
	return m_Values[r] ;
}


bool
double_matrix::operator !=(const double_matrix & srce) const
{
	if(ColCount() != srce.ColCount() || RowCount() != srce.RowCount()) {
		return true;
	}
	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		const double_vector & row1 = m_Values[r];
		const double_vector & row2 = srce.m_Values[r];
		if(row1 != row2) {
			return true;
		}
	}
	return false; //	identical
}


bool
double_matrix::operator ==(const double_matrix & srce) const
{
	return ! (*this != srce) ;
}


//	********************************	multithreading functions	**************************

//	 303 ms to multiply two (1000 * 1000) matrixes, 8 threads, maxComputations = 100 to 10000
void
OperatorStar(size_t p_Id, matrix_context * ctx)
{
	const size_t maxComputations = 1000 ;	//	9140 ms for 1000 and 10000, 9700 ms for 100000
	while(true) {
		ctx->m_IdxMutex.lock() ;		//	lock the indexes

		size_t rmin = ctx->r ;			//	get the first row to compute
		size_t rmax = rmin + max_(1ull, maxComputations / (ctx->cmax - ctx->cmin));		//	search for rmax giving us at least 1000 computations
		rmax = min_(rmax, ctx->rmax);
		ctx->r = rmax ;					//	update the next row to compute in the context

		ctx->m_WaitIdx += (rmax - rmin) * (ctx->cmax - ctx->cmin);			//	increment the computation counter before unlocking m_IdxMutex

		ctx->m_IdxMutex.unlock() ;	//	unlock the indexes

		if(rmin >= ctx->rmax) {		// check if job done
			return;
		}

		//	r and c are the destination indexes
		for(size_t r = rmin; r < rmax; ++r) {
			double_vector row ;	//	destination row
			for(size_t c = ctx->cmin; c < ctx->cmax; ++c) {
				row.push_back(ctx->mat1[r] * ctx->mat2[c]) ;	//	scalar product
			}
			ctx->mat1[r] = row ;	//	replace our row
		}
	}
}


//	fast method, but transpose srce
//	note: 22 ms to transpose a (3162 * 3162) matrix
//	33.8 s to multiply two (3162 * 3162) matrixes, 1 thread
//	 9.3 s to multiply two (3162 * 3162) matrixes, 8 threads
//	8 ms for (100 * 100) - 1 thread, 40 ms for (200 * 200) - 1 thread, (300 * 300) 2 threads, 300 ms for (1000 * 1000) - 8 threads
void
	double_matrix::operator *=(const double_matrix & srce)
{
	if(ColCount() != srce.RowCount()) {
		std::string errorMessage = std::string("Invalid row and column count in matrix::operator *=");
		throw std::out_of_range(errorMessage);
	}
	double_matrix srcet(srce) ;	//	the transposed matrix
	srcet.Transpose() ;

	size_t rmax = RowCount();
	size_t cmax = srcet.RowCount();	//	= srce.ColCount();

	size_t op = rmax * cmax ;	//	number of destination values to compute

	//	a single thread is much faster for a 100 * 100 multiplication. Multithreading benefits start around 30000 - 40000 computations per thread
	size_t numThread = max_(1u, min_(static_cast<uint32_t>(std::ceil(op / 30000.0)), g_MaxThread)) ;

	//std::wostringstream stream ;
	//stream << L"\r\nStarting " << numThread << " threads from double_matrix::operator *=()" ;
	//OutputDebugString(stream.str().c_str());
	
	if(numThread == 1) {
		for(size_t r = 0; r < rmax; ++r) { //	r and c are the destination indexes
			double_vector row ;	//	destination row
			for(size_t c = 0; c < cmax; ++c) {
				row.push_back(m_Values[r] * srcet[c]) ;	//	scalar product
			}
			m_Values[r] = row ;	//	replace our row
		}
	}
	else {
		std::vector<std::thread>	threads ;

		matrix_context context(0, rmax, 0, cmax, *this, srcet); //	r and c are the destination indexes

		for(size_t i = 0; i < numThread; ++i) {
			threads.push_back(std::thread(::OperatorStar, i, & context)) ;
		}
		//	catch each thread results and wait for all threads to finish
		for(std::thread & item : threads) {
			item.join() ;
		}
		//	clear all existing threads
		threads.clear() ;
	}
}


void
OperatorPLus(size_t p_Id, matrix_context * ctx)
{
	const size_t maxComputations = 40000;
	//const size_t cmax = ctx->mat1.ColCount();

	while (true) {
		ctx->m_IdxMutex.lock();		//	lock the indexes

		size_t rmin = ctx->r;			//	get the first row to compute
		size_t rmax = rmin + max_(1ull, maxComputations / (ctx->cmax - ctx->cmin));		//	search for rmax giving us at least 100000 computations
		rmax = min_(rmax, ctx->rmax);
		ctx->r = rmax;					//	update the next row to compute in the context

		ctx->m_WaitIdx += (rmax - rmin) * (ctx->cmax - ctx->cmin);			//	increment the computation counter before unlocking m_IdxMutex

		ctx->m_IdxMutex.unlock();	//	unlock the indexes

		if (rmin >= ctx->rmax) {		// check if job done
			return;
		}

		for (size_t r = 0; r < rmax; ++r) {
			ctx->mat1[r] += ctx->mat2[r];
		}
	}
}


void
double_matrix::operator+=(const double_matrix & srce)
{
	if (RowCount() != srce.RowCount() || ColCount() != srce.ColCount()) {
		std::string errorMessage = std::string("Invalid row and column count in matrix::operator +=");
		throw std::out_of_range(errorMessage);
	}

	size_t rmax = RowCount();
	size_t cmax = ColCount();

	size_t op = rmax * cmax;	//	number of destination values to compute

										//	TODO : optimize this : a single thread is much faster for a 100 * 100 addition. Multithreading benefits start around 30000 - 40000 computations per thread
	size_t numThread = max_(1u, min_(static_cast<uint32_t>(std::ceil(op / 40000.0)), g_MaxThread));

	if (numThread == 1) {
		for (size_t r = 0; r < rmax; ++r) {
			m_Values[r] += srce.m_Values[r];
		}
	}
	else {
		std::vector<std::thread>	threads;

		matrix_context context(0, rmax, 0, cmax, *this, srce); //	r and c are the destination indexes

		for (size_t i = 0; i < numThread; ++i) {
			threads.push_back(std::thread(::OperatorPLus, i, &context));
		}
		//	catch each thread results and wait for all threads to finish
		for (std::thread & item : threads) {
			item.join();
		}
		//	clear all existing threads
		threads.clear();
	}
}


void
	double_matrix::operator *=(double p)
{
	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		for(double & x : m_Values[r]) {
			x *= p ;
		}
	}
}


void
	double_matrix::operator /=(double p)
{
	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		for(double & x : m_Values[r]) {
			x /= p ;
		}
	}
}


void
	double_matrix::operator +=(double p)
{
	//	TODO: easy multithreading here
	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		for(double & x : m_Values[r]) {
			x += p ;
		}
	}
}


void
	double_matrix::operator -=(double p)
{
	//	TODO: easy multithreading here
	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		for(double & x : m_Values[r]) {
			x -= p ;
		}
	}
}


//	generate a random matrix for debugging
double_matrix & 
double_matrix::RandomDebug()
{
	size_t rmax = RowCount() ;
	size_t cmax = ColCount() ;

	for(size_t r = 0; r < rmax; ++r) {
		for(size_t c = 0; c < cmax; ++c) {
			if(c < 10) {
				m_Values[r][c] = (r + 1) + (c / 10.0) ;
			}
			else if(c < 100) {
				m_Values[r][c] = (r + 1) + (c / 100.0) ;
			}
			else if(c < 1000) {
				m_Values[r][c] = (r + 1) + (c / 1000.0) ;
			}
			else {
				m_Values[r][c] = (r + 1) + (c / 10000.0) ;
			}
		}
	}
	Round(6);
	return *this;
}


//	generate a random matrix
double_matrix & 
double_matrix::Random()
{
	std::default_random_engine generator;
	generator.seed(GetTickCount());
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	for(double_vector & row : m_Values) {
		for(double & x : row) {
			x = distribution(generator) ;
			x = static_cast<int32_t>(std::ceil(10.0 * x));
		}
	}
	return *this;
}


//	generate a random distance matrix. Diagonal values are 0.0, and increase when you get far away from the diagonal
double_matrix & 
double_matrix::RandomDistance()
{
	std::default_random_engine generator;
	generator.seed(GetTickCount());
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		for(size_t c = 0; c < r; ++c) {
			double x = distribution(generator) * std::abs(static_cast<double>(r) - c) / rmax ;	//	for a 10 * 10 matrix, weight values from 1/10 to 1.0
			m_Values[r][c] = m_Values[c][r] = x ;
		}
		m_Values[r][r] = 0.0 ;
	}
	return *this;
}


double_matrix & 
double_matrix::PowMe(double p)
{
	for(double_vector & row : m_Values) {
		for(double & x : row) {
			x = std::pow(x, p) ;
		}
	}
	return *this;
}


//	return this->Pow();
double_matrix
double_matrix::Pow(double p) const
{
	double_matrix mat(*this);
	return mat.PowMe(p);
}


double_matrix & 
double_matrix::Pow2Me()
{
	//	C++11 version
	for(double_vector & row : m_Values) {
		for(double & x : row) {
			x *= x ;
		}
	}
	//for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
	//	for(size_t c = 0, cmax = ColCount(); c < cmax; ++c) {
	//		m_Values[r][c] *= m_Values[r][c];
	//	}
	//}
	return *this;
}


//	return this->Pow2();
double_matrix
double_matrix::Pow2() const
{
	double_matrix mat(*this);
	return mat.Pow2Me();
}


double_matrix & 
double_matrix::RootMe()
{
	for(double_vector & row : m_Values) {
		for(double & x : row) {
			x = x < 0.0 ? cNaN : std::sqrt(x) ;
		}
	}
	return *this;
}


//	return this->Root();
double_matrix
double_matrix::Root() const
{
	double_matrix mat(*this);
	return mat.RootMe();
}


double
double_matrix::Average() const
{
	double tot = static_cast<double>(size()) ;	//	ok up to 51 bits...
	double avg = 0 ;
	for(const double_vector & row : m_Values) {
		for(double x : row) {
			avg += x / tot ;	//	avoid an overflow
		}
	}
	return avg ;
}


double_matrix & 
double_matrix::SymetricAverageMe()
{
	if(RowCount() != ColCount()) {	//	Matrix must be square
		return *this;
	}
	for(size_t r = 0; r < RowCount(); ++r) {
		for(size_t c = r + 1; c < ColCount(); ++c) {
			double x = ((m_Values[r][c] + m_Values[c][r]) / 2.0) ;
			m_Values[r][c] = m_Values[c][r] = x ;
		}
	}
	return *this;
}


double_matrix
double_matrix::SymetricAverage() const
{
	double_matrix mat(*this);
	return mat.SymetricAverageMe();
}


//	in place transposition
//	320 ms for (1000 * 5000)
//	480 ms for (5000 * 1000)
//	 64 ms for (3162 * 3162)
double_matrix &
double_matrix::TransposeMe()
{
	size_t rmax = RowCount() ;
	size_t cmax = ColCount() ;
	size_t n = std::min(rmax, cmax);

	//	transpose the top left square
	for(size_t r = 1; r < n; ++r) {
		for(size_t c = 0; c < r; ++c) {
			Swap(m_Values[r][c], m_Values[c][r]);
			//std::swap(m_Values[r][c], m_Values[c][r]);// not faster
		}
	}
	if(rmax > cmax) {		//	transpose the remaining rows
		for(size_t c = 0; c < cmax; ++c) {
			m_Values[c].reserve(rmax);
		}
		for(size_t r = n; r < rmax; ++r) {
			for(size_t c = 0; c < cmax; ++c) {
				m_Values[c].push_back(m_Values[r][c]);
			}
		}
		//	remove the extra rows
		m_Values.erase(m_Values.begin() + n, m_Values.end());
	}
	else if(cmax < rmax) { //	transpose the remaining columns
		m_Values.reserve(cmax);
		for(size_t c = n; c < cmax; ++c) {
			double_vector row;
			row.reserve(n);
			for(size_t r = 0; r < rmax; ++r) {
				row.push_back(m_Values[r][c]);
			}
			m_Values.push_back(row);
		}
		//	remove the extra columns
		for(size_t r = 0; r < n; ++r) {
			double_vector & row = m_Values[r];
			row.erase(row.begin() + n, row.end());
		}
	}
	return *this;
}


//	105 ms for (1000 * 5000)
//	 95 ms for (5000 * 1000)
// 200 ms for (3162 * 3162)
double_matrix
double_matrix::Transpose() const
{
	//	allocate the new values
	double_matrix mat ;
	mat.reserve(ColCount());

	//	read column per colum
	for(size_t c = 0, cmax = ColCount(); c < cmax; ++c) {
		//	get a vecteur with the source column, so that we copy only once each column in a new vector
		double_vector col = GetCol(c);
		mat.m_Values.push_back(col);
	}
	return mat;
}


//	return the Gram matrix, where each x(i, j) = row(i) * row(j), = (mat * mat')
//	see http://www.encyclopediaofmath.org/index.php/Gram_matrix
//	13.8 s for (1000 * 5000)
//	 2.8 s for (5000 * 1000)
//	17.0 s for (3162 * 3162)
double_matrix
double_matrix::Gram() const
{
	size_t cmax = ColCount() ;

	double_matrix trans = this->Transpose() ;

	//	allocate the new matrix
	double_matrix mat ;
	mat.resize(cmax, cmax);	//	A (5 * 2) => (2 * 5).(5 * 2) => (2 * 2)

	//	r and c are the destination indexes
	for(size_t r = 0; r < cmax; ++r) {
		mat[r][r] = trans[r] * trans[r] ;
		for(size_t c = 0; c < r; ++c) {
			mat[r][c] = mat[c][r] = trans[r] * trans[c] ;	//	scalar product
		}
	}
	return mat;
}

//	return a J Matrix of size (n * n)
//	for n = 4:
//	 0.75	-0.25	-0.25	-0.25
//	-0.25	 0.75	-0.25	-0.25	
//	-0.25	-0.25	 0.75	-0.25	
//	-0.25	-0.25	-0.25	 0.75
//
double_matrix J_double_matrix(size_t n)
{
	double x = -1.0 / n ;
	double_matrix mat(n, n, x);

	//	add 1 to the diagonal
	for(size_t r = 0; r < n; ++r) {
		mat[r][r] += 1.0 ;
	}
	return mat ;
}

//	see http://homepages.uni-tuebingen.de/florian.wickelmaier/pubs/Wickelmaier2003SQRU.pdf
//	compute B = -0.5 J.(P²).J
double_matrix
	double_matrix::DoubleCentered() const
{
	size_t rmax = RowCount() ;

	//	get the corresponding J matrix
	const double_matrix & J = J_double_matrix(rmax) ;
	double_matrix mat = -0.5 * J * this->Pow2() * J;
	return mat;
}


double
double_matrix::Sum() const
{
	double tot = 0.0 ;
	for(const double_vector & row : m_Values) {
		for(double x : row) {
			tot += x ;
		}
	}
	return tot ;
}


double
double_matrix::SumOfSquares() const
{
	double tot = 0.0 ;
	for(const double_vector & row : m_Values) {
		for(double x : row) {
			tot += x * x ;
		}
	}
	return tot ;
}


double_matrix & 
double_matrix::ComplementMe(double p)
{
	for(double_vector & row : m_Values) {
		for(double & x : row) {
			x = p - x ;
		}
	}
	return *this;
}


//	return this->Complement();
double_matrix
double_matrix::Complement(double p) const
{
	double_matrix mat(*this);
	return mat.ComplementMe(p);
}


////	see inverse matrix here :http://en.wikipedia.org/wiki/Gaussian_elimination
//m_Values[0][0] = 2 ;
//m_Values[0][1] = -1 ;
//m_Values[0][2] = 0 ;
//m_Values[0][3] = 1 ;
//m_Values[0][4] = 0 ;
//m_Values[0][5] = 0 ;

//m_Values[1][0] = -1 ;
//m_Values[1][1] = 2 ;
//m_Values[1][2] = -1 ;
//m_Values[1][3] = 0 ;
//m_Values[1][4] = 1 ;
//m_Values[1][5] = 0 ;

//m_Values[2][0] = 0 ;
//m_Values[2][1] = -1 ;
//m_Values[2][2] = 2 ;
//m_Values[2][3] = 0 ;
//m_Values[2][4] = 0 ;
//m_Values[2][5] = 1 ;


double_matrix & 
double_matrix::TriangleMe()
{
	double_vector averages ;

	//	keep in mind the average value of each row (only the non zero values) to keep the whole amtrix around 1.0 and avoid rounding errors
	averages.resize(RowCount(), 1.0);
	
	for(size_t t = 0, tmax = RowCount() - 1; t < tmax; ++t) {	//	this is the pivot row index
		double len = static_cast<double>(ColCount() - t) ;	//	the number of items to compute, from [t+1] to the end

		for(size_t r = RowCount() - 1; r > t; --r) {				//	this is the dest row index
			//	get the dest row
			double_vector & dest = m_Values[r];
			if(dest[t] == 0.0) { // nothing to do
				continue ;
			}
			//	get the pivot row
			const double_vector & pivot = m_Values[t];
			if(pivot[t] == 0.0) { // swap row[t] and row[r]
				SwapRows(t, r);
				std::swap(averages[t], averages[r]);			//	swap also the rows averages
				continue ;
			}
			//	compute values in dest from column[t+1] to the end
			double mult = pivot[t] / dest[t] ;
			double avgr = averages[r] ;							//	the average computed during the previous loop
			double avg = 0.0 ;
			for(size_t c = pivot.size() - 1; c > t; --c) {		//	scan the whole row, especially when not square (linear equations matrix)
				double x = mult * dest[c] - pivot[c];			//	this is (x / dest[t]), similar to: x = pivot[t] * dest[c] - dest[t] * pivot[c];
				//double x = pivot[t] * dest[c] - dest[t] * pivot[c];
				dest[c] = x / avgr ;
				avg += std::abs(dest[c]) / len ;				//	compute the average of this row, null value not included
			}
			dest[t] = 0.0;
			//	save the new average value of this row
			if(avg != 0.0) {
				averages[r] = avg ;
			}
		}
	}
	
	//	normalise around the average
	for(size_t r = 1, rmax = RowCount(); r < rmax; ++r) {
		double_vector & row = m_Values[r];
		double div = averages[r] ;							//	the average computed during the previous loop
		if(div != 0.0) {
			for(size_t c = r, cmax = ColCount(); c < cmax; ++c) {
				row[c] /= div ;
			}
		}
	}
	return *this ;
}


//	return this->Triangle();
double_matrix
double_matrix::Triangle() const
{
	double_matrix mat(*this);
	return mat.TriangleMe();
}


double_matrix & 
double_matrix::DiagonalMe()
{
	double_vector averages ;

	//	keep in mind the average value of each row (only the non zero values) to keep the whole amtrix around 1.0 and avoid rounding errors
	averages.resize(RowCount(), 1.0);
	
	for(size_t t = 0, tmax = RowCount(); t < tmax; ++t) {				//	this is the pivot row index
		double len = static_cast<double>(ColCount() - t) ;	//	the number of items to compute, from [t+1] to the end

		for(int64_t rr = RowCount() - 1; rr >= 0; --rr) {				//	this is the dest row index, cannot be  of type size_t, otherwise --r fails if r = 0
			size_t r = static_cast<size_t>(rr);	//	avoid plenty of warnings
			if(r == t) {
				continue;
			}
			//	get the dest row
			double_vector & dest = m_Values[r];
			if(dest[t] == 0.0) { // nothing to do
				continue ;
			}
			//	get the pivot row
			const double_vector & pivot = m_Values[t];
			if(pivot[t] == 0.0) { // swap row[t] and row[r]
				SwapRows(t, r);
				std::swap(averages[t], averages[r]);				//	swap also the rows averages
				continue ;
			}
			//	compute values in dest from column[t+1] to the end
			double mult = pivot[t] / dest[t] ;
			double avgr = averages[r] ;								//	the average computed during the previous loop
			double avg = 0.0 ;
			for(size_t c = pivot.size() - 1; c > t; --c) {		//	scan the whole row, especially when not square (linear equations matrix)
				double x = mult * dest[c] - pivot[c];				//	this is (x / dest[t]), similar to: x = pivot[t] * dest[c] - dest[t] * pivot[c];
				//double x = pivot[t] * dest[c] - dest[t] * pivot[c];
				dest[c] = x / avgr ;
				avg += std::abs(dest[c]) / len ;						//	compute the average of this row, null value not included
			}

			//	compute values in dest from column[0] to column[t-1]
			//	don't loop all columns as most computations = 0.0, just recompute the diagonal values
			if(r < t) {
				double x = mult * dest[r] - pivot[r];
				dest[r] = x / avgr ;
				avg += std::abs(dest[r]) / len ;				//	compute the average of this row, null value not included
			}
			dest[t] = 0.0;
			//	save the new average value of this row
			if(avg != 0.0) {
				averages[r] = avg ;
			}
		}
	}

	//	divide by the diagonal element
	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		double_vector & row = m_Values[r];
		double div = row[r];							//	the average computed during the previous loop
		if(div != 0.0) {
			row[r] = 1.0;
			for(size_t c = rmax, cmax = ColCount(); c < cmax; ++c) {
				row[c] /= div ;
			}
		}
	}
	return *this;
}


//	return this->Diagonal();
double_matrix
double_matrix::Diagonal() const
{
	double_matrix mat(*this);
	return mat.DiagonalMe();
}


//	matrix must be square
double_matrix & 
double_matrix::InvertMe()
{
	if(RowCount() != ColCount()) {
		std::string errorMessage = std::string("Can invert only square matrices");
		throw std::range_error(errorMessage);
	}
	//	add the identity matrix on the right
	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		double_vector & row = m_Values[r];
		row.resize(2 * rmax, 0.0) ;
		row[rmax + r] = 1.0 ;
	}

	//	Diagonalize
	Diagonal() ;

	//	keep right half
	for(size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
		double_vector & row = m_Values[r];
		row.erase(row.begin(), row.begin() + rmax) ;
	}
	return *this;	
}


//	return this->Root();
double_matrix
double_matrix::Invert() const
{
	double_matrix mat(*this);
	return mat.InvertMe();
}


//	two steps process, no other way.
double_matrix & 
double_matrix::NormalizeRowsMe()
{
	for(double_vector row : m_Values) {
		row.Normalize();
	}
	return *this;
}


//	return this->NormalizeRows();
double_matrix
double_matrix::NormalizeRows() const
{
	double_matrix mat(*this);
	return mat.NormalizeRowsMe();
}


//	round this
double_matrix &
double_matrix::RoundMe(int32_t decimals)
{
	if(decimals >= 15) {//	double datatype have around a 15 decimal precision
		return *this ;
	}
	if(decimals <= 0) {
		for(double_vector & row : m_Values) {
			for(double & x : row) {
				x = round(x);
			}
		}
	}
	else {
		double mult = pow(10, decimals) ;
		for(double_vector & row : m_Values) {
			for(double & x : row) {
				x = round(x * mult) / mult ;
			}
		}
	}
	return *this ;
}


//	return this->Round();
double_matrix
double_matrix::Round(int32_t decimals) const
{
	double_matrix mat(*this);
	return mat.RoundMe(decimals);
}


void
double_matrix::Print() const
{
	std::wostringstream stream ;
	Print(stream) ;
	OutputDebugString(stream.str().c_str());
}


void
double_matrix::Print(std::wstring & dest) const
{
	using boost::lexical_cast;
	using std::wstring;

	size_t rmax = RowCount() ;
	size_t cmax = ColCount() ;

	std::wstringstream stream;
	stream << L"(" << rmax << L" * " << cmax << L") matrix:" << std::endl ;
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
	const size_t decimals = 4 ;			//	4 decimals

	// compute the largest width
   size_t maxWidth = 0;

	wchar_t buffer[512] ;

	for(const double_vector & row : m_Values) {
		for(double x : row) {
			ToString(buffer, x, decimals);	//	CETIC optimized
			maxWidth = std::max(maxWidth, wcslen(buffer));
		}
	}

	//	you can't reserve memory for a stream, so use a std::wstring here
	dest.reserve(dest.size() + rmax * 2 + rmax * cmax * (maxWidth + 2)) ;

	//	time needed is the same for all three options : iterators, indexes and C++11 loops: 1306-1315 ms for a (1000 * 1000) matrix
	//for(std::vector<double_vector>::const_iterator rit = m_Values.begin(); rit != m_Values.end(); ++rit) {
	//	const double_vector & row = *rit ;
	//	for(double_vector::const_iterator cit = rit->begin(); cit != rit->end(); ++cit) {
	//		double x = *cit;
	//
	//for(size_t r = 0; r < rmax; ++r) {
	//	const double_vector & row = m_Values[r] ;
	//	for(size_t c = 0; c < cmax; ++c) {
	//		double x = row[c];

	for(const double_vector & row : m_Values) {
		for(double x : row) {
			bool firstRow = true;

			ToString(buffer, x, decimals);	//	CETIC optimized
			size_t width = wcslen(buffer);						//	0 to maxLen

			if(width < maxWidth) {
				dest.append(spaces[maxWidth - width]) ;		//	0 to maxWidth
			}
			if(firstRow) {
				firstRow = false;
			} else {
				dest.append(separator);
			}
			dest.append(buffer);
		}
		dest.append(L"\r\n");
	}
	return ;
}


std::wostringstream &
double_matrix::Print(std::wostringstream & stream) const
{
	std::wstring dest;
	Print(dest);

	stream << dest;
	return stream;
}


void
double_matrix::PrintRoots(const double_matrix & p_OriginalMatrix) const
{
	if(ColCount() != RowCount() + 1) {
		return;
	}

	size_t rmax = RowCount() ;
	size_t cmax = ColCount() ;

	double_vector roots ;
	roots.resize(rmax, 0.0);


	for(size_t r = rmax - 1; r != -1; --r) { // (r >= 0) cannot work on an unsigned datatype. (size_t)-1 = 0xFFFFFFFF (or 0xFFFFFFFFFFFFFFFF if x64)
		double sum = m_Values[r][cmax-1] ;

		for(size_t c = r + 1; c < rmax; ++c) {
			sum += m_Values[r][c] * roots[c] ;
		}
		roots[r] = -sum / m_Values[r][r] ;
	}
	//	print roots
	for(size_t r = 0; r < rmax; ++r) {
		std::wostringstream stream ;    
		stream << L"Root[" << r << L"] = " <<   roots[r] << L"\r\n" ;
		OutputDebugString(stream.str().c_str());
	}

	//	check roots
	for(size_t r = 0; r < rmax; ++r) {
		double sum = p_OriginalMatrix[r][cmax-1] ;

		for(size_t c = 0; c < cmax-1; ++c) {
			sum += p_OriginalMatrix[r][c] * roots[c] ;
		}
		std::wostringstream stream ;    
		stream << L"Sum[" << r << L"] = " << sum << L"\r\n" ;
		OutputDebugString(stream.str().c_str());
	}
	return ;
}


//	Pearson correlation matrix computation
//	warning : works only to detect linear correlation between variables !
double
double_matrix::Correlation(double_matrix & srce)
{
	if(srce.RowCount() != RowCount() || srce.ColCount() != ColCount() || RowCount() != ColCount()) {
		return(0.0) ;
	}
	//	the summ of X, Y, X square, Y Square and XY
	double X = 0.0, Y = 0.0, XX = 0.0, YY = 0.0, XY = 0.0 ;

	size_t n = 0 ;
	for(size_t r = 0; r < RowCount(); ++r) {
		for(size_t c = r + 1; c < ColCount(); ++c) {
			if(r == c) {
				continue ;
			}
			XY += m_Values[r][c] * srce.m_Values[r][c] ;
			X += m_Values[r][c] ;
			Y += srce.m_Values[r][c] ;
			XX += m_Values[r][c] * m_Values[r][c] ;
			YY += srce.m_Values[r][c] * srce.m_Values[r][c] ;
			++n ;
		}
	}
	//	compute the numerator and denominator
	double num = n * XY - X * Y ;
	double denom = sqrt((n * XX - X * X) * (n * YY - Y * Y)) ;
	//	compute the correlation
	double Corr = 0.0 ;
	if(denom != 0.0) {
		Corr = num / denom ;
	}
	return(max_(-1.0, min_(1.0, Corr))) ;	//	avoid rounding errors and Corr < -1.0 or Corr > 1.0
}


//	Pearson correlation matrix computation
//	warning : works only to detect linear correlation between variables !
double
double_matrix::CorrelationNotZero(double_matrix & srce)
{
	if(srce.RowCount() != RowCount() || srce.ColCount() != ColCount() || RowCount() != ColCount()) {
		return(0.0) ;
	}
	//	the summ of X, Y, X square, Y Square and XY
	double X = 0.0, Y = 0.0, XX = 0.0, YY = 0.0, XY = 0.0 ;

	size_t n = 0 ;
	for(size_t r = 0; r < RowCount(); ++r) {
		for(size_t c = r + 1; c < ColCount(); ++c) {
			if(r == c) {
				continue ;
			}
			if(m_Values[r][c] == 0.0 || srce.m_Values[r][c] == 0.0) {	//	missing data
				continue ;
			}
			XY += m_Values[r][c] * srce.m_Values[r][c] ;
			X += m_Values[r][c] ;
			Y += srce.m_Values[r][c] ;
			XX += m_Values[r][c] * m_Values[r][c] ;
			YY += srce.m_Values[r][c] * srce.m_Values[r][c] ;
			++n ;	//	the number of points, as points at [r][c] are skipped
		}
	}
	//	compute the numerator and denominator
	double num = n * XY - X * Y ;
	double denom = sqrt((n * XX - X * X) * (n * YY - Y * Y)) ;
	//	compute the correlation
	double Corr = 0.0 ;
	if(denom != 0.0) {
		Corr = num / denom ;
	}
	return(max_(-1.0, min_(1.0, Corr))) ;	//	avoid rounding errors and Corr < -1.0 or Corr > 1.0
}


//	****************************************************************************
//											Out of class functions								*
//	****************************************************************************

//	return a (n * m) matrix from the product of a (n * z) matrix with a (z * m) matrix
double_matrix operator *(const double_matrix & a, const double_matrix & b)
{
	if(a.ColCount() != b.RowCount()) {
		std::string errorMessage = std::string("Invalid row and column count in (matrix * matrix) multiplication");
		throw std::out_of_range(errorMessage);
	}
	double_matrix result(a) ;
	result *= b ;
	return result;
}


double_matrix operator +(const double_matrix & a, const double_matrix & b)
{
	if (a.RowCount() != b.RowCount() || a.ColCount() != b.ColCount()) {
		std::string errorMessage = std::string("Invalid row and column count in (matrix + matrix) addition");
		throw std::out_of_range(errorMessage);
	}
	double_matrix result(a);
	result += b;
	return result;
}


double_matrix operator *(double p, const double_matrix & a)
{
	double_matrix result(a) ;
	for(double_vector & row : result.Values()) {
		for(double & x : row) {
			x *= p;
		}
	}
	return result;
}


double_matrix operator *(const double_matrix & a, double p)
{
	return p * a;
}


//	return a (1 * m) vector from the product of a (1 * n) vector with a (n * m) matrix
double_vector operator *(const double_vector & vec, const double_matrix & mat)
{
	if(vec.size() != mat.RowCount()) {
		std::string errorMessage = std::string("Invalid row and column count in (vector * matrix) multiplication");
		throw std::out_of_range(errorMessage);
	}
	double_vector result(mat.RowCount());
	for(size_t c = 0, cmax = mat.ColCount(); c < cmax; ++c) {		//	get a vecteur with this column, so that we copy only once each column in a new vector
		const double_vector & col = mat.GetCol(c);
		result[c] = vec * col ;	//	scalar product
	}
	return result;
}


//	return a (n * 1) vector from the product of a (n * m) matrix with a (m * 1) vector
double_vector operator *(const double_matrix & mat, const double_vector & vec)
{
	if(vec.size() != mat.ColCount()) {
		std::string errorMessage = std::string("Invalid row and column count in (matrix * vector) multiplication");
		throw std::out_of_range(errorMessage);
	}
	double_vector result(mat.RowCount()) ;
	for(size_t r = 0, rmax = mat.RowCount(); r < rmax; ++r) {
		result[r] = mat[r] * vec ;	//	scalar product
	}
	return result;
}


//
//
////	return a (1 * m) matrix from the product of a (1 * n) vector with a (n * m) matrix
//double_matrix operator *(const double_vector & vec, const double_matrix & mat)
//{
//	if (vec.size() != mat.RowCount()) {
//		std::string errorMessage = std::string("Invalid row and column count in (vector * matrix) multiplication");
//		throw std::out_of_range(errorMessage);
//	}
//	double_matrix result(1, mat.ColCount());
//	double_vector & row = result[0];	//	the the first row
//	for (size_t c = 0, cmax = mat.ColCount(); c < cmax; ++c) {		//	get a vecteur with this column, so that we copy only once each column in a new vector
//		const double_vector & col = mat.GetCol(c);
//		row[c] = vec * col;	//	scalar product
//	}
//	return result;
//}
//
//
////	return a (n * 1) matrix from the product of a (n * m) matrix with a (m * 1) vector
//double_matrix operator *(const double_matrix & mat, const double_vector & vec)
//{
//	if (vec.size() != mat.ColCount()) {
//		std::string errorMessage = std::string("Invalid row and column count in (matrix * vector) multiplication");
//		throw std::out_of_range(errorMessage);
//	}
//	double_matrix result(mat.RowCount(), 1);
//	for (size_t r = 0, rmax = mat.RowCount(); r < rmax; ++r) {
//		result[r][0] = mat[r] * vec;	//	scalar product
//	}
//	return result;
//}


std::wostringstream & operator << (std::wostringstream & stream, const double_matrix & mat)
{
	return mat.Print(stream);
}



