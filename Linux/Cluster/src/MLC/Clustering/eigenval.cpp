
//#include "stdafx.h"

#include <iostream>

#include <Eigen/Dense>
        
using namespace std;
//using namespace Eigen;

#include "double_matrix.h"
#include "std_plugin.h"

#include <math.h>
#include <stdio.h>
#include <thread>

// zp 24062017 Removing this since it is obtained from DLL
//extern uint32_t g_MaxThread ;	//	depend in the number of cores and processors
uint32_t g_MaxThread = 1;	//	depend in the number of cores and processors

//#define _TESTING_

namespace EigenVal {


/******************************************************************************
Compute the eigen values and vectors from a square distance matrix

INPUTS:
	p_Distance		: a distance matrix containing all data

OUTPUTS:
	p_EigenValues	: all eigen values (n values at most)
	p_EigenVectors	: all eigen vectors (n * n) each in one column                 

*******************************************************************************/

void
Eigen(const double_matrix & p_Distance, double_vector & p_EigenValues, double_matrix & p_EigenVectors)
{
    double start = 0;//timeGetTime() ;
	
	p_EigenVectors.clear() ;
	p_EigenValues.clear() ;

	size_t n = p_Distance.RowCount() ;
	if(n < 2) {
		std::string errorMessage = std::string("Matrix must be at least (2 * 2) to compute the eigen values and eigen vectors");
		throw std::range_error(errorMessage);
		return ;
	}
	if(n != p_Distance.ColCount()) {
		std::string errorMessage = std::string("Matrix must be square to compute the eigen values and eigen vectors");
		throw std::range_error(errorMessage);
		return ;
	}
	
	using namespace Eigen;

	MatrixXd A(n, n) ;
	for(size_t r = 0; r < n; ++r) {
		for(size_t c = 0; c < n; ++c) {
			A(r, c) = p_Distance[r][c];
		}
	}
	
	{
#ifdef _TESTING_
		std::wostringstream stream ;    
		stream << L"A:" << std::endl;
		stream << p_Distance << std::endl ;
#endif
		SelfAdjointEigenSolver<MatrixXd> eigensolver(A);
		//GeneralizedEigenSolver<MatrixXd> eigensolver(A);

		if (eigensolver.info() != Success) {
			std::string errorMessage = std::string("Error during eigen values and vectors computation");
			throw std::range_error(errorMessage);
			return ;
		}
		//stream << "The eigenvalues of A are:" << std::endl << eigensolver.eigenvalues() << std::endl;
		//stream << "Here's a matrix whose columns are eigenvectors of A corresponding to these eigenvalues:" << std::endl;
		//stream << eigensolver.eigenvectors() << std::endl;
	
		//	copy data back
		p_EigenValues.reserve(n) ;
		p_EigenVectors.reserve(n) ;
		for (size_t r = 0; r < n; ++r) {
			p_EigenValues.push_back(eigensolver.eigenvalues()(r)) ;

			double_vector row ;
			row.reserve(n);
		
			for (size_t c = 0; c < n; ++c) {
				row.push_back(eigensolver.eigenvectors()(r, c)) ;
			}
			p_EigenVectors.AddRow(row);
		}
	}	//	free memory

#ifdef _TESTING_
	//	display for debugging
	stream << "The eigenvalues of A are:" << std::endl;
	stream << p_EigenValues << std::endl;
	stream << "Here's a matrix whose columns are eigenvectors of A corresponding to these eigenvalues:" << std::endl;
	stream << p_EigenVectors << std::endl;

	//	testing: by definition, if V is an eigen vector (n * 1) and λ is the corresponding eigen value of (n * n) matrix A, then: AV = λV

	for(size_t i = 0, imax = p_EigenValues.size(); i < imax; ++i) {
		//	get the eigen value
		double lambda = p_EigenValues[i] ;

		//	get the corresponding eigen vector
		double_matrix v;
		v.AddColumn(p_EigenVectors.GetCol(i)) ;	//	(n * 1)
		//	multiply
		double_matrix AV(p_Distance * v) ;			//	(n * n) * (n * 1) => (n * 1)
		double_matrix λV(lambda * v) ;				// (n * 1)

		AV.Round(6); 
		λV.Round(6);

		if(AV != λV) {
			stream << L"Error during eigen values and vectors computations for λ[" << i << L"]" << std::endl ;
			stream << L"left:" << std::endl;
			stream << AV << std::endl;

			stream << L"λV:" << std::endl;
			stream << λV << std::endl ;
		}
		else {
			stream << L"AV = λV checked successfully" << std::endl ;
		}
	}
	OutputDebugString(stream.str().c_str());
#endif
	return ;
}


/******************************************************************************
Compute the MDS matrix from a square distance matrix

INPUTS:
	p_Distance		: a distance matrix containing all data

OUTPUTS:
	p_EigenValues	: all eigen values (n values at most)
	p_EigenVectors	: all eigen vectors (n * n) each in one column                 
	p_Mds				: (n * 3) matris with n points in 3 dimensions. This matrix is empty if an error occured.

*******************************************************************************/

void
Mds(const double_matrix & p_Distance, double_vector & p_EigenValues, double_matrix & p_EigenVectors, double_matrix & p_Mds, double_matrix & p_EstimDist, double_vector & p_Stress)
{
	p_Mds.clear() ;	//	the result matrix will be (n * 3) if no error occurs

	//	compute the double centered matrix
	double_matrix mat = p_Distance.DoubleCentered();

	size_t n = mat.RowCount() ;

	if(n < 3) {
		return ;	//	three dimentional Mds doesn't make sense on a two-dimentional array
	}

	Eigen(mat, p_EigenValues, p_EigenVectors) ;

	if(p_EigenValues.size() < 3) { // no convergence, or no eigen values
		return ;
	}

	//	get the index and the values of the three greatest eigen positive values in one pass
	//	note that the current algorithm returns the eigen values in ascending order. Taking the last and positive values would be enough.
	//	but in the future we may use other algorithms that don't return the eigen values in any predefined order.
	double_vector eigenValues ;	//	in descending order
	eigenValues.resize(3, 0.0) ;	//	initializing with 0.0 will avoid keeping negative eigen values

	std::vector<size_t>	eigenIdx ;	//	could be int, until we get a (2E9 * 2E9) matrix...
	eigenIdx.resize(3);

	for(size_t i = 0; i < n; ++i) {
		if(p_EigenValues[i] > eigenValues[2]) {
			if(p_EigenValues[i] > eigenValues[1]) {
				if(p_EigenValues[i] > eigenValues[0]) { // insert at [0]
					eigenValues[2] = eigenValues[1] ;
					eigenValues[1] = eigenValues[0] ;
					eigenValues[0] = p_EigenValues[i] ;
					eigenIdx[2] = eigenIdx[1] ;
					eigenIdx[1] = eigenIdx[0] ;
					eigenIdx[0] = i ;
				}
				else {	 // insert at [1]
					eigenValues[2] = eigenValues[1] ;
					eigenValues[1] = p_EigenValues[i] ;
					eigenIdx[2] = eigenIdx[1] ;
					eigenIdx[1] = i ;
				}
			}
			else { // insert at [2]
				eigenValues[2] = p_EigenValues[i] ;
				eigenIdx[2] = i ;
			}
		}
	}
	//	remove trailing zeroes, in case there is not 3 positive eigen values
	while(eigenValues.size() > 0 && eigenValues.back() == 0) {
		eigenValues.pop_back() ;
		eigenIdx.pop_back() ;
	}
	//	now we known that we have q positive eigen values
	size_t q = eigenValues.size() ;

	//	keep q eigen vectors linked to the q bigger positive eigen values. This is a (n * q) matrix

	//	normalize the eigen vectors, so that (eigenVectors * eigenVectors') = 1
	//	note that the current algorithm returns already normalized eigen vector
	//	eigenVectors.NormalizeColumns() ;

	//	If V is the (n * q) eigen vector matrix and L is a (q * q) diagonal matrix with the eigen values, final points are given by X = V * L^0.5
	p_Mds.clear();
	p_Mds.reserve(n, 3);

	for (size_t r = 0; r < n; ++r) {
		double_vector vec;

		for (size_t k = 0; k < q; ++k) {
			size_t c = eigenIdx[k] ;	//	get the column index of the eigen vector ( = index of the eigen values)
			vec.push_back(p_EigenVectors[r][c] * std::sqrt(p_EigenValues[c])) ;
		}
		p_Mds.AddRow(vec) ;
	}
	//	if there is only 0, 1 or 2 positive eigen values, returns 3D^points anyway ?
	p_Mds.resize(n, 3);

	//	compute the stress for one and two dimensions. cf page 14 of http://homepages.uni-tuebingen.de/florian.wickelmaier/pubs/Wickelmaier2003SQRU.pdf
	double stress1 = 0.0 ;
	double stress2 = 0.0 ;
	for (size_t r = 1; r < n; ++r) {
		for (size_t c = 0; c < r; ++c) {
			double dx = p_Mds[r][0] - p_Mds[c][0];
			double dy = p_Mds[r][1] - p_Mds[c][1];
			double estimDist1 = std::abs(dx);
			double estimDist2 = sqrt(dx * dx + dy * dy);
			stress1 += (p_Distance[r][c] - estimDist1) * (p_Distance[r][c] - estimDist1) ;
			stress2 += (p_Distance[r][c] - estimDist2) * (p_Distance[r][c] - estimDist2) ;
		}
	}

	//	compute the estimated distances for three dimensions
	p_EstimDist.resize(n, n);
	double stress3 = 0.0 ;
	for (size_t r = 1; r < n; ++r) {
		for (size_t c = 0; c < r; ++c) {
			double estimDist3 = Distance(p_Mds[r], p_Mds[c]);
			p_EstimDist[r][c] = p_EstimDist[c][r] = estimDist3;
			stress3 += (p_Distance[r][c] - estimDist3) * (p_Distance[r][c] - estimDist3) ;
		}
	}

	double sumDistSquare = p_Distance.SumOfSquares() ;
	p_Stress.clear();
	p_Stress.push_back(std::sqrt(2 * stress1 / sumDistSquare)) ;
	p_Stress.push_back(std::sqrt(2 * stress2 / sumDistSquare)) ;
	p_Stress.push_back(std::sqrt(2 * stress3 / sumDistSquare)) ;

	return ;	//	check p_Mds.RowCount() for error checking
}

} //	end namespace EigenVal
