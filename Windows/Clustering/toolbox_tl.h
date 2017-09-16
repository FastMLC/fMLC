

#pragma once

//	WARNING: keep this file pure standard C++, without any Windows-only code

#include <math.h>
#include <vector>
#include <stdint.h>


//	***************************	Unicode	***************************************

const std::string cUtf8Header = "\xEF\xBB\xBF";
const wchar_t cUtf16Header = 0xFEFF; //	 65279 ;	//	the utf-16 standard Byte Order Mark, see http://en.wikipedia.org/wiki/Byte_order_mark


//	***************************	#define, def, undef, ifdef et al.	*************************

//	Before others : kill macro from Windef.h, which cause unbelievabale side effects with functions using
//	static allocation. Example max_(0, min(256, strcspn(String1, String2)))	=>	strcspn() is executed
//	twice	=>	Trap !!


#undef RGB	//	use Rgb()

template <class T>	//	T can be (unsigned) char, short, int, int32_t
inline uint32_t	Rgb(T r, T g, T b)	{	return((r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16)) ;	}

template <class T>	//	T can be (unsigned) char, short, int, int32_t
inline uint32_t	Bgr(T b, T g, T r)	{	return((r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16)) ;	}

//	convert from Microsoft (0x00BBGGRR) <=> the standard format (0x00RRGGBB) ;
inline uint32_t Rgb(int bgr)			{	return(((bgr & 0xFF) << 16) | (bgr & 0xFF00) | ((bgr & 0xFF0000) >> 16)) ;	}
inline uint32_t Bgr(int rgb)			{	return(Rgb(rgb)) ;	}

template <class T> inline T round(T a)							{	return static_cast<T>(floor(a + 0.5)) ;							}
template <class T> inline void Swap(T &x, T &y)				{	T z = x ; x = y ; y = z ;								}

#ifdef _WINDOWS
	const char gNewLine[] = "\r\n" ;
	const wchar_t gNewLineW[] = L"\r\n" ;
#else
	const char gNewLine[] = "\n" ;
	const wchar_t gNewLineW[] = L"\n" ;
#endif



//	*********************	min & max	*****************************

// min and max implementation without pipeline break ( no if-else-endif ) => faster for processing streaming data


#undef min_
#undef max_

//	awful macros defined in <Windef.h> : the most unsafe and the slowest solution
//
//#define min_(a,b)            (((a) < (b)) ? (a) : (b))
//#define max_(a,b)            (((a) > (b)) ? (a) : (b))
//

template <class T>
inline const T & min_(const T & p_a, const T & p_b)
{
	return( (p_a < p_b) ? p_a : p_b) ;
}

template <class T>
inline const T & max_(const T & p_a, const T & p_b)
{
	return( (p_a > p_b) ? p_a : p_b) ;
}

template <class T>
inline T & min_(T & p_a, T & p_b)
{
	return( (p_a < p_b) ? p_a : p_b) ;
}

template <class T>
inline T & max_(T & p_a, T & p_b)
{
	return( (p_a > p_b) ? p_a : p_b) ;
}


inline int min_(int p_a, int p_b)
{
	int mask = -(p_a < p_b) ;
	return (p_a & mask) | (p_b & ~mask) ;
}


inline int max_(int p_a, int p_b)
{
	int mask = -(p_a > p_b) ;
	return (p_a & mask) | (p_b & ~mask) ;
}


inline uint32_t min_(uint32_t p_a, uint32_t p_b)
{
	int mask = -(p_a < p_b) ;
	return (p_a & mask) | (p_b & ~mask) ;
}


inline uint32_t max_(uint32_t p_a, uint32_t p_b)
{
	int mask = -(p_a > p_b) ;
	return (p_a & mask) | (p_b & ~mask) ;
}


inline int64_t min_(int64_t & p_a, int64_t & p_b)
{
	int64_t mask = -(p_a < p_b) ;
	return (p_a & mask) | (p_b & ~mask) ;
}


inline int64_t max_(int64_t & p_a, int64_t & p_b)
{
	int64_t mask = -(p_a > p_b) ;
	return (p_a & mask) | (p_b & ~mask) ;
}


inline uint64_t min_(uint64_t & p_a, uint64_t & p_b)
{
	int64_t mask = -(p_a < p_b) ;
	return (p_a & mask) | (p_b & ~mask) ;
}


inline uint64_t max_(uint64_t & p_a, uint64_t & p_b)
{
	int64_t mask = -(p_a > p_b) ;
	return (p_a & mask) | (p_b & ~mask) ;
}


inline double min_(double & p_a, double & p_b) {
	union u_t {
		double d64 ;
		int64_t u64 ;
	};
	u_t a, b ;
	a.d64 = p_a ;
	b.d64 = p_b ;

	int64_t mask = -(a.d64 < b.d64) ;	//	0 or -1, 0x0000000000000000 or 0xFFFFFFFFFFFFFFFF
	a.u64 &= mask ;				//	0 or unchanged if smaller than b
	b.u64 &= ~mask ;				//	0 or unchanged if smaller than a
	a.u64 |= b.u64 ;				//	a or b
	return a.d64 ;
}

inline double max_(double & p_a, double & p_b) {
	union u_t {
		double d64 ;
		int64_t u64 ;
	};
	u_t a, b ;
	a.d64 = p_a ;
	b.d64 = p_b ;

	int64_t mask = -(a.d64 > b.d64);
	a.u64 &= mask ;
	b.u64 &= ~mask ;
	a.u64 |= b.u64 ;
	return a.d64 ;
}


template <typename T>
inline const T * min_(const T * p_a, const T * p_b)
{
	union u_t {
		const T * pt ;	//	32 bit for x86, 64 bit for x64
#if defined(_WIN64)
		int64_t i ;
#else
		int i ;
#endif
	} ;
	u_t a, b, sign ;
	a.pt = p_a ;
	b.pt = p_b ;

	sign.i = -(a.pt < b.pt);
	a.i &= sign.i;
	b.i &= ~sign.i;
	a.i |= b.i;
	return a.pt;
}


template <typename T>
inline const T * max_(const T * p_a, const T * p_b)
{
	union u_t {
		const T * pt ;	//	32 bit for x86, 64 bit for x64
#if defined(_WIN64)
		int64_t i ;
#else
		int i ;
#endif
	};
	u_t a, b, sign ;
	a.pt = p_a ;
	b.pt = p_b ;

	sign.i = -(a.pt > b.pt);
	a.i &= sign.i;
	b.i &= ~sign.i;
	a.i |= b.i;
	return a.pt;
}


inline float min_(const float &p_a, const float &p_b)
{
	union u_t {
		float d32;
		int	u32;
	};
	u_t a, b, sign;
	a.d32 = p_a;
	b.d32 = p_b;

	sign.u32 = -(a.d32 < b.d32);
	a.u32 &= sign.u32;
	b.u32 &= ~sign.u32;
	a.u32 |= b.u32;
	return a.d32;
}


inline float max_(const float &p_a, const float &p_b)
{
	union u_t {
		float d32;
		int	u32;
	};
	u_t a, b, sign;
	a.d32 = p_a;
	b.d32 = p_b;

	sign.u32 = -(a.d32 > b.d32);
	a.u32 &= sign.u32;
	b.u32 &= ~sign.u32;
	a.u32 |= b.u32;
	return a.d32;
}


////	NaN is 0x7FFxxxxxxxxxxxxx where x is not zero
////	-NaN is 0xFFFxxxxxxxxxxxxx where x is not zero
//inline bool IsNaN(double x) {	//	little endian version, very fast on 32 bit machines : 8 ticks only
//	return(( *(reinterpret_cast<uint32_t *>(&x) + 1) & 0x7FF00000) == 0x7FF00000) ;
//}


inline double NaN() //	+NaN // std::numeric_limits<double>::quiet_NaN()
{
	//return std::numeric_limits<double>::quiet_NaN();
	union data_t {
		uint64_t u64;
		double dbl;
	};
	data_t data;
	data.u64 = 0x7FF8000000000000;
	return data.dbl;
}

inline double PlusInf() //	+INF
{
	union data_t {
		uint64_t u64;
		double dbl;
	};
	data_t data;
	data.u64 = 0x7FF0000000000000;
	return data.dbl;
}

inline double MinusInf() //	-INF
{
	union data_t {
		uint64_t u64;
		double dbl;
	};
	data_t data;
	data.u64 = 0xFFF0000000000000;
	return data.dbl;
}
/*
// Due to a compiler bug, 
// -- invoking explicitly the default c-tor of
// an C-array type from a template, as 
//			
//
template<class _T1, class _T2> struct pair {
	typedef _T1 first_type;
	typedef _T2 second_type;
	pair() {}
	pair(const _T1& _V1, const _T2& _V2)
		: first(_V1), second(_V2) {}
	template<class U, class V> pair(const pair<U, V> &p)
		: first(p.first), second(p.second) {}
	_T1 first;
	_T2 second;
	};
template<class _T1, class _T2> inline
	bool __cdecl operator==(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_X.first == _Y.first && _X.second == _Y.second); }
template<class _T1, class _T2> inline
	bool __cdecl operator!=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_X == _Y)); }
template<class _T1, class _T2> inline
	bool __cdecl operator<(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_X.first < _Y.first ||
		!(_Y.first < _X.first) && _X.second < _Y.second); }
template<class _T1, class _T2> inline
	bool __cdecl operator>(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_Y < _X); }
template<class _T1, class _T2> inline
	bool __cdecl operator<=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_Y < _X)); }
template<class _T1, class _T2> inline
	bool __cdecl operator>=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_X < _Y)); }
template<class _T1, class _T2> inline
	pair<_T1, _T2> __cdecl make_pair(const _T1& _X, const _T2& _Y)
	{return (pair<_T1, _T2>(_X, _Y)); }

	*/

//	**********************	BufferSize(), FromBuffer(), ToBuffer() templates	*************

namespace Sztd {
	//	How to use ToBuffer() From Buffer() and BufferSize() ?
	//	3 situations :
	//	1. T is a static object (no dynamicaly allocated member)
	//		=>	Call MemmoveBufferSize(), MemmoveToBuffer(), MemmoveFromBuffer()
	//
	//	2. T contains dynamicaly allocated object, but each T has the same size, so static int T::BufferSize() exist
	//		=> call BufferSize(), ToBuffer(), FromBuffer()
	//
	//	3. T contains dynamicaly allocated object and each T object has a different size, so static int T::BufferSize() does not exist
	//		=> call DynamicBufferSize(), ToBuffer(), FromBuffer()

	//	Try this one first, if static int T::BufferSize() does not exist, try the next one
	template<class T>
	int BufferSize(const std::vector<T> & vector)	//	Static Version
	{
		return(sizeof(int) + vector.size() * T::BufferSize()) ;
	}

	//	Second one, used if each T object has a different BufferSize()
	template<class T>
	int DynamicBufferSize(const std::vector<T> & vector)	//	Full Version
	{
		int Size = sizeof(int) ;
		int imax = vector.size() ;
		for(int i = 0; i < imax; ++i) {
			Size += vector[i].BufferSize() ;	//	if T::BufferSize() does not exist, call FastBufferSize() instead
		}
		return(Size) ;
	}


	template<class T>
	int ToBuffer(const std::vector<T> & vector, char * buffer)	//	Full version
	{
		char * Dest = buffer ;
		int VectorSize = vector.size() ;
		*reinterpret_cast<int *>(Dest) = VectorSize ;
		Dest += sizeof(VectorSize) ;

		for(int i = 0; i < VectorSize; ++i) {
			Dest += vector[i].ToBuffer(Dest) ;
		}

		return(Dest - buffer) ;
	}


	template<class T>
	int FromBuffer(std::vector<T> & vector, const char * buffer)	//	Full version
	{
		const char * Srce = buffer ;
		
		int VectorSize = *(int *)Srce ;
		Srce += sizeof(VectorSize) ;

		vector.resize(VectorSize) ;
		for(int i = 0; i < VectorSize; ++i) {
			Srce += vector[i].FromBuffer(Srce) ;
		}
		return(Srce - buffer) ;
	}


	template<class T>
	int MemmoveBufferSize(const std::vector<T> & vector)	//	memmove version
	{
		return(sizeof(int) + vector.size() * sizeof(T)) ;
	}


	template<class T>
	int MemmoveToBuffer(const std::vector<T> & vector, char * buffer)	//	memmove version
	{
		char * Dest = buffer ;
		int VectorSize = vector.size() ;
		*(int *)Dest = VectorSize ;
		Dest += sizeof(VectorSize) ;

		VectorSize *= sizeof(T) ;
		memmove(Dest, vector.MyPtr(), VectorSize) ;
		Dest += VectorSize ;

		return(Dest - buffer) ;
	}


	template<class T>
	int MemmoveFromBuffer(std::vector<T> & vector, const char * buffer)	//	memmove version
	{
		const char * Srce = buffer ;
		
		int VectorSize = *(int *)Srce ;
		vector.resize(VectorSize) ;
		Srce += sizeof(VectorSize) ;

		VectorSize *= sizeof(T) ;
		memmove(vector.MyPtr(), Srce, VectorSize) ;
		Srce += VectorSize ;

		return(Srce - buffer) ;
	}

	//	a pair of objects
	template <class T, class U>
	class TDuo {
		public :
			TDuo(T first = 0, U second = 0)
			: m_A(first), m_B(second)
			{}

			T m_A ;
			U m_B ;
	} ;

	//	a trio of objects
	typedef TDuo<double, double>	TDD ;
	typedef TDuo<double, int>		TDI ;
	typedef TDuo<int, double>		TID ;
	typedef TDuo<int, int>			TII ;


	template <class T, class U, class V>
	class TTrio : public TDuo<T, U> {
		public :
			TTrio(T first = 0, U second = 0, V third = 0)
			: TDuo<T, U> (first, second), m_C(third)
			{}

			V m_C ;
	} ;

	typedef TTrio<double, double, double>	TDDD ;
	typedef TTrio<double, double, int>		TDDI ;
	typedef TTrio<double, int, double>		TDID ;
	typedef TTrio<double, int, int>			TDII ;
	typedef TTrio<int, double, double>		TIDD ;
	typedef TTrio<int, double, int>			TIDI ;
	typedef TTrio<int, int, double>			TIID ;
	typedef TTrio<int, int, int>				TIII ;

	class TDDC : public TDD {
		public :
			TDDC(double x, double y, const wchar_t * s, int stringSize) : TDD(x, y) {
				wcsncpy_s(m_C, 256, s, stringSize) ;
			}
			wchar_t m_C[264] ;
	} ;


	template <class T, class U, class V, class W>
	class TQuatro : public TTrio<T, U, V> {
		public :
			TQuatro(T a, U b, V c, W d)
			: TTrio<T, U, V> (a, b, c), D(d)
			{}

			V m_D ;
	} ;

	typedef TQuatro<double, double, int, double>	TDDID ;

}	//	end of namespace Sztd


//	***********************************	constant	using code above *****************************

const double	cNaN = NaN(); //	+NaN
const double	cPlusInf = PlusInf(); //	+INF
const double	cMinusInf = MinusInf(); //	-INF


template <class T> T round(T x, int decimals)
{
	decimals = max_(0, min_(18, decimals));
	double mult = pow(10, decimals);

	return static_cast<T>(round(x * mult)) / mult;
}
