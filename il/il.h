//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:29:25 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __IL_H_BDC61880_F105_4C38_AA39_B3793276E0BD_
#define __IL_H_BDC61880_F105_4C38_AA39_B3793276E0BD_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "core/types.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace il
{

	///////////////////////////////////////////////////////////////////////////
	// Types
	///////////////////////////////////////////////////////////////////////////
	
	enum class types : tycho::core::int8
	{
		// misc types
		string,
		pixelshader,
		vertexshader,
		sampler,
		function,
		complex,	

		// float types
		float1x1, float1x2, float1x3, float1x4,
		float2x1, float2x2, float2x3, float2x4,
		float3x1, float3x2, float3x3, float3x4,
		float4x1, float4x2, float4x3, float4x4,

		// double types
		double1x1, double1x2, double1x3, double1x4,
		double2x1, double2x2, double2x3, double2x4,
		double3x1, double3x2, double3x3, double3x4,
		double4x1, double4x2, double4x3, double4x4,

		// half types
		half1x1, half1x2, half1x3, half1x4,
		half2x1, half2x2, half2x3, half2x4,
		half3x1, half3x2, half3x3, half3x4,
		half4x1, half4x2, half4x3, half4x4,

		// int types
		int1x1, int1x2, int1x3, int1x4,
		int2x1, int2x2, int2x3, int2x4,
		int3x1, int3x2, int3x3, int3x4,
		int4x1, int4x2, int4x3, int4x4,

		// uint types
		uint1x1, uint1x2, uint1x3, uint1x4,
		uint2x1, uint2x2, uint2x3, uint2x4,
		uint3x1, uint3x2, uint3x3, uint3x4,
		uint4x1, uint4x2, uint4x3, uint4x4,

		// bool types
		bool1x1, bool1x2, bool1x3, bool1x4,
		bool2x1, bool2x2, bool2x3, bool2x4,
		bool3x1, bool3x2, bool3x3, bool3x4,
		bool4x1, bool4x2, bool4x3, bool4x4,
	};

	///////////////////////////////////////////////////////////////////////////
	// Components and swizzling
	///////////////////////////////////////////////////////////////////////////

	enum class component : tycho::core::int8
	{
		invalid = 0,

		x = 1, 
		y = 2,
		z = 3,
		w = 4,

		r = x,
		g = y,
		b = z,
		a = w
	};
	
	typedef core::uint32 swizzle_t;

	const int SwizzleMask0 = 0x000000ff;
	const int SwizzleMask1 = 0x0000ff00;
	const int SwizzleMask2 = 0x00ff0000;
	const int SwizzleMask3 = 0xff000000;

	const int SwizzleShift0 = 0;
	const int SwizzleShift1 = 8;
	const int SwizzleShift2 = 16;
	const int SwizzleShift3 = 24;

	const int SwizzleShifts[] = { SwizzleShift0, SwizzleShift1, SwizzleShift2, SwizzleShift3 };
	const int SwizzleMasks[] = { SwizzleMask0, SwizzleMask1, SwizzleMask2, SwizzleMask3 };

	/// Construct a swizzle with each component
	constexpr swizzle_t make_swizzle(component c0, component c1, component c2, component c3)
	{
		return	((swizzle_t)c0) | 
				((swizzle_t)c1 << SwizzleShift1) | 
				((swizzle_t)c2 << SwizzleShift2) | 
				((swizzle_t)c3 & SwizzleShift3);
	}

	/// Get the swizzle component at the specified position
	component get_component(swizzle_t s, const int p)
	{
		return (component)((s & SwizzleMasks[p]) >> SwizzleShifts[p]);
	}

	///////////////////////////////////////////////////////////////////////////
	// Useful swizzles
	///////////////////////////////////////////////////////////////////////////
	enum class swizzle : core::uint32
	{
		xyzw = make_swizzle(component::x, component::y, component::z, component::w),
		xxxx = make_swizzle(component::x, component::x, component::x, component::x),
		yyyy = make_swizzle(component::y, component::y, component::y, component::y),
		zzzz = make_swizzle(component::z, component::z, component::z, component::z),
		wwww = make_swizzle(component::w, component::w, component::w, component::w),

		rgba = make_swizzle(component::r, component::g, component::b, component::a),
		rrrr = make_swizzle(component::r, component::r, component::r, component::r),
		gggg = make_swizzle(component::g, component::g, component::g, component::g),
		bbbb = make_swizzle(component::b, component::b, component::b, component::b),
		aaaa = make_swizzle(component::a, component::a, component::a, component::a),
	};

	///////////////////////////////////////////////////////////////////////////
	// Opcodes
	///////////////////////////////////////////////////////////////////////////

	enum class opcode : core::uint16
	{
		invalid = 0,

		// arithmetic
		aadd,
		asub,
		amul,
		adiv,
		amod,

		// special
		sin,
		tan,
		cos,
		atan,
		asin,
		acos,

		// logical
		land,
		lor,
		lxor,
		lnot,

		// control flow
		cif,
		cwhile,
		cdo,
		ccall,

		// comparison
		cmp_eq,
		cmp_gt,
		cmp_lt,
		cmp_geq,
		cmp_leq


	};

	///////////////////////////////////////////////////////////////////////////
	// Instructions
	///////////////////////////////////////////////////////////////////////////

	struct target
	{
		core::uint16 pool;
		core::uint16 reg;
	};

	struct operand
	{
		target	  reg;
		swizzle_t swizzle;
	};

	struct instruction
	{
		opcode 	op;
		operand dst;
		operand src1;
		operand src2;
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __IL_H_BDC61880_F105_4C38_AA39_B3793276E0BD_
