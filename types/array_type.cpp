//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:54 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "array_type.h"
#include "slang/compiler/program.h"
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

	array_type::array_type(program *p, const type_info *t, int *dimensions, int num_dimensions) :
		m_type(t),
		m_num_dimensions(num_dimensions),
		m_name(0),
		m_mangled_name(0)
	{		
		TYCHO_ASSERT(num_dimensions);	
		m_dimensions = HLSL_ALLOC_ARRAY(p->get_allocator(), int, num_dimensions);			
		m_byte_size = m_type->get_byte_size();
		for(int i = 0; i < num_dimensions; ++i)
		{
			TYCHO_ASSERT(dimensions[i] > 0);
			m_byte_size *= dimensions[i];
			m_dimensions[i] = dimensions[1];
		}				
		
		
		// human readable name
		char buffer[1024];
		core::strcpy(buffer, t->get_name());
		for(int i = 0; i < num_dimensions; ++i)
		{
			char tb[64];
			core::snprintf(tb, 64, "[%d]", dimensions[i]);
			core::strcat(buffer, tb);
		}
		m_name = slang::clone_string(p->get_allocator(), buffer);
		
		// mangled name
		core::string_builder builder(256);
		make_mangled_name(builder, t, dimensions, num_dimensions);
		m_mangled_name = slang::clone_string(p->get_allocator(), builder.string());
	}

    //--------------------------------------------------------------------
	
	const char* array_type::get_name() const
	{
		return m_name;
	}

    //--------------------------------------------------------------------
	
	const char* array_type::get_mangled_name() const
	{
		return m_mangled_name;
	}
	
    //--------------------------------------------------------------------
	
	int array_type::get_byte_size() const
	{ 
		return m_byte_size; 
	}

    //--------------------------------------------------------------------

	const type_info* array_type::validate_array_access(program *p, const int* dimensions, int num_dimensions) const
	{ 
		if(!dimensions)
		{
			return (num_dimensions == m_num_dimensions) ? m_type : 0;
		}
		if(num_dimensions < 1 || num_dimensions != m_num_dimensions)
		{				
			p->add_message(mt_invalid_number_of_array_dimensions, p->get_current_token().pos/*, num_dimensions*/);
            return nullptr;
		}
		
		for(int i = 0; i < num_dimensions; ++i)
		{
			if(dimensions[i] < 0 || dimensions[i] > m_dimensions[i]-1)	
                return nullptr;
		}				
		return m_type;
	}


    //--------------------------------------------------------------------
				
	bool array_type::make_mangled_name(core::string_builder &builder, const type_info *t, int *dimensions, int num_dimensions)
	{
		builder.append(mangler::prefix);
		for(int i = 0; i < num_dimensions; ++i)
		{
			builder.append(mangler::array_type);
			builder.append(dimensions[i]);
			builder.append('_');
		}
		builder.append(t->get_undecorated_mangled_name());

		return true;
	}

    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
