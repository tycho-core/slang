//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:18 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __TEXTURE_TYPES_H_39B5FC95_7740_4856_AF7F_0E7FB13EEB5F_
#define __TEXTURE_TYPES_H_39B5FC95_7740_4856_AF7F_0E7FB13EEB5F_

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

	/// texture 
	struct TYCHO_SLANG_ABI texture_type : object_type
	{
		texture_type() : object_type("texture") {}
	};

	/// 1d texture
	struct TYCHO_SLANG_ABI texture_1d_type : object_type
	{
		texture_1d_type() : object_type("texture1d") {}
		virtual type get_type() const { return t_texture1d; }
	};

	/// 2d texture
	struct TYCHO_SLANG_ABI texture_2d_type : object_type
	{
		texture_2d_type() : object_type("texture2d") {}
		virtual type get_type() const { return t_texture2d; }
	};
	/// 3d texture
	struct TYCHO_SLANG_ABI texture_3d_type : object_type
	{
		texture_3d_type() : object_type("texture3d") {}
		virtual type get_type() const { return t_texture3d; }
	};
	/// cube texture 		
	struct TYCHO_SLANG_ABI texture_cube_type : object_type
	{
		texture_cube_type() : object_type("textureCUBE") {}
		virtual type get_type() const { return t_texture_cube; }
	};
				
} // end namespace
} // end namespace
} // end namespace

#endif // __TEXTURE_TYPES_H_39B5FC95_7740_4856_AF7F_0E7FB13EEB5F_
