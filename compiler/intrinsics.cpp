//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:33 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "intrinsics.h"
#include "core/string.h"

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

	namespace detail
	{
		/// intrinsic function definitions
		static intrinsic_function functions[] = 
		{
			{ "abs", lang_hlsl_dx9 | lang_hlsl_dx10, 
				irt_param1_type, { }, 1, 
				{ {"x", false, tt_numeric, ct_numeric, 0}}
			},
			{ "acos", lang_hlsl_dx9 | lang_hlsl_dx10, 
				irt_param1_type, { }, 1, 
				{ {"x", false, tt_numeric, ct_float, 0}}
			},
			{ "all", lang_hlsl_dx9 | lang_hlsl_dx10, 
				irt_fixed_type, { tt_scalar, ct_bool, 1 }, 1, 
				{ {"x", false, tt_numeric, ct_all, 0}}
			},
			{ "max", lang_hlsl_dx9 | lang_hlsl_dx10,
				irt_param1_type, { }, 2,
				{
					{ "x", false, tt_numeric, ct_float | ct_int, 0 },
					{ "y", false, tt_none, ct_none, 0 } // same type as parameter 1
				}
			},
			{ "dot", lang_hlsl_dx9 | lang_hlsl_dx10,
				irt_param1_component_type, { tt_scalar, ct_none, 1 }, 2,
				{
					{ "x", false, tt_numeric, ct_float | ct_int, 0 },
					{ "y", false, tt_none, ct_none, 0 } // same type as parameter 1
				}
			},
			{ "normalize", lang_hlsl_dx9 | lang_hlsl_dx10, 
				irt_param1_template_type, { tt_none, ct_float, 0 }, 1, 
				{ {"x", false, tt_vector, ct_float, 0}}
			},
			//TODO: mul() is complicated, it has a whole bunch of classical overloads that means
			// we either need to introduce full overload resolution or hack it in somewhere for 
			// this cases. We already need full overload resolution for other functions anyway.
			{ "mul", lang_hlsl_dx9 | lang_hlsl_dx10, 
				irt_param1_template_type, { tt_none, ct_float, 0 }, 1, 
				{ {"x", false, tt_vector, ct_float, 0}}
			},
			{ "sin", lang_hlsl_dx9 | lang_hlsl_dx10, 
				irt_param1_template_type, { tt_none, ct_float, 0 }, 1, 
				{ {"x", false, tt_numeric, ct_float, 0}}
			},
			// terminator
			{ 0 }
		};
	}

    //--------------------------------------------------------------------

	//TODO: optimise, O(N) currently
	TYCHO_SLANG_ABI const intrinsic_function* find_intrinsic(const char *name)
	{
		const intrinsic_function *fn = detail::functions;
		while(fn->name)
		{
			if(core::strcmp(fn->name, name) == 0)
				return fn;
			++fn;
		}
		return 0;
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
