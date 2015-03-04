//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:04 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __MATRIX_TYPE_H_BA30BC86_4438_4C93_836C_ECBCB4DB4B5B_
#define __MATRIX_TYPE_H_BA30BC86_4438_4C93_836C_ECBCB4DB4B5B_

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

	/// matrix<T, n, m>
	class TYCHO_SLANG_ABI matrix_type : public template_type
	{
	public:
		matrix_type(compiler::program *p, type_info *component_type, int num_rows, int num_cols);
		bool equals(const type_info *component_type, int num_rows, int num_cols) const;

		/// make the mangled name for passed matrix parameters
		static bool make_mangled_name(core::string_builder &builder, const type_info *t, int num_rows, int num_cols);

		// type_info interface			
		
		virtual type get_type() const 
			{ return t_matrix; }
		
		virtual int get_byte_size() const 
			{ return m_component_type->get_byte_size() * m_num_rows * m_num_cols; }
					
		virtual const char* get_name() const
			{ return m_name; }

		virtual const char* get_mangled_name() const
			{ return m_mangled_name; }
			
		virtual bool equals(const type_info *t) const;				
			
			
	private:
		type_info* m_component_type;
		int   m_num_rows;
		int   m_num_cols;
		const char* m_name;
		const char* m_mangled_name;
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __MATRIX_TYPE_H_BA30BC86_4438_4C93_836C_ECBCB4DB4B5B_
