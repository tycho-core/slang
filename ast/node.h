//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:04 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __NODE_H_B9C6BBB3_0F7F_4FA7_ADD7_7F59B1923890_
#define __NODE_H_B9C6BBB3_0F7F_4FA7_ADD7_7F59B1923890_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/ast/visitor.h"
#include "slang/parsing/tokens.h"
#include "slang/parsing/token_def.h"
#include "slang/utils/list.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace ast
{

	/// base of all nodes in the ast
	class TYCHO_SLANG_ABI node : public utils::intrusive_list<node>
	{
	public:
		/// poor mans type id so we don't need to rely on RTTI
		enum type
		{
			t_program,
			
			// literals
			t_int_literal,
			t_float_literal,
			t_double_literal,
			t_bool_literal,
			t_string_literal,
			t_id_literal,
			
			t_function,
			t_type_id,
			t_non_type_id,
			t_declaration,t_typedef,
			t_const_type,
			t_const_type_dim,
			t_scalar_type,
			t_vector_type,
			t_matrix_type,
			t_identifier_list,
			t_declaration_list,
			t_usage,
			t_usage_list,
			t_usage_type,
			t_object_type,
			t_variable_decl,
			t_variable_decl_list,
			t_struct,
			t_cbuffer,
			t_var_struct_decl,
			t_variable_dim,
			t_variable,
			t_variable_list,
			t_initialiser,
			t_semantic_list,
			t_annotation_list,
			
			t_id_semantic,
			t_register_semantic,
			
			t_annotation,
			t_function_decl,
			t_parameter,
			t_statement,
			t_statement_list,
			t_empty_statement,
			t_return_statement,
			t_expression,
			t_expression_list,
			
			/// expressions
			t_constant_expression,
			t_constant_expression_list,
			t_logical_and_expression,
			t_logical_or_expression,
			t_comparision_ge_expression,
			t_comparision_le_expression,
			t_comparision_lt_expression,
			t_comparision_gt_expression,
			t_comparision_eq_expression,
			t_comparision_ne_expression,
			t_divide_expression,
			t_add_expression,
			t_minus_expression,
			t_times_expression,
			t_mod_expression,
			t_assign_expression,
			t_times_assign_expression,
			t_divide_assign_expression,
			t_add_assign_expression,
			t_minus_assign_expression,
			t_mod_assign_expression,

			t_expression_statement,
			t_do_while_statement,
			t_while_statement,
			t_discard_statement,
			t_declaration_statement,
			t_for_statement,
			t_if_else_statement,
			t_ternary_expression,
			t_cast_expression,
			t_constructor_expression,
			
			t_variable_dim_list,

			t_prefix_inc_expression,
			t_prefix_dec_expression,
			t_prefix_not_expression,
			t_prefix_plus_expression,
			t_prefix_minus_expression,
			
			t_postfix_inc_expression,
			t_postfix_dec_expression,
			t_postfix_member_expression,
			t_postfix_member_call_expression,
			t_postfix_array_expression,
			
			t_function_call,
			
			// d3d9 states
			t_state,
			t_state_list,
			t_sampler_state_expression,
			t_stateblock_state_expression,
			t_state_variable_expression,
			t_sampler_state10_expression,
			
			// d3d10 states
			t_state10,
			t_state10_list,
							
			// d3d9 techniques
			t_pass_decl,
			t_pass_decl_list,
			t_technique,
			t_technique_decl,			

			// d3d10 techniques
			t_pass10_decl,
			t_pass10_decl_list,
			t_technique10,
			t_technique10_decl,			
			
			// compile
			t_compile_expression,
			t_compile_fragment_expression,
		};	
		
		
		/// primitive node annotations
		enum annotation
		{
			annotation_none		 = 0,
			annotation_internal  = (1 << 0)
		};

		/// constructor
		node(type t);
		
		/// destructor
		virtual ~node() {}
		
		/// query type
		bool isa(type t) const { return m_type == t; };
		
		/// query name of the name
		virtual const char* get_node_name() const { return "<unknown>"; }
		
		/// visitor interface
		virtual visitor::result accept(visitor &, node *) { return visitor::visit_continue; } //= 0;
		
		/// \returns simple string representation of node
		virtual const char* string() const { return ""; }
								
		/// \returns the type of this node.
		type get_type() const { return m_type; }

		/// add an annotation to the node
		void add_annotation(annotation a) { m_annotations |= a; }
		
		/// check for a particular annotation
		bool has_annotation(annotation a) const { return (m_annotations & a) != 0; }
		
		/// clear an annotation from the node
		void clear_annotation(annotation a) { m_annotations &= ~a;}
					
		/// set the token where this node was generated
		void set_token(const parsing::token_def &t) { m_token = t; }
		
		/// get the token where this node was generated
		const parsing::token_def& get_token() const  { return m_token; }

		/// set type information for this node, this points back into the symbol table
		void set_type_info(const types::type_info *t ) { m_type_info = t; }
		
		/// get type information for this node, this points back into the symbol table
		const types::type_info* get_type_info() const { return m_type_info; }
								
		/// set the identifier_info for this node
		const compiler::identifier_info* get_id_info() const { return m_id_info; }

		/// get the identifier_info for this node
		void set_id_info(const compiler::identifier_info *id) { m_id_info = id; }
							
		//////////////////////////////////////////////////////////////////////////
		// Node querying, all this should be moved into more specific base classes
		//////////////////////////////////////////////////////////////////////////
					
		/// \return the type name of this node or 0 if not a type node
		virtual const char* query_type_name() { return 0; } 
		
	private:
		type						m_type;
		parsing::token_def			m_token;
		core::uint32				m_annotations;
		const types::type_info*	m_type_info;
		const compiler::identifier_info* m_id_info;
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __NODE_H_B9C6BBB3_0F7F_4FA7_ADD7_7F59B1923890_
