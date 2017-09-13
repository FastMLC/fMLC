
//	see: http://eigen.tuxfamily.org/dox/GettingStarted.html

//#include "stdafx.h"

#include "double_matrix.h"

namespace EigenVal {

void Eigen(const double_matrix & p_Distance, 
			  double_vector & p_EigenValues, 
			  double_matrix & p_EigenVectors);

void Mds(const double_matrix & p_Distance, 
			double_vector & p_EigenValues, 
			double_matrix & p_EigenVectors, 
			double_matrix & p_Mds, 
			double_matrix & p_EstimDistMds, 
			double_vector & p_Stress);

};
