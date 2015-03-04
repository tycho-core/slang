//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:33 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __INTRINSICS_H_8669FD6F_7BFB_4611_9955_2FD37F67D25B_
#define __INTRINSICS_H_8669FD6F_7BFB_4611_9955_2FD37F67D25B_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/compiler/slang.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace compiler
{

	// intrinsic function definitions, these are parameterised over a variety of types
	// but these just refer to the 'family' of types they support and calls are checked
	// against this during the type checking phase.
	
	enum template_type_t
	{
		tt_none		= 0,
		tt_scalar	= (1 << 0),
		tt_vector	= (1 << 1),
		tt_matrix	= (1 << 2),
		tt_sampler	= (1 << 3),
		
		// convenient definitions			
		tt_numeric  = tt_scalar | tt_vector | tt_matrix	
	};
	typedef int template_types_t;
	
	
	/// basic component types
	enum component_type_t
	{
		ct_none		= 0,
		ct_half		= (1 << 1),
		ct_float	= (1 << 2),
		ct_double	= (1 << 3),
		ct_int		= (1 << 4),
		ct_uint		= (1 << 5),
		ct_bool		= (1 << 6),
		
		// convenient definitions
		ct_floating = ct_half | ct_float | ct_double,
		ct_integral = ct_int | ct_uint,
		ct_numeric  = ct_floating | ct_integral,
		ct_all		= ct_numeric | ct_bool
	};
	typedef int component_types_t;
	
	
	/// basic type
	struct basic_type
	{
		template_type_t  template_type;
		component_type_t component_type;
		int				 size;	
	};
	
	/// 
	enum intrinsic_return_type
	{
		irt_void,		 ///< function does not return anything
		irt_param1_type, ///< return type is the same as the type of the first parameter
		irt_fixed_type,  ///< return type is fixed
		irt_param1_component_type, ///< return type is specified except for the component type which comes from parameter 1
		irt_param1_template_type,  ///< return type is specified except for the template type which comes from parameter 1
	};
	
	/// parameter information
	struct intrinsic_parameter
	{
		const char*		 name;
		bool			 out_param;	
		template_types_t	 template_types;   ///< template types allowed for the parameter
		component_types_t  component_types;  ///< component types allowed for the template
		int			     size;			   ///< number of components allowed, 0 for any number
	};
			
	/// intrinsic function definition
	struct intrinsic_function
	{		
		const char*				name;				///< name of the function
		languages				language;			///< language versions that support this intrinsic
		intrinsic_return_type   return_type_spec;	///< how the return type is evaluated
		basic_type				return_type;		///< if return_type_spec is irt_fixed_type then this is that type
		int						num_params;			///< number of parameters the function take
		intrinsic_parameter 	parameters[4];		///< parameter definitions, max 4, increase if necessary
	};
	

	/// lookup an intrinsic function definition by name
	TYCHO_SLANG_ABI const intrinsic_function* find_intrinsic(const char *);
	
} // end namespace
} // end namespace
} // end namespace

#endif // __INTRINSICS_H_8669FD6F_7BFB_4611_9955_2FD37F67D25B_
