
#pragma once

#include <vector>

class double_vector : public std::vector<double> {
public:
	double_vector();
	double_vector(const double_vector & srce);	//	copy ctor
	explicit double_vector(size_t p_Count);
	double_vector(size_t p_Count, double x);

	double_vector(std::initializer_list<value_type> il, const allocator_type& a = allocator_type());	//	allow writing: double_vector pos = { 1.0, 2.0, 3.0 };
	double_vector(const double * _First_arg, const double * _Last_arg);

	//	operators
	void	operator +=(double p);
	void	operator -=(double p);
	void	operator *=(double p);
	void	operator /=(double p);

	void	operator +=(const double_vector & p);
	void	operator	-=(const double_vector & p);

	void	Print() const;
	void	Print(std::wstring & dest) const;
	std::wostringstream &	Print(std::wostringstream & stream) const;

	//	operations, allow cascading: matrix.Pow(3).Round().SymetricAverage();

	const double_vector & Random();										//	generate a random matrix
																					//const double_vector & RandomDistance() ;							//	generate a random distance matrix, symmetric, diagonal values = 0.0

																					//	all these functions exist in two versions:
																					//	- to be used on a double_matrix A, eventualy with cascading:	A.Pow2().Round(4);
																					//	- to get a copy from a const double_matrix A:						double_matrix mat = A.Round();

	double_vector &		RoundMe(int32_t decimals);							//	round to n decimals
	const double_vector	Round(int32_t decimals) const;

	double_vector &		PowMe(double p);									//	x = x^p_Power
	const double_vector	Pow(double p) const;

	double_vector &		Pow2Me();												//	x = x * x
	const double_vector	Pow2() const;

	double_vector &		RootMe();												//	x = sqrt(x)
	const double_vector	Root() const;

	double_vector &		ComplementMe(double p);							//	x = p - x
	const double_vector	Complement(double p) const;

	//const double_matrix & GaussJordan(double_vector & p_Averages, bool p_Triangle) ;			//	used to triangle, diagonal and invert
	double_vector &		NormalizeMe();										//	normalize each row separately, so that sum(xi * xi) = 1
	const double_vector	Normalize() const;

	//	functions
	double	Average() const;													//	return the average of all matrix values
	double	Magnitude() const;												//	return the length of the vector (summ of square of each x)
	bool		IsNull() const;													//	return true if all values are 0.0

};

//	Out of class operations
double				operator *(const double_vector & a, const double_vector & b);		//	scalar product (dot product) : double_vector * double_vector
double_vector		operator +(const double_vector & a, const double_vector & b);		//	simple addition of each item
double				Distance(const double_vector & a, const double_vector & b);

double_vector		operator *(double p, const double_vector & a);							//	5.2 * double_vector
double_vector		operator *(const double_vector & a, double p);							//	double_vector * 2.5

std::wostringstream & operator << (std::wostringstream & stream, const double_vector & mat);


