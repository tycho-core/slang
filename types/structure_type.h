//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:12 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __STRUCTURE_TYPE_H_67F122FD_AEDF_46FB_9C89_83FED26F90C5_
#define __STRUCTURE_TYPE_H_67F122FD_AEDF_46FB_9C89_83FED26F90C5_

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

	/// user defined structure
	class TYCHO_SLANG_ABI structure_type : public type_info
	{
	public:
		struct member : utils::intrusive_list<member>
		{
			const char*	m_name;
			const type_info*	m_type;
		};
		
		typedef utils::list<member> member_list;
	
	public:
		/// constructor
		structure_type(allocator *a, const char *name);				
		
		/// add a member to the structure
		void add_member(const char *name, const type_info *t);
		
		/// \returns list of all structure members
		const member_list& get_members() const { return m_members; }
		
		// type_info interface
		virtual type get_type() const { return t_structure; }
		virtual const char* get_name() const;
		virtual const char* get_mangled_name() const;
		virtual const type_info* validate_accessor(compiler::symbol_table &, const char *name) const;
		virtual int get_byte_size() const;
			
	private:		
		allocator*		m_allocator;
		const char*		m_name;	
		const char*		m_mangled_name;
		member_list		m_members;
	};
	
} // end namespace
} // end namespace
} // end namespace

#endif // __STRUCTURE_TYPE_H_67F122FD_AEDF_46FB_9C89_83FED26F90C5_
