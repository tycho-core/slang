//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:34:17 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace compiler
{

    //--------------------------------------------------------------------

	const char *get_language_as_string(language_t lang)
	{
		switch(lang)
		{
			case lang_hlsl_dx9	: return "Hlsl dx 9.0";
			case lang_hlsl_dx10 : return "Hlsl dx 10.0";
			case lang_v			: return "v";
			case lang_cg		: return "cg";
		}
		
		return "<unknown>";
	}

    //--------------------------------------------------------------------

	const char* get_usage_string(usage_t u)
	{
		switch(u)
		{
			// parameter usage
			case usage_param_in			: return "usage_param_in";
			case usage_param_out			: return "usage_param_out";
			case usage_param_inout		: return "usage_param_inout";
			case usage_param_uniform		: return "usage_param_uniform";
			
			// misc usages
			case usage_static			: return "usage_static";
			case usage_uniform			: return "usage_uniform";
			case usage_volatile			: return "usage_volatile";
			case usage_inline			: return "usage_inline";
			case usage_shared			: return "usage_shared";
			case usage_target			: return "usage_target";
			case usage_extern			: return "usage_extern";
			
			// const usages 
			case usage_const_const			: return "usage_const_const";
			case usage_const_row_major		: return "usage_const_row_major";
			case usage_const_column_major	: return "usage_const_column_major";
                
            default: TYCHO_NOT_IMPLEMENTED; break;
		}
		
		return "<invalid>";
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
