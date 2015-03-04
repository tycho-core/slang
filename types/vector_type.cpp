//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:30 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "vector_type.h"
#include "slang/compiler/symbol_table.h"
#include "slang/compiler/program.h"
#include "core/string_builder.h"
#include "core/string.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace types
{
	using namespace utils;
	using namespace compiler;

    //--------------------------------------------------------------------

	vector_type::vector_type(program *p, builtin_type *component_type, int size) :
		template_type(f_supports_arithmetic),
		m_component_type(component_type),
		m_size(size),
		m_name(0),
		m_mangled_name(0)
	{	
		// make human readable name
		char buf[1024] = { 0 };
		core::snprintf(buf, 1024, "vector<%s,%d>", component_type->get_name(), size);
		m_name = slang::clone_string(p->get_allocator(), buf);				
					
		// make mangled named
		core::string_builder builder(256);
		make_mangled_name(builder, component_type, size);
		m_mangled_name = slang::clone_string(p->get_allocator(), builder.string());
	}
	
    //--------------------------------------------------------------------

	bool vector_type::equals(const type_info *component_type, int size) const
	{
		return m_component_type == component_type && m_size == size;
	}


    //--------------------------------------------------------------------

	const type_info* vector_type::validate_accessor(symbol_table &symbols, const char *accessor) const
	{ 
		size_t len = core::strlen(accessor);
		if((int)len > m_size)
            return nullptr;
		
		const char *p = accessor;
		size_t num = 0;
		while(*p)
		{			
			char c = 0;				
			switch(*p)
			{
				case 'x' : c = 0; break;
				case 'y' : c = 1; break;
				case 'z' : c = 2; break;
				case 'w' : c = 3; break;
				case 'r' : c = 0; break;
				case 'g' : c = 1; break;
				case 'b' : c = 2; break;
				case 'a' : c = 3; break;
				
				default:
                    return nullptr;
			}
			
			if(c < 0 || c > 3)
                return nullptr;
			if(c > m_size)
                return nullptr;
			++num;
			++p;
		}											
		
		return symbols.instantiate_vector_type(m_component_type, (int)num); 
	}		

    //--------------------------------------------------------------------

	const char* vector_type::get_mangled_name() const
	{
		return m_mangled_name;	
	}

    //--------------------------------------------------------------------

	bool vector_type::equals(const type_info *t) const
	{
		if(t->get_type() == get_type())
		{
			const vector_type *other = safe_cast<vector_type>(t);
			return equals(other->m_component_type, other->m_size);
		}
		return false;
	}

    //--------------------------------------------------------------------
	
	bool vector_type::make_mangled_name(core::string_builder &builder, const type_info *t, int size)
	{
		builder.append(mangler::prefix);
		builder.append("uV@");
		builder.append(size);
		builder.append('_');
		builder.append(t->get_undecorated_mangled_name());
		return true;
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
