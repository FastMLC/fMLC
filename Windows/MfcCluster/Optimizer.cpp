
#include "stdafx.h"

#include "boost_plugin.h"
#include "Optimizer.h"
#include <sstream>
#include <algorithm>

//#include "../../Program Files (x86)/Microsoft Visual Studio 14.0/VC/include/sstream"


TOptimizer::TOptimizer(vector<double> & p_StartParameters, double p_Precision)
	: m_Precision(p_Precision), m_W(p_StartParameters)
{
	Init();
	for (double p : m_W) {
		if(p != 0) {
			m_Dw.push_back(fabs(p) / 10.0) ;
		}
		else {
			m_Dw.push_back(0.1) ;
		}
	}
}


TOptimizer::TOptimizer(vector<double> & p_StartParameters, const vector<double> & p_Steps, double p_Precision)
	: m_Precision(p_Precision), m_W(p_StartParameters), m_Dw(p_Steps)
{
	Init();
}


TOptimizer::~TOptimizer()
{
}


void
TOptimizer::SetAlgorithm(Optimizer::AlgorithmEnum p_Algo)
{
	m_Algo = p_Algo;
}


//	call this function to re-use the same instance a second time
void
TOptimizer::Init()
{
	m_NumPnts = static_cast<int>(m_W.size()) ;
	m_ErrorCode = 0 ;
	m_ErrorValue = 0.0 ;
	m_Algo = Optimizer::StepByStep ;
	m_Step = 0 ;
	m_BestChi = HUGE_VAL ;

	m_Failures = 0;
	m_CycleNo = 0;

	// ReSharper disable once CppEntityNeverUsed
	m_Chi.resize(m_W.size(), 0.0) ;
	m_Improving.resize(m_W.size(), false) ;
	m_Hold.resize(m_W.size(), false) ;

	m_NumParam = 0;	//	the number of real parameters to fit (less than NumPoints if some parameters are hold)
	m_OneParam = 0;	//	the index of the unique parameter if there is only one parameter to fit

	m_BestW = m_W ;

	for(int i = 0; i < m_NumPnts; ++i) {
		if(! m_Hold[i]) {
			m_NumParam += 1 ;
			m_OneParam = i ;
		}
	}
	if(m_NumParam == 1) { //	optimized on a parabolic curve with prec = 0.0001, note that m_Mult can be nearly anything between 1.0 and 2, if m_Div = 2.8, the cycleNo remains constant.
		m_Mult = 1.157 ;
		m_Div = 2.0 ;
	}
	else { //	optimized on a parabolic cup with prec = 0.0001
		m_Mult = 1.157 ;
		m_Div = 2.0;//3.2996 ;
	}
	return;
}


//	return true if at least one parameter can be optimized
bool
TOptimizer::IsValidParameter() const
{
	for(bool x : m_Hold) {
		if(x == false) {
			return true;
		}
	}
	return false;
}

//
//const std::string
//TOptimizer::KeyParam() const
//{
//	char buffer[32 + 6 * m_W.size()];
//	std::ostringstream stream;
//	for(double x : m_W) {
//		sprintf_s(buffer, 32, "%a*", x);
//		stream << buffer << "*";
//	}
//	return stream.str();
//}


void 
	TOptimizer::Test()
{
	vector<double> parameters ;
	//	vector<double> steps ;

	parameters.push_back(10) ;
	parameters.push_back(20) ;

	TOptimizer opt(parameters, 0.0001);
	opt.m_Algo = Optimizer::AlgorithmEnum::Simplex ;

	//	direct call functions
	//	opt.Minimize();
	// opt.Simplex();

	double err ;
	do {
		err = TheModel(parameters);
	} while(opt.Next(err));
}


//	optimize the MULT and DIV variables of our own algorithm
int 
	TOptimizer::OptimizeMultAndDiv(vector<double> & p_X)
{
	vector<double> parameters ;
	vector<double> steps ;

	parameters.push_back(10) ;
	parameters.push_back(20) ;

	TOptimizer opt(parameters, 0.0001);
	double err;

	p_X[0] = std::max(1.05, p_X[0]);
	opt.m_Mult = p_X[0];
	opt.m_Div = p_X[1];
	//	opt.Minimize();

	do {
		err = TOptimizer::TheModel(parameters);
	} while(opt.Next(err));

	parameters.clear();
	parameters.push_back(4.9) ;
	parameters.push_back(4.1) ;

	TOptimizer opt2(parameters, 0.0001);

	p_X[0] = std::max(1.05, p_X[0]);
	opt2.m_Mult = p_X[0];
	opt2.m_Div = p_X[1];

	do {
		err = TOptimizer::TheModel(parameters);
	} while(opt2.Next(err));

	return opt.m_CycleNo + opt2.m_CycleNo ;
}


double 
	TOptimizer::TheModel(const vector<double> & p_Params)
{
	if(p_Params.size() == 1) {		//	simple parabol with bottm at (5, 3)
		double x = p_Params[0];
		double z = 3 + (x-5) * (x-5);
		return z;
	}
	else if(p_Params.size() == 2) {	//	paraboloic cup with 2 dimensions, vertical axes being (x = 5, y = 4), bottom at z = 3
		double x = p_Params[0] ;
		double y = p_Params[1] ;

		double z = 3 + (x-5) * (x-5) + (y-4) * (y-4) ;
		return z;
	}
	return 0.0;
}


bool
	TOptimizer::CheckParamPrecision() const
{
	for(size_t i = 0, imax = m_Dw.size(); i < imax; ++i) {
		if(m_W[i] == 0.0) {
			if(fabs(m_Dw[i]) > m_Precision) {
				return false ;
			}
		}
		else {
			if(fabs(m_Dw[i] / m_W[i]) > m_Precision) {
				return false ;
			}
		}
	}
	return true;
}


void
	TOptimizer::OutputDebug(double p_Error) const
{
	std::wostringstream stream ;    
	stream << L"\r\nCycle = " << m_CycleNo << L", f(" ;
	for(size_t i = 0; i < m_W.size(); ++i) {
		if(i > 0) {
			stream << L", " ;
		}
		stream << m_W[i] ;
	}
	stream << L") = " << p_Error ;
	OutputDebugString(stream.str().c_str());
}


bool
	TOptimizer::Minimize()
{
	if(m_NumParam == 0) {	//	no coefficient to fit
		return false ;
	}

	if(m_NumParam == 1) {		//	only one parameter
		m_Chi[m_OneParam] = TheModel(m_W) ;
		if(isnan(m_Chi[m_OneParam])) {
			m_Chi[m_OneParam] = HUGE_VAL;
		}

		//double x = m_W[m_OneParam], dx = m_Dw[m_OneParam];	//	very useful for debugging

		double MULT = 1.5, DIV = 2 ;		//	30 cycles, the best

		for(m_CycleNo = 0; m_CycleNo  < 1000 ; ++m_CycleNo) {
			/*x =*/ (m_W[m_OneParam] += m_Dw[m_OneParam]) ;

			double chi = TheModel(m_W) ;
			if(isnan(chi)) {
				chi = HUGE_VAL;
			}
			if(chi < m_Chi[m_OneParam]) {	//	result is better : continue in that direction
				if (m_Improving[m_OneParam]) { //	improving result for the second time : accelarate
					/*dx =*/ m_Dw[m_OneParam] *= MULT ;
				}
				m_Improving[m_OneParam] = true;
			}
			else if(m_CycleNo == 0) {	//	during the first cycle we were going in the bad direction, just search the other way
				/*dx =*/ m_Dw[m_OneParam] = - m_Dw[m_OneParam] ;
				/*x =*/ (m_W[m_OneParam] += m_Dw[m_OneParam]) ;	//	return to the previous better parameter
				m_Improving[m_OneParam] = false;
			}
			else {
				/*dx =*/ m_Dw[m_OneParam] = - m_Dw[m_OneParam] / DIV ;	//	we have passed the minimum, go back with a smaller increment
				if(CheckParamPrecision()) {
					break ;
				}
				m_Improving[m_OneParam] = false;
			}
			m_Chi[m_OneParam] = chi;
		} //	next cycle

		return false ;
	}	//	end of single parameter

	//	1.3	2.0	30 cycles
	//	1.5	2.0	38
	//	1.3	1.8	35
	//	1.2	2.2	33
	//	1.3	2.1	32
	double MULT = 1.3, DIV = 2.0 ;

	for(m_Failures = 0, m_CycleNo = 0; (m_CycleNo < 1000) && (m_Failures < 11) && (m_NumParam != 0) ; ++m_CycleNo) {

		std::wostringstream stream ;    
		stream << L"*****************************\r\n";
		OutputDebugString(stream.str().c_str());

		m_FirstChi = TheModel(m_W) ;
		if(isnan(m_FirstChi)) {
			m_FirstChi = HUGE_VAL;
		}
		//double x = m_W[0], y = m_W[1], z = m_FirstChi;

		if(m_FirstChi > m_BestChi) {		//	changing all parameters gives a worst result : roll back to the best already computed parameters
			m_W = m_BestW ;
			m_FirstChi = m_BestChi ;
		}

		//	change parameters one by one
		m_Besti = -1;
		for(m_i = 0; m_i < m_NumPnts; ++m_i) {
			if(m_Hold[m_i]) {
				continue ;
			}
			m_W[m_i] += m_Dw[m_i] ;
			m_Chi[m_i] = TheModel(m_W) ;

			if(isnan(m_Chi[m_i])) {
				m_Chi[m_i] = HUGE_VAL;
			}

			if(m_Chi[m_i] < m_BestChi) {	//	il y a un nouveau minima
				m_Failures = 0 ;
				m_BestChi = m_Chi[m_i] ;
				m_BestW = m_W ;
				m_Besti = m_i ;
			}

			m_W[m_i] -= m_Dw[m_i] ;
		}

		if(m_Besti == -1)	 {		//	no new minima
			++m_Failures ;
		}

		//double dx = m_Dw[0], dy = m_Dw[1];

		//	compute the new coordinates
		for(int i = 0; i < m_NumPnts; ++i) {
			if(m_Hold[i]) {
				continue;
			}
			if(m_Chi[i] > m_FirstChi) {		//	this parameter change is giving bad results, go in the opposite direction
				m_Dw[i] = -m_Dw[i] ;
				m_W[i] += m_Dw[i] ;
				m_Improving[i] = false ;
			}
			else {									//	this parameter improves the result, or keep it unchanged
				m_W[i] += m_Dw[i] ;
				if(m_Improving[i]) {				//	improving result for the second time : accelarate
					m_Dw[i] *= MULT ;
				}
				m_Improving[i] = true;
			}
			if(m_Failures > 1) {					//	no improvement during two successive cycles
				m_Dw[i] /= DIV ;
			}
		}
		if(CheckParamPrecision()) {
			break ;
		}
	}	//	next cycle

	m_W = m_BestW ;

	return false ;	//	stop
}	/*	Minimize()	*/


bool
TOptimizer::Simplex()
{
	//	S[0]					m_W[0] + m_Dw[0]	m_W[1]				m_W[2]				...
	//	S[1]					m_W[0]				m_W[1] + m_Dw[1]	m_W[2]				...
	//	S[2]					m_W[0]				m_W[1]				m_W[2] + m_Dw[2]	...
	//	...					...					...					...					...
	//	S[m_NumPnts-1]		m_W[0]				m_W[1]				m_W[2]				m_W[m_NumPnts-1] + m_Dw[m_NumPnts-1]
	//	S[m_NumPnts]		m_W[0]				m_W[1]				m_W[2]				m_W[m_NumPnts-1]									<= the original vector at startup

	if (m_NumParam == 0) { //	no coefficient to fit
		return false ;
	}
	m_Hold.resize(m_NumPnts, false);
	m_Hold.push_back(false);	//	take S[m_NumPnts] into account, as it is the original vector

	m_Chi.resize(m_NumPnts, 0.0);
	m_Chi.push_back(0.0);		//	store the result of the original model

	/******	calcul du simplex initial	******/

	vector<vector<double> > S;	//	[MAX_PARAM_NUMBER + 1][MAX_PARAM_NUMBER];

	for (int r = 0; r < m_NumPnts; ++r) {
		if (m_Hold[r]) {
			continue;
		}
		S.push_back(m_W);
		S[r][r] += m_Dw[r] ;
	}

	//	add the original vector
	S.push_back(m_W);

	/******	calcul de l'erreur de chaque vecteur	******/

	for (int r = 0; r <= m_NumPnts; ++r) {
		if (m_Hold[r]) {
			continue ;
		}
		m_W = S[r];
		double err = TheModel(m_W) ;
		OutputDebug(err);
		if (isnan(err)) {
			err = HUGE_VAL;
		}
		m_Chi[r] = err ;
	}

	for (m_Failures = 0, m_CycleNo = 0; (m_CycleNo < 150) && (m_Failures < 11); ++m_CycleNo) {

		//	*****	search for the bext row, the worst row, the minimal and maximal coordinates	*****

		int minLoc = -1;				//	the location of the best result
		int maxLoc = -1;				//	the location of the worst result
		double minChi = HUGE_VAL;		//	the best result
		double maxChi = -HUGE_VAL;		//	the worst result

		vector<double> minima(m_NumPnts, std::numeric_limits<double>::max());
		vector<double> maxima(m_NumPnts, -std::numeric_limits<double>::max());

		for (int r = 0; r <= m_NumPnts; ++r) {
			if (m_Hold[r]) {
				continue ;
			}
			if (m_Chi[r] < minChi) {
				minChi = m_Chi[r];
				minLoc = r ;
			}
			if (m_Chi[r] > maxChi) {
				maxChi = m_Chi[r] ;
				maxLoc = r ;
			}

			for (int c = 0; c < m_NumPnts; ++c) {
				if (S[r][c] < minima[c]) {
					minima[c] = S[r][c] ; //	minima
				}
				if (S[r][c] > maxima[c]) {
					maxima[c] = S[r][c]; //	maxima
				}
			}
		}
		if (minLoc == -1 || maxLoc == -1) {	//	the error function ic completely flat, or returns NaN
			return false ;
		}

		//	keep in mind the range of each parameters, to stop when the precision is reached
		for (int c = 0; c < m_NumPnts; ++c) {
			m_Dw[c] = fabs(maxima[c] - minima[c]);
		}
		if(CheckParamPrecision()) {
			m_W = S[minLoc] ;
			m_BestChi = m_Chi[minLoc];
			break ;
		}

		/******	mise à zéro et calcul des centres de gravité, Sgr	******/

		vector<double> gravityCenter;			//	gravity center without taking into account the point giving the worst result (max)
		gravityCenter.resize(m_NumPnts, 0.0);		//	filled with 0.0

		for (int r = 0; r <= m_NumPnts; ++r) {
			if (m_Hold[r]) {
				continue;
			}
			if (r != maxLoc) { // 	maxima exclu mais avec le vecteur original
				for (int c = 0; c < m_NumPnts; ++c) {
					gravityCenter[c] += S[r][c] / m_NumPnts ;
				}
			}
		}

		/******	calcul du vecteur S* et de son erreur	******/

		OutputDebugString(L"***** Computing new point *****\r\n") ;

		double Alfa = 1, Beta = 0.5, Gamma = 2;

		for (int c = 0; c < m_NumPnts; ++c) {
			m_W[c] = (1 + Alfa) * gravityCenter[c] - Alfa * S[maxLoc][c] ;
		}
		double err = TheModel(m_W);
		OutputDebug(err);
		if (isnan(err)) {
			err = HUGE_VAL;
		}

		/******	Recherche du nouveau vecteur	******/

		if (err < m_Chi[minLoc]) {					//	*****	Expansion	*****
			OutputDebugString(L"Expansion\r\n") ;

			for (int c = 0; c < m_NumPnts; ++c) {
				m_W[c] = (1 + Gamma) * m_W[c] - Gamma * gravityCenter[c] ;
			}
			err = TheModel(m_W);
			OutputDebug(err);
			if (isnan(err)) {
				err = HUGE_VAL;
			}
			S[maxLoc] = m_W ;	//	replace the worst point
			m_Chi[maxLoc] = err ;
		}
		else if (err > m_Chi[maxLoc]) {			//	*****	Contraction	*****
			OutputDebugString(L"Contraction\r\n") ;

			for (int c = 0; c < m_NumPnts; c++) {
				m_W[c] = Beta * S[maxLoc][c] + (1 - Beta) * gravityCenter[c] ;
			}
			err = TheModel(m_W);
			OutputDebug(err);
			if (isnan(err)) {
				err = HUGE_VAL;
			}
			if (err < m_Chi[maxLoc]) {	//	the new point is better than the previous worst point, replace it
				OutputDebugString(L"Contraction is better\r\n") ;
				S[maxLoc] = m_W ;	//	replace the worst point
				m_Chi[maxLoc] = err ;
			}
			else {								//	the new point is worse than the previous worse point (Rapprochement), go closer to the best point
				OutputDebugString(L"Contraction is worse\r\n") ;
				for (int r = 0; r <= m_NumPnts; r++) {
					if (m_Hold[r]) {
						continue;
					}
					for (int c = 0; c < m_NumPnts; c++) {
						S[r][c] = (S[r][c] + S[minLoc][c]) / 2 ;
					}
					m_W = S[r] ;
					err = TheModel(m_W) ;
					OutputDebug(err);
					m_Chi[r] = err;
				}
			}
		}
		else {												//	*****	ni expansion, ni contraction	*****
			OutputDebugString(L"Neither contraction nor expansion\r\n") ;
			if (err < m_Chi[maxLoc]) {
				S[maxLoc] = m_W ;	//	replace the worst point
				m_Chi[maxLoc] = err ;
			}
			else {
				++m_Failures;
			}
		}
	} //	next cycle

	return false ;
} //	simplex


bool
TOptimizer::Next(double p_Error)
{
	OutputDebug(p_Error);

	if (m_NumParam == 0) {
		return false ; //	no coefficient to fit
	}

	switch (m_Algo) {
		case Optimizer::AlgorithmEnum::Simplex: {
			return NextSimplex(p_Error);
		}

		default: {
			if (m_NumParam > 1) {
				return Next2(p_Error);
			}
			return Next1(p_Error);	// m_NumParam == 1
		}
	}
}


bool
TOptimizer::Next1(double p_Error)
{
	if (isnan(p_Error)) {
		p_Error = HUGE_VAL;
	}

	if(m_Step == 0) {
		m_BestChi = p_Error;
		m_BestW = m_W;

		m_Chi[m_OneParam] = p_Error;
		m_W[m_OneParam] += m_Dw[m_OneParam] ;

		m_CycleNo = 0;
		m_Step = 1;
		m_Results.clear();
		//	keep in mind this result
		m_Results[m_W] = p_Error;
	}
	else {
		//	keep in mind this result
		m_Results[m_W] = p_Error;

		if (p_Error < m_BestChi) {
			m_BestChi = p_Error;
			m_BestW = m_W;
		}
		if (p_Error < m_Chi[m_OneParam]) {	//	result is better : continue in that direction
			if (m_Improving[m_OneParam]) { //	improving result for the second time : accelarate
				m_Dw[m_OneParam] *= m_Mult;
			}
			m_Improving[m_OneParam] = true;
		}
		else if (m_CycleNo == 0) {	//	during the first cycle we were going in the bad direction, just search the other way
			m_Dw[m_OneParam] = -m_Dw[m_OneParam];
			m_W[m_OneParam] += m_Dw[m_OneParam];	//	return to the previous better parameter
			m_Improving[m_OneParam] = false;
		}
		else {
			m_Dw[m_OneParam] = -m_Dw[m_OneParam] / m_Div;	//	we have passed the minimum, go back with a smaller increment

			if (CheckParamPrecision()) {
				m_W = m_BestW;
				std::wostringstream stream;
				stream << L"\r\n***** Final result *****\r\n";
				OutputDebugString(stream.str().c_str());
				OutputDebug(m_BestChi);
				return false;	//	stop
			}
			m_Improving[m_OneParam] = false;
		}
		m_Chi[m_OneParam] = p_Error;
		m_W[m_OneParam] += m_Dw[m_OneParam];

		if (++m_CycleNo >= 1000) {
			m_W = m_BestW;
			std::wostringstream stream;
			stream << L"\r\n***** No convergence *****\r\n";
			OutputDebugString(stream.str().c_str());
			OutputDebug(m_BestChi);
			return false;	//	stop
		}
		//	check if the current parameter set is not already computed
		auto it = m_Results.find(m_W);
		if(it != m_Results.end()) {	//	re-use this value
			Next(it->second);
		}
	}
	return true;	//	continue to the next cycle
}	//	Next1()


bool
	TOptimizer::Next2(double p_Error)
{
	if (isnan(p_Error)) {
		p_Error = HUGE_VAL;
	}

	if(m_Step == 0) {
		m_FirstChi = p_Error; 
		if(isnan(m_FirstChi)) {
			m_FirstChi = HUGE_VAL;
		}

		if(m_FirstChi > m_BestChi) {		//	changing all parameters gives a worst result : roll back to the best already computed parameters
			m_W = m_BestW ;
			m_FirstChi = m_BestChi ;
		}
		//	prepare the first parameter
		m_Besti = -1;

		//	search for the first parameter index we have to optimize
		for(m_i = 0; m_i < m_NumPnts; ++m_i) {
			if(! m_Hold[m_i]) {
				break;
			}
		}
		m_W[m_i] += m_Dw[m_i] ;
		m_Besti = -1;
		m_Step = 1;
		m_Results.clear();
		//	keep in mind this result
		m_Results[m_W] = p_Error;
	} //	end Step 0
	else {

		//	keep in mind this result
		m_Results[m_W] = p_Error;

		m_Chi[m_i] = p_Error;
		m_W[m_i] -= m_Dw[m_i];

		if (m_Chi[m_i] < m_BestChi) {	//	il y a un nouveau minima
			m_Failures = 0;
			m_BestChi = m_Chi[m_i];
			m_BestW = m_W;
			m_Besti = m_i;
		}


		//	look for the next parameter to optimize
		for (++m_i; m_i < m_NumPnts; ++m_i) {
			if (!m_Hold[m_i]) {
				//	prepare the next computation
				m_W[m_i] += m_Dw[m_i];

				//	check if the current parameter set is not already computed
				auto it = m_Results.find(m_W);
				if (it != m_Results.end()) {	//	re-use this value
					Next(it->second);
				}
				else {
					return true; //	continue
				}
			}
		}

		//	 analyse the results computed for each parameter change

		if (m_Besti == -1) {		//	no new minima
			++m_Failures;
		}

		//	compute the new coordinates
		for (int i = 0; i < m_NumPnts; ++i) {
			if (m_Hold[i]) {
				continue;
			}
			if (m_Chi[i] > m_FirstChi) {		//	this parameter change is giving bad results, go in the opposite direction
				m_Dw[i] = -m_Dw[i];
				m_W[i] += m_Dw[i];
				m_Improving[i] = false;
			}
			else {									//	this parameter improves the result, or keep it unchanged
				m_W[i] += m_Dw[i];
				if (m_Improving[i]) {				//	improving result for the second time : accelarate
					m_Dw[i] *= m_Mult;
				}
				m_Improving[i] = true;
			}
			if (m_Failures > 1) {					//	no improvement during two successive cycles
				m_Dw[i] /= m_Div;
			}
		}
		if (CheckParamPrecision()) {
			m_W = m_BestW;
			std::wostringstream stream;
			stream << L"\r\n***** Final result *****\r\n";
			OutputDebugString(stream.str().c_str());
			OutputDebug(m_BestChi);
			return false;	//	stop
		}
		if (++m_CycleNo >= 1000 || m_Failures >= 11) {
			m_W = m_BestW;
			std::wostringstream stream;
			stream << L"\r\n***** No convergence *****\r\n";
			OutputDebugString(stream.str().c_str());
			OutputDebug(m_BestChi);
			return false;	//	stop
		}

		//	go back to Step 0
		std::wostringstream stream;
		stream << L"*****************************\r\n";
		OutputDebugString(stream.str().c_str());

		m_Step = 0;
	}
	return true ;	//	continue to the next cycle
}	//	Next2()


bool	
	TOptimizer::NextSimplex(double p_Error)
{
	if (m_NumParam == 0) { //	no coefficient to fit
		return false ;
	}
	if (isnan(p_Error)) {
		p_Error = HUGE_VAL;
	}
	if(m_Step == 0) {
		m_Hold.resize(m_NumPnts, false);
		m_Hold.push_back(false);	//	take S[m_NumPnts] into account, as it is the original vector

		m_Chi.resize(m_NumPnts, 0.0);
		m_Chi.push_back(0.0);		//	store the result of the original model

		/******	calcul du simplex initial	******/

		m_S.clear();	//	[MAX_PARAM_NUMBER + 1][MAX_PARAM_NUMBER];

		for (int r = 0; r < m_NumPnts; ++r) {
			if (m_Hold[r]) {
				continue;
			}
			m_S.push_back(m_W);
			m_S[r][r] += m_Dw[r] ;
		}

		//	add the original vector
		m_S.push_back(m_W);

		/******	calcul de l'erreur de chaque vecteur	******/
		for(m_R = 0; m_R <= m_NumPnts; ++m_R) {
			if (! m_Hold[m_R]) {
				m_W = m_S[m_R];
				m_Step = 1 ;
				return true;	//	run the model
			}
		}
		return false;	//	no parameter to optimize
	}

	if(m_Step == 1) {
		m_Chi[m_R] = p_Error ;

		for(++m_R; m_R <= m_NumPnts; ++m_R) {
			if (! m_Hold[m_R]) {
				m_W = m_S[m_R];
				return true;	//	run the model
			}
		}
		//	all parameters have been processed
		m_Failures = 0;
		m_CycleNo = 0;

		m_Step = 2;
	}

	if(m_Step == 3) {		/******	Recherche du nouveau vecteur	******/
		double Beta = 0.5, Gamma = 2;

		if (p_Error < m_Chi[m_MinLoc]) {					//	*****	Expansion	*****
			OutputDebugString(L"Expansion\r\n") ;

			for (int c = 0; c < m_NumPnts; ++c) {
				m_W[c] = (1 + Gamma) * m_W[c] - Gamma * m_GravityCenter[c] ;
			}
			m_Step = 4 ;
			return true;	//	run the model
		}
		
		if (p_Error > m_Chi[m_MaxLoc]) {			//	*****	Contraction	*****
			OutputDebugString(L"Contraction\r\n") ;

			for (int c = 0; c < m_NumPnts; c++) {
				m_W[c] = Beta * m_S[m_MaxLoc][c] + (1 - Beta) * m_GravityCenter[c] ;
			}
			m_Step = 5;
			return true;	//	run the model
		}
		//	*****	ni expansion, ni contraction	*****
		OutputDebugString(L"Neither contraction nor expansion\r\n") ;
		if (p_Error < m_Chi[m_MaxLoc]) {
			m_S[m_MaxLoc] = m_W ; //	replace the worst point
			m_Chi[m_MaxLoc] = p_Error ;
		}
		else {
			++m_Failures;
		}
		m_Step = 2;
	}

	if(m_Step == 4) {
		m_S[m_MaxLoc] = m_W ;	//	replace the worst point
		m_Chi[m_MaxLoc] = p_Error ;
		m_Step = 2;
	}

	if(m_Step == 5) {
		if (p_Error < m_Chi[m_MaxLoc]) {	//	the new point is better than the previous worst point, replace it
			OutputDebugString(L"Contraction is better\r\n") ;
			m_S[m_MaxLoc] = m_W ;	//	replace the worst point
			m_Chi[m_MaxLoc] = p_Error ;
			m_Step = 2 ;
		}
		else {								//	the new point is worse than the previous worse point (Rapprochement), go closer to the best point
			OutputDebugString(L"Contraction is worse\r\n") ;
			for (m_R = 0; m_R <= m_NumPnts; ++m_R) {
				if (! m_Hold[m_R]) {
					for (int c = 0; c < m_NumPnts; c++) {
						m_S[m_R][c] = (m_S[m_R][c] + m_S[m_MinLoc][c]) / 2 ;
					}
					m_W = m_S[m_R] ;
					m_Step = 6;
					return true ;	//	recompute
				}
			}
		}
	}
	
	if(m_Step == 6) {
		m_Chi[m_R] = p_Error;
		for (++m_R ; m_R <= m_NumPnts; ++m_R) {
			if (! m_Hold[m_R]) {
				for (int c = 0; c < m_NumPnts; c++) {
					m_S[m_R][c] = (m_S[m_R][c] + m_S[m_MinLoc][c]) / 2;
				}
				m_W = m_S[m_R];
				return true;	//	recompute
			}
		}
		//	next cycle
		m_Step = 2;
	}

	//	if(m_Step == 2), this is the main loop beginning:	for (m_Failures = 0, m_CycleNo = 0; (m_CycleNo < 150) && (m_Failures < 11); ++m_CycleNo)

	if (++m_CycleNo >= 150 || m_Failures >= 11) { //	no convergence
		m_ErrorCode = 1;
		m_ErrorName = L"No convergence";
		std::wostringstream stream ;    
		stream << L"\r\n***** No convergence *****\r\n";
		OutputDebugString(stream.str().c_str());
		OutputDebug(m_BestChi);
		return false;	//	stop
	}

	//	*****	search for the bext row, the worst row, the minimal and maximal coordinates	*****

	m_MinLoc = -1;				//	the location of the best result
	m_MaxLoc = -1;				//	the location of the worst result
	m_Min = HUGE_VAL;		//	the best result
	m_Max = -HUGE_VAL;		//	the worst result

	vector<double> minima(m_NumPnts, std::numeric_limits<double>::max());
	vector<double> maxima(m_NumPnts, -std::numeric_limits<double>::max());

	for (int r = 0; r <= m_NumPnts; ++r) {
		if (m_Hold[r]) {
			continue ;
		}
		if (m_Chi[r] < m_Min) {
			m_Min = m_Chi[r];
			m_MinLoc = r ;
		}
		if (m_Chi[r] > m_Max) {
			m_Max = m_Chi[r] ;
			m_MaxLoc = r ;
		}

		for (int c = 0; c < m_NumPnts; ++c) {
			if (m_S[r][c] < minima[c]) {
				minima[c] = m_S[r][c] ; //	minima
			}
			if (m_S[r][c] > maxima[c]) {
				maxima[c] = m_S[r][c]; //	maxima
			}
		}
	}
	if (m_MinLoc == -1 || m_MaxLoc == -1) {	//	the error function ic completely flat, or returns NaN
		return false ;
	}

	//	keep in mind the range of each parameters, to stop when the precision is reached
	for (int c = 0; c < m_NumPnts; ++c) {
		m_Dw[c] = fabs(maxima[c] - minima[c]);
	}
	if(CheckParamPrecision()) 
	
	
	{
		m_W = m_S[m_MinLoc] ;
		m_BestChi = m_Chi[m_MinLoc];
		return false ;	//	stop
	}

	/******	mise à zéro et calcul des centres de gravité, Sgr	******/

	m_GravityCenter.clear();			//	gravity center without taking into account the point giving the worst result (max)
	m_GravityCenter.resize(m_NumPnts, 0.0);		//	filled with 0.0

	for (int r = 0; r <= m_NumPnts; ++r) {
		if (m_Hold[r]) {
			continue;
		}
		if (r != m_MaxLoc) { // 	maxima exclu mais avec le vecteur original
			for (int c = 0; c < m_NumPnts; ++c) {
				m_GravityCenter[c] += m_S[r][c] / m_NumPnts ;
			}
		}
	}

	/******	calcul du vecteur S* et de son erreur	******/

	OutputDebugString(L"***** Computing new point *****\r\n") ;

	double Alfa = 1;

	for (int c = 0; c < m_NumPnts; ++c) {
		m_W[c] = (1 + Alfa) * m_GravityCenter[c] - Alfa * m_S[m_MaxLoc][c] ;
	}
	m_Step = 3;
	return true;	//	run the model
}