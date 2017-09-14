
#pragma once

#include <vector>
#include <map>
#include <limits>

#define HUGE_VAL std::numeric_limits<double>::max()

namespace Optimizer {
	enum AlgorithmEnum {
		Unknown = 0,
		StepByStep = 1,
		Simplex = 2
	};
}

//	a cluster is made of one reference sequence and a list of sequence IDs
class TOptimizer
{
	public:
		explicit TOptimizer(vector<double> & p_StartParameters, double p_MaxError);
		TOptimizer(vector<double> & p_StartParameters, const vector<double> & p_Steps, double p_MaxError);
		virtual ~TOptimizer(void);

		void	SetAlgorithm(Optimizer::AlgorithmEnum	p_Algo);

		bool	Next(double p_Error) ;

		bool	Minimize();	//	the original functions, calling a model directly
		bool	Simplex();

		//	testing functions
		static void		Test();
		static int32_t		OptimizeMultAndDiv(vector<double> & p_X);
		static double	TheModel(const vector<double> & p_Params);

	private :
		//	data returned by the model and set by Next()
		double						m_ErrorValue;			//	the error value. m_ErrorValue² will be minimized

		//	data given to ctor or by properties before running the optimization
		Optimizer::AlgorithmEnum	m_Algo ;					//	the algorithm to use
		double						m_Mult ;
		double						m_Div ;

		vector<bool>				m_Hold;					//	if true, the corresponding parameter is not modified during optimization
	 	double 						m_Precision;			//	the precision for all parameters. If all parameters change by less than m_Precision between two cycles, we stop searching? Typically 0.01 %, so give 0.0001
		
		vector<double>				& m_W;					//	the parameters
		vector<double>				m_Dw;						//	the parameter steps
		vector<double>				m_Chi;					//	the error results for each of the parameter changes, so m_Chi[1] = error for Run(w[0], w[1]+dw[1], w[2], ...)
		vector<bool>				m_Improving;			//	true if the corresponding parameter change improves the result

		//	data used to keep in mind what we are doing between each call to Next()
		int32_t						m_Step ;											//	the algorithm step

		int32_t						m_Besti;					//	the index of the modified parameter giving the best result	
		double						m_BestChi ;				//	the best chi² so far
		vector<double>				m_BestW;					//	the best parameters

		//	computed only during step 0
		int32_t						m_NumPnts ;				//	= static_cast<int32_t>(m_W.size()); the number of parameters, including those we don't optimize
		int32_t						m_NumParam ;			//	the number of real parameters to fit (less than NumPoints if some parameters are hold)
		int32_t						m_OneParam ;			//	the index of the unique parameter if there is only one parameter to fit
		int32_t						m_CycleNo ;				//	the number of trial, index in the cycle loop
		int32_t						m_Failures;				//	count the number of cycles during which no improvement could be made. Used to stop if we cannot imrpove the result for about 11 trials
		int32_t						m_i;						//	parameter index in the central loop
		double						m_FirstChi;				//	the error of the original parameters
		std::map<vector<double>, double>	m_Results;		//	all previous results, to avoid computing twice the same parameters. The key is given by function KeyParam()

		//	simplex variables
		int32_t						m_R;			
		int32_t						m_MinLoc;				//	the location of the best result
		int32_t						m_MaxLoc;				//	the location of the worst result
		double						m_Min;					//	the best result
		double						m_Max;					//	the worst result
		vector<double>				m_GravityCenter;		//	gravity center without taking into account the point giving the worst result
		vector<vector<double> > m_S;			//	[MAX_PARAM_NUMBER + 1][MAX_PARAM_NUMBER];

		//	others
		int32_t							m_ErrorCode ;
		std::wstring					m_ErrorName;		//	exlanation about any failure in the process

		bool	Next1(double p_Error) ;
		bool	Next2(double p_Error) ;
		bool	NextSimplex(double p_Error) ;

		bool	IsValidParameter() const ;					//	return true if at least one parameter can be optimized
		void	Init();
		bool	CheckParamPrecision() const ;				//	return true if all parameters have a precision good enough
		void OutputDebug(double p_Error) const;

		//const std::string  KeyParam() const;			//	return the current key used to retrieve any previous result from the parameter list. The key is the parameter list : "15.2 * 8.9 * 250"

};


