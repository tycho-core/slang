//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:34:17 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __SLANG_H_E7EE4C22_E1B1_4C85_B3BB_88368C3E230F_
#define __SLANG_H_E7EE4C22_E1B1_4C85_B3BB_88368C3E230F_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace compiler
{

	/// input source language
	enum language_t
	{
		lang_hlsl_dx9	= (1 << 0),	
		lang_hlsl_dx10	= (1 << 1),
		lang_cg			= (1 << 2),
		lang_v			= (1 << 3)
	};		
	typedef int languages;
	
	/// \returns language string suitable for presentation
	const char *get_language_as_string(language_t);

	/// optimisations
	enum optimisation_t
	{
		opt_remove_unused_inputs  = (1 << 0),
		opt_remove_unused_outputs = (1 << 1),
		opt_inline_functions	  = (1 << 2)
	};		
	typedef int optimisations;

	/// compile flags
	enum compiler_flag_t
	{
		comp_trace_lexer	= (1 << 0),
		comp_trace_parser	= (1 << 1),
		comp_trace_ast		= (1 << 2),
		comp_trace_symbols	= (1 << 3)
	};
	typedef int compiler_flags;
	
	/// semantic type
	//TODO : this is going to need to be made per language	
	enum semantic_t
	{
		semantic_none
	};

	/// usage
	enum usage_t
	{
		usage_none = 0,
		
		// parameter usage
		usage_param_in			= (1 << 1),
		usage_param_out			= (1 << 2),
		usage_param_inout		= (1 << 3),
		usage_param_uniform		= (1 << 4),
		
		// misc usages
		usage_static			= (1 << 5),
		usage_uniform			= (1 << 6),
		usage_volatile			= (1 << 7),
		usage_inline			= (1 << 8),
		usage_shared			= (1 << 9),
		usage_target			= (1 << 10),
		usage_extern			= (1 << 11),
		
		// const usages 
		usage_const_const			= (1 << 12),
		usage_const_row_major		= (1 << 13),
		usage_const_column_major	= (1 << 14)
	};

	const char* get_usage_string(usage_t u);

	inline usage_t operator|(usage_t lhs, usage_t rhs)
	{
		return (usage_t)((int)lhs | (int)rhs);
	}
	
	inline usage_t& operator|=(usage_t &lhs, usage_t rhs)
	{
		lhs = (usage_t)((int)lhs | (int)rhs);
		return lhs;
	}
	
	// constants
	const int max_mangled_name_length = 256;
	const int max_parameters = 128;

} // end namespace
} // end namespace
} // end namespace

#endif // __SLANG_H_E7EE4C22_E1B1_4C85_B3BB_88368C3E230F_
