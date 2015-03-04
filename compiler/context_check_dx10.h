//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:57 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __CONTEXT_CHECK_DX10_H_AEB11EDC_4B31_4F87_A03F_805B8E1C9A27_
#define __CONTEXT_CHECK_DX10_H_AEB11EDC_4B31_4F87_A03F_805B8E1C9A27_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/ast/visitor.h"
#include "slang/ast/ast.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
	namespace ast
	{
		struct function_decl_node;
		struct struct_node;
	}

namespace compiler
{

	/// Context checking entails :
	///		building the symbol table
	///		calculating type information
	///		semantic checking
	/// This is broken out into separate 'contexts' to validate a particular
	/// subtree of the ast. The symbol table may have already been partially 
	/// evaluated during parsing but we do the minimal possible there to 
	/// correctly parse the source data and we also want to be able to properly 
	/// validate generated trees without having to generate source and re-parse; 
	/// so we generate a fresh complete one during this phase.	
	class TYCHO_SLANG_ABI context_checker_dx10
	{		
	public:
		context_checker_dx10(program *p);
	
	private:
		/// validates a variable declaration
		static bool check_variable_decl(program *p, ast::node *);
		
		/// validates a structure declaration.
		static bool check_var_struct_decl(program *p, ast::node *);
		
		/// validates a typedef node
		static bool check_typedef(program *p, ast::node *);
		
		/// Constructs a list of the parameters for a function node.
		static bool make_param_list(program *p, ast::parameter_list_node *params, types::function_type::param_list &list, const char **names, int max_names);

		/// evaluate as integral constant expression trees and record all the results
		/// \returns false if the expression tree wasn't an ICE.
		static bool evaluate_constant_tree(compiler::program *p, ast::expression_list_node *n, int *results);
		
		/// \returns true if passed type is convertible to a bool
		static bool check_convertable_to_bool(program *p, const types::type_info *type);
		
		/// \returns evaluate the type of a variable node
		static const types::type_info* evaluate_type(program *p, ast::variable_node *n, const types::type_info *base_type);

		/// \returns evaluate the type of a variable_dim node
		static const types::type_info* evaluate_type(program *p, ast::variable_dim_node *n, const types::type_info *base_type);

		/// \returns evaluate the type of a parameter node
		static const types::type_info* evaluate_type(program *p, ast::parameter_node *n);
	
	private:
		typedef ast::virtual_dispatcher<ast::visitor_preorder> visitor_type;
		
		struct checker : visitor_type
		{			
			checker(program *p) : visitor_type(visit_internal), m_program(p) {}
			
		protected:
			program *m_program;
		};
		
		/// evaluates the type of leaf basic type nodes
		struct basic_type_checker : checker
		{
			basic_type_checker(program *p, ast::node*);
			static const types::type_info* evaluate(program *p, ast::node *);
			
		private:
			virtual visitor::result visit(ast::scalar_type_node*, ast::node*);
			virtual visitor::result visit(ast::matrix_type_node*, ast::node*);
			virtual visitor::result visit(ast::vector_type_node*, ast::node*);
			virtual visitor::result visit(ast::object_type_node*, ast::node*);
			virtual visitor::result visit(ast::type_id_node*, ast::node*);
			virtual visitor::result unhandled_node(ast::node *, ast::node *);
		};
		
		/// validate global name space definitions
		struct global_checker : checker
		{
			global_checker(program *p) : checker(p) {}
			
		private:
			virtual visitor::result visit(ast::function_decl_node *, ast::node *);
			virtual visitor::result visit(ast::var_struct_decl_node *, ast::node *);
			virtual visitor::result visit(ast::variable_decl_node *, ast::node *);
			virtual visitor::result visit(ast::typedef_node *, ast::node *);
			virtual visitor::result visit(ast::cbuffer_node *n, ast::node *);
			virtual visitor::result unhandled_node(ast::node *, ast::node *);
		};	
		
		/// validate function
		struct function_checker : checker
		{
			function_checker(program *p, ast::function_decl_node *fn, ast::node *parent);
			virtual result visit(ast::node *, ast::node *parent);							
		};
		
		/// validate a statement list
		struct statement_list_checker : checker
		{
			statement_list_checker(program *p, 
								   ast::statement_list_node *sln, 
								   ast::function_decl_node *, 
								   ast::node *parent,
								   bool suppress_scope);
								   
			statement_list_checker(program *p, 
								   ast::statement_node *sln, 
								   ast::function_decl_node *, 
								   ast::node *parent,
								   bool suppress_scope);
			
		private:
			virtual visitor::result visit(ast::empty_statement_node *, ast::node *);
			virtual visitor::result visit(ast::for_statement_node *, ast::node *);
			virtual visitor::result visit(ast::do_while_statement_node *, ast::node *);
			virtual visitor::result visit(ast::while_statement_node *, ast::node *);
			virtual visitor::result visit(ast::if_else_statement_node *, ast::node *);
			virtual visitor::result visit(ast::return_statement_node *, ast::node *);
			virtual visitor::result visit(ast::declaration_statement_node *, ast::node *);
			virtual visitor::result visit(ast::typedef_node *, ast::node *);
			virtual visitor::result visit(ast::variable_decl_node *, ast::node *);
			virtual visitor::result visit(ast::var_struct_decl_node *, ast::node *);
			virtual visitor::result visit(ast::expression_statement_node *, ast::node *);
			virtual visitor::result visit(ast::statement_list_node *, ast::node *);
			virtual visitor::result unhandled_node(ast::node *, ast::node *);
			
		private:
			ast::function_decl_node *m_function; ///< function this statement is within
		};
		
		/// validate an expression list
		struct expression_list_checker : checker
		{
			expression_list_checker(program *p, ast::node *sln, ast::node *parent);
		
		private:
			void check_unary_arithmetic(ast::unary_expression_node *);			
			void check_logical_expression(ast::binary_expression_node *);
			void check_arithmetic_expression(ast::binary_expression_node *);
			void check_comparison_expression(ast::binary_expression_node *);
			void check_assign_expression(ast::binary_expression_node *);

		private:						
			// misc. expressions
			visitor::result visit(ast::expression_list_node *n, ast::node *);
			visitor::result visit(ast::function_call_node *n, ast::node *);
			visitor::result visit(ast::constructor_expression_node *n, ast::node *);
			visitor::result visit(ast::ternary_expression_node *n, ast::node *);
			visitor::result visit(ast::non_type_id_node *n, ast::node *);
			visitor::result visit(ast::variable_decl_node *n, ast::node *);
			visitor::result visit(ast::cast_expression_node *n, ast::node *);

			
			// assignment expressions
			visitor::result visit(ast::assign_expression_node *n, ast::node *);
			visitor::result visit(ast::add_assign_expression_node *, ast::node *);
			visitor::result visit(ast::minus_assign_expression_node *, ast::node *);
			visitor::result visit(ast::times_assign_expression_node *, ast::node *);
			visitor::result visit(ast::divide_assign_expression_node *, ast::node *);
			visitor::result visit(ast::mod_assign_expression_node *, ast::node *);
			
			// prefix expressions
			visitor::result visit(ast::prefix_inc_expression_node *, ast::node *);
			visitor::result visit(ast::prefix_dec_expression_node *, ast::node *);
			visitor::result visit(ast::prefix_not_expression_node *, ast::node *);
			visitor::result visit(ast::prefix_plus_expression_node *, ast::node *);
			visitor::result visit(ast::prefix_minus_expression_node *, ast::node *);

			// postfix expressions
			visitor::result visit(ast::postfix_member_expression_node *n, ast::node *);
			visitor::result visit(ast::postfix_array_expression_node *n, ast::node *);
			visitor::result visit(ast::postfix_inc_expression_node *, ast::node *);
			visitor::result visit(ast::postfix_dec_expression_node *, ast::node *);
			
			// binary expressions
			visitor::result visit(ast::logical_or_expression_node *, ast::node *);
			visitor::result visit(ast::logical_and_expression_node *, ast::node *);
			visitor::result visit(ast::comparision_ge_expression_node *, ast::node *);
			visitor::result visit(ast::comparision_le_expression_node *, ast::node *);
			visitor::result visit(ast::comparision_lt_expression_node *, ast::node *);
			visitor::result visit(ast::comparision_gt_expression_node *, ast::node *);
			visitor::result visit(ast::comparision_eq_expression_node *, ast::node *);
			visitor::result visit(ast::comparision_ne_expression_node *, ast::node *);
			visitor::result visit(ast::add_expression_node *, ast::node *);
			visitor::result visit(ast::minus_expression_node *, ast::node *);
			visitor::result visit(ast::times_expression_node *, ast::node *);
			visitor::result visit(ast::divide_expression_node *, ast::node *);
			visitor::result visit(ast::mod_expression_node *, ast::node *);
													
			// literal expressions
			visitor::result visit(ast::float_literal_node *n, ast::node *);
			visitor::result visit(ast::double_literal_node *n, ast::node *);
			visitor::result visit(ast::int_literal_node *n, ast::node *);
			visitor::result visit(ast::bool_literal_node *n, ast::node *);

			virtual visitor::result unhandled_node(ast::node *, ast::node *);			
		};
					
		/// validate structure declaration
		struct struct_checker : checker
		{
			struct_checker(program *p, ast::struct_node *fn, ast::node *parent);
			
		private:
			virtual visitor::result unhandled_node(ast::node *, ast::node *);
			virtual result visit(ast::variable_decl_node*, ast::node *parent);			
			identifier_info *m_iinfo;
		};
		
		/// validate pass declaration
		struct pass_checker : checker
		{
			pass_checker(program *p) : checker(p) {}
			virtual result visit(ast::node *, ast::node *parent);
		};
		
		/// constant buffer checker
		struct cbuffer_checker : checker
		{
			cbuffer_checker(program *p, ast::node *n);
			virtual result visit(ast::variable_decl_node *n, ast::node *);
		};
		
	private:
		program *m_program;		
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __CONTEXT_CHECK_DX10_H_AEB11EDC_4B31_4F87_A03F_805B8E1C9A27_
