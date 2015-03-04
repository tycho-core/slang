//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:45 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __SYMBOL_TABLE_H_AA9A2A67_C9B3_4FF0_B2B9_7F1A32E7938B_
#define __SYMBOL_TABLE_H_AA9A2A67_C9B3_4FF0_B2B9_7F1A32E7938B_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/compiler/slang.h"
#include "slang/utils/allocator.h"
#include "slang/utils/hash_table.h"
#include "slang/types/function_type.h"
#include "core/printing/text/formatter.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace compiler
{
						
	/// identifier type
	enum identifier_type
	{
		it_invalid = 0,
		it_function,
		it_intrinsic_function,
		it_macro,
		it_type,
		it_structure,
		it_variable,
		it_sampler_state,
		it_count
	};
	
	static const char *identifier_type_strings[it_count] =
	{
		"invalid",
		"function",
		"intrinsic_function",
		"macro",
		"type",
		"structure",
		"variable",
		"sampler_state",
	};
	
	/// preprocessor macro definition
	struct TYCHO_SLANG_ABI macro
	{
		char *expansion;	// expansion for this macro
	};
	
	struct scope;
			
	/// each identifier in each scope in the source has a associated identifier_info
	struct TYCHO_SLANG_ABI identifier_info
	{
		identifier_info() :
			type(it_invalid),
			name(0),
			macro_def(0),
			ast_node(0),
			intrinsic_func(0),
			type_id(0),
			decl_scope(0),
			next(0),
			sibling(0)
		{}
			
		identifier_type type;		 ///<
		const char*	name;			 ///< original textual name as seen in source
		macro*		macro_def;		 ///< optional. macro definition if this identifier was seen previously as a #define
		ast::node*  ast_node;		 ///< optional. ast node where this identifier was used
		const intrinsic_function*  intrinsic_func; ///< optional. the identifier refers to an intrinsic function
		const types::type_info*  type_id;		 ///< optional. type information for this identifier
		scope*		decl_scope;		 ///< scope that this identifier was declared in
		identifier_info *next;		 ///< next identifier in linked list
		identifier_info *sibling;	 ///< due to function overloading there may be more than one identifier_info per name
	};

		
	/// contains all identifier and type info for all identifiers in a single scope.
	/// this contains a pointer to a linked list of child scopes and a back pointer 
	/// to its parent scope.
	/// \todo identifiers should be stored in a hash table for search speed, currently O(N).
	struct TYCHO_SLANG_ABI scope
	{
		const char*		 name;		  ///< arbitrary but unique name
		types::type_info*		 types;		  ///< types declared in this scope
		identifier_info* identifiers; ///< identifiers declared in this scope
		scope*			 children;	  ///< scopes contained with this scope
		scope*			 parent;      ///< enclosing scope or null if super scope			
		scope*			 next;		  ///< next scope in linked list		
	};
			
	/// 
	class TYCHO_SLANG_ABI symbol_table
	{
	public:			
		/// constructor
		symbol_table(program *, language_t lang);
		
		/// destructor
		~symbol_table();
		
		/// reset the symbol table
		void clear();
							
		/// add a new macro identifier to the current scope
		identifier_info* add_macro(const char *name, macro *);
		
		/// add a new type identifier to the current scope
		identifier_info* add_type(const char *name, const types::type_info *base_type, ast::node *n);
		
		/// add a new function identifier to the current scope
		identifier_info* add_function(identifier_info *parent, const char *name, ast::node *n);
		
		/// add a new structure identifier to the current scope
		identifier_info* add_structure(const char *name, ast::node *n);
		
		/// add a new variable to the current scope
		identifier_info* add_variable(const char *name, const types::type_info* base_type, ast::node *n);
		
		/// add a new sampler state to the current scope
		identifier_info* add_sampler_state(const char *name, ast::node *n);

		/// \returns type information for the passed name or 0 if the name does not refer to a type.
		const types::type_info* identify_type(const char *name);
		
		/// \returns the identifier_info for the passed identifier
		identifier_info* identify(const char *name, bool current_scope_only = false);
					
		/// \returns the SuperGlobalScope which is the root of all scopes
		const scope* get_root_scope() const 
			{ return m_root; }
			
		/// push a new scope onto the stack
		void push_scope(const char *name);
		
		/// pop the current scope from the stack
		void pop_scope();
		
		/// print the symbol table
		void print(core::printing::text::formatter *, bool print_internal);
		
		/// pushes a predefined symbol table with definitions valid in pass statements
		void push_pass_scope(const char *);
		
		/// reduces all type identifiers to most basic types to simplify type checking
		void reduce_all_types();
											
		/// instantiate a vector template type		
		const types::vector_type* instantiate_vector_type(const types::type_info *component_type, int size);
		
		/// instantiate a matrix template type
		const types::matrix_type* instantiate_matrix_type(const types::type_info *component_type, int num_rows, int num_cols);

		/// instantiate array type
		const types::array_type* instantiate_array_type(const types::type_info *base_type, int *dimensions, int num_dimensions);
		
		/// instantiate a function type
		const types::function_type* instantiate_function_type(const types::type_info *return_type, types::function_type::param_list *params);
		
		/// add a new builtin type
		void initialise_builtin_types();	
		void add_builtin_type(const types::type_info *ti);
		
	private:		
		/// create a new identifier and links to current scope
		identifier_info* add_identifier(scope *s = 0);

		/// create a new type info structure and links to current scope
		void link_type_info(types::type_info *);
		
		/// recursively print symbol table
		void print_aux(core::printing::text::formatter *printer, scope *s, bool print_internal);
		
	private:
		typedef utils::hash_table<const char*, types::type_info*, 128> type_list;
		
	private:
		program*	m_program;
		allocator*	m_allocator;		///< allocator to use for all memory allocation
		scope*		m_root;				///< root of all scopes including global scope
		scope*		m_current_scope;	///< current scope symbols are being added to		
		type_list	m_type_list;		///< list of all types defined by the user that are not built in types
		const types::type_factory* m_type_factory;	///< built in types			
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __SYMBOL_TABLE_H_AA9A2A67_C9B3_4FF0_B2B9_7F1A32E7938B_
