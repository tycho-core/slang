//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:22 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __TYPE_FACTORY_H_85896491_291B_4091_8D76_EC7DFB70EC8E_
#define __TYPE_FACTORY_H_85896491_291B_4091_8D76_EC7DFB70EC8E_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/compiler/slang.h"
#include "slang/types/type_system.h"
#include "slang/utils/list.h"
#include "slang/types/vector_type.h"
#include "slang/types/matrix_type.h"
#include "slang/types/component_types.h"
#include "slang/types/texture_types.h"
#include "slang/types/sampler_types.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace types
{

	/// store of all basic type information, all pointers are shared
	/// between all parsers so must not be modified
	//TODO : change implementation to use mangled names and hash table lookup instead
	class TYCHO_SLANG_ABI type_factory
	{
	public:
		/// constructor, creates all types valid for the specified language
		type_factory(compiler::language_t lang);
		
		/// destructor
		~type_factory();
		
		/// \returns the type specified by the passed name
		const type_info* find_type(const char *) const;
		
		/// \returns the full vector type for the passed parameters
		const vector_type* find_vector_type(const type_info *component_type, int size) const;
		
		/// \returns the full matrix type for the passed parameters
		const matrix_type* find_matrix_type(const type_info *component_type, int num_rows, int num_cols) const;					
		
		/// create identifiers for all builtin types in passed symbol table
		void create_identifiers(compiler::symbol_table &) const;
		
	private:
		/// noncopyable
		type_factory& operator==(const type_factory&);
		
	private:
		typedef utils::list<component_type> components;
		typedef utils::list<vector_type> vectors;
		typedef utils::list<matrix_type> matrices;
		typedef utils::list<object_type> objects;
		
		compiler::program*	m_program; // dummy program to pass to allocators, this should really just be an allocator...
		components	m_component_types;
		objects		m_object_types;
		vectors		m_vector_types;
		matrices	m_matrix_types;
		void_type   m_void_type;
	};

	TYCHO_SLANG_ABI const type_factory& get_type_factory(compiler::language_t lang);	
	
} // end namespace
} // end namespace
} // end namespace

#endif // __TYPE_FACTORY_H_85896491_291B_4091_8D76_EC7DFB70EC8E_
