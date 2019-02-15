/****************************************
 * NxN Matrix function templatization 
 * By Bill Perone (billperone@yahoo.com)
 * Original: 16-09-2002 
 * Revised:  12-12-2003
 *
 * Special Thanks: Pete Isensee, Braeden Shosa
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once

// need swap
#include "utility.h"


#pragma inline_depth( 255 )
#pragma inline_recursion( on )


//! used in creating an identity matrix
template<class Mtx, unsigned N> 
struct IdentityMtx
{
   static inline void eval(Mtx &mtx)
   {  IdentityMtxImpl<Mtx, N, 0, 0, 0>::eval(mtx);  }
};

//! Used in creating an identity matrix 
//! Assigns each element of the matrix
template<class Mtx, unsigned N, unsigned C, unsigned R, unsigned I>
struct IdentityMtxImpl
{
   enum
   {
      NxtI = I + 1,          // Counter
      NxtR = NxtI % N,       // Row (inner loop)
      NxtC = NxtI / N % N    // Column (outer loop)
   };
   static inline void eval( Mtx& mtx )
   {
      mtx[C][R] = ( C == R ) ? 1.0 : 0.0;
      IdentityMtxImpl<Mtx, N, NxtC, NxtR, NxtI>::eval(mtx);
   }
};


//! Used in creating an identity matrix
//! \remarks Makes the template appear like a function
#define IdentityMtxT(MtxType, Mtx, N) IdentityMtx< MtxType, N >::eval(Mtx)




//! used in matrix initialization
//! \remarks N is matrix size
template <class Mtx, unsigned N, class F, int Init> 
struct InitMtx
{
   static inline void eval(Mtx &mtx)
   {  InitMtxImpl<Mtx, N, F, Init, 0, 0, 0>::eval(mtx);   }
};

//! used in matrix initialization
//! \remarks Assigns each element of the matrix
template <class Mtx, unsigned N, class F, int Init, unsigned C, unsigned R, unsigned I>
struct InitMtxImpl
{
   enum
   {
      NxtI = I + 1,          // Counter
      NxtR = NxtI % N,       // Row (inner loop)
      NxtC = NxtI / N % N    // Column (outer loop)
   };
   static inline void eval(Mtx &mtx)
   {
      mtx[C][R] = static_cast<F>(Init);
      InitMtxImpl<Mtx, N, F, Init, NxtC, NxtR, NxtI>::eval(mtx);
   }
};


//! Used in matrix initialization
//! \remarks Makes the template appear like a function
#define InitMtxT(MtxType, Mtx, N, F, Init) InitMtx<MtxType, N, F, Init >::eval(Mtx)



//! Used in transposing a matrix; N is matrix size
template<class Mtx, unsigned N> 
struct TransposeMtx
{
   static inline void eval(Mtx &mtx)
   {  TransposeMtxImpl<Mtx, N, 0, 1, 0>::eval(mtx);  }
};

//! Used in transposing a matrix
template<class Mtx, unsigned N, unsigned C, unsigned R, unsigned I>
struct TransposeMtxImpl
{
   enum
   {
      NxtI = I + 1,
      NxtC = NxtI / N % N,
      NxtR = ( NxtI % N ) + NxtC + 1
   };
   static inline void eval(Mtx &mtx)
   {
      if (R < N) Swap(mtx[C][R], mtx[R][C]);
      TransposeMtxImpl<Mtx, N, NxtC, NxtR, NxtI>::eval(mtx);
   }
};


//! Used in transposing a matrix
//! \remarks Makes the template appear like a function 
#define TransposeMtxT(MtxType, Mtx, N) TransposeMtx<MtxType, N >::eval(Mtx)




//! internal matrix class definition macros
#define MATRIX_COMMON_INTERNAL(dimension)                                   \
	/* the data of the matrix */											\
	union																	\
	{																		\
		struct { vector##dimension##f col[dimension]; };					\
		struct { float     elem[dimension*dimension]; };					\
	};																		\
																			\
	/* trivial ctor */														\
	matrix##dimension () {}													\
	/* C array ctor */														\
	matrix##dimension (bool colmajor, float *entries)						\
	{																		\
		memcpy((float *)&col, entries, dimension*dimension*sizeof(float));	\
		if (!colmajor) transpose();											\
	}																		\
	/* Array indexing */													\
	vector##dimension##f &operator [] (unsigned int i)						\
	{  	return(vector##dimension##f &)col[i];   }							\
	/* Array indexing */													\
	const vector##dimension##f &operator [] (unsigned int i) const			\
	{  return(vector##dimension##f &)col[i];  }								\
	/* perform a fast identity */											\
	void identity()															\
	{   IdentityMtxT(matrix##dimension, *this, dimension);   }				\
	/* perform a fast zero */												\
	void zero()																\
	{   InitMtxT(matrix##dimension, *this, dimension, float, 0);  }			\
	/* perform a fast transpose */											\
	void transpose()														\
	{   TransposeMtxT(matrix##dimension, *this, dimension);   }				\
	/* multiplication by a matrix */                                        \
	const matrix##dimension operator *(const matrix##dimension &m) const	\
	{																		\
		matrix##dimension temp(*this);										\
		return temp*=m;														\
	}																		\
	/* multiplication by a scalar */										\
	const matrix##dimension operator *(const float s) const					\
	{																		\
		matrix##dimension temp(*this);										\
		return temp*=s;														\
	}																		\
	/* division by a scalar */												\
	const matrix##dimension operator /(const float s) const					\
	{																		\
		matrix##dimension temp(*this);										\
		return temp/=s;														\
	}																		\
	    

//! external matrix class defines
#define MATRIX_COMMON_EXTERNAL(dimension)									\
/* base case specialization for Identity matrix*/							\
template<> struct IdentityMtxImpl<matrix##dimension, dimension, 0, 0, dimension*dimension>	\
{  static inline void eval(matrix##dimension &) {}  };						\
/* base case specialization for initializing a matrix */					\
template<> struct InitMtxImpl<matrix##dimension, dimension, float, 0, 0, 0, dimension*dimension>	\
{  static inline void eval(matrix##dimension &) {}  };						\
/* base case specialization for transposing a matrix */						\
template<> struct TransposeMtxImpl<matrix##dimension, dimension, 0, 1, dimension*dimension>	\
{   static inline void eval(matrix##dimension &) {}  };						\

