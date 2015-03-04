//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:45 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "symbol_table.h"
#include "slang/printing/pretty_print.h"
#include "slang/compiler/intrinsics.h"
#include "slang/compiler/program.h"
#include "slang/types/type_system.h"
#include "slang/types/type_factory.h"
#include "slang/types/array_type.h"
#include "slang/types/structure_type.h"
#include "slang/utils/allocator.h"
#include "core/string_builder.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
	using namespace types;
	using namespace utils;

namespace compiler
{

	//--------------------------------------------------------------------

	symbol_table::symbol_table(program *p, language_t lang) :
		m_program(p),
		m_allocator(p->get_allocator()),
		m_root(0),
        m_current_scope(0),
        m_type_factory(&get_type_factory(lang))
	{
		clear();
	}
	
	//--------------------------------------------------------------------

	symbol_table::~symbol_table()
	{
	}
	
	//--------------------------------------------------------------------

	void symbol_table::clear()
	{
		m_type_list.clear();
		m_root = 0;
		m_current_scope = 0;			
		push_scope("$SuperGlobal");
		initialise_builtin_types();
	}
	
	//--------------------------------------------------------------------

	void symbol_table::initialise_builtin_types()
	{
		m_type_factory->create_identifiers(*this);
	}
	
	//--------------------------------------------------------------------

	void symbol_table::add_builtin_type(const types::type_info *ti)
	{
		identifier_info *iinfo = add_identifier();
		iinfo->type = it_type;
		iinfo->name = ti->get_name();
		iinfo->type_id = ti;						
	}
	
	//--------------------------------------------------------------------

	identifier_info*  symbol_table::add_macro(const char *name, macro *macro_def)
	{
		TYCHO_ASSERT(name);
		TYCHO_ASSERT(macro_def);
		TYCHO_ASSERT(m_current_scope);
	
		identifier_info *info = add_identifier();			
		info->name = name;
		info->macro_def = macro_def;
		info->type = it_macro;
		return info;
	}
	
	//--------------------------------------------------------------------

	identifier_info* symbol_table::add_type(const char *name, const type_info *base_type, ast::node *n)
	{
		TYCHO_ASSERT(name);
		TYCHO_ASSERT(m_current_scope);
// allow null base_types during parsing?			
//			TYCHO_ASSERT(base_type); 
								
		// add a new entry to the identifier list
		identifier_info *info = add_identifier();
		info->name = name;
		info->ast_node = n;		
		info->type = it_type;			
		info->type_id = base_type; // link it to its base type 		
		
		return info;
	}
	
	//--------------------------------------------------------------------

	identifier_info* symbol_table::add_function(identifier_info *parent, const char *name, ast::node *n)
	{
		TYCHO_ASSERT(name);
		TYCHO_ASSERT(m_current_scope);

		// we handle function overloads by allowing a list of identifiers per name, so 
		// first we check to see if the name has already been used, indicated by parent
		// being non zero
		identifier_info *info = 0;
		if(!parent)
		{
			info = add_identifier();
			info->name = name;
			info->ast_node = n;
			info->type = it_function;			
		}
		else
		{
			// walk list of identifiers until we find the end
			while(parent->sibling)
				parent = parent->sibling;
				
			info = HLSL_ALLOC(m_allocator, identifier_info);
			info->name = name;
			info->ast_node = n;
			info->type = it_function;
			info->decl_scope = m_current_scope;
			parent->sibling = info;
		}
		
		return info;
	}
	
	//--------------------------------------------------------------------

	identifier_info* symbol_table::add_structure(const char *name, ast::node *n)
	{
		TYCHO_ASSERT(name);
		TYCHO_ASSERT(m_current_scope);
		
		// add the structure to the type table
		type_info *tinfo = HLSL_ALLOC(m_allocator, structure_type, m_allocator, name);
//			link_type_info(tinfo);
		
		identifier_info *info = add_identifier();
		info->name = name;
		info->ast_node = n;		
		info->type = it_structure;
		info->type_id = tinfo;
		return info;
	}
	
	//--------------------------------------------------------------------

	identifier_info* symbol_table::add_variable(const char *name, const type_info *base_type, ast::node *n)
	{
		TYCHO_ASSERT(name);
		TYCHO_ASSERT(base_type);
		
		identifier_info *info = add_identifier();
		info->type = it_variable;
		info->name = name;
		info->ast_node = n;
		info->type_id = base_type;			
		return info;
	}

	//--------------------------------------------------------------------

	identifier_info* symbol_table::add_sampler_state(const char *name, ast::node *n)
	{
		TYCHO_ASSERT(name);
		TYCHO_ASSERT(n);
		
		identifier_info *info = add_identifier();
		info->type = it_sampler_state;
		info->name = name;
		info->ast_node = n;
		return info;
	}		
			
	//--------------------------------------------------------------------

	//TODO : Make fast , O(N) currently.
	const type_info* symbol_table::identify_type(const char *name)
	{
		TYCHO_ASSERT(name);
						
		identifier_info *info = identify(name);
		if(!info)
			return 0;
			
		if(info->type == it_variable)
			return 0;
			
		return info->type_id;
	}
	
	//--------------------------------------------------------------------

	//TODO : Make fast , O(N) currently.
	identifier_info* symbol_table::identify(const char *name, bool current_scope_only)
	{
		TYCHO_ASSERT(name);
		// walk up the stack from the current scope looking for this name
		scope *s = m_current_scope;
		while(s)
		{
			// check identifiers
			identifier_info *info = s->identifiers;
			while(info)
			{
				if(core::strcmp(info->name, name) == 0)
				{
					// we have a match
					return info;
				}
				info = info->next;
			}
			
			if(current_scope_only)
				return 0;
			
			// not found, try next scope up
			s = s->parent;
		}
		
		// not found so far to try looking for an intrinsic function and add an identifier
		// for it in the super scope
		const intrinsic_function *i = find_intrinsic(name);
		if(i)
		{
			identifier_info *info = add_identifier(m_root);
			info->name = name;
			info->ast_node = 0;
			info->type = it_intrinsic_function;
			info->intrinsic_func = i;
			return info;
		}
		
		return 0;
	}

	//--------------------------------------------------------------------

	identifier_info* symbol_table::add_identifier(scope *s)
	{
		if(s == 0)
			s = m_current_scope;
		identifier_info *info = HLSL_ALLOC(m_allocator, identifier_info);
		core::mem_set(info, 0, sizeof(*info));
		info->next = s->identifiers;
		s->identifiers = info;			
		info->decl_scope = s;
		return info;
	}
	
	//--------------------------------------------------------------------

	void symbol_table::link_type_info(type_info *info)
	{
		info->intrusive_list_set_next(m_current_scope->types);
		m_current_scope->types = info;			
	}
	
	//--------------------------------------------------------------------

	void symbol_table::push_scope(const char *name)
	{
		TYCHO_ASSERT(name);
		scope *new_scope = HLSL_ALLOC(m_allocator, scope);
		core::mem_set(new_scope, 0L, sizeof(*new_scope));
		new_scope->name = name;
		
		// link it to its parent and add to it its parents children
		if(m_current_scope)
		{
			new_scope->parent = m_current_scope;
			new_scope->next = m_current_scope->children;
			m_current_scope->children  = new_scope;			
		}
		if(!m_root)
		{
			m_root = new_scope;
		}
		
		// set as current scope
		m_current_scope = new_scope;
	}
	
	//--------------------------------------------------------------------

	void symbol_table::pop_scope()
	{
		TYCHO_ASSERT(m_current_scope->parent != 0);
		
		// back to current scopes parent
		m_current_scope = m_current_scope->parent;	
	}
		
	//--------------------------------------------------------------------

	void symbol_table::print(core::printing::text::formatter *printer, bool print_internal)
	{
		print_aux(printer, m_root, print_internal);
		printer->newline();
	}
	
	//--------------------------------------------------------------------

	void symbol_table::print_aux(core::printing::text::formatter *printer, scope *s, bool print_internal)
	{
		printer->horizontal_bar(0);
		printer->format("Scope : %s", s->name ? s->name : "<anonymous>");				
		printer->newline();
		printer->horizontal_bar(0);
		printer->horizontal_bar(0);
		printer->writeln("Types");
		printer->format("%32s%32s%32s\n", "Name", "Size", "Alignment");
		printer->horizontal_bar(0);
		type_info *type = s->types;
		while(type)
		{
			char buf[256] = { 0 };
			core::snprintf(buf, 256, "%32s %32d %32d", type->get_name(), type->get_byte_size(), type->get_alignment());
			printer->writeln(buf);
			
			type = type->intrusive_list_get_next();
		}

		printer->horizontal_bar(0);
		printer->writeln("Identifiers");
		printer->format("%32s%32s%32s%32s\n", "Name", "ID Type", "Type Name", "Mangled Type Name");
		printer->horizontal_bar(0);
		identifier_info *id = s->identifiers;
		while(id)
		{
			char buf[256] = { 0 };
			core::snprintf(buf, 256, "%32s%32s%32s%32s", id->name, 
										 identifier_type_strings[id->type], 
										 id->type_id ? id->type_id->get_name() : "N/A",
										 id->type_id ? id->type_id->get_mangled_name() : "N/A");
			printer->writeln(buf);
			
			// siblings
			identifier_info *sid = id->sibling;
			while(sid)
			{
				char buf[256] = { 0 };
				core::snprintf(buf, 256, "%32s%32s%32s%32s", sid->name, 
											 identifier_type_strings[sid->type], 
											 sid->type_id ? sid->type_id->get_name() : "N/A",
											 sid->type_id ? sid->type_id->get_mangled_name() : "N/A");
				printer->writeln(buf);
				sid = sid->sibling;				
			}
			
			id = id->next;
		}

		printer->inc_indent();
		printer->newline();
		  
		// print children
		scope *child = s->children;
		while(child)
		{
			print_aux(printer, child, print_internal);
			child = child->next;
		}
			
		printer->dec_indent();
		printer->newline();
	}

	/// pushes a predefined symbol table with definitions valid in pass statements
	void symbol_table::push_pass_scope(const char *name)
	{	
		push_scope(name);

		// shader profile type
		add_builtin_type(HLSL_ALLOC(m_allocator, shader_profile_type));
		
		// shader profiles
		add_variable("vs_4_0", identify_type("shader_profile"), 0);
		add_variable("ps_4_0", identify_type("shader_profile"), 0);
		
		// functions
		add_function(0, "CompileShader", 0);
		add_function(0, "SetVertexShader", 0);
		add_function(0, "SetPixelShader", 0);
		add_function(0, "SetGeometryShader", 0);
		
		// null identifier
		add_builtin_type(HLSL_ALLOC(m_allocator, null_type));
		add_variable("NULL", identify_type("NULL"), 0);			
	}	
					
	/// instantiate a vector template type		
	const vector_type* symbol_table::instantiate_vector_type(const type_info *component_type, int size)
	{
		return m_type_factory->find_vector_type(component_type, size);
	}
	
	/// instantiate a matrix template type
	const matrix_type* symbol_table::instantiate_matrix_type(const type_info *component_type, int num_rows, int num_cols)
	{
		return m_type_factory->find_matrix_type(component_type, num_rows, num_cols);		
	}

	/// instantiate array type
	const array_type* symbol_table::instantiate_array_type(const type_info *base_type, int *dimensions, int num_dimensions)
	{
		core::string_builder builder(compiler::max_mangled_name_length);
		if(!array_type::make_mangled_name(builder, base_type, dimensions, num_dimensions))
			return 0;
			
		const char *mangled_name = builder.string();
		type_info **t = m_type_list.find(mangled_name);
		if(t)
		{
			TYCHO_ASSERT((*t)->get_type() == type_info::t_array);
			return (array_type*)*t;
		}						
		array_type *type = HLSL_ALLOC(m_allocator, array_type, m_program, base_type, dimensions, num_dimensions);	
		mangled_name = slang::clone_string(m_allocator, mangled_name);
		m_type_list.insert(mangled_name, type);
		return type;
	}

	/// instantiate a function type
	const function_type* symbol_table::instantiate_function_type(const type_info *return_type, 
													function_type::param_list *params)
	{
		// get the mangled name for this function and look it up
		core::string_builder builder(compiler::max_mangled_name_length);
		if(!function_type::make_mangled_name(builder, return_type, params))
			return 0;
			
		const char *mangled_name = builder.string();
		type_info **t = m_type_list.find(mangled_name);
		if(t)
		{				
			TYCHO_ASSERT((*t)->get_type() == type_info::t_function);
			return (function_type*)(*t);
		}
		function_type *type = HLSL_ALLOC(m_allocator, function_type, m_allocator, return_type, params);		
		mangled_name = slang::clone_string(m_allocator, mangled_name);
		m_type_list.insert(mangled_name, type);
		return type;
	}
	
    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
