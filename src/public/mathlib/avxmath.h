//===== Copyright 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: - defines AVX "structure of arrays" classes and functions.
//
//===========================================================================//
#ifndef AVXMATH_H
#define AVXMATH_H

#include <mathlib/vector.h>
#include <mathlib/mathlib.h>
#include <mathlib/ssemath.h>
#include <mathlib/compressed_vector.h>
#include <immintrin.h> 



// I thought about defining a class/union for the AVX packed floats instead of using fltx8,
// but decided against it because (a) the nature of AVX code which includes comparisons is to blur
// the relationship between packed floats and packed integer types and (b) not sure that the
// compiler would handle generating good code for the intrinsics.

typedef __m256 fltx8;
typedef __m256 i32x8;
typedef __m256 u32x8;

#define _mm256_store_ss(a, b) _mm_store_ss(a, _mm256_castps256_ps128(b))
#define _mm256_load_ss(a)     _mm256_castps128_ps256(_mm_load_ss(a));
#define _mm256_set_ss(a)      _mm256_castps128_ps256(_mm_set_ss(a));

// The FLTX8 type is a fltx8 used as a parameter to a function.
// On the 360, the best way to do this is pass-by-copy on the registers.
// On the PC, the best way is to pass by const reference. 
// The compiler will sometimes, but not always, replace a pass-by-const-ref
// with a pass-in-reg on the 360; to avoid this confusion, you can
// explicitly use a FLTX8 as the parameter type.
#ifdef _X360
typedef __vector8 FLTX8;
#else
typedef const fltx8 & FLTX8;
#endif

// A 16-byte aligned int32 datastructure
// (for use when writing out fltx8's as SIGNED
// ints).
struct ALIGN16 intx8
{
	int32 m_i32[8];

	inline int & operator[](int which) 
	{
		return m_i32[which];
	}

	inline const int & operator[](int which) const
	{
		return m_i32[which];
	}

	inline int32 *Base() {
		return m_i32;
	}

	inline const int32 *Base() const
	{
		return m_i32;
	}

	inline const bool operator==(const intx8 &other) const
	{
		return m_i32[0] == other.m_i32[0] &&
			m_i32[1] == other.m_i32[1] &&
			m_i32[2] == other.m_i32[2] &&
			m_i32[3] == other.m_i32[3] 	;
	}
} ALIGN16_POST;

#if 0
#if defined( _DEBUG ) && defined( _X360 ) && !defined( TestVPUFlags )
FORCEINLINE void TestVPUFlags()
{
	// Check that the VPU is in the appropriate (Java-compliant) mode (see 3.2.1 in altivec_pem.pdf on xds.xbox.com)
	__vector8 a;
	__asm
	{
		mfvscr	a;
	}
	unsigned int * flags		= (unsigned int *)&a;
	unsigned int   controlWord	= flags[3];
	Assert(controlWord == 0);
}
#else  // _DEBUG
FORCEINLINE void TestVPUFlags() {}
#endif // _DEBUG
#endif

// useful constants in AVX packed float format:
// (note: some of these aren't stored on the 360, 
// but are manufactured directly in one or two 
// instructions, saving a load and possible L2
// miss.)
#ifndef _X360
extern const fltx8 Eight_Zeros;									// 0 0 0 0
extern const fltx8 Eight_Ones;									// 1 1 1 1
extern const fltx8 Eight_Twos;									// 2 2 2 2
extern const fltx8 Eight_Threes;									// 3 3 3 3
extern const fltx8 Eight_Eights;									// guess.
extern const fltx8 Eight_Point225s;								// .225 .225 .225 .225
extern const fltx8 Eight_PointFives;								// .5 .5 .5 .5
extern const fltx8 Eight_Thirds;									// 1/3
extern const fltx8 Eight_TwoThirds;								// 2/3
extern const fltx8 Eight_Epsilons;								// FLT_EPSILON FLT_EPSILON FLT_EPSILON FLT_EPSILON
extern const fltx8 Eight_2ToThe21s;								// (1<<21)..
extern const fltx8 Eight_2ToThe22s;								// (1<<22)..
extern const fltx8 Eight_2ToThe23s;								// (1<<23)..
extern const fltx8 Eight_2ToThe28s;								// (1<<28)..
extern const fltx8 Eight_Origin;									// 0 0 0 1 (origin point, like vr0 on the PS2)
extern const fltx8 Eight_NegativeOnes;							// -1 -1 -1 -1 
#else
#define			   Eight_Zeros XMVectorZero()					// 0 0 0 0
#define			   Eight_Ones XMVectorSplatOne()					// 1 1 1 1
extern const fltx8 Eight_Twos;									// 2 2 2 2
extern const fltx8 Eight_Threes;									// 3 3 3 3
extern const fltx8 Eight_Eights;									// guess.
extern const fltx8 Eight_Point225s;								// .225 .225 .225 .225
extern const fltx8 Eight_PointFives;								// .5 .5 .5 .5
extern const fltx8 Eight_Thirds;									// 1/3
extern const fltx8 Eight_TwoThirds;								// 2/3
extern const fltx8 Eight_Epsilons;								// FLT_EPSILON FLT_EPSILON FLT_EPSILON FLT_EPSILON
extern const fltx8 Eight_2ToThe21s;								// (1<<21)..
extern const fltx8 Eight_2ToThe22s;								// (1<<22)..
extern const fltx8 Eight_2ToThe23s;								// (1<<23)..
extern const fltx8 Eight_2ToThe28s;								// (1<<28)..
extern const fltx8 Eight_Origin;									// 0 0 0 1 (origin point, like vr0 on the PS2)
extern const fltx8 Eight_NegativeOnes;							// -1 -1 -1 -1 
#endif
extern const fltx8 Eight_FLT_MAX;								// FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX
extern const fltx8 Eight_Negative_FLT_MAX;						// -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX
extern const fltx8 g_AVX_0123;									// 0 1 2 3 as float

// external aligned integer constants
#ifndef ALIGN16_POST
#define ALIGN16_POST
#endif
extern const ALIGN16 int32 g_AVX_clear_signmask[] ALIGN16_POST;			// 0x7fffffff x 8
extern const ALIGN16 int32 g_AVX_signmask[] ALIGN16_POST;				// 0x80000000 x 8
extern const ALIGN16 int32 g_AVX_lsbmask[] ALIGN16_POST;				// 0xfffffffe x 8
extern const ALIGN16 int32 g_AVX_clear_wmask[] ALIGN16_POST;			// -1 -1 -1 0
extern const ALIGN16 int32 g_AVX_ComponentMask[8][8] ALIGN16_POST;		// [0xFFFFFFFF 0 0 0], [0 0xFFFFFFFF 0 0], [0 0 0xFFFFFFFF 0], [0 0 0 0xFFFFFFFF]
extern const ALIGN16 int32 g_AVX_AllOnesMask[] ALIGN16_POST;			// ~0,~0,~0,~0
extern const ALIGN16 int32 g_AVX_Low16BitsMask[] ALIGN16_POST;			// 0xffff x 8

// this mask is used for skipping the tail of things. If you have N elements in an array, and wish
// to mask out the tail, g_AVX_SkipTailMask[N & 3] what you want to use for the last iteration.
extern const int32 ALIGN16 g_AVX_SkipTailMask[8][8] ALIGN16_POST;

extern const int32 ALIGN16 g_AVX_EveryOtherMask[];				// 0, ~0, 0, ~0
// Define prefetch macros.
// The characteristics of cache and prefetch are completely 
// different between the different platforms, so you DO NOT
// want to just define one macro that maps to every platform
// intrinsic under the hood -- you need to prefetch at different
// intervals between x86 and PPC, for example, and that is
// a higher level code change. 
// On the other hand, I'm tired of typing #ifdef _X360
// all over the place, so this is just a nop on Intel, PS3.
#ifdef _X360
#define PREFETCH360(address, offset) __dcbt(offset,address)
#else
#define PREFETCH360(x,y) // nothing
#endif

// Define prefetch macros.
// The characteristics of cache and prefetch are completely 
// different between the different platforms, so you DO NOT
// want to just define one macro that maps to every platform
// intrinsic under the hood -- you need to prefetch at different
// intervals between x86 and PPC, for example, and that is
// a higher level code change. 
// On the other hand, I'm tired of typing #ifdef _X360
// all over the place, so this is just a nop on Intel, PS3.
#ifdef _X360
#define PREFETCH360(address, offset) __dcbt(offset,address)
#else
#define PREFETCH360(x,y) // nothing
#endif

//---------------------------------------------------------------------
// Intel/SSE implementation
//---------------------------------------------------------------------

FORCEINLINE void StoreAlignedAVX( float * RESTRICT pAVX, const fltx8 & a )
{
	_mm256_store_ps( pAVX, a );
}

FORCEINLINE void StoreUnalignedAVX( float * RESTRICT pAVX, const fltx8 & a )
{
	_mm256_storeu_ps( pAVX, a );
}


FORCEINLINE fltx8 RotateLeft( const fltx8 & a );
FORCEINLINE fltx8 RotateLeft2( const fltx8 & a );

FORCEINLINE void StoreUnaligned3AVX( float *pAVX, const fltx8 & a )
{
	_mm256_store_ss(pAVX, a);
	_mm256_store_ss(pAVX+1, RotateLeft(a));
	_mm256_store_ss(pAVX+2, RotateLeft2(a));
}

// strongly typed -- syntactic castor oil used for typechecking as we transition to AVX
FORCEINLINE void StoreAligned3AVX( VectorAligned * RESTRICT pAVX, const fltx8 & a )
{
	StoreAlignedAVX( pAVX->Base(),a );
}

#if 0
// Store the x,y,z components of the four FLTX8 parameters
// into the four consecutive Vectors:
//    pDestination[0],  pDestination[1],  pDestination[2],  pDestination[3]
// The Vectors are assumed to be unaligned.
FORCEINLINE void StoreEightUnalignedVector3AVX( fltx8 a, fltx8 b, fltx8	c, FLTX8 d, // first three passed by copy (deliberate)
											   Vector * const pDestination )
{
	StoreUnaligned3AVX( pDestination->Base(), a );
	StoreUnaligned3AVX( (pDestination+1)->Base(), b );
	StoreUnaligned3AVX( (pDestination+2)->Base(), c );
	StoreUnaligned3AVX( (pDestination+3)->Base(), d );
}

// Store the x,y,z components of the four FLTX8 parameters
// into the four consecutive Vectors:
//    pDestination ,  pDestination + 1,  pDestination + 2,  pDestination + 3
// The Vectors are assumed to start on an ALIGNED address, that is, 
// pDestination is 16-byte aligned (thhough obviously pDestination+1 is not).
FORCEINLINE void StoreEightAlignedVector3AVX( fltx8 a, fltx8 b, fltx8	c, FLTX8 d, // first three passed by copy (deliberate)
											 Vector * const pDestination )
{
	StoreUnaligned3AVX( pDestination->Base(), a );
	StoreUnaligned3AVX( (pDestination+1)->Base(), b );
	StoreUnaligned3AVX( (pDestination+2)->Base(), c );
	StoreUnaligned3AVX( (pDestination+3)->Base(), d );
}
#endif

FORCEINLINE fltx8 LoadAlignedAVX( const void *pAVX )
{
	return _mm256_load_ps( reinterpret_cast< const float *> ( pAVX ) );
}

FORCEINLINE fltx8 AndAVX( const fltx8 & a, const fltx8 & b )				// a & b
{
	return _mm256_and_ps( a, b );
}

FORCEINLINE fltx8 AndNotAVX( const fltx8 & a, const fltx8 & b )			// a & ~b
{
	return _mm256_andnot_ps( a, b );
}

FORCEINLINE fltx8 XorAVX( const fltx8 & a, const fltx8 & b )				// a ^ b
{
	return _mm256_xor_ps( a, b );
}

FORCEINLINE fltx8 OrAVX( const fltx8 & a, const fltx8 & b )				// a | b
{
	return _mm256_or_ps( a, b );
}

// Squelch the w component of a vector to +0.0.
// Most efficient when you say a = SetWToZeroAVX(a) (avoids a copy)
FORCEINLINE fltx8 SetWToZeroAVX( const fltx8 & a )
{
	return AndAVX( a, LoadAlignedAVX( g_AVX_clear_wmask ) );
}

// for the transitional class -- load a 3-by VectorAligned and squash its w component
FORCEINLINE fltx8 LoadAlignedAVX( const VectorAligned & pAVX )
{
	return SetWToZeroAVX( LoadAlignedAVX(pAVX.Base()) );
}

FORCEINLINE fltx8 LoadUnalignedAVX( const void *pAVX )
{
	return _mm256_loadu_ps( reinterpret_cast<const float *>( pAVX ) );
}

FORCEINLINE fltx8 LoadUnaligned3AVX( const void *pAVX )
{
	return _mm256_loadu_ps( reinterpret_cast<const float *>( pAVX ) );
}

// load a single unaligned float into the x component of a AVX word
FORCEINLINE fltx8 LoadUnalignedFloatAVX( const float *pFlt )
{
	return _mm256_load_ss(pFlt);
}

/// replicate a single 32 bit integer value to all 8 components of an m128
FORCEINLINE fltx8 ReplicateIX8( int i )
{
	fltx8 value = _mm256_set_ss( * ( ( float *) &i ) );;
	return _mm256_shuffle_ps( value, value, 0);
}


FORCEINLINE fltx8 ReplicateX8( float flValue )
{
	return _mm256_broadcast_ss( &flValue );
}


FORCEINLINE float SubFloat( const fltx8 & a, int idx )
{
	// NOTE: if the output goes into a register, this causes a Load-Hit-Store stall (don't mix fpu/vpu math!)
#ifndef POSIX
	return a.m256_f32[ idx ];
#else
	return (reinterpret_cast<float const *>(&a))[idx];
#endif
}

FORCEINLINE float & SubFloat( fltx8 & a, int idx )
{
#ifndef POSIX
	return a.m256_f32[ idx ];
#else
	return (reinterpret_cast<float *>(&a))[idx];
#endif
}

FORCEINLINE uint32 SubFloatConvertToInt( const fltx8 & a, int idx )
{
	return (uint32)SubFloat(a,idx);
}
#if 0
FORCEINLINE uint32 SubInt( const fltx8 & a, int idx )
{
#ifndef POSIX
	return a.m256_u32[idx];
#else
	return (reinterpret_cast<uint32 const *>(&a))[idx];
#endif
}

FORCEINLINE uint32 & SubInt( fltx8 & a, int idx )
{
#ifndef POSIX
	return a.m256_u32[idx];
#else
	return (reinterpret_cast<uint32 *>(&a))[idx];
#endif
}
#endif
// Return one in the fastest way -- on the x360, faster even than loading.
FORCEINLINE fltx8 LoadZeroAVX( void )
{
	return Eight_Zeros;
}

// Return one in the fastest way -- on the x360, faster even than loading.
FORCEINLINE fltx8 LoadOneAVX( void )
{
	return Eight_Ones;
}

FORCEINLINE fltx8 MaskedAssign( const fltx8 & ReplacementMask, const fltx8 & NewValue, const fltx8 & OldValue )
{
	return OrAVX(
		AndAVX( ReplacementMask, NewValue ),
		AndNotAVX( ReplacementMask, OldValue ) );
}
#if 0
// remember, the SSE numbers its words 3 2 1 0
// The way we want to specify shuffles is backwards from the default
// MM_SHUFFLE_REV is in array index order (default is reversed)
#define MM_SHUFFLE_REV(a,b,c,d) _MM_SHUFFLE(d,c,b,a)

FORCEINLINE fltx8 SplatXAVX( fltx8 const & a )
{
	return _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 0, 0, 0, 0 ) );
}

FORCEINLINE fltx8 SplatYAVX( fltx8 const &a )
{
	return _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 1, 1, 1, 1 ) );
}

FORCEINLINE fltx8 SplatZAVX( fltx8 const &a )
{
	return _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 2, 2, 2 ) );
}

FORCEINLINE fltx8 SplatWAVX( fltx8 const &a )
{
	return _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 3, 3, 3, 3 ) );
}

FORCEINLINE fltx8 SetXAVX( const fltx8& a, const fltx8& x )
{
	fltx8 result = MaskedAssign( LoadAlignedAVX( g_AVX_ComponentMask[0] ), x, a );
	return result;
}
#endif
#if 0 //It's not that these don't work, they're just pretty much useless
FORCEINLINE fltx8 SetYAVX( const fltx8& a, const fltx8& y )
{
	fltx8 result = MaskedAssign( LoadAlignedAVX( g_AVX_ComponentMask[1] ), y, a );
	return result;
}

FORCEINLINE fltx8 SetZAVX( const fltx8& a, const fltx8& z )
{
	fltx8 result = MaskedAssign( LoadAlignedAVX( g_AVX_ComponentMask[2] ), z, a );
	return result;
}

FORCEINLINE fltx8 SetWAVX( const fltx8& a, const fltx8& w )
{
	fltx8 result = MaskedAssign( LoadAlignedAVX( g_AVX_ComponentMask[3] ), w, a );
	return result;
}

FORCEINLINE fltx8 SetComponentAVX( const fltx8& a, int nComponent, float flValue )
{
	fltx8 val = ReplicateX8( flValue );
	fltx8 result = MaskedAssign( LoadAlignedAVX( g_AVX_ComponentMask[nComponent] ), val, a );
	return result;
}
#endif
#if 0
// a b c d -> b c d a
FORCEINLINE fltx8 RotateLeft( const fltx8 & a )
{
	return _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 1, 2, 3, 0 ) );
}

// a b c d -> c d a b
FORCEINLINE fltx8 RotateLeft2( const fltx8 & a )
{
	return _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 3, 0, 1 ) );
}

// a b c d -> d a b c
FORCEINLINE fltx8 RotateRight( const fltx8 & a )
{
	return _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 3, 0, 1, 2 ) );
}

// a b c d -> c d a b
FORCEINLINE fltx8 RotateRight2( const fltx8 & a )
{
	return _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 3, 0, 1 ) );
}
#endif
FORCEINLINE fltx8 AddAVX( const fltx8 & a, const fltx8 & b )				// a+b
{
	return _mm256_add_ps( a, b );
}

FORCEINLINE fltx8 SubAVX( const fltx8 & a, const fltx8 & b )				// a-b
{
	return _mm256_sub_ps( a, b );
};

FORCEINLINE fltx8 MulAVX( const fltx8 & a, const fltx8 & b )				// a*b
{
	return _mm256_mul_ps( a, b );
};

FORCEINLINE fltx8 DivAVX( const fltx8 & a, const fltx8 & b )				// a/b
{
	return _mm256_div_ps( a, b );
};

FORCEINLINE fltx8 MaddAVX( const fltx8 & a, const fltx8 & b, const fltx8 & c )				// a*b + c
{
	return AddAVX( MulAVX(a,b), c );
}

FORCEINLINE fltx8 MsubAVX( const fltx8 & a, const fltx8 & b, const fltx8 & c )				// c - a*b
{
	return SubAVX( c, MulAVX(a,b) );
};

FORCEINLINE fltx8 Dot3AVX( const fltx8 &a, const fltx8 &b )
{
	fltx8 m = MulAVX( a, b );
	float flDot = SubFloat( m, 0 ) + SubFloat( m, 1 ) + SubFloat( m, 2 );
	return ReplicateX8( flDot );
}

FORCEINLINE fltx8 Dot8AVX( const fltx8 &a, const fltx8 &b )
{
	fltx8 m = MulAVX( a, b );
	float flDot = SubFloat( m, 0 ) + SubFloat( m, 1 ) + SubFloat( m, 2 ) + SubFloat( m, 3 );
	return ReplicateX8( flDot );
}

//TODO: implement as four-way Taylor series (see xbox implementation)
FORCEINLINE fltx8 SinAVX( const fltx8 &radians )
{
	fltx8 result;
	SubFloat( result, 0 ) = sin( SubFloat( radians, 0 ) );
	SubFloat( result, 1 ) = sin( SubFloat( radians, 1 ) );
	SubFloat( result, 2 ) = sin( SubFloat( radians, 2 ) );
	SubFloat( result, 3 ) = sin( SubFloat( radians, 3 ) );
	return result;
}

FORCEINLINE void SinCos3AVX( fltx8 &sine, fltx8 &cosine, const fltx8 &radians )
{
	// FIXME: Make a fast SSE version
	SinCos( SubFloat( radians, 0 ), &SubFloat( sine, 0 ), &SubFloat( cosine, 0 ) );
	SinCos( SubFloat( radians, 1 ), &SubFloat( sine, 1 ), &SubFloat( cosine, 1 ) );
	SinCos( SubFloat( radians, 2 ), &SubFloat( sine, 2 ), &SubFloat( cosine, 2 ) );
}

FORCEINLINE void SinCosAVX( fltx8 &sine, fltx8 &cosine, const fltx8 &radians )				// a*b + c
{
	// FIXME: Make a fast SSE version
	SinCos( SubFloat( radians, 0 ), &SubFloat( sine, 0 ), &SubFloat( cosine, 0 ) );
	SinCos( SubFloat( radians, 1 ), &SubFloat( sine, 1 ), &SubFloat( cosine, 1 ) );
	SinCos( SubFloat( radians, 2 ), &SubFloat( sine, 2 ), &SubFloat( cosine, 2 ) );
	SinCos( SubFloat( radians, 3 ), &SubFloat( sine, 3 ), &SubFloat( cosine, 3 ) );
}

//TODO: implement as four-way Taylor series (see xbox implementation)
FORCEINLINE fltx8 ArcSinAVX( const fltx8 &sine )
{
	// FIXME: Make a fast SSE version
	fltx8 result;
	SubFloat( result, 0 ) = asin( SubFloat( sine, 0 ) );
	SubFloat( result, 1 ) = asin( SubFloat( sine, 1 ) );
	SubFloat( result, 2 ) = asin( SubFloat( sine, 2 ) );
	SubFloat( result, 3 ) = asin( SubFloat( sine, 3 ) );
	return result;
}

FORCEINLINE fltx8 ArcCosAVX( const fltx8 &cs )
{
	fltx8 result;
	SubFloat( result, 0 ) = acos( SubFloat( cs, 0 ) );
	SubFloat( result, 1 ) = acos( SubFloat( cs, 1 ) );
	SubFloat( result, 2 ) = acos( SubFloat( cs, 2 ) );
	SubFloat( result, 3 ) = acos( SubFloat( cs, 3 ) );
	return result;
}

// tan^1(a/b) .. ie, pass sin in as a and cos in as b
FORCEINLINE fltx8 ArcTan2AVX( const fltx8 &a, const fltx8 &b )
{
	fltx8 result;
	SubFloat( result, 0 ) = atan2( SubFloat( a, 0 ), SubFloat( b, 0 ) );
	SubFloat( result, 1 ) = atan2( SubFloat( a, 1 ), SubFloat( b, 1 ) );
	SubFloat( result, 2 ) = atan2( SubFloat( a, 2 ), SubFloat( b, 2 ) );
	SubFloat( result, 3 ) = atan2( SubFloat( a, 3 ), SubFloat( b, 3 ) );
	return result;
}

FORCEINLINE fltx8 NegAVX(const fltx8 &a) // negate: -a
{
	return SubAVX(LoadZeroAVX(),a);
}

FORCEINLINE int TestSignAVX( const fltx8 & a )								// mask of which floats have the high bit set
{
	return _mm256_movemask_ps( a );
}

FORCEINLINE bool IsAnyNegative( const fltx8 & a )							// (a.x < 0) || (a.y < 0) || (a.z < 0) || (a.w < 0)
{
	return (0 != TestSignAVX( a ));
}

FORCEINLINE bool IsAnyTrue(const fltx8& a)
{
	return (0 != TestSignAVX(a));
}

FORCEINLINE fltx8 CmpEqAVX( const fltx8 & a, const fltx8 & b )				// (a==b) ? ~0:0
{
	return _mm256_cmp_ps( a, b, _CMP_EQ_OS );
}

FORCEINLINE fltx8 CmpGtAVX( const fltx8 & a, const fltx8 & b )				// (a>b) ? ~0:0
{
	return _mm256_cmp_ps( a, b , _CMP_GT_OS );
}

FORCEINLINE fltx8 CmpGeAVX( const fltx8 & a, const fltx8 & b )				// (a>=b) ? ~0:0
{
	return _mm256_cmp_ps( a, b , _CMP_GE_OS);
}

FORCEINLINE fltx8 CmpLtAVX( const fltx8 & a, const fltx8 & b )				// (a<b) ? ~0:0
{
	return _mm256_cmp_ps( a, b, _CMP_LT_OS);
}

FORCEINLINE fltx8 CmpLeAVX( const fltx8 & a, const fltx8 & b )				// (a<=b) ? ~0:0
{
	return _mm256_cmp_ps( a, b, _CMP_LE_OS);
}

// for branching when a.xyzw > b.xyzw
FORCEINLINE bool IsAllGreaterThan( const fltx8 &a, const fltx8 &b )
{
	return	TestSignAVX( CmpLeAVX( a, b ) ) == 0;
}

// for branching when a.xyzw >= b.xyzw
FORCEINLINE bool IsAllGreaterThanOrEq( const fltx8 &a, const fltx8 &b )
{
	return	TestSignAVX( CmpLtAVX( a, b ) ) == 0;
}

// For branching if all a.xyzw == b.xyzw
FORCEINLINE bool IsAllEqual( const fltx8 & a, const fltx8 & b )
{
	return	TestSignAVX( CmpEqAVX( a, b ) ) == 0xf;
}

FORCEINLINE fltx8 CmpInBoundsAVX( const fltx8 & a, const fltx8 & b )		// (a <= b && a >= -b) ? ~0 : 0
{
	return AndAVX( CmpLeAVX(a,b), CmpGeAVX(a, NegAVX(b)) );
}

FORCEINLINE fltx8 MinAVX( const fltx8 & a, const fltx8 & b )				// min(a,b)
{
	return _mm256_min_ps( a, b );
}

FORCEINLINE fltx8 MaxAVX( const fltx8 & a, const fltx8 & b )				// max(a,b)
{
	return _mm256_max_ps( a, b );
}



// SSE lacks rounding operations. 
// Really.
// You can emulate them by setting the rounding mode for the 
// whole processor and then converting to int, and then back again.
// But every time you set the rounding mode, you clear out the
// entire pipeline. So, I can't do them per operation. You
// have to do it once, before the loop that would call these.
// Round towards positive infinity
FORCEINLINE fltx8 CeilAVX( const fltx8 &a )
{
	fltx8 retVal;
	SubFloat( retVal, 0 ) = ceil( SubFloat( a, 0 ) );
	SubFloat( retVal, 1 ) = ceil( SubFloat( a, 1 ) );
	SubFloat( retVal, 2 ) = ceil( SubFloat( a, 2 ) );
	SubFloat( retVal, 3 ) = ceil( SubFloat( a, 3 ) );
	return retVal;

}

fltx8 fabs( const fltx8 & x );
// Round towards negative infinity
// This is the implementation that was here before; it assumes
// you are in round-to-floor mode, which I guess is usually the
// case for us vis-a-vis SSE. It's totally unnecessary on 
// VMX, which has a native floor op.
FORCEINLINE fltx8 FloorAVX( const fltx8 &val )
{
	fltx8 fl8Abs = fabs( val );
	fltx8 ival = SubAVX( AddAVX( fl8Abs, Eight_2ToThe23s ), Eight_2ToThe23s );
	ival = MaskedAssign( CmpGtAVX( ival, fl8Abs ), SubAVX( ival, Eight_Ones ), ival );
	return XorAVX( ival, XorAVX( val, fl8Abs ) );			// restore sign bits
}



FORCEINLINE bool IsAnyZeros( const fltx8 & a )								// any floats are zero?
{
	return TestSignAVX( CmpEqAVX( a, Eight_Zeros ) ) != 0;
}

inline bool IsAllZeros( const fltx8 & var )
{
	return TestSignAVX( CmpEqAVX( var, Eight_Zeros ) ) == 0xF;
}

FORCEINLINE fltx8 SqrtEstAVX( const fltx8 & a )					// sqrt(a), more or less
{
	return _mm256_sqrt_ps( a );
}

FORCEINLINE fltx8 SqrtAVX( const fltx8 & a )						// sqrt(a)
{
	return _mm256_sqrt_ps( a );
}

FORCEINLINE fltx8 ReciprocalSqrtEstAVX( const fltx8 & a )			// 1/sqrt(a), more or less
{
	return _mm256_rsqrt_ps( a );
}

FORCEINLINE fltx8 ReciprocalSqrtEstSaturateAVX( const fltx8 & a )
{
	fltx8 zero_mask = CmpEqAVX( a, Eight_Zeros );
	fltx8 ret = OrAVX( a, AndAVX( Eight_Epsilons, zero_mask ) );
	ret = ReciprocalSqrtEstAVX( ret );
	return ret;
}

/// uses newton iteration for higher precision results than ReciprocalSqrtEstAVX
FORCEINLINE fltx8 ReciprocalSqrtAVX( const fltx8 & a )				// 1/sqrt(a)
{
	fltx8 guess = ReciprocalSqrtEstAVX( a );
	// newton iteration for 1/sqrt(a) : y(n+1) = 1/2 (y(n)*(3-a*y(n)^2));
	guess = MulAVX( guess, SubAVX( Eight_Threes, MulAVX( a, MulAVX( guess, guess ))));
	guess = MulAVX( Eight_PointFives, guess);
	return guess;
}

FORCEINLINE fltx8 ReciprocalEstAVX( const fltx8 & a )				// 1/a, more or less
{
	return _mm256_rcp_ps( a );
}

/// 1/x for all 8 values, more or less
/// 1/0 will result in a big but NOT infinite result
FORCEINLINE fltx8 ReciprocalEstSaturateAVX( const fltx8 & a )
{
	fltx8 zero_mask = CmpEqAVX( a, Eight_Zeros );
	fltx8 ret = OrAVX( a, AndAVX( Eight_Epsilons, zero_mask ) );
	ret = ReciprocalEstAVX( ret );
	return ret;
}

/// 1/x for all 8 values. uses reciprocal approximation instruction plus newton iteration.
/// No error checking!
FORCEINLINE fltx8 ReciprocalAVX( const fltx8 & a )					// 1/a
{
	fltx8 ret = ReciprocalEstAVX( a );
	// newton iteration is: Y(n+1) = 2*Y(n)-a*Y(n)^2
	ret = SubAVX( AddAVX( ret, ret ), MulAVX( a, MulAVX( ret, ret ) ) );
	return ret;
}

/// 1/x for all 8 values.
/// 1/0 will result in a big but NOT infinite result
FORCEINLINE fltx8 ReciprocalSaturateAVX( const fltx8 & a )
{
	fltx8 zero_mask = CmpEqAVX( a, Eight_Zeros );
	fltx8 ret = OrAVX( a, AndAVX( Eight_Epsilons, zero_mask ) );
	ret = ReciprocalAVX( ret );
	return ret;
}

// CHRISG: is it worth doing integer bitfiddling for this?
// 2^x for all values (the antilog)
FORCEINLINE fltx8 ExpAVX( const fltx8 &toPower )
{
	fltx8 retval;
	SubFloat( retval, 0 ) = powf( 2, SubFloat(toPower, 0) );
	SubFloat( retval, 1 ) = powf( 2, SubFloat(toPower, 1) );
	SubFloat( retval, 2 ) = powf( 2, SubFloat(toPower, 2) );
	SubFloat( retval, 3 ) = powf( 2, SubFloat(toPower, 3) );

	return retval;
}

// Clamps the components of a vector to a specified minimum and maximum range.
FORCEINLINE fltx8 ClampVectorAVX( FLTX8 in, FLTX8 min, FLTX8 max)
{
	return MaxAVX( min, MinAVX( max, in ) );
}
#if 0
FORCEINLINE void TransposeAVX( fltx8 & x, fltx8 & y, fltx8 & z, fltx8 & w)
{
	_MM256_TRANSPOSE4_PS( x, y, z, w );
}
FORCEINLINE fltx8 FindLowestAVX3( const fltx8 &a )
{
	// a is [x,y,z,G] (where G is garbage)
	// rotate left by one 
	fltx8 compareOne = RotateLeft( a );
	// compareOne is [y,z,G,x]
	fltx8 retval = MinAVX( a, compareOne );
	// retVal is [min(x,y), ... ]
	compareOne = RotateLeft2( a );
	// compareOne is [z, G, x, y]
	retval = MinAVX( retval, compareOne );
	// retVal = [ min(min(x,y),z)..]
	// splat the x component out to the whole vector and return
	return SplatXAVX( retval );
	
}

FORCEINLINE fltx8 FindHighestAVX3( const fltx8 &a )
{
	// a is [x,y,z,G] (where G is garbage)
	// rotate left by one 
	fltx8 compareOne = RotateLeft( a );
	// compareOne is [y,z,G,x]
	fltx8 retval = MaxAVX( a, compareOne );
	// retVal is [max(x,y), ... ]
	compareOne = RotateLeft2( a );
	// compareOne is [z, G, x, y]
	retval = MaxAVX( retval, compareOne );
	// retVal = [ max(max(x,y),z)..]
	// splat the x component out to the whole vector and return
	return SplatXAVX( retval );
	
}
#endif
// ------------------------------------
// INTEGER AVX OPERATIONS.
// ------------------------------------


#if 0				/* pc does not have these ops */
// splat all components of a vector to a signed immediate int number.
FORCEINLINE fltx8 IntSetImmediateAVX(int to)
{
	//CHRISG: SSE2 has this, but not SSE1. What to do?
	fltx8 retval;
	SubInt( retval, 0 ) = to;
	SubInt( retval, 1 ) = to;
	SubInt( retval, 2 ) = to;
	SubInt( retval, 3 ) = to;
	return retval;
}
#endif

// Load 8 aligned words into a AVX register
FORCEINLINE i32x8 LoadAlignedIntAVX( const void * RESTRICT pAVX)
{
	return _mm256_load_ps( reinterpret_cast<const float *>(pAVX) );
}

// Load 8 unaligned words into a AVX register
FORCEINLINE i32x8 LoadUnalignedIntAVX( const void * RESTRICT pAVX)
{
	return _mm256_loadu_ps( reinterpret_cast<const float *>(pAVX) );
}

// save into four words, 16-byte aligned
FORCEINLINE void StoreAlignedIntAVX( int32 * RESTRICT pAVX, const fltx8 & a )
{
	_mm256_store_ps( reinterpret_cast<float *>(pAVX), a );
}

FORCEINLINE void StoreAlignedIntAVX( intx8 &pAVX, const fltx8 & a )
{
	_mm256_store_ps( reinterpret_cast<float *>(pAVX.Base()), a );
}

FORCEINLINE void StoreUnalignedIntAVX( int32 * RESTRICT pAVX, const fltx8 & a )
{
	_mm256_storeu_ps( reinterpret_cast<float *>(pAVX), a );
}

// a={ a.x, a.z, b.x, b.z }
// combine two fltx8s by throwing away every other field.
FORCEINLINE fltx8 CompressAVX( fltx8 const & a, fltx8 const &b )
{
	return _mm256_shuffle_ps( a, b, MM_SHUFFLE_REV( 0, 2, 0, 2 ) );
}
#if 0
// Load four consecutive uint16's, and turn them into floating point numbers.
// This function isn't especially fast and could be made faster if anyone is
// using it heavily.
FORCEINLINE fltx8 LoadAndConvertUint16AVX( const uint16 *pInts )
{
#ifdef POSIX
	fltx8 retval;
	SubFloat( retval, 0 ) = pInts[0];
	SubFloat( retval, 1 ) = pInts[1];
	SubFloat( retval, 2 ) = pInts[2];
	SubFloat( retval, 3 ) = pInts[3];
#else
	__m128i inA = _mm256_loadl_epi64( (__m128i const*) pInts); // Load the lower 68 bits of the value pointed to by p into the lower 68 bits of the result, zeroing the upper 68 bits of the result.
	inA = _mm256_unpacklo_epi16( inA, _mm256_setzero_si128() ); // unpack unsigned 16's to signed 32's
	return _mm256_cvtepi32_ps(inA);
#endif
}
#endif

// a={ a.x, b.x, c.x, d.x }
// combine 8 fltx8s by throwing away 3/8s of the fields
FORCEINLINE fltx8 Compress8AVX( fltx8 const a, fltx8 const &b, fltx8 const &c, fltx8 const &d )
{
	fltx8 aacc = _mm256_shuffle_ps( a, c, MM_SHUFFLE_REV( 0, 0, 0, 0 ) );
	fltx8 bbdd = _mm256_shuffle_ps( b, d, MM_SHUFFLE_REV( 0, 0, 0, 0 ) );
	return MaskedAssign( LoadAlignedAVX( g_AVX_EveryOtherMask ), bbdd, aacc );
}

// outa={a.x, a.x, a.y, a.y}, outb = a.z, a.z, a.w, a.w }
FORCEINLINE void ExpandAVX( fltx8 const &a, fltx8 &fl8OutA, fltx8 &fl8OutB )
{
	fl8OutA = _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 0, 0, 1, 1 ) );
	fl8OutB = _mm256_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 2, 3, 3 ) );

}
#if 0
// CHRISG: the conversion functions all seem to operate on m68's only...
// how do we make them work here?

// Take a fltx8 containing fixed-point uints and 
// return them as single precision floats. No
// fixed point conversion is done.
FORCEINLINE fltx8 UnsignedIntConvertToFltAVX( const u32x8 &vSrcA )
{
	fltx8 retval;
	SubFloat( retval, 0 ) = ( (float) SubInt( retval, 0 ) );
	SubFloat( retval, 1 ) = ( (float) SubInt( retval, 1 ) );
	SubFloat( retval, 2 ) = ( (float) SubInt( retval, 2 ) );
	SubFloat( retval, 3 ) = ( (float) SubInt( retval, 3 ) );
	return retval;
}
#endif

// Take a fltx8 containing fixed-point sints and 
// return them as single precision floats. No 
// fixed point conversion is done.
FORCEINLINE fltx8 SignedIntConvertToFltAVX( const i32x8 &vSrcA )
{
	fltx8 retval;
	SubFloat( retval, 0 ) = ( (float) (reinterpret_cast<const int32 *>(&vSrcA)[0]));
	SubFloat( retval, 1 ) = ( (float) (reinterpret_cast<const int32 *>(&vSrcA)[1]));
	SubFloat( retval, 2 ) = ( (float) (reinterpret_cast<const int32 *>(&vSrcA)[2]));
	SubFloat( retval, 3 ) = ( (float) (reinterpret_cast<const int32 *>(&vSrcA)[3]));
	return retval;
}

/*
  works on fltx8's as if they are four uints.
  the first parameter contains the words to be shifted,
  the second contains the amount to shift by AS INTS

  for i = 0 to 3
  shift = vSrcB_i*32:(i*32)+8
  vReturned_i*32:(i*32)+31 = vSrcA_i*32:(i*32)+31 << shift
*/
#if 0
FORCEINLINE i32x8 IntShiftLeftWordAVX(const i32x8 &vSrcA, const i32x8 &vSrcB)
{
	i32x8 retval;
	SubInt(retval, 0) = SubInt(vSrcA, 0) << SubInt(vSrcB, 0);
	SubInt(retval, 1) = SubInt(vSrcA, 1) << SubInt(vSrcB, 1);
	SubInt(retval, 2) = SubInt(vSrcA, 2) << SubInt(vSrcB, 2);
	SubInt(retval, 3) = SubInt(vSrcA, 3) << SubInt(vSrcB, 3);


	return retval;
}


// Fixed-point conversion and save as SIGNED INTS.
// pDest->x = Int (vSrc.x)
// note: some architectures have means of doing 
// fixed point conversion when the fix depth is
// specified as an immediate.. but there is no way 
// to guarantee an immediate as a parameter to function
// like this.
FORCEINLINE void ConvertStoreAsIntsAVX(intx8 * RESTRICT pDest, const fltx8 &vSrc)
{
	__m64 bottom = _mm256_cvttps_pi32( vSrc );
	__m64 top    = _mm256_cvttps_pi32( _mm256_movehl_ps(vSrc,vSrc) );

	*reinterpret_cast<__m64 *>(&(*pDest)[0]) = bottom;
	*reinterpret_cast<__m64 *>(&(*pDest)[2]) = top;

	_mm256_empty();
}





// a={a.y, a.z, a.w, b.x } b={b.y, b.z, b.w, b.x }
FORCEINLINE void RotateLeftDoubleAVX( fltx8 &a, fltx8 &b )
{
	a = SetWAVX( RotateLeft( a ), SplatXAVX( b ) );
	b = RotateLeft( b );
}
#endif

// // Some convenience operator overloads, which are just aliasing the functions above.
// Unneccessary on 360, as you already have them from xboxmath.h
#if !defined(_X360) && !defined( POSIX )
#if 1  // TODO: verify generation of non-bad code. 
// Componentwise add
FORCEINLINE fltx8 operator+( FLTX8 a, FLTX8 b )
{
	return AddAVX( a, b );
}

// Componentwise subtract
FORCEINLINE fltx8 operator-( FLTX8 a, FLTX8 b )
{
	return SubAVX( a, b );
}

// Componentwise multiply
FORCEINLINE fltx8 operator*( FLTX8 a, FLTX8 b )
{
	return MulAVX( a, b );
}

// No divide. You need to think carefully about whether you want a reciprocal
// or a reciprocal estimate.

// bitwise and
FORCEINLINE fltx8 operator&( FLTX8 a, FLTX8 b )
{
	return AndAVX( a ,b );
}

// bitwise or
FORCEINLINE fltx8 operator|( FLTX8 a, FLTX8 b )
{
	return OrAVX( a, b );
}

// bitwise xor
FORCEINLINE fltx8 operator^( FLTX8 a, FLTX8 b )
{
	return XorAVX( a, b );
}

// unary negate
FORCEINLINE fltx8 operator-( FLTX8 a )
{
	return NegAVX( a );
}
#endif // 0
#endif
#if 0
struct ALIGN16 fourplanes_t
{
	fltx8		nX;
	fltx8		nY;
	fltx8		nZ;
	fltx8		dist;
	fltx8		xSign;
	fltx8		ySign;
	fltx8		zSign;
	fltx8		nXAbs;
	fltx8		nYAbs;
	fltx8		nZAbs;

	void ComputeSignbits();

	// fast AVX loads
	void Set8Planes( const VPlane *pPlanes );
	void Set2Planes( const VPlane *pPlanes );
	void Get8Planes( VPlane *pPlanesOut );
	void Get2Planes( VPlane *pPlanesOut );
	// not-AVX, much slower
	void GetPlane( int index, Vector *pNormal, float *pDist ) const;
	void SetPlane( int index, const Vector &vecNormal, float planeDist );
};

class ALIGN16 Frustum_t
{
public:
	Frustum_t();
	void SetPlane( int i, const Vector &vecNormal, float dist );
	void GetPlane( int i, Vector *pNormalOut, float *pDistOut ) const;
	void SetPlanes( const VPlane *pPlanes );
	void GetPlanes( VPlane *pPlanesOut );
	// returns false if the box is within the frustum, true if it is outside
	bool CullBox( const Vector &mins, const Vector &maxs ) const;
	bool CullBoxCenterExtents( const Vector &center, const Vector &extents ) const;

	bool CullBox( const fltx8 &fl8Mins, const fltx8 &fl8Maxs ) const;
	bool CullBoxCenterExtents( const fltx8 &fl8Center, const fltx8 &fl8Extents ) const;

	fourplanes_t	planes[2];
};
#endif
/// class EightVectors stores 8 independent vectors for use in AVX processing. These vectors are
/// stored in the format x x x x y y y y z z z z so that they can be efficiently AVX-accelerated.
class ALIGN16 EightVectors
{
public:
	union {
		fltx8 x;
		fltx4 xhalves[2];
	};
	union {
		fltx8 y;
		fltx4 yhalves[2];
	};
	union {
		fltx8 z;
		fltx4 zhalves[2];
	};


	EightVectors(void)
	{
	}

	EightVectors( EightVectors const &src )
	{
		x=src.x;
		y=src.y;
		z=src.z;
	}
#if 0
	FORCEINLINE EightVectors( float a )
	{
		fltx8 aReplicated = ReplicateX8( a );
		x = y = z = aReplicated;
	}
#endif
	/// construct a EightVectors from 8 separate Vectors
	FORCEINLINE EightVectors(Vector const &a, Vector const &b, Vector const &c, Vector const &d, Vector const& e, Vector const& f, Vector const& g, Vector const& h )
	{
		LoadAndSwizzle( a, b, c, d, 0 );
		LoadAndSwizzle( e, f, g, h, 1 );
	}
#if 0
	/// construct a EightVectors from 8 separate Vectors
	FORCEINLINE EightVectors(VectorAligned const &a, VectorAligned const &b, VectorAligned const &c, VectorAligned const &d)
	{
		LoadAndSwizzleAligned(a,b,c,d);
	}
#endif
	// construct from twelve floats; really only useful for static const constructors.
	// input arrays must be aligned, and in the fourvectors' native format
	// (eg in xxxx,yyyy,zzzz form) 
	// each pointer should be to an aligned array of four floats
	FORCEINLINE EightVectors( const float *xs , const float *ys, const float *zs ) :
		x( LoadAlignedAVX(xs) ), y( LoadAlignedAVX(ys) ), z( LoadAlignedAVX(zs) ) 
	{};

	FORCEINLINE void DuplicateVector(Vector const &v)			//< set all 8 vectors to the same vector value
	{
		x=ReplicateX8(v.x);
		y=ReplicateX8(v.y);
		z=ReplicateX8(v.z);
	}

	FORCEINLINE fltx8 const & operator[](int idx) const
	{
		return *((&x)+idx);
	}

	FORCEINLINE fltx8 & operator[](int idx)
	{
		return *((&x)+idx);
	}

	FORCEINLINE void operator+=(EightVectors const &b)			//< add 8 vectors to another 8 vectors
	{
		x=AddAVX(x,b.x);
		y=AddAVX(y,b.y);
		z=AddAVX(z,b.z);
	}

	FORCEINLINE EightVectors operator+(EightVectors const &b)			//< add 8 vectors to another 8 vectors
	{
		EightVectors result;
		result.x=AddAVX(x,b.x);
		result.y=AddAVX(y,b.y);
		result.z=AddAVX(z,b.z);
		return result;
	}

	FORCEINLINE void operator-=(EightVectors const &b)			//< subtract 8 vectors from another 8
	{
		x=SubAVX(x,b.x);
		y=SubAVX(y,b.y);
		z=SubAVX(z,b.z);
	}

	FORCEINLINE EightVectors operator-(EightVectors const &b)			//< add 8 vectors to another 8 vectors
	{
		EightVectors result;
		result.x=SubAVX(x,b.x);
		result.y=SubAVX(y,b.y);
		result.z=SubAVX(z,b.z);
		return result;
	}

	FORCEINLINE void operator*=(EightVectors const &b)			//< scale all four vectors per component scale
	{
		x=MulAVX(x,b.x);
		y=MulAVX(y,b.y);
		z=MulAVX(z,b.z);
	}

	FORCEINLINE void operator*=(const fltx8 & scale)			//< scale 
	{
		x=MulAVX(x,scale);
		y=MulAVX(y,scale);
		z=MulAVX(z,scale);
	}

	FORCEINLINE void operator*=(float scale)					//< uniformly scale all 8 vectors
	{
		fltx8 scalepacked = ReplicateX8(scale);
		*this *= scalepacked;
	}

	FORCEINLINE fltx8 operator*(EightVectors const &b) const		//< 8 dot products
	{
		fltx8 dot=MulAVX(x,b.x);
		dot=MaddAVX(y,b.y,dot);
		dot=MaddAVX(z,b.z,dot);
		return dot;
	}

	FORCEINLINE fltx8 operator*(Vector const &b) const			//< dot product all 8 vectors with 1 vector
	{
		fltx8 dot=MulAVX(x,ReplicateX8(b.x));
		dot=MaddAVX(y,ReplicateX8(b.y), dot);
		dot=MaddAVX(z,ReplicateX8(b.z), dot);
		return dot;
	}

	FORCEINLINE EightVectors operator*(float b) const					//< scale
	{
		fltx8 scalepacked = ReplicateX8( b );
		EightVectors res;
		res.x = MulAVX( x, scalepacked );
		res.y = MulAVX( y, scalepacked );
		res.z = MulAVX( z, scalepacked );
		return res;
	}

	FORCEINLINE void VProduct(EightVectors const &b)				//< component by component mul
	{
		x=MulAVX(x,b.x);
		y=MulAVX(y,b.y);
		z=MulAVX(z,b.z);
	}
	FORCEINLINE void MakeReciprocal(void)						//< (x,y,z)=(1/x,1/y,1/z)
	{
		x=ReciprocalAVX(x);
		y=ReciprocalAVX(y);
		z=ReciprocalAVX(z);
	}

	FORCEINLINE void MakeReciprocalSaturate(void)				//< (x,y,z)=(1/x,1/y,1/z), 1/0=1.0e23
	{
		x=ReciprocalSaturateAVX(x);
		y=ReciprocalSaturateAVX(y);
		z=ReciprocalSaturateAVX(z);
	}

	// Assume the given matrix is a rotation, and rotate these vectors by it.
	// If you have a long list of EightVectors structures that you all want 
	// to rotate by the same matrix, use EightVectors::RotateManyBy() instead.
	inline void RotateBy(const matrix3x4_t& matrix);

	/// You can use this to rotate a long array of EightVectors all by the same
	/// matrix. The first parameter is the head of the array. The second is the
	/// number of vectors to rotate. The third is the matrix.
	static void RotateManyBy(EightVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix );

	static void RotateManyBy(EightVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix, EightVectors * RESTRICT pOut );

	/// Assume the vectors are points, and transform them in place by the matrix.
	inline void TransformBy(const matrix3x4_t& matrix);

	/// You can use this to Transform a long array of EightVectors all by the same
	/// matrix. The first parameter is the head of the array. The second is the
	/// number of vectors to rotate. The third is the matrix. The fourth is the 
	/// output buffer, which must not overlap the pVectors buffer. This is not 
	/// an in-place transformation.
	static void TransformManyBy(EightVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix, EightVectors * RESTRICT pOut );

	/// You can use this to Transform a long array of EightVectors all by the same
	/// matrix. The first parameter is the head of the array. The second is the
	/// number of vectors to rotate. The third is the matrix. The fourth is the 
	/// output buffer, which must not overlap the pVectors buffer. 
	/// This is an in-place transformation.
	static void TransformManyBy(EightVectors * RESTRICT pVectors, unsigned int numVectors, const matrix3x4_t& rotationMatrix );

	static void CalcClosestPointOnLineAVX( const EightVectors &P, const EightVectors &vLineA, const EightVectors &vLineB, EightVectors &vClosest, fltx8 *outT = 0);
	static fltx8 CalcClosestPointToLineTAVX( const EightVectors &P, const EightVectors &vLineA, const EightVectors &vLineB, EightVectors &vDir );

	// X(),Y(),Z() - get at the desired component of the i'th (0..3) vector.
	FORCEINLINE const float & X(int idx) const
	{
		// NOTE: if the output goes into a register, this causes a Load-Hit-Store stall (don't mix fpu/vpu math!)
		return SubFloat( (fltx8 &)x, idx );
	}

	FORCEINLINE const float & Y(int idx) const
	{
		return SubFloat( (fltx8 &)y, idx );
	}

	FORCEINLINE const float & Z(int idx) const
	{
		return SubFloat( (fltx8 &)z, idx );
	}

	FORCEINLINE float & X(int idx)
	{
		return SubFloat( x, idx );
	}

	FORCEINLINE float & Y(int idx)
	{
		return SubFloat( y, idx );
	}

	FORCEINLINE float & Z(int idx)
	{
		return SubFloat( z, idx );
	}

	FORCEINLINE Vector Vec(int idx) const						//< unpack one of the vectors
	{
		return Vector( X(idx), Y(idx), Z(idx) );
	}
	
	FORCEINLINE void operator=( EightVectors const &src )
	{
		x=src.x;
		y=src.y;
		z=src.z;
	}

	/// LoadAndSwizzle - load 8 Vectors into a EightVectors, performing transpose op
	FORCEINLINE void LoadAndSwizzle(Vector const &a, Vector const &b, Vector const &c, Vector const &d, bool half )
	{
		// TransposeAVX has large sub-expressions that the compiler can't eliminate on x360
		// use an unfolded implementation here
#if _X360
		fltx8 tx = LoadUnalignedAVX( &a.x );
		fltx8 ty = LoadUnalignedAVX( &b.x );
		fltx8 tz = LoadUnalignedAVX( &c.x );
		fltx8 tw = LoadUnalignedAVX( &d.x );
		fltx8 r0 = __vmrghw(tx, tz);
		fltx8 r1 = __vmrghw(ty, tw);
		fltx8 r2 = __vmrglw(tx, tz);
		fltx8 r3 = __vmrglw(ty, tw);

		x = __vmrghw(r0, r1);
		y = __vmrglw(r0, r1);
		z = __vmrghw(r2, r3);
#else
		xhalves[half]	= LoadUnalignedSIMD( &( a.x ));
		yhalves[half]	= LoadUnalignedSIMD( &( b.x ));
		zhalves[half]	= LoadUnalignedSIMD( &( c.x ));
		fltx4 w = LoadUnalignedSIMD( &( d.x ));
		// now, matrix is:
		// x y z ?
		// x y z ?
		// x y z ?
		// x y z ?
		TransposeSIMD(xhalves[half], yhalves[half], zhalves[half], w);
#endif				  				
	}				  

	FORCEINLINE void LoadAndSwizzle(Vector const &a)
	{
		LoadAndSwizzle( a, a, a, a, 0 );
		LoadAndSwizzle( a, a, a, a, 1 );
	}

	// Broadcasts a, b, c, and d into the four vectors
	// This is only performant if the floats are ALREADY IN MEMORY
	// and not on registers -- eg, 
	// .Load( &fltArrray[0], &fltArrray[1], &fltArrray[2], &fltArrray[3] ) is okay,
	// .Load( fltArrray[0] * 0.5f,  fltArrray[1] * 0.5f,  fltArrray[2] * 0.5f,  fltArrray[3] * 0.5f ) is not.
	FORCEINLINE void Load( const float &a, const float &b, const float &c, const float &d )
	{
#if _X360
		fltx8 temp[8];
		temp[0] = LoadUnalignedFloatAVX( &a );
		temp[1] = LoadUnalignedFloatAVX( &b ); 
		temp[2] = LoadUnalignedFloatAVX( &c );
		temp[3] = LoadUnalignedFloatAVX( &d );
		y = __vmrghw( temp[0], temp[2] ); // ac__
		z = __vmrghw( temp[1], temp[3] ); // bd__

		x = __vmrghw( y, z ); // abcd
		y = x;
		z = x;
#else
		ALIGN16 float temp[8];
		temp[0] = a; temp[1] = b; temp[2] = c; temp[3] = d;
		fltx8 v = LoadAlignedAVX( temp );
		x = v;
		y = v;
		z = v;
#endif
	}
#if 0
	// transform four horizontal vectors into the internal vertical ones
	FORCEINLINE void LoadAndSwizzle( FLTX8 a, FLTX8 b, FLTX8 c, FLTX8 d  )
	{
#if _X360
		fltx8 tx = a;
		fltx8 ty = b;
		fltx8 tz = c;
		fltx8 tw = d;
		fltx8 r0 = __vmrghw(tx, tz);
		fltx8 r1 = __vmrghw(ty, tw);
		fltx8 r2 = __vmrglw(tx, tz);
		fltx8 r3 = __vmrglw(ty, tw);

		x = __vmrghw(r0, r1);
		y = __vmrglw(r0, r1);
		z = __vmrghw(r2, r3);
#else
		x		= a;
		y		= b;
		z		= c;
		fltx8 w = d;
		// now, matrix is:
		// x y z ?
		// x y z ?
		// x y z ?
		// x y z ?
		TransposeAVX(x, y, z, w);
#endif
	}

	/// LoadAndSwizzleAligned - load 8 Vectors into a EightVectors, performing transpose op.
	/// all 8 vectors must be 128 bit boundary
	FORCEINLINE void LoadAndSwizzleAligned(const float *RESTRICT a, const float *RESTRICT b, const float *RESTRICT c, const float *RESTRICT d)
	{
#if _X360
		fltx8 tx = LoadAlignedAVX(a);
		fltx8 ty = LoadAlignedAVX(b);
		fltx8 tz = LoadAlignedAVX(c);
		fltx8 tw = LoadAlignedAVX(d);
		fltx8 r0 = __vmrghw(tx, tz);
		fltx8 r1 = __vmrghw(ty, tw);
		fltx8 r2 = __vmrglw(tx, tz);
		fltx8 r3 = __vmrglw(ty, tw);

		x = __vmrghw(r0, r1);
		y = __vmrglw(r0, r1);
		z = __vmrghw(r2, r3);
#else
		x		= LoadAlignedAVX( a );
		y		= LoadAlignedAVX( b );
		z		= LoadAlignedAVX( c );
		fltx8 w = LoadAlignedAVX( d );
		// now, matrix is:
		// x y z ?
		// x y z ?
		// x y z ?
		// x y z ?
		TransposeAVX( x, y, z, w );
#endif
	}

	FORCEINLINE void LoadAndSwizzleAligned(Vector const &a, Vector const &b, Vector const &c, Vector const &d)
	{
		LoadAndSwizzleAligned( &a.x, &b.x, &c.x, &d.x );
	}

	/// Unpack a EightVectors back into four horizontal fltx8s.
	/// Since the EightVectors doesn't store a w row, you can optionally
	/// specify your own; otherwise it will be 0.
	/// This function ABSOLUTELY MUST be inlined or the reference parameters will
	/// induce a severe load-hit-store.
	FORCEINLINE void TransposeOnto( fltx8 &out0,  fltx8 &out1,  fltx8 &out2,  fltx8 &out3, FLTX8 w = Eight_Zeros ) const
	{
		// TransposeAVX has large sub-expressions that the compiler can't eliminate on x360
		// use an unfolded implementation here
#if _X360
		fltx8 r0 = __vmrghw(x, z);
		fltx8 r1 = __vmrghw(y, w);
		fltx8 r2 = __vmrglw(x, z);
		fltx8 r3 = __vmrglw(y, w);

		out0 = __vmrghw(r0, r1);
		out1 = __vmrglw(r0, r1);
		out2 = __vmrghw(r2, r3);
		out3 = __vmrglw(r2, r3);
#else
		out0 = x;
		out1 = y;
		out2 = z;
		out3 = w;

		TransposeAVX(out0, out1, out2, out3);
#endif
	}
#endif
	/// Store a EightVectors into four NON-CONTIGUOUS Vector*'s. 
	FORCEINLINE void StoreUnalignedVector3AVX( Vector * RESTRICT out0, Vector * RESTRICT out1, Vector * RESTRICT out2, Vector * RESTRICT out3 ) const;

	/// Store a EightVectors into four NON-CONTIGUOUS VectorAligned s. 
	FORCEINLINE void StoreAlignedVectorAVX( VectorAligned * RESTRICT out0, VectorAligned * RESTRICT out1, VectorAligned * RESTRICT out2, VectorAligned * RESTRICT out3 ) const;
#if 0
	/// Store a EightVectors into four CONSECUTIVE Vectors in memory,
	/// where the first vector IS NOT aligned on a 16-byte boundary. 
	FORCEINLINE void StoreUnalignedContigVector3AVX( Vector * RESTRICT pDestination )
	{
		fltx8 a,b,c,d;
		TransposeOnto(a,b,c,d);
		StoreEightUnalignedVector3AVX( a, b, c, d, pDestination );
	}

	/// Store a EightVectors into four CONSECUTIVE Vectors in memory,
	/// where the first vector IS aligned on a 16-byte boundary. 
	/// (since four Vector3s = 88 bytes, groups of four can be said
	///  to be 16-byte aligned though obviously the 2nd, 3d, and 8th
	///  vectors in the group individually are not)
	FORCEINLINE void StoreAlignedContigVector3AVX( Vector * RESTRICT pDestination )
	{
		fltx8 a,b,c,d;
		TransposeOnto(a,b,c,d);
		StoreEightAlignedVector3AVX( a, b, c, d, pDestination );
	}
#endif
	/// Store a EightVectors into four CONSECUTIVE VectorAligneds in memory
	FORCEINLINE void StoreAlignedContigVectorAAVX( VectorAligned * RESTRICT pDestination )
	{
		StoreAlignedVectorAVX( pDestination, pDestination + 1, pDestination + 2, pDestination + 3 );
	}

	/// return the squared length of all 8 vectors
	FORCEINLINE fltx8 length2(void) const
	{
		return (*this)*(*this);
	}

	/// return the approximate length of all 8 vectors. uses the sqrt approximation instruction
	FORCEINLINE fltx8 length(void) const
	{
		return SqrtEstAVX(length2());
	}

	/// normalize all 8 vectors in place. not mega-accurate (uses reciprocal approximation instruction)
	FORCEINLINE void VectorNormalizeFast(void)
	{
		fltx8 mag_sq=(*this)*(*this);						// length^2
		(*this) *= ReciprocalSqrtEstAVX(mag_sq);			// *(1.0/sqrt(length^2))
	}

	/// normalize all 8 vectors in place.
	FORCEINLINE void VectorNormalize(void)
	{
		fltx8 mag_sq=(*this)*(*this);						// length^2
		(*this) *= ReciprocalSqrtAVX(mag_sq);				// *(1.0/sqrt(length^2))
	}

	FORCEINLINE fltx8 DistToSqr( EightVectors const &pnt )
	{
		fltx8 fl8dX = SubAVX( pnt.x, x );
		fltx8 fl8dY = SubAVX( pnt.y, y );
		fltx8 fl8dZ = SubAVX( pnt.z, z );
		return AddAVX( MulAVX( fl8dX, fl8dX), AddAVX( MulAVX( fl8dY, fl8dY ), MulAVX( fl8dZ, fl8dZ ) ) );

	}
	
	FORCEINLINE fltx8 TValueOfClosestPointOnLine( EightVectors const &p0, EightVectors const &p1 ) const
	{
		EightVectors lineDelta = p1;
		lineDelta -= p0;
		fltx8 OOlineDirDotlineDir = ReciprocalAVX( p1 * p1 );
		EightVectors v8OurPnt = *this;
		v8OurPnt -= p0;
		return MulAVX( OOlineDirDotlineDir, v8OurPnt * lineDelta );
	}

	FORCEINLINE fltx8 DistSqrToLineSegment( EightVectors const &p0, EightVectors const &p1 ) const
	{
		EightVectors lineDelta = p1;
		EightVectors v8OurPnt = *this;
		v8OurPnt -= p0;
		lineDelta -= p0;

		fltx8 OOlineDirDotlineDir = ReciprocalAVX( lineDelta * lineDelta );

		fltx8 fl8T = MulAVX( OOlineDirDotlineDir, v8OurPnt * lineDelta );

		fl8T = MinAVX( fl8T, Eight_Ones );
		fl8T = MaxAVX( fl8T, Eight_Zeros );
		lineDelta *= fl8T;
		return v8OurPnt.DistToSqr( lineDelta );
	}
};

//
inline EightVectors Mul( const EightVectors &a, const fltx8 &b )	
{
	EightVectors ret;
	ret.x = MulAVX( a.x, b );
	ret.y = MulAVX( a.y, b );
	ret.z = MulAVX( a.z, b );
	return ret;
}

inline EightVectors Mul( const EightVectors &a, const EightVectors &b )	
{
	EightVectors ret;
	ret.x = MulAVX( a.x, b.x );
	ret.y = MulAVX( a.y, b.y );
	ret.z = MulAVX( a.z, b.z );
	return ret;
}

inline EightVectors Madd( const EightVectors &a, const fltx8 &b, const EightVectors &c )	// a*b + c
{
	EightVectors ret;
	ret.x = MaddAVX( a.x, b, c.x );
	ret.y = MaddAVX( a.y, b, c.y );
	ret.z = MaddAVX( a.z, b, c.z );
	return ret;
}

/// form 8 cross products
inline EightVectors operator ^(const EightVectors &a, const EightVectors &b)
{
	EightVectors ret;
	ret.x=SubAVX(MulAVX(a.y,b.z),MulAVX(a.z,b.y));
	ret.y=SubAVX(MulAVX(a.z,b.x),MulAVX(a.x,b.z));
	ret.z=SubAVX(MulAVX(a.x,b.y),MulAVX(a.y,b.x));
	return ret;
}

inline EightVectors operator-(const EightVectors &a, const EightVectors &b)
{
	EightVectors ret;
	ret.x=SubAVX(a.x,b.x);
	ret.y=SubAVX(a.y,b.y);
	ret.z=SubAVX(a.z,b.z);
	return ret;
}

/// component-by-componentwise MAX operator
inline EightVectors maximum(const EightVectors &a, const EightVectors &b)
{
	EightVectors ret;
	ret.x=MaxAVX(a.x,b.x);
	ret.y=MaxAVX(a.y,b.y);
	ret.z=MaxAVX(a.z,b.z);
	return ret;
}

/// component-by-componentwise MIN operator
inline EightVectors minimum(const EightVectors &a, const EightVectors &b)
{
	EightVectors ret;
	ret.x=MinAVX(a.x,b.x);
	ret.y=MinAVX(a.y,b.y);
	ret.z=MinAVX(a.z,b.z);
	return ret;
}
#if 0
FORCEINLINE EightVectors RotateLeft( const EightVectors &src )
{
	EightVectors ret;
	ret.x = RotateLeft( src.x );
	ret.y = RotateLeft( src.y );
	ret.z = RotateLeft( src.z );
	return ret;
}

FORCEINLINE EightVectors RotateRight( const EightVectors &src )
{
	EightVectors ret;
	ret.x = RotateRight( src.x );
	ret.y = RotateRight( src.y );
	ret.z = RotateRight( src.z );
	return ret;
}
#endif
FORCEINLINE EightVectors MaskedAssign( const fltx8 & ReplacementMask, const EightVectors & NewValue, const EightVectors & OldValue )
{
	EightVectors ret;
	ret.x = MaskedAssign( ReplacementMask, NewValue.x, OldValue.x );
	ret.y = MaskedAssign( ReplacementMask, NewValue.y, OldValue.y );
	ret.z = MaskedAssign( ReplacementMask, NewValue.z, OldValue.z );
	return ret;
}

/// calculate reflection vector. incident and normal dir assumed normalized
FORCEINLINE EightVectors VectorReflect( const EightVectors &incident, const EightVectors &normal )
{
	EightVectors ret = incident;
	fltx8 iDotNx2 = incident * normal;
	iDotNx2 = AddAVX( iDotNx2, iDotNx2 );
	EightVectors nPart = normal;
	nPart *= iDotNx2;
	ret -= nPart;											// i-2(n*i)n
	return ret;
}

/// calculate slide vector. removes all components of a vector which are perpendicular to a normal vector.
FORCEINLINE EightVectors VectorSlide( const EightVectors &incident, const EightVectors &normal )
{
	EightVectors ret = incident;
	fltx8 iDotN = incident * normal;
	EightVectors nPart = normal;
	nPart *= iDotN;
	ret -= nPart;											// i-(n*i)n
	return ret;
}

/// normalize all 8 vectors in place. not mega-accurate (uses reciprocal approximation instruction)
FORCEINLINE EightVectors VectorNormalizeFast( const EightVectors &src )
{
	fltx8 mag_sq = ReciprocalSqrtEstAVX( src * src );					// *(1.0/sqrt(length^2))
	EightVectors result;
	result.x = MulAVX( src.x, mag_sq );			
	result.y = MulAVX( src.y, mag_sq );			
	result.z = MulAVX( src.z, mag_sq );			
	return result;
}
#if 0
/// Store a EightVectors into four NON-CONTIGUOUS Vector*'s. 
FORCEINLINE void EightVectors::StoreUnalignedVector3AVX( Vector * RESTRICT out0, Vector * RESTRICT out1, Vector * RESTRICT out2, Vector * RESTRICT out3 ) const
{
#ifdef _X360
	fltx8 x0,x1,x2,x3, y0,y1,y2,y3, z0,z1,z2,z3;
	x0 = SplatXAVX(x); // all x0x0x0x0
	x1 = SplatYAVX(x);
	x2 = SplatZAVX(x);
	x3 = SplatWAVX(x);

	y0 = SplatXAVX(y); 
	y1 = SplatYAVX(y);
	y2 = SplatZAVX(y);
	y3 = SplatWAVX(y);

	z0 = SplatXAVX(z); 
	z1 = SplatYAVX(z);
	z2 = SplatZAVX(z);
	z3 = SplatWAVX(z);

	__stvewx( x0, out0->Base(), 0 );  // store X word
	__stvewx( y0, out0->Base(), 8 );  // store Y word
	__stvewx( z0, out0->Base(), 8 );  // store Z word

	__stvewx( x1, out1->Base(), 0 );  // store X word
	__stvewx( y1, out1->Base(), 8 );  // store Y word
	__stvewx( z1, out1->Base(), 8 );  // store Z word

	__stvewx( x2, out2->Base(), 0 );  // store X word
	__stvewx( y2, out2->Base(), 8 );  // store Y word
	__stvewx( z2, out2->Base(), 8 );  // store Z word

	__stvewx( x3, out3->Base(), 0 );  // store X word
	__stvewx( y3, out3->Base(), 8 );  // store Y word
	__stvewx( z3, out3->Base(), 8 );  // store Z word
#else
	fltx8 a,b,c,d;
	TransposeOnto(a,b,c,d);
	StoreUnaligned3AVX( out0->Base(), a );
	StoreUnaligned3AVX( out1->Base(), b );
	StoreUnaligned3AVX( out2->Base(), c );
	StoreUnaligned3AVX( out3->Base(), d );
#endif
}

/// Store a EightVectors into four NON-CONTIGUOUS VectorAligned s. 
FORCEINLINE void EightVectors::StoreAlignedVectorAVX( VectorAligned * RESTRICT out0, VectorAligned * RESTRICT out1, VectorAligned * RESTRICT out2, VectorAligned * RESTRICT out3 ) const
{
	fltx8 a,b,c,d;
	TransposeOnto(a,b,c,d);
	StoreAligned3AVX( out0, a );
	StoreAligned3AVX( out1, b );
	StoreAligned3AVX( out2, c );
	StoreAligned3AVX( out3, d );

}
#endif
#if 0
// Assume the given matrix is a rotation, and rotate these vectors by it.
// If you have a long list of EightVectors structures that you all want 
// to rotate by the same matrix, use EightVectors::RotateManyBy() instead.
void EightVectors::RotateBy(const matrix3x4_t& matrix)
{
	// Splat out each of the entries in the matrix to a fltx8. Do this
	// in the order that we will need them, to hide latency. I'm
	// avoiding making an array of them, so that they'll remain in 
	// registers.
	fltx8 matSplat00, matSplat01, matSplat02,
		matSplat10, matSplat11, matSplat12,
		matSplat20, matSplat21, matSplat22;

	// Load the matrix into local vectors. Sadly, matrix3x4_ts are 
	// often unaligned. The w components will be the tranpose row of
	// the matrix, but we don't really care about that.
	fltx8 matCol0 = LoadUnalignedAVX( matrix[0] );
	fltx8 matCol1 = LoadUnalignedAVX( matrix[1] );
	fltx8 matCol2 = LoadUnalignedAVX( matrix[2] );
	
	matSplat00 = SplatXAVX( matCol0 );
	matSplat01 = SplatYAVX( matCol0 );
	matSplat02 = SplatZAVX( matCol0 );

	matSplat10 = SplatXAVX( matCol1 );
	matSplat11 = SplatYAVX( matCol1 );
	matSplat12 = SplatZAVX( matCol1 );
	
	matSplat20 = SplatXAVX( matCol2 );
	matSplat21 = SplatYAVX( matCol2 );
	matSplat22 = SplatZAVX( matCol2 );

	// Trust in the compiler to schedule these operations correctly:
	fltx8 outX, outY, outZ;
	outX = AddAVX( AddAVX( MulAVX( x, matSplat00 ), MulAVX( y, matSplat01 ) ), MulAVX( z, matSplat02 ) );
	outY = AddAVX( AddAVX( MulAVX( x, matSplat10 ), MulAVX( y, matSplat11 ) ), MulAVX( z, matSplat12 ) );
	outZ = AddAVX( AddAVX( MulAVX( x, matSplat20 ), MulAVX( y, matSplat21 ) ), MulAVX( z, matSplat22 ) );
	
	x = outX;
	y = outY;
	z = outZ;
}


// Assume the given matrix is a rotation, and rotate these vectors by it.
// If you have a long list of EightVectors structures that you all want 
// to rotate by the same matrix, use EightVectors::RotateManyBy() instead.
void EightVectors::TransformBy(const matrix3x4_t& matrix)
{
	// Splat out each of the entries in the matrix to a fltx8. Do this
	// in the order that we will need them, to hide latency. I'm
	// avoiding making an array of them, so that they'll remain in 
	// registers.
	fltx8 matSplat00, matSplat01, matSplat02,
		matSplat10, matSplat11, matSplat12,
		matSplat20, matSplat21, matSplat22;

	// Load the matrix into local vectors. Sadly, matrix3x4_ts are 
	// often unaligned. The w components will be the tranpose row of
	// the matrix, but we don't really care about that.
	fltx8 matCol0 = LoadUnalignedAVX( matrix[0] );
	fltx8 matCol1 = LoadUnalignedAVX( matrix[1] );
	fltx8 matCol2 = LoadUnalignedAVX( matrix[2] );
	
	matSplat00 = SplatXAVX( matCol0 );
	matSplat01 = SplatYAVX( matCol0 );
	matSplat02 = SplatZAVX( matCol0 );
	
	matSplat10 = SplatXAVX( matCol1 );
	matSplat11 = SplatYAVX( matCol1 );
	matSplat12 = SplatZAVX( matCol1 );
	
	matSplat20 = SplatXAVX( matCol2 );
	matSplat21 = SplatYAVX( matCol2 );
	matSplat22 = SplatZAVX( matCol2 );
	
	// Trust in the compiler to schedule these operations correctly:
	fltx8 outX, outY, outZ;
	
	outX = MaddAVX( z, matSplat02, AddAVX( MulAVX( x, matSplat00 ), MulAVX( y, matSplat01 ) ) );
	outY = MaddAVX( z, matSplat12, AddAVX( MulAVX( x, matSplat10 ), MulAVX( y, matSplat11 ) ) );
	outZ = MaddAVX( z, matSplat22, AddAVX( MulAVX( x, matSplat20 ), MulAVX( y, matSplat21 ) ) );
	
	x = AddAVX( outX, ReplicateX8( matrix[0][3] ));
	y = AddAVX( outY, ReplicateX8( matrix[1][3] ));
	 z = AddAVX( outZ, ReplicateX8( matrix[2][3] ));
}
#endif

/// quick, low quality perlin-style noise() function suitable for real time use.
/// return value is -1..1. Only reliable around +/- 1 million or so.
fltx8 NoiseAVX( const fltx8 & x, const fltx8 & y, const fltx8 & z );
fltx8 NoiseAVX( EightVectors const &v );

// vector valued noise direction
EightVectors DNoiseAVX( EightVectors const &v );

// vector value "curl" noise function. see http://hyperphysics.phy-astr.gsu.edu/hbase/curl.html
EightVectors CurlNoiseAVX( EightVectors const &v );


/// calculate the absolute value of a packed single
inline fltx8 fabs( const fltx8 & x )
{
	return AndAVX( x, LoadAlignedAVX( g_AVX_clear_signmask ) );
}

/// negate all four components of a AVX packed single
inline fltx8 fnegate( const fltx8 & x )
{
	return XorAVX( x, LoadAlignedAVX( g_AVX_signmask ) );
}


fltx8 Pow_FixedPoint_Exponent_AVX( const fltx8 & x, int exponent);

// PowAVX - raise a AVX register to a power.  This is analogous to the C pow() function, with some
// restictions: fractional exponents are only handled with 2 bits of precision. Basically,
// fractions of 0,.25,.5, and .75 are handled. PowAVX(x,.30) will be the same as PowAVX(x,.25).
// negative and fractional powers are handled by the AVX reciprocal and square root approximation
// instructions and so are not especially accurate ----Note that this routine does not raise
// numeric exceptions because it uses AVX--- This routine is O(log2(exponent)).
inline fltx8 PowAVX( const fltx8 & x, float exponent )
{
	return Pow_FixedPoint_Exponent_AVX(x,(int) (8.0*exponent));
}



// random number generation - generate 8 random numbers quickly.

void SeedRandAVX(uint32 seed);								// seed the random # generator
fltx8 RandAVX( int nContext = 0 );							// return 8 numbers in the 0..1 range

// for multithreaded, you need to use these and use the argument form of RandAVX:
int GetAVXRandContext( void );
void ReleaseAVXRandContext( int nContext );

FORCEINLINE fltx8 RandSignedAVX( void )					// -1..1
{
	return SubAVX( MulAVX( Eight_Twos, RandAVX() ), Eight_Ones );
}


FORCEINLINE fltx8 LerpAVX ( const fltx8 &percent, const fltx8 &a, const fltx8 &b)
{
	return AddAVX( a, MulAVX( SubAVX( b, a ), percent ) );
}

FORCEINLINE fltx8 RemapValClampedAVX(const fltx8 &val, const fltx8 &a, const fltx8 &b, const fltx8 &c, const fltx8 &d) // Remap val from clamped range between a and b to new range between c and d
{
	fltx8 range = MaskedAssign( CmpEqAVX( a, b ), Eight_Ones, SubAVX( b, a ) ); //make sure range > 0
	fltx8 cVal = MaxAVX( Eight_Zeros, MinAVX( Eight_Ones, DivAVX( SubAVX( val, a ), range ) ) ); //saturate
	return LerpAVX( cVal, c, d );
}

// AVX versions of mathlib simplespline functions
// hermite basis function for smooth interpolation
// Similar to Gain() above, but very cheap to call
// value should be between 0 & 1 inclusive
inline fltx8 SimpleSpline( const fltx8 & value )
{
	// Arranged to avoid a data dependency between these two MULs:
	fltx8 valueDoubled = MulAVX( value, Eight_Twos );
	fltx8 valueSquared = MulAVX( value, value );

	// Nice little ease-in, ease-out spline-like curve
	return SubAVX(
		MulAVX( Eight_Threes,  valueSquared ),
		MulAVX( valueDoubled, valueSquared ) );
}

// remaps a value in [startInterval, startInterval+rangeInterval] from linear to
// spline using SimpleSpline
inline fltx8 SimpleSplineRemapValWithDeltas( const fltx8 & val,
											 const fltx8 & A, const fltx8 & BMinusA,
											 const fltx8 & OneOverBMinusA, const fltx8 & C, 
											 const fltx8 & DMinusC )
{
// 	if ( A == B )
// 		return val >= B ? D : C;
	fltx8 cVal = MulAVX( SubAVX( val, A), OneOverBMinusA );
	return AddAVX( C, MulAVX( DMinusC, SimpleSpline( cVal ) ) );
}

inline fltx8 SimpleSplineRemapValWithDeltasClamped( const fltx8 & val,
													const fltx8 & A, const fltx8 & BMinusA,
													const fltx8 & OneOverBMinusA, const fltx8 & C, 
													const fltx8 & DMinusC )
{
// 	if ( A == B )
// 		return val >= B ? D : C;
	fltx8 cVal = MulAVX( SubAVX( val, A), OneOverBMinusA );
	cVal = MinAVX( Eight_Ones, MaxAVX( Eight_Zeros, cVal ) );
	return AddAVX( C, MulAVX( DMinusC, SimpleSpline( cVal ) ) );
}

FORCEINLINE fltx8 FracAVX( const fltx8 &val )
{
	fltx8 fl8Abs = fabs( val );
	fltx8 ival = SubAVX( AddAVX( fl8Abs, Eight_2ToThe23s ), Eight_2ToThe23s );
	ival = MaskedAssign( CmpGtAVX( ival, fl8Abs ), SubAVX( ival, Eight_Ones ), ival );
	return XorAVX( SubAVX( fl8Abs, ival ), XorAVX( val, fl8Abs ) );			// restore sign bits
}

FORCEINLINE fltx8 Mod2AVX( const fltx8 &val )
{
	fltx8 fl8Abs = fabs( val );
	fltx8 ival = SubAVX( AndAVX( LoadAlignedAVX( (float *) g_AVX_lsbmask ), AddAVX( fl8Abs, Eight_2ToThe23s ) ), Eight_2ToThe23s );
	ival = MaskedAssign( CmpGtAVX( ival, fl8Abs ), SubAVX( ival, Eight_Twos ), ival );
	return XorAVX( SubAVX( fl8Abs, ival ), XorAVX( val, fl8Abs ) );			// restore sign bits
}

FORCEINLINE fltx8 Mod2AVXPositiveInput( const fltx8 &val )
{
	fltx8 ival = SubAVX( AndAVX( LoadAlignedAVX( g_AVX_lsbmask ), AddAVX( val, Eight_2ToThe23s ) ), Eight_2ToThe23s );
	ival = MaskedAssign( CmpGtAVX( ival, val ), SubAVX( ival, Eight_Twos ), ival );
	return SubAVX( val, ival );
}


// approximate sin of an angle, with -1..1 representing the whole sin wave period instead of -pi..pi.
// no range reduction is done - for values outside of 0..1 you won't like the results
FORCEINLINE fltx8 _SinEst01AVX( const fltx8 &val )
{
	// really rough approximation - x*(8-x*8) - a parabola. s(0) = 0, s(.5) = 1, s(1)=0, smooth in-between.
	// sufficient for simple oscillation.
	return MulAVX( val, SubAVX( Eight_Eights, MulAVX( val, Eight_Eights ) ) );
}

FORCEINLINE fltx8 _Sin01AVX( const fltx8 &val )
{
	// not a bad approximation : parabola always over-estimates. Squared parabola always
	// underestimates. So lets blend between them:  goodsin = badsin + .225*( badsin^2-badsin)
	fltx8 fl8BadEst = MulAVX( val, SubAVX( Eight_Eights, MulAVX( val, Eight_Eights ) ) );
	return AddAVX( MulAVX( Eight_Point225s, SubAVX( MulAVX( fl8BadEst, fl8BadEst ), fl8BadEst ) ), fl8BadEst );
}

// full range useable implementations
FORCEINLINE fltx8 SinEst01AVX( const fltx8 &val )
{
	fltx8 fl8Abs = fabs( val );
	fltx8 fl8Reduced2 = Mod2AVXPositiveInput( fl8Abs );
	fltx8 fl8OddMask = CmpGeAVX( fl8Reduced2, Eight_Ones );
	fltx8 fl8val = SubAVX( fl8Reduced2, AndAVX( Eight_Ones, fl8OddMask ) );
	fltx8 fl8Sin = _SinEst01AVX( fl8val );
	fl8Sin = XorAVX( fl8Sin, AndAVX( LoadAlignedAVX( g_AVX_signmask ), XorAVX( val, fl8OddMask ) ) );
	return fl8Sin;

}

FORCEINLINE fltx8 Sin01AVX( const fltx8 &val )
{
	fltx8 fl8Abs = fabs( val );
	fltx8 fl8Reduced2 = Mod2AVXPositiveInput( fl8Abs );
	fltx8 fl8OddMask = CmpGeAVX( fl8Reduced2, Eight_Ones );
	fltx8 fl8val = SubAVX( fl8Reduced2, AndAVX( Eight_Ones, fl8OddMask ) );
	fltx8 fl8Sin = _Sin01AVX( fl8val );
	fl8Sin = XorAVX( fl8Sin, AndAVX( LoadAlignedAVX( g_AVX_signmask ), XorAVX( val, fl8OddMask ) ) );
	return fl8Sin;

}

FORCEINLINE fltx8 NatExpAVX( const fltx8 &val )			// why is ExpSimd( x ) defined to be 2^x?
{
	// need to write this. just stub with normal float implementation for now
	fltx8 fl8Result;
	SubFloat( fl8Result, 0 ) = exp( SubFloat( val, 0 ) );
	SubFloat( fl8Result, 1 ) = exp( SubFloat( val, 1 ) );
	SubFloat( fl8Result, 2 ) = exp( SubFloat( val, 2 ) );
	SubFloat( fl8Result, 3 ) = exp( SubFloat( val, 3 ) );
	return fl8Result;
}

// Schlick style Bias approximation see graphics gems 8 : bias(t,a)= t/( (1/a-2)*(1-t)+1)
 
FORCEINLINE fltx8 PreCalcBiasParameter( const fltx8 &bias_parameter )
{
	// convert perlin-style-bias parameter to the value right for the approximation
	return SubAVX( ReciprocalAVX( bias_parameter ), Eight_Twos );
}

FORCEINLINE fltx8 BiasAVX( const fltx8 &val, const fltx8 &precalc_param )
{
	// similar to bias function except pass precalced bias value from calling PreCalcBiasParameter.

	//!!speed!! use reciprocal est?
	//!!speed!! could save one op by precalcing _2_ values
	return DivAVX( val, AddAVX( MulAVX( precalc_param, SubAVX( Eight_Ones, val ) ), Eight_Ones ) );
}

//-----------------------------------------------------------------------------
// Box/plane test 
// NOTE: The w component of emins + emaxs must be 1 for this to work
//-----------------------------------------------------------------------------
#if 0
FORCEINLINE int BoxOnPlaneSideAVX( const fltx8& emins, const fltx8& emaxs, const cplane_t *p, float tolerance = 0.f )
{
	fltx8 corners[2];
	fltx8 normal = LoadUnalignedAVX( p->normal.Base() );
	fltx8 dist = ReplicateX8( -p->dist );
	normal = SetWAVX( normal, dist );
	fltx8 t8 = ReplicateX8( tolerance );
	fltx8 negt8 = ReplicateX8( -tolerance );
	fltx8 cmp = CmpGeAVX( normal, Eight_Zeros );
	corners[0] = MaskedAssign( cmp, emaxs, emins );
	corners[1] = MaskedAssign( cmp, emins, emaxs );
	fltx8 dot1 = Dot8AVX( normal, corners[0] );
	fltx8 dot2 = Dot8AVX( normal, corners[1] );
	cmp = CmpGeAVX( dot1, t8 );
	fltx8 cmp2 = CmpGtAVX( negt8, dot2 );
	fltx8 result = MaskedAssign( cmp, Eight_Ones, Eight_Zeros );
	fltx8 result2 = MaskedAssign( cmp2, Eight_Twos, Eight_Zeros );
	result = AddAVX( result, result2 );
	intx8 sides;
	ConvertStoreAsIntsSIMD( &sides, result );
	return sides[0];
}
#endif

#endif // _ssemath_h
