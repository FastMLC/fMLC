
#include "stdafx.h"

#include "matrix_context.h"

matrix_context::matrix_context(size_t p_RMin, size_t p_RMax, size_t p_CMin, size_t p_CMax, double_matrix & p_Mat1, const double_matrix & p_Mat2)
:		r(p_RMin),
		rmin(p_RMin),
		rmax(p_RMax),
		c(p_CMin),
		cmin(p_CMin),
		cmax(p_CMax),
		mat1(p_Mat1),
		mat2(p_Mat2)
{
	m_WaitIdx = 0;
	return ;
}


matrix_context::~matrix_context(void)
{
}
