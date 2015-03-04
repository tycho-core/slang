//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:12 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "structure_type.h"
#include "slang/compiler/mangle.h"
#include "slang/compiler/slang.h"
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

	structure_type::structure_type(allocator *a, const char *name) :
		m_allocator(a),
		m_name(name),
		m_mangled_name(0)
	{
		core::string_builder builder(compiler::max_mangled_name_length);
		builder.append(mangler::prefix);
		builder.append((int)core::strlen(m_name));
		builder.append(name);
		m_mangled_name = slang::clone_string(a, builder.string());
	}
		
    //--------------------------------------------------------------------

	void structure_type::add_member(const char *name, const type_info *t)
	{
		member *m = HLSL_ALLOC(m_allocator, member);
		m->m_name = name;
		m->m_type = t;
		m_members.push_back(m);			
	}
	
    //--------------------------------------------------------------------

	const char* structure_type::get_name() const 
	{ 
		return m_name; 
	}

    //--------------------------------------------------------------------

	const char* structure_type::get_mangled_name() const 
	{ 
		return m_mangled_name; 
	}

    //--------------------------------------------------------------------

	const type_info* structure_type::validate_accessor(symbol_table &, const char *name) const
	{
		list<member>::const_iterator it = m_members.begin();
		list<member>::const_iterator end = m_members.end();
		
		while(it != end)
		{
			if(core::strcmp((*it)->m_name, name) == 0)
				return (*it)->m_type;
			++it;
		}
		return 0;	
	}
	
    //--------------------------------------------------------------------

	int structure_type::get_byte_size() const
	{
		int size = 0;
		list<member>::const_iterator it = m_members.begin();
		list<member>::const_iterator end = m_members.end();
		
		while(it != end)
		{
			//TODO: Account for alignment
			size += (*it)->m_type->get_byte_size();
			++it;
		}
		return size;
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
