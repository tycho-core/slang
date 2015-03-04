//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:54 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __ARRAY_TYPE_H_FF394C33_2816_49D3_BF02_4245745A3AB0_
#define __ARRAY_TYPE_H_FF394C33_2816_49D3_BF02_4245745A3AB0_

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

	/// a homogeneous N dimensional array of a particular type
	class TYCHO_SLANG_ABI array_type : public type_info
	{
	public:
		array_type(compiler::program *p, const type_info *t, int *dimensions, int num_dimensions);
		
		// type_info interface
		virtual type get_type() const { return t_array; }
		virtual const char* get_name() const;		
		virtual const char* get_mangled_name() const;		
		virtual int get_byte_size() const;
		virtual const type_info* validate_array_access(compiler::program *p, const int* dimensions, int num_dimensions) const;

		/// make the mangled string version for the passed array type parameters
		/// format is : array(mangled_base_name, d0, d1, d2) where dN is the number of elements in that dimension
		static bool make_mangled_name(core::string_builder &, const type_info *t, int *dimensions, int num_dimensions);				

	private:
		const type_info*	m_type;
		int			m_byte_size;
		int*		m_dimensions;
		int			m_num_dimensions;
		const char* m_name;
		const char* m_mangled_name;
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __ARRAY_TYPE_H_FF394C33_2816_49D3_BF02_4245745A3AB0_
