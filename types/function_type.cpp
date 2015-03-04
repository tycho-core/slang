//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:01 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "function_type.h"
#include "slang/compiler/mangle.h"
#include "core/string_builder.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
	using namespace compiler;
	using namespace utils;

namespace types
{

	//--------------------------------------------------------------------

	function_type::function_type(allocator *a, const type_info *return_type, param_list *params) :
		m_allocator(a),
        m_return_type(return_type),
		m_name(0),
		m_mangled_name(0),
		m_params(params)
	{
		// human readable name
		core::string_builder builder(compiler::max_mangled_name_length);
		builder.append(return_type->get_name());
		builder.append(" (");
		bool add_comma = false;
		for(param_list::iterator it = params->begin(); it != params->end(); ++it)
		{
			if(add_comma)
				builder.append(',');					
			builder.append((*it)->m_type->get_name());
			add_comma = true;
		}
		builder.append(")");
		m_name = slang::clone_string(m_allocator, builder.string());
		
	
		// mangled name
		builder.clear();
		if(make_mangled_name(builder, return_type, params))
		{
			m_mangled_name = slang::clone_string(a, builder.string());
		}
		TYCHO_ASSERT(m_mangled_name);
	}
		
	
	//--------------------------------------------------------------------

	int function_type::num_params() const
	{
		if(m_params)
		{
			return (int)m_params->size();
		}
		return 0;
	}

	//--------------------------------------------------------------------

	bool function_type::make_mangled_name(core::string_builder &builder, const type_info * /*return_type*/, const param_list *params)
	{
		builder.append(mangler::prefix);
		builder.append(mangler::function_type);
		if(params)
		{
			for(param_list::const_iterator it = params->begin(); it != params->end(); ++it)
			{
				const param *p = *it;
				if(p && p->m_type)
				{
					builder.append((*it)->m_type->get_undecorated_mangled_name());
				}
				else
				{
					return false;
				}
			}
		}
		
		return true;
	}
	
	//--------------------------------------------------------------------

	bool mangle_function_name(core::string_builder &builder, const char *name, 
							  const type_info * /*return_type*/, const function_type::param_list *params)
	{
		builder.append((int)core::strlen(name));
		builder.append(name);
		if(params)
		{
			for(function_type::param_list::const_iterator it = params->begin(); it != params->end(); ++it)
			{
				const function_type::param *p = *it;
				if(p && p->m_type)
				{
					builder.append((*it)->m_type->get_undecorated_mangled_name());
				}
				else
				{
					return false;
				}
			}
		}
		
		return true;		
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
