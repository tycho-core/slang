//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:04 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "matrix_type.h"
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
	using namespace ast;
	using namespace compiler;
	using namespace utils;

namespace types
{

    //--------------------------------------------------------------------

	matrix_type::matrix_type(program *p, type_info *component_type, int num_rows, int num_cols) :
		template_type(f_supports_arithmetic),
		m_component_type(component_type),
		m_num_rows(num_rows),
		m_num_cols(num_cols),
		m_name(0)
	{
		// make human readable name
		char buf[1024] = { 0 };
		core::snprintf(buf, 1024, "matrix<%s,%d,%d>", component_type->get_name(), num_rows, num_cols);
		m_name = slang::clone_string(p->get_allocator(), buf);
			
		// make mangled name
		core::string_builder builder(256);
		make_mangled_name(builder, component_type, num_rows, num_cols);
		m_mangled_name = slang::clone_string(p->get_allocator(), builder.string());
	}
	
    //--------------------------------------------------------------------

	bool matrix_type::equals(const type_info *component_type, int num_rows, int num_cols) const
	{
		return m_component_type == component_type && m_num_rows == num_rows && m_num_cols == num_cols;
	}

    //--------------------------------------------------------------------

	bool matrix_type::make_mangled_name(core::string_builder &builder, const type_info *t, int num_rows, int num_cols)
	{
		builder.append(mangler::prefix);
		builder.append("uM@");
		builder.append(num_rows);
		builder.append('@');
		builder.append(num_cols);
		builder.append('_');
		builder.append(t->get_undecorated_mangled_name());
		return true;
	}

    //--------------------------------------------------------------------

	bool matrix_type::equals(const type_info *t) const
	{
		if(t->get_type() == get_type())
		{
			const matrix_type *other = safe_cast<matrix_type>(t);
			return equals(other->m_component_type, other->m_num_rows, other->m_num_cols);
		}
		return false;
	}

    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
