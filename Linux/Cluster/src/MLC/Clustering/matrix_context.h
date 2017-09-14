
#pragma once

#include "double_matrix.h"

#include <mutex>


class matrix_context
{
	public:
		matrix_context(size_t p_RMin, size_t p_RMax, size_t p_CMin, size_t p_CMax, double_matrix & p_Mat1, const double_matrix & p_Mat2);
		~matrix_context(void) ;

		//	multithreading indexes and mutex
		std::mutex	m_IdxMutex ;		//	lock access to m_r and m_RefIdx from PreComputeRef_thread() and Compare_thread()

		//	members set from ctor parameters
		size_t	r;	//	usage: for(size_t r = rmin; r < rmax; ++r)
		size_t	c;
		size_t	rmin;
		size_t	rmax;
		size_t	cmin;
		size_t	cmax;

		size_t	m_WaitIdx ;
		
		double_matrix &			mat1;		//	a first (destination) matrix
		const double_matrix &	mat2;		//	a second matrix

	private :
		explicit matrix_context(const matrix_context & ) ;
		matrix_context & operator=(const matrix_context & ) = delete ;	//	avoid implicit assignement operator
};

