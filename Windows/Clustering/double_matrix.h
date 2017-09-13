
#pragma once

#include "MatrixTpl.h"
#include "double_vector.h"

class double_matrix : public TMatrixTpl<double, double_vector> {
	public :
		typedef std::vector<double_vector>::value_type value_type;
		typedef std::vector<double_vector>::allocator_type allocator_type;

		double_matrix() ;
		double_matrix(const double_matrix & srce) ;	//	copy ctor
		double_matrix(size_t p_RowNo, size_t p_ColNo) ;
		double_matrix(size_t p_RowNo, size_t p_ColNo, double x) ;
		double_matrix(std::initializer_list<value_type> il, const allocator_type& a = allocator_type());	//	allow writing: double_matrix pos = {{ 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0}};
			
		//	operators
		bool	operator !=(const double_matrix & srce) const;
		bool	operator ==(const double_matrix & srce) const;
		void	operator *=(const double_matrix & srce);
		void	operator +=(const double_matrix & srce);
		void	operator *=(double p);
		void	operator /=(double p);
		void	operator +=(double p);
		void	operator -=(double p);

		void operator =(const char * srce) ;

		double_vector & operator [](size_t r) override;
		const double_vector & operator [](size_t r) const override;

		//	loading and saving
		bool Load(const wchar_t * p_FullFileName) ;
		bool Save(const wchar_t * p_FullFileName) const ;

		void	Print() const ;
		void	Print(std::wstring & dest) const;
		std::wostringstream &	Print(std::wostringstream & stream) const;
		void	PrintRoots(const double_matrix & p_OriginalMatrix) const ;

		//bool LoadW(const wchar_t * p_FullFileName) ;
		//bool SaveW(const wchar_t * p_FullFileName) ;

		//	operations, allow cascading: matrix.Pow(3).Round().SymetricAverage();

		double_matrix &	RandomDebug() ;									//	generate a random matrix
		double_matrix &	Random() ;											//	generate a random matrix
		double_matrix &	RandomDistance() ;								//	generate a random distance matrix, symmetric, diagonal values = 0.0

		//	all these functions exist in two versions:
		//	- to be used on a double_matrix A, eventualy with cascading:	A.Pow2().Round(4);
		//	- to get a copy from a const double_matrix A:						double_matrix mat = A.Round();

		double_matrix &	RoundMe(int32_t decimals) ;						//	round to n decimals
		double_matrix		Round(int32_t decimals) const ;

		double_matrix &	PowMe(double p) ;						//	x = x^p_Power
		double_matrix		Pow(double p) const ;

		double_matrix &	Pow2Me() ;											//	x = x * x
		double_matrix		Pow2() const ;

		double_matrix &	RootMe() ;												//	x = sqrt(x)
		double_matrix		Root() const ;

		double_matrix &	SymetricAverageMe() ;								//	compute the average value of the symmetric pairs and store them in both left and right triangles
		double_matrix		SymetricAverage() const ;

		double_matrix &	ComplementMe(double p) ;							//	x = p - x
		double_matrix		Complement(double p) const ;
		
		double_matrix &	TriangleMe() ;										//	triangularize the matrix by Gauss-Jordan elimination
		double_matrix		Triangle() const ;

		double_matrix &	DiagonalMe() ;										//	double-triangularize the matrix: keep only the diagonal by Gauss-Jordan elimination
		double_matrix		Diagonal() const ;

		double_matrix &	InvertMe() ;											//	invert a square matrix
		double_matrix		Invert() const ;

		double_matrix &	NormalizeRowsMe() ;									//	normalize each row separately, so that sum(xi * xi) = 1
		double_matrix		NormalizeRows() const ;

		double_matrix &	TransposeMe() ;										//	return the transposed matrix, useful in: double_matrix A = B * B.Transposed();
		double_matrix		Transpose() const ;

		double_matrix		Gram() const ;										//	return the Gram matrix, where each x(i, j) = row(i) * row(j)
		double_matrix		DoubleCentered() const ;

		double	Correlation(double_matrix & srce) ;				//	compute the Pearson correlation coefficient. Diagonal values are ignored
		double	CorrelationNotZero(double_matrix & srce) ;	//	compute the Pearson correlation coefficient. Diagonal values and null values are ignored

		//	functions
		double	Average() const ;							//	return the average of all matrix values
		double	Sum() const ;								//	return the summ of all matrix values
		double	SumOfSquares() const ;					//	return the summ of all matrix (values^2)

} ;

//	Out of class operations
double_matrix		operator *(const double_matrix & a, const double_matrix & b);		//	Matrix1 * Matrix2
double_matrix		operator +(const double_matrix & a, const double_matrix & b);		//	Matrix1 + Matrix2

double_matrix		operator *(double p, const double_matrix & a);							//	5.2 * Matrix
double_matrix		operator *(const double_matrix & a, double p);							//	Matrix * 2.5

double_vector		operator *(const double_vector & a, const double_matrix & b);		//	Vector * Matrix
double_vector		operator *(const double_matrix & a, const double_vector & b);		//	Matrix * Vector
//double_matrix		operator *(const double_vector & a, const double_matrix & b);		//	Vector * Matrix
//double_matrix		operator *(const double_matrix & a, const double_vector & b);		//	Matrix * Vector

std::wostringstream & operator << (std::wostringstream & stream, const double_matrix & mat);


