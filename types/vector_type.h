//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:30 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __VECTOR_TYPE_H_4FED47AE_DC71_4F7C_80DF_494137DA6AB2_
#define __VECTOR_TYPE_H_4FED47AE_DC71_4F7C_80DF_494137DA6AB2_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/types/type_system.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace types
{

	/// vector<T, n>
	class TYCHO_SLANG_ABI vector_type : public template_type
	{
	public:
		vector_type(compiler::program *p, builtin_type *component_type, int size);
		bool equals(const type_info *component_type, int size) const;

		/// type_info interface			
		virtual type get_type() const { return t_vector; }			
		virtual int get_byte_size() const  { return m_component_type->get_byte_size() * m_size; }
		virtual const char* get_name() const { return m_name; }
		virtual const char* get_mangled_name() const;				
		virtual const type_info* validate_accessor(compiler::symbol_table &, const char *accessor) const;
		virtual bool equals(const type_info *) const;

		/// \returns mangled name for passed vector parameters
		static bool make_mangled_name(core::string_builder &builder, const type_info *t, int size);
					
	private:
		type_info*	m_component_type;
		int			m_size;
		const char* m_name;
		const char* m_mangled_name;
	};
	
} // end namespace
} // end namespace
} // end namespace

#endif // __VECTOR_TYPE_H_4FED47AE_DC71_4F7C_80DF_494137DA6AB2_
