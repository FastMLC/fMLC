
#pragma once

#undef min
#undef max

#include <vector>
#include "double_vector.h"
#include <string>
#include <stdexcept>

class double_vector;

template <class T, class R> //	T for the data type, R for the row type
class TMatrixTpl
{
public:
	//typedef std::vector<R>::value_type value_type;
	//typedef std::vector<double_vector>::allocator_type allocator_type;

	TMatrixTpl<T, R>();
	TMatrixTpl<T, R>(const TMatrixTpl<T, R> & srce);
	TMatrixTpl<T, R>(size_t p_RowCount, size_t p_ColCount);
	TMatrixTpl<T, R>(size_t p_RowCount, size_t p_ColCount, const T & x);
	TMatrixTpl<T, R>(std::initializer_list<R> il, const std::allocator<R>& a = std::allocator<R>());	//	allow writing: double_matrix pos = {{ 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0}};

	virtual ~TMatrixTpl<T, R>(void);

	virtual void SymetricKeepTop(); //	copy the elemens above the diagonal into the bottom
	virtual void SymetricKeepBottom(); //	copy the elemens below the diagonal into the top

	virtual void AddRow(const R & p_Srce);
	virtual void AddColumn(const R & p_Srce);
	virtual void RemoveRow(size_t p_RowIdx);
	virtual void RemoveColumn(size_t p_ColumnIdx);
	virtual void resize(size_t p_RowCount, size_t p_ColCount);
	virtual void resize(size_t p_RowCount, size_t p_ColCount, const T & x);
	virtual void reserve(size_t p_RowCount, size_t p_ColCount);
	virtual void reserve(size_t p_RowCount);
	virtual void clear();
	virtual void Free();

	//	Properties
	virtual R & operator [](size_t r);
	virtual const R & operator [](size_t r) const;

	R GetCol(size_t p_Idx) const; //	avoid as much as possible, returns a COPY of the data, not the data
	virtual bool IsSquare() const;

	//	Operations

	std::vector<R> &
	Values()
	{
		return m_Values ;
	}

	const std::vector<R> &
	Values() const
	{
		return m_Values ;
	}

	size_t RowCount() const;
	size_t ColCount() const;
	size_t size() const;

	void SwapRows(size_t r1, size_t r2);
	void Transpose();
	//void Complement();

	//const TMatrixTpl<T, R> Transposed() const ;

protected :
	std::vector<R> m_Values; //	values

	R m_NullRow; //	returned when an error occured, as a null row
};


template <class T, class R>
TMatrixTpl<T, R>::TMatrixTpl(void)
{
	return;
}


template <class T, class R>
TMatrixTpl<T, R>::TMatrixTpl(size_t p_RowCount, size_t p_ColCount)
{
	TMatrixTpl<T, R>::resize(p_RowCount, p_ColCount) ;
	return;
}


template <class T, class R>
TMatrixTpl<T, R>::TMatrixTpl(size_t p_RowCount, size_t p_ColCount, const T & x)
{
	TMatrixTpl<T, R>::resize(p_RowCount, p_ColCount, x) ;
	return;
}

template <class T, class R>
TMatrixTpl<T, R>::TMatrixTpl(std::initializer_list<R> il, const std::allocator<R> & a)
{
	m_Values = std::vector<R>(il, a);
}

template <class T, class R>
TMatrixTpl<T, R>::TMatrixTpl(const TMatrixTpl<T, R> & srce)
{
	m_Values = srce.m_Values ;
	return;
}


template <class T, class R>
TMatrixTpl<T, R>::~TMatrixTpl(void)
{
}


template <class T, class R>
size_t
TMatrixTpl<T, R>::RowCount() const
{
	return m_Values.size() ;
}


template <class T, class R>
size_t
TMatrixTpl<T, R>::ColCount() const
{
	if (m_Values.size() == 0) {
		return 0 ;
	}
	return (m_Values[0].size()) ;
}


template <class T, class R>
size_t
TMatrixTpl<T, R>::size() const
{
	return RowCount() * ColCount() ;
}


template <class T, class R>
void
TMatrixTpl<T, R>::resize(size_t p_RowCount, size_t p_ColCount)
{
	if (p_RowCount <= 0) {
		m_Values.clear() ;
		return;
	}
	m_Values.resize(p_RowCount) ;

	if (p_ColCount > 0) {
		for (typename std::vector<R>::iterator rit = m_Values.begin(); rit < m_Values.end(); ++rit) {
			rit->resize(p_ColCount) ;
		}
	}
	return;
}


template <class T, class R>
void
TMatrixTpl<T, R>::resize(size_t p_RowCount, size_t p_ColCount, const T & x)
{
	if (p_RowCount <= 0) {
		m_Values.clear() ;
		return;
	}
	m_Values.resize(p_RowCount) ;

	if (p_ColCount > 0) {
		for (typename std::vector<R>::iterator rit = m_Values.begin(); rit < m_Values.end(); ++rit) {
			rit->resize(p_ColCount, x) ;
		}
	}
	return;
}


template <class T, class R>
void
TMatrixTpl<T, R>::reserve(size_t p_RowCount)
{
	m_Values.reserve(p_RowCount) ;
}


template <class T, class R>
void
TMatrixTpl<T, R>::reserve(size_t p_RowCount, size_t p_ColCount)
{
	m_Values.reserve(p_RowCount) ;

	for (typename std::vector<R>::iterator rit = m_Values.begin(); rit < m_Values.end(); ++rit) {
		rit->reserve(p_ColCount) ;
	}
	return;
}


template <class T, class R>
R &
TMatrixTpl<T, R>::operator [](size_t r)
{
	return (m_Values[r]) ;
}


template <class T, class R>
const R &
TMatrixTpl<T, R>::operator [](size_t r) const
{
	return (m_Values[r]) ;
}


template <class T, class R>
R
TMatrixTpl<T, R>::GetCol(size_t p_Idx) const
{
	R newCol;
	newCol.reserve(RowCount()) ;

	for (const R & row : m_Values) {
		//for(long r = 0, rmax = RowCount(); r < rmax; ++r) {
		newCol.push_back(row[p_Idx]);
	}
	return newCol;
}

template <class T, class R>
bool
TMatrixTpl<T, R>::IsSquare() const
{
	return ColCount() == RowCount() ;
}

template <class T, class R>
void
TMatrixTpl<T, R>::clear()
{
	//for(std::vector<R >::iterator rit = m_Values.begin(); rit < m_Values.end(); ++rit) {
	for (R & row : m_Values) {
		row.clear() ;
	}
	m_Values.clear() ;
	return;
}


template <class T, class R>
void
TMatrixTpl<T, R>::Free()
{
	clear() ;
	//for (R & row : m_Values) {
	//	row = R();
	//}
	m_Values = std::vector<R>();
}


template <class T, class R>
void
TMatrixTpl<T, R>::AddRow(const R & p_Srce)
{
	if (RowCount() > 0 && p_Srce.size() != ColCount()) {
		std::string errorMessage = std::string("New row has a wrong column number");
		throw std::out_of_range(errorMessage);
	}
	m_Values.push_back(p_Srce);
	return;
}


template <class T, class R>
void
TMatrixTpl<T, R>::RemoveRow(size_t p_RowIdx)
{
	if (p_RowIdx >= RowCount()) {
		std::string errorMessage = std::string("Row index out of range");
		throw std::out_of_range(errorMessage);
	}
	m_Values.erase(m_Values.begin() + p_RowIdx) ;
	return;
}


template <class T, class R>
void
TMatrixTpl<T, R>::AddColumn(const R & p_Srce)
{
	if (RowCount() == 0) {
		for (const T & x : p_Srce) {
			m_Values.push_back(R());
			m_Values.back().push_back(x) ;
		}
	}
	else {
		if (p_Srce.size() != RowCount()) {
			std::string errorMessage = std::string("New column has a wrong item number");
			throw std::out_of_range(errorMessage);
		}
		size_t r = 0;
		for (const T & x : p_Srce) {
			m_Values[r].push_back(x) ;
			++r ;
		}
	}
	return;
}


template <class T, class R>
void
TMatrixTpl<T, R>::RemoveColumn(size_t p_ColIdx)
{
	if (p_ColIdx >= ColCount()) {
		std::string errorMessage = std::string("Column index out of range");
		throw std::out_of_range(errorMessage);
	}
	for (R & row : m_Values) {
		row.erase(row.begin() + p_ColIdx) ;
	}
	return;
}


//	copy the top triangle of a symetrical matrix into the bottom part. Diagonal values are untouched
template <class T, class R>
void
TMatrixTpl<T, R>::SymetricKeepTop()
{
	if (! IsSquare()) { //	Matrix must be square
		return;
	}
	for (size_t r = 1, rmax = RowCount(); r < rmax; ++r) {
		for (size_t c = 0; c < r; ++c) {
			m_Values[r][c] = m_Values[c][r] ;
		}
	}
	return;
}


//	copy the bottom triangle of a symetrical matrix into the top part. Diagonal values are untouched
template <class T, class R>
void
TMatrixTpl<T, R>::SymetricKeepBottom()
{
	if (! IsSquare()) { //	Matrix must be square
		std::string errorMessage = std::string("SymetricKeepBottom() error: matrix must be square");
		throw std::invalid_argument(errorMessage);
	}
	for (size_t r = 0, rmax = RowCount() - 1, cmax = RowCount(); r < rmax; ++r) {
		for (size_t c = r + 1; c < cmax; ++c) {
			m_Values[r][c] = m_Values[c][r] ;
		}
	}
	return;
}


template <class T, class R>
void
TMatrixTpl<T, R>::SwapRows(size_t r1, size_t r2)
{
	if (r1 >= RowCount() || r2 >= RowCount()) {
		std::string errorMessage = std::string("SwapRows() error: out of range row index");
		throw std::out_of_range(errorMessage);
	}
	if (r1 != r2) {
		std::swap(m_Values[r1], m_Values[r2]);
	}
	return;
}


template <class T, class R>
void
TMatrixTpl<T, R>::Transpose()
{
	////	in place transposition for a square matrix
	//for(int32_t r = 0, rmax = min(m_ColCount, m_RowCount); r < rmax; ++r) {
	//	for(int32_t c = 0, cmax = r - 1; c < cmax; ++c) {
	//		std::swap(m_Values[r][c], m_Values[c][r]) ;
	//	}
	//}

	//	allocate the new values
	std::vector<R> newValues;

	//	read column per colum
	for (size_t c = 0, cmax = ColCount(); c < cmax; ++c) {
		//	get a vecteur with the source column, so that we copy only once each column in a new vector
		double_vector col = GetCol(c);
		newValues.push_back(col);
	}
	m_Values = newValues ;
	return;
}


//template<class T, class R>
//const TMatrixTpl<T, R>
//TMatrixTpl<T, R>::Transposed() const
//{
//	TMatrixTpl<T, R> result(* this);
//	result.Transpose();
//	return result;
//}

////	****************************************************	TNumMatrix	***********************************************************
//
//template<class T, class R>
//class TNumMatrix : public TMatrixTpl<T, R> {
//	public:
//		TNumMatrix<T, R>() ;
//		TNumMatrix<T, R>(size_t p_RowNo, size_t p_ColNo) ;
//		TNumMatrix<T, R>(const TNumMatrix<T, R> & srce) ;
//		 
//		virtual ~TNumMatrix<T, R>(void) ;
//
//		void						SetName(const wchar_t * p_Name)	{	m_Name = p_Name ;					}
//		const std::wstring	GetName() const						{	return m_Name ;					}
//		
//		void						SetRowName(int32_t rowIdx, const wchar_t * p_Name)	{	if(rowIdx >= 0 && rowIdx < RowCount()) {	m_vRowNames[rowIdx] = p_Name ;	}	}
//		const std::wstring	GetRowName(int32_t rowIdx) const							{	if(rowIdx >= 0 && rowIdx < RowCount()) {	return m_vRowNames[rowIdx] ;		}	}
//		
//		void						SetColName(int32_t colIdx, const wchar_t * p_Name)	{	if(colIdx >= 0 && colIdx < ColCount()) {	m_vColNames[colIdx] = p_Name ;	}	}
//		const std::wstring	GetColName(int32_t colIdx) const							{	if(colIdx >= 0 && colIdx < ColCount()) {	return m_vColNames[colIdx] ;		}	}
//		
//		virtual void	resize(size_t p_RowNo, size_t p_ColNo) ;
//		virtual void	clear() ;
//
//		void	Complement(T p_Value) ;							//	Recompute each value x of the matrix as p_Value - x
//	
//	protected :	
//		std::wstring					m_Name ;					//	a name for this matrix
//		std::vector<std::wstring>	m_vRowNames ;			//	row names
//		std::vector<std::wstring>	m_vColNames ;			//	column names
//};


//template<class T, class R>
//TNumMatrix<T, R>::TNumMatrix(void)
//: TMatrixTpl<T, R>()
//{
//	return ;
//}
//
//
//template<class T, class R>
//TNumMatrix<T, R>::TNumMatrix(size_t p_RowNo, size_t p_ColNo)
//: TMatrixTpl<T, R> (p_RowNo, p_ColNo)
//{
//	m_vRowNames.resize(RowCount()) ;
//	m_vColNames.resize(ColCount()) ;
//	return ;
//}
//
//
//template<class T, class R>
//TNumMatrix<T, R>::TNumMatrix(const TNumMatrix & srce)
//: TMatrixTpl<T, R> (srce)
//{
//	m_Name = srce.m_Name ;
//	m_vRowNames = srce.m_vRowNames ;
//	m_vColNames = srce.m_vColNames ;
//	return ;
//}
//
//
//template<class T, class R>
//TNumMatrix<T, R>::~TNumMatrix(void)
//{
//}
//
//
//template<class T, class R>
//void
//TNumMatrix<T, R>::resize(size_t p_RowNo, size_t p_ColNo)
//{
//	TMatrixTpl<T, R>::resize(p_RowNo, p_ColNo) ;
//	
//	m_vRowNames.resize(RowCount()) ;
//	m_vColNames.resize(ColCount()) ;
//	return ;
//}
//
//
//template<class T, class R>
//void
//TNumMatrix<T, R>::RemoveRow(size_t p_RowIdx)
//{
//	TMatrixTpl<T, R>::RemoveRow(p_RowIdx) ;
//	m_vRowNames.erase(m_vRowNames.begin() + p_RowIdx) ;
//	return ;
//}
//
//
//template<class T, class R>
//void
//TNumMatrix<T, R>::RemoveColumn(size_t p_ColumnIdx)
//{
//	TMatrixTpl<T, R>::RemoveColumn(p_ColumnIdx) ;
//	m_vColNames.erase(m_vColNames.begin() + p_ColumnIdx) ;
//	return ;
//}
//
//
////	Update computed values
//template<class T, class R>
//void
//TNumMatrix<T, R>::clear()
//{
//	TMatrixTpl<T, R>::clear() ;
//
//	m_vRowNames.clear() ;
//	m_vColNames.clear() ;
//	return ;
//}
//
//
//template<class T, class R>
//void
//TNumMatrix<T, R>::Complement(T p_Value)
//{
//	//for(std::vector<R >::iterator rit = m_Values.begin(); rit < m_Values.end(); ++rit) {
//	//	R & Row = * rit ;
//	//	for(R::iterator it = Row.begin(); it < Row.end(); ++it) {
//	//		* it = p_Value - (* it) ;
//	//	}
//	//}
//	//	C++11 version
//	for(double_vector & row : m_Values) {
//		for(double & x : row) {
//			x = p_Value - x ;
//		}
//	}
//	return ;
//}


//	******************************************************	int32_t vector	***************************************************************

typedef std::vector<int32_t> int_vector;


//	******************************************************	std::string matrix	***************************************************************

typedef std::vector<std::string> string_vector;

class string_matrix : public TMatrixTpl<std::string, string_vector>
{
public :
	string_matrix() : TMatrixTpl<std::string, string_vector>()
	{
	}

	string_matrix(const string_matrix & srce) : TMatrixTpl<std::string, string_vector>(srce)
	{
	}

	string_matrix(size_t p_RowNo, size_t p_ColNo) : TMatrixTpl<std::string, string_vector>(p_RowNo, p_ColNo)
	{
	}

	size_t
	SizeAsString()
	{
		size_t len = 0;
		for (size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
			for (size_t c = 0, cmax = ColCount(); c < cmax; ++c) {
				len += m_Values[r][c].size() ;
			}
		}
		return len ;
	}

	std::string &
	AppendTo(std::string & dest)
	{
		dest.reserve(dest.size() + SizeAsString()) ;
		for (size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
			for (size_t c = 0, cmax = ColCount(); c < cmax; ++c) {
				dest += m_Values[r][c] ;
			}
		}
		return dest ;
	}
};


//	******************************************************	std::wstring matrix	***************************************************************

typedef std::vector<std::wstring> wstring_vector;

class wstring_matrix : public TMatrixTpl<std::wstring, wstring_vector>
{
public :
	wstring_matrix() : TMatrixTpl<std::wstring, wstring_vector>()
	{
	}

	wstring_matrix(const wstring_matrix & srce) : TMatrixTpl<std::wstring, wstring_vector>(srce)
	{
	}

	wstring_matrix(size_t p_RowNo, size_t p_ColNo) : TMatrixTpl<std::wstring, wstring_vector>(p_RowNo, p_ColNo)
	{
	}

	size_t
	SizeAsString()
	{
		size_t len = 0;
		for (size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
			for (size_t c = 0, cmax = ColCount(); c < cmax; ++c) {
				len += m_Values[r][c].size() ;
			}
		}
		return len ;
	}

	std::wstring &
	AppendTo(std::wstring & dest)
	{
		dest.reserve(dest.size() + SizeAsString()) ;
		for (size_t r = 0, rmax = RowCount(); r < rmax; ++r) {
			for (size_t c = 0, cmax = ColCount(); c < cmax; ++c) {
				dest += m_Values[r][c] ;
			}
		}
		return dest ;
	}
};
