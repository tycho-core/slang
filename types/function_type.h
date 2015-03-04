//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:01 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __FUNCTION_TYPE_H_25584906_C42B_4E6A_BC78_935D99EEB91F_
#define __FUNCTION_TYPE_H_25584906_C42B_4E6A_BC78_935D99EEB91F_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/compiler/slang.h"
#include "slang/types/type_system.h"
#include "slang/utils/list.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace types
{

	/// function
	class TYCHO_SLANG_ABI function_type : public type_info
	{
	public:
		struct param : utils::intrusive_list<param>
		{
			const type_info*	m_type;
			compiler::usage_t		m_usage;
			compiler::semantic_t	m_semantic;
		};
		
		typedef utils::list<param> param_list;
	
	public:
		/// constructor
		function_type(allocator *a, const type_info *return_type, param_list *params);
		
		/// \returns the number of parameters the function takes
		int num_params() const;
		
		/// \returns the parameter list
		const param_list* get_params() const
			{ return m_params; }
			
		/// \return the return type of the function
		const type_info* return_type() const 
			{ return m_return_type; }
		
		/// make the mangled string version for the passed function type parameters
		/// format is : 
		static bool make_mangled_name(core::string_builder &, const type_info *return_type, const param_list *params);
					
		// type_info interface
					
		virtual type get_type() const 
			{ return t_function; }

		virtual const char* get_name() const
			{ return m_name; }
			
		virtual const char* get_mangled_name() const
			{ return m_mangled_name; }
						
	private:			
		allocator*			m_allocator;
		const type_info*	m_return_type;
		const char*			m_name;
		const char*			m_mangled_name;
		param_list*			m_params;
	};
	
	//TODO : should live elsewhere
	bool mangle_function_name(core::string_builder &builder, const char *name, 
							  const type_info *return_type, const function_type::param_list *params);
	
	/// intrinsic function
	class intrinisic_function_type : public type_info
	{
	private:
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __FUNCTION_TYPE_H_25584906_C42B_4E6A_BC78_935D99EEB91F_
