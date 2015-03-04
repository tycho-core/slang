//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:08 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __SAMPLER_TYPES_H_C484C9AC_2A33_41C4_ABC4_29DE7FFCE0AC_
#define __SAMPLER_TYPES_H_C484C9AC_2A33_41C4_ABC4_29DE7FFCE0AC_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/types/type_system.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace types
{
   
	/// texture sampler
	struct TYCHO_SLANG_ABI sampler_type : object_type
	{
		sampler_type() : object_type("sampler") {}
	};
	
	/// 1d texture sampler
	struct TYCHO_SLANG_ABI sampler_1d_type : object_type
	{
		sampler_1d_type() : object_type("sampler1d") {}
		virtual type get_type() const { return t_sampler1d; }
	};

	/// 2d texture sampler
	struct TYCHO_SLANG_ABI sampler_2d_type : object_type
	{
		sampler_2d_type() : object_type("sampler2d") {}
		virtual type get_type() const { return t_sampler2d; }
	};
	/// 3d texture sampler
	struct TYCHO_SLANG_ABI sampler_3d_type : object_type
	{
		sampler_3d_type() : object_type("sampler3d") {}
		virtual type get_type() const { return t_sampler3d; }
	};
	/// cube texture sampler
	struct TYCHO_SLANG_ABI sampler_cube_type : object_type
	{
		sampler_cube_type() : object_type("samplerCUBE") {}
		virtual type get_type() const { return t_sampler_cube; }
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __SAMPLER_TYPES_H_C484C9AC_2A33_41C4_ABC4_29DE7FFCE0AC_
