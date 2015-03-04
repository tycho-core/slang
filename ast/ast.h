//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:24:51 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __AST_H_0BD3E857_15CB_4C9C_8A41_8012D32AB2EC_
#define __AST_H_0BD3E857_15CB_4C9C_8A41_8012D32AB2EC_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/ast/visitor.h"
#include "slang/ast/node_list.h"
#include "slang/ast/tools.h"
#include "slang/compiler/program.h"
#include "slang/compiler/slang.h"
#include "slang/compiler/message.h"
#include "slang/utils/utils.h"
#include "slang/types/type_system.h"

//TODO : need to remove all implementation from this header and cut down the include dependencies
#include "slang/types/function_type.h"
#include "slang/types/array_type.h"
#include "slang/types/vector_type.h"
#include "slang/types/matrix_type.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace ast
{


	// forward decls
	struct typedef_node;
	struct program_node;
	struct basic_type_node;
	struct id_node;
	struct var_struct_decl_node;
	struct struct_node;
	struct usage_type_node;
	struct non_type_id_node;
	
	//////////////////////////////////////////////////////////////////////////
	// Helper macros for defining ast nodes
	//////////////////////////////////////////////////////////////////////////
	#define AST_NODE(_name, _base) struct TYCHO_SLANG_ABI _name##_node : _base { \
		virtual const char* get_node_name() const { return #_name; }
		
	//////////////////////////////////////////////////////////////////////////
	// AST nodes
	//////////////////////////////////////////////////////////////////////////

	/// base class for all statements
	AST_NODE(statement, node)
		statement_node(node::type t) : node(t) {}
	};
	
	
	typedef node_list<statement_node, node::t_statement_list, statement_node> statement_list_node;

	/// empty statement, only kept for pretty printing
	AST_NODE(empty_statement, statement_node)
		empty_statement_node() : statement_node(node::t_empty_statement) {}
	};

	/// base class for all expressions
	AST_NODE(expression, statement_node)
		expression_node(node::type type) : statement_node(type) {}
	};
			
	typedef node_list<expression_node, node::t_expression_list, expression_node> expression_list_node;

	/// expression statement
	AST_NODE(expression_statement, statement_node)
		expression_statement_node(expression_node *expr) : 
			statement_node(node::t_expression_statement),
			expression(expr){}

		VISITOR_BEGIN
			VISIT(expression)
		VISITOR_END

		expression_node *expression;
	};

	//////////////////////////////////////////////////////////////////////////
	// Statements
	//////////////////////////////////////////////////////////////////////////
	
	AST_NODE(return_statement, statement_node)
		return_statement_node(expression_node *expr) : 
			statement_node(node::t_return_statement),
			expression(expr) {}

		VISITOR_BEGIN
			VISIT(expression)
		VISITOR_END
		
		expression_node *expression;
	};

	AST_NODE(do_while_statement, statement_node)
		do_while_statement_node(statement_node *stmt, expression_node *expr) :
			statement_node(node::t_do_while_statement) ,
			statement(stmt),
			expression(expr)
		{}

		VISITOR_BEGIN
			VISIT(statement)
			VISIT(expression)
		VISITOR_END

		statement_node *statement;
		expression_node *expression;
	};

	AST_NODE(while_statement, statement_node)
		while_statement_node(statement_node *stmt, expression_node *expr) :
		statement_node(node::t_while_statement) ,
			statement(stmt),
			expression(expr)
		{}

		VISITOR_BEGIN
			VISIT(statement)
			VISIT(expression)
		VISITOR_END

		statement_node *statement;
		expression_node *expression;			
	};

	AST_NODE(for_statement, statement_node)
		for_statement_node(node *init_, node *cond_, node *step_, statement_node *stmt_) :
			statement_node(node::t_for_statement),
			initialiser(init_),
			step(step_),
			condition(cond_),
			statement(stmt_)
			{}

		VISITOR_BEGIN
			VISIT(initialiser)
			VISIT(condition)
			VISIT(step)
			VISIT(statement)
		VISITOR_END

		node *initialiser;
		node *step;
		node *condition;
		statement_node *statement;
	};

	AST_NODE(if_else_statement, expression_node)
		if_else_statement_node(expression_node *condition_, statement_node *if_statement_, statement_node *else_statement_) :
			expression_node(node::t_if_else_statement),
				condition(condition_),
				if_statement(if_statement_),
				else_statement(else_statement_)
			{}


		VISITOR_BEGIN
			VISIT(condition)
			VISIT(if_statement)
			VISIT(else_statement)
		VISITOR_END

		expression_node *condition;
		statement_node *if_statement;
		statement_node *else_statement;
	};

	AST_NODE(ternary_expression, expression_node)
		ternary_expression_node(expression_node *condition_, expression_node *if_expression_, expression_node *else_expression_) :
			expression_node(node::t_ternary_expression),
				condition(condition_),
				if_expression(if_expression_),
				else_expression(else_expression_)
			{}


		VISITOR_BEGIN
			VISIT(condition)
			VISIT(if_expression)
			VISIT(else_expression)
		VISITOR_END	

		expression_node *condition;
		expression_node *if_expression;
		expression_node *else_expression;
	};

	AST_NODE(discard_statement, statement_node)
		discard_statement_node() : statement_node(node::t_discard_statement) {}
		
		VISITOR_LEAF;
	};

	AST_NODE(declaration_statement, statement_node)
		declaration_statement_node(node *decl) : 
			statement_node(node::t_declaration_statement),
			declaration(decl){}

		VISITOR_BEGIN
			VISIT(declaration)
		VISITOR_END

		node *declaration;
	};


	AST_NODE(basic_type, node)
		basic_type_node(node::type t) : node(t) {}

		VISITOR_BEGIN
		VISITOR_END
	};

	AST_NODE(scalar_type, basic_type_node)
		enum builtin_type
		{
			bt_int,
			bt_uint,
			bt_float,
			bt_double,
			bt_half,
			bt_bool,
			bt_void
		};
		
		scalar_type_node(builtin_type type_) : basic_type_node(node::t_scalar_type), type(type_) {}


		virtual const char* string() const { 
			static const char *builtin_type_strings[] = {
				"int",
				"uint",
				"float",
				"double",
				"half",
				"bool",
				"void",
			};
			return builtin_type_strings[type];
		}
					
		virtual const char* query_type_name() { return string(); } 

		builtin_type type;
	};
	
	AST_NODE(vector_type, basic_type_node)
		vector_type_node(scalar_type_node *type_, expression_node *size_) :
			basic_type_node(node::t_vector_type),
			type(type_),
			size_expr(size_), size(0) {}

		virtual const char* query_type_name() { return "vector"; } 
			
		VISITOR_BEGIN
			VISIT(type)
			VISIT(size_expr)
		VISITOR_END

			
		scalar_type_node *type;
		expression_node  *size_expr;
		int				  size;
	};

	AST_NODE(matrix_type, basic_type_node)
		matrix_type_node(scalar_type_node *type_, expression_node *num_rows_, expression_node *num_columns_) :
			basic_type_node(node::t_matrix_type),
			type(type_),
			num_rows(num_rows_),
			num_columns(num_columns_) {}
			
		virtual const char* query_type_name() { return "matrix"; } 
			
		VISITOR_BEGIN
			VISIT(type)
			VISIT(num_rows)
			VISIT(num_columns)
		VISITOR_END
			
		scalar_type_node *type;
		expression_node  *num_rows;
		expression_node  *num_columns;
	};

	AST_NODE(object_type, basic_type_node)
		enum object_type
		{
			ot_string,
			ot_texture,
			ot_texture1d,
			ot_texture2d,
			ot_texture3d,
			ot_texturecube,
			ot_sampler,
			ot_sampler1d,
			ot_sampler2d,
			ot_sampler3d,
			ot_samplercube,
			ot_pixelshader,
			ot_vertexshader,
			ot_pixelfragment,
			ot_vertexfragment,
			ot_stateblock
		};
	
		object_type_node(object_type type_) : basic_type_node(node::t_object_type), type(type_) {}

		VISITOR_LEAF
		
		virtual const char* query_type_name() { return string(); }
		virtual const char* string() const { 
			static const char *object_type_strings[] = {
				"string",
				"texture",
				"texture1d",
				"texture2d",
				"texture3d",
				"textureCUBE",
				"sampler",
				"sampler1d",
				"sampler2d",
				"sampler3d",
				"samplercube",
				"pixelshader",
				"vertexshader",
				"pixelfragment",
				"vertexfragment",
				"stateblock"		
			};

			return object_type_strings[type]; 
		}
		
		object_type type;
	};

	/// base identifier type
	AST_NODE(id, expression_node)
		id_node(node::type type) : expression_node(type) 
		{}
	};

	/// type identifier
	AST_NODE(type_id, id_node)
		type_id_node(const char *name) 
			: id_node(node::t_type_id), type_name(name) 
		{}

		VISITOR_LEAF

		virtual const char* query_type_name() { return type_name; }
		virtual const char* string() const { return type_name; }

		const char *type_name;
	};

	/// non type identifier
	AST_NODE(non_type_id, id_node)
		non_type_id_node(const char *name) 
			: id_node(node::t_non_type_id), name(name), iinfo(0) 
		{}

		VISITOR_LEAF

		virtual const char* string() const { return name; }			

		const char *name;
		compiler::identifier_info *iinfo;
	};


	//////////////////////////////////////////////////////////////////////////
	// Const usages
	//////////////////////////////////////////////////////////////////////////
	
	AST_NODE(usage, node)
		usage_node(compiler::usage_t u, node *other_ = 0) : node(node::t_usage), usage(u), other(other_) {}

		VISITOR_LEAF

		virtual const char* string() const { 
			return get_usage_string(usage);
		}
		compiler::usage_t usage;
		node *other;
	};

	typedef node_list<usage_node, node::t_usage_list> usage_list_node;

	AST_NODE(usage_type, node)
		usage_type_node(node *type_, usage_list_node *usages_) : 
			node(node::t_usage_type), type(type_), usages(usages_) {}

		VISITOR_BEGIN
			VISIT(type)
			VISIT(usages)
		VISITOR_END

		node *type;
		usage_list_node *usages;
	};

	AST_NODE(const_type, node)
		const_type_node(node *type_, usage_list_node *usages_) 
			: node(node::t_const_type), 
			type(type_),
			usages(usages_)
		{
// 				TYCHO_ASSERT(type->get_type_info());
		}

		VISITOR_BEGIN
			VISIT(type)
			VISIT(usages)
		VISITOR_END
					
		node *type;
		usage_list_node *usages;
	};

	// dimensioned const type
	AST_NODE(const_type_dim, node)
		const_type_dim_node(const_type_node *type_) :
			node(node::t_const_type_dim),
			type(type_),
			dimensions(0) {}
			
		VISITOR_BEGIN
			VISIT(type)
			VISIT(dimensions)
		VISITOR_END
		
		const_type_node *type;
		expression_list_node *dimensions;
	};
	

	//////////////////////////////////////////////////////////////////////////
	// Expressions
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Cast expression
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(cast_expression, expression_node)
		cast_expression_node(const_type_dim_node *type_, expression_node *expr_) :
			expression_node(node::t_cast_expression),
			type(type_),
			expr(expr_) {}				

		VISITOR_BEGIN
			VISIT(type)
			VISIT(expr)
		VISITOR_END
		
		const_type_dim_node *type;
		expression_node *expr;
	};

	//////////////////////////////////////////////////////////////////////////
	// Binary expression
	//////////////////////////////////////////////////////////////////////////
	
	AST_NODE(binary_expression, expression_node)
		binary_expression_node(node::type type, expression_node *lhs_, expression_node *rhs_) :
			expression_node(type),
			lhs(lhs_),
			rhs(rhs_) 
		{
// 				TYCHO_ASSERT(lhs);
// 				TYCHO_ASSERT(rhs);
		}

		VISITOR_BEGIN
			VISIT(lhs)
			VISIT(rhs)
		VISITOR_END
		
		expression_node *lhs;
		expression_node *rhs;
	};

	AST_NODE(unary_expression, expression_node)
		unary_expression_node(node::type type, expression_node *value_) :
			expression_node(type),
			value(value_)
			{}

		VISITOR_BEGIN
			VISIT(value)
		VISITOR_END

		expression_node *value;
	};

	AST_NODE(postfix_expression, unary_expression_node)
		postfix_expression_node(node::type type, expression_node *value) :
			unary_expression_node(type, value)
		{}
	};
	

	#define POSTFIX_EXPRESSION(_name, _type) \
			AST_NODE(_name##_expression, postfix_expression_node) \
				_name##_expression_node(expression_node *value) : \
					postfix_expression_node(_type, value) {} \
			};
	
	#define UNARY_EXPRESSION(_name, _type) \
			AST_NODE(_name##_expression, unary_expression_node) \
				_name##_expression_node(expression_node *value) : \
					unary_expression_node(_type, value) {} \
			};

	#define BINARY_EXPRESSION(_name, _type) \
			AST_NODE(_name##_expression, binary_expression_node) \
				_name##_expression_node(expression_node *lhs, expression_node *rhs) : \
					binary_expression_node(_type, lhs, rhs) {} \
			};

	BINARY_EXPRESSION(logical_or, node::t_logical_or_expression)
	BINARY_EXPRESSION(logical_and, node::t_logical_or_expression)
	BINARY_EXPRESSION(comparision_ge, node::t_comparision_ge_expression)
	BINARY_EXPRESSION(comparision_le, node::t_comparision_le_expression)
	BINARY_EXPRESSION(comparision_lt, node::t_comparision_lt_expression)
	BINARY_EXPRESSION(comparision_gt, node::t_comparision_gt_expression)
	BINARY_EXPRESSION(comparision_eq, node::t_comparision_eq_expression)
	BINARY_EXPRESSION(comparision_ne, node::t_comparision_ne_expression)
	BINARY_EXPRESSION(add,		node::t_add_expression)
	BINARY_EXPRESSION(minus,	node::t_minus_expression)
	BINARY_EXPRESSION(times,	node::t_times_expression)
	BINARY_EXPRESSION(divide,	node::t_divide_expression)
	BINARY_EXPRESSION(mod,		node::t_mod_expression)
	
	BINARY_EXPRESSION(assign,	node::t_assign_expression)
	BINARY_EXPRESSION(add_assign,	node::t_add_assign_expression)
	BINARY_EXPRESSION(minus_assign,	node::t_minus_assign_expression)
	BINARY_EXPRESSION(times_assign,	node::t_times_assign_expression)
	BINARY_EXPRESSION(divide_assign,node::t_divide_assign_expression)
	BINARY_EXPRESSION(mod_assign,	node::t_mod_assign_expression)
	
	UNARY_EXPRESSION(prefix_inc,   node::t_prefix_inc_expression)
	UNARY_EXPRESSION(prefix_dec,   node::t_prefix_dec_expression)
	UNARY_EXPRESSION(prefix_not,   node::t_prefix_not_expression)
	UNARY_EXPRESSION(prefix_plus,  node::t_prefix_plus_expression)
	UNARY_EXPRESSION(prefix_minus, node::t_prefix_minus_expression)

	POSTFIX_EXPRESSION(postfix_inc, node::t_postfix_inc_expression)
	POSTFIX_EXPRESSION(postfix_dec, node::t_postfix_dec_expression)
	
	AST_NODE(postfix_member_expression, expression_node)
		postfix_member_expression_node(expression_node *expr, non_type_id_node *member_) :
			expression_node(node::t_postfix_member_expression),
			expression(expr),
			member(member_) 
		{
//				TYCHO_ASSERT(member->get_type_info());
		}
			
		VISITOR_BEGIN
			VISIT(expression)
			VISIT(member)
		VISITOR_END						
		
		expression_node *expression;
		non_type_id_node *member;
	};

	AST_NODE(postfix_member_call_expression, expression_node)
		postfix_member_call_expression_node(expression_node *expr, id_node *member_, expression_list_node *args_) :
			expression_node(node::t_postfix_member_call_expression),
			expression(expr),
			member(member_),
			args(args_) {}
			
		VISITOR_BEGIN
			VISIT(expression)
			VISIT(member)
			VISIT(args)
		VISITOR_END
		
		expression_node *expression;
		id_node *member;
		expression_list_node *args;
	};
	
	AST_NODE(postfix_array_expression, expression_node)
		postfix_array_expression_node(expression_node *expr_, expression_node *dim) :
			expression_node(node::t_postfix_array_expression),
			expr(expr_)
		{				
			dimensions.push_back(dim);
		}
			
		VISITOR_BEGIN
			VISIT(expr)
			VISIT(dimensions)
		VISITOR_END
			
		expression_node*	  expr;
		utils::list<expression_node> dimensions; // expressions for each dimension in the array
	};
	
	//////////////////////////////////////////////////////////////////////////
	// Variables
	//////////////////////////////////////////////////////////////////////////
	
	/// a dimensioned variable, i.e. foo[3][3]
	AST_NODE(variable_dim, node)
		typedef expression_node type;

		variable_dim_node(non_type_id_node *id_, expression_list_node *dims_) :
			node(node::t_variable_dim),
			id(id_),
			dimensions(dims_)
			{
			}

		void add_node(type* n) { dimensions->add_node(n); }

		VISITOR_BEGIN
			VISIT(id)
			VISIT(dimensions)
		VISITOR_END
		
		non_type_id_node *id;
		expression_list_node *dimensions;
	};

	typedef node_list<variable_dim_node, node::t_variable_dim_list> variable_dim_list_node;

	//////////////////////////////////////////////////////////////////////////
	// Compile expressions
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(compile_expression_base, expression_node)
		compile_expression_base_node(node::type t, non_type_id_node *target_, non_type_id_node *function_, expression_list_node *args_) : 
			expression_node(t),
			target(target_),
			function(function_),
			args(args_) {}
			
		VISITOR_BEGIN
			VISIT(target)
			VISIT(function)
			VISIT(args)
		VISITOR_END
			
		non_type_id_node *target;
		non_type_id_node *function;
		expression_list_node *args;
	};
	
	AST_NODE(compile_expression, compile_expression_base_node)
		compile_expression_node(non_type_id_node *target_, non_type_id_node *function_, expression_list_node *args_) :
			compile_expression_base_node(node::t_compile_expression, target_, function_, args_) {}
	};

	AST_NODE(compile_fragment_expression, compile_expression_base_node)
		compile_fragment_expression_node(non_type_id_node *target_, non_type_id_node *function_, expression_list_node *args_) :
			compile_expression_base_node(node::t_compile_fragment_expression, target_, function_, args_) {}
	};

	//////////////////////////////////////////////////////////////////////////
	// Constructor expression
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(constructor_expression, expression_node)
		constructor_expression_node(type_id_node *type_id_, expression_list_node *args_) :
			expression_node(node::t_constructor_expression),
			type_id(type_id_),
			args(args_) {}				
			
		VISITOR_BEGIN
			VISIT(type_id)
			VISIT(args)
		VISITOR_END
						
		type_id_node *type_id;
		expression_list_node *args;
	};
	

	//////////////////////////////////////////////////////////////////////////
	// Function call expression
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(function_call, expression_node)
		function_call_node(non_type_id_node *id_, expression_list_node *args_) :
			expression_node(node::t_function_call),
			id(id_),
			args(args_),
			resolved_call(0) {}
		
		VISITOR_BEGIN
			VISIT(id)
			VISIT(args)
		VISITOR_END
		
		non_type_id_node *id;
		expression_list_node *args;
		
		// attributes generated during context checking
		compiler::identifier_info* resolved_call;
	};


	//////////////////////////////////////////////////////////////////////////
	// Semantics
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(semantic, node)
		semantic_node(node::type t) : node(t) {}
	};

	AST_NODE(id_semantic, semantic_node)
		id_semantic_node(non_type_id_node *id_, expression_node *expression_) :
			semantic_node(node::t_id_semantic),
			id(id_),
			expression(expression_)
		{}
		
		VISITOR_BEGIN
			VISIT(id)
			VISIT(expression)
		VISITOR_END
		
		non_type_id_node *id;
		expression_node  *expression;
	};

	typedef node_list<semantic_node, node::t_semantic_list> semantic_list_node;

	//////////////////////////////////////////////////////////////////////////
	// Annotations
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(annotation, node)
		annotation_node() : node(node::t_annotation) {}

		VISITOR_BEGIN
		VISITOR_END
	};
	
	typedef node_list<annotation_node, node::t_annotation_list> annotation_list_node;

	AST_NODE(initialiser, node)
		initialiser_node() : node(node::t_initialiser) {}

		VISITOR_BEGIN
		VISITOR_END
	};

	AST_NODE(variable, node)
		variable_node(variable_dim_node *var_, 
					  semantic_list_node *semantics_, 
					  annotation_list_node *annotations_,
					  expression_node *initialiser_)  :
		node(node::t_variable),
		var(var_),
		semantics(semantics_),
		annotations(annotations_),
		initialiser(initialiser_)
		{
			TYCHO_ASSERT(var);
		}

		VISITOR_BEGIN
			VISIT(var)
			VISIT(semantics)
			VISIT(annotations)
			VISIT(initialiser)
		VISITOR_END
					
		const char* get_name() const { return var->id->name; }

		variable_dim_node*		var;
		semantic_list_node*		semantics;
		annotation_list_node*	annotations;
		expression_node*		initialiser; 			
	};

	typedef node_list<variable_node, node::t_variable_list> variable_list_node;

	AST_NODE(variable_decl, node)
		variable_decl_node(usage_type_node *type_, variable_list_node *variables_) : node(node::t_variable_decl), type(type_), variables(variables_) {}

		VISITOR_BEGIN
			VISIT(type)
			VISIT(variables)
		VISITOR_END		

		usage_type_node *type;
		variable_list_node *variables;
	};

	typedef node_list<variable_decl_node, node::t_variable_decl_list> variable_decl_list_node;


	//////////////////////////////////////////////////////////////////////////
	// Parameters
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(parameter, node)
		parameter_node(usage_type_node *type_, variable_node *var_) : 
			node(node::t_parameter),
			type(type_),
			var(var_)
		{}

		VISITOR_BEGIN
			VISIT(type)
			VISIT(var)
		VISITOR_END
		
		usage_type_node *type;
		variable_node *var;
	};

	typedef node_list<parameter_node, node::t_parameter> parameter_list_node;


	//////////////////////////////////////////////////////////////////////////
	// Structures
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(struct, node)
		struct_node(non_type_id_node *id_, variable_decl_list_node *decls_)  : 
			node(node::t_struct), id(id_), decls(decls_) 
		{
			TYCHO_ASSERT(id);
		}

		VISITOR_BEGIN
			VISIT(id)
			VISIT(decls)
		VISITOR_END

		non_type_id_node *id;
		variable_decl_list_node *decls;
	};

	AST_NODE(cbuffer, node)
		cbuffer_node(id_node *id_, variable_decl_list_node *decls_)  : node(node::t_cbuffer), id(id_), decls(decls_) {}

		VISITOR_BEGIN
			VISIT(id)
			VISIT(decls)
		VISITOR_END

		id_node *id;
		variable_decl_list_node *decls;
	};

	AST_NODE(var_struct_decl, node)
		var_struct_decl_node(struct_node *s_, variable_list_node *vars_) : node(node::t_var_struct_decl), s(s_), vars(vars_) {}

		VISITOR_BEGIN
			VISIT(s)
			VISIT(vars)
		VISITOR_END

		struct_node *s;
		variable_list_node *vars;
	};

	AST_NODE(function, node)
		function_node(usage_type_node *usage_, non_type_id_node *name_, parameter_list_node *params_, semantic_list_node *semantics_) :
			node(node::t_function),
				usage(usage_),
				name(name_),
				params(params_),
				semantics(semantics_)
		{}

		function_node(usage_type_node *usage_, non_type_id_node *name_) :
			node(node::t_function),
				usage(usage_),
				name(name_),
				params(0),
				semantics(0)
		{}

		VISITOR_BEGIN
			VISIT(usage)
			VISIT(name)
			VISIT(params)
			VISIT(semantics)
		VISITOR_END

		usage_type_node  *usage;
		non_type_id_node *name;
		parameter_list_node *params;
		semantic_list_node *semantics;
	};

	//////////////////////////////////////////////////////////////////////////
	// D3D9 States
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(state, expression_node)
		state_node(id_node *id_, node *index_, expression_node *expression_) :
			expression_node(node::t_state),
			id(id_),
			index(index_),
			expression(expression_) {}
			
		VISITOR_BEGIN
			VISIT(id)
			VISIT(index)
			VISIT(expression)
		VISITOR_END
		
		id_node *id;
		node *index;
		expression_node *expression;
	};
	
	typedef node_list<state_node, node::t_state_list, expression_node> state_list_node;
	
	AST_NODE(state_expression, expression_node)
		state_expression_node(node::type t, state_list_node *states_) :
			expression_node(t),
			states(states_) {}
			
		VISITOR_BEGIN
			VISIT(states)
		VISITOR_END
			
		state_list_node *states;
	};
	
	AST_NODE(sampler_state_expression, state_expression_node)
		sampler_state_expression_node(state_list_node *states) :
			state_expression_node(node::t_sampler_state_expression, states) {}
	};

	AST_NODE(stateblock_state_expression, state_expression_node)
		stateblock_state_expression_node(state_list_node *states) :
			state_expression_node(node::t_stateblock_state_expression, states) {}
	};
	
	AST_NODE(state_variable_expression, expression_node)
		state_variable_expression_node(expression_node *expr_) :
			expression_node(node::t_state_variable_expression),
			expr(expr_) {}
			
		VISITOR_BEGIN
			VISIT(expr)
		VISITOR_END
			
		expression_node *expr;
	};
	
	//////////////////////////////////////////////////////////////////////////
	// D3D10 States
	//////////////////////////////////////////////////////////////////////////

// 		struct state10_node : expression_node
// 		{
// 			state10_node(non_type_id_node *id_, expression_list_node *args_) :
// 				expression_node(node::t_state10),
// 				id(id_),
// 				args(args_)
// 				{}
// 				
// 			VISITOR_BEGIN
// 				VISIT(id)
// 				VISIT(args)
// 			VISITOR_END
// 			
// 			id_node *id;
// 			expression_list_node *args;
// 		};
	
	typedef node_list<function_call_node, node::t_state10_list, expression_node> state10_list_node;
	
	AST_NODE(sampler_state10_expression, expression_node)
		sampler_state10_expression_node(non_type_id_node *id_, state_list_node *states_) :
			expression_node(node::t_sampler_state10_expression),
			id(id_),
			states(states_) {}
		
		VISITOR_BEGIN
			VISIT(id)
			VISIT(states)
		VISITOR_END
		
		non_type_id_node *id;
		state_list_node *states;
	};

	
	//////////////////////////////////////////////////////////////////////////
	// Literals
	//////////////////////////////////////////////////////////////////////////
			
	template<class Type, node::type NType, scalar_type_node::builtin_type BType>
	struct literal_node_t : expression_node
	{
		literal_node_t(Type i, const char *str) 
			: expression_node(NType), value(i), value_as_string(str), type(BType)
		{}

		VISITOR_LEAF

		/// \returns the string representation of the literal
		virtual const char* string() const { return value_as_string; }

		Type		 value;
		const char  *value_as_string;
		scalar_type_node::builtin_type type;
	};
	
	typedef literal_node_t<float, node::t_float_literal, scalar_type_node::bt_float> float_literal_node;
	typedef literal_node_t<int, node::t_int_literal, scalar_type_node::bt_int> int_literal_node;
	typedef literal_node_t<double, node::t_double_literal, scalar_type_node::bt_double> double_literal_node;
	typedef literal_node_t<bool, node::t_bool_literal, scalar_type_node::bt_bool> bool_literal_node;		
	
	//TODO : make string a builtin type?
	//typedef literal_node_t<const char*, node::t_string_literal, bt_string> string_literal_node;
			
	typedef node_list<id_node, node::t_identifier_list> identifier_list_node;
	typedef node_list<node, node::t_declaration_list> declaration_list_node;

	AST_NODE(id_literal, expression_node)
		id_literal_node(id_node *id_) 
			: expression_node(node::t_id_literal), id(id_) {}

		VISITOR_BEGIN
			VISIT(id)
		VISITOR_END

		id_node *id;		
	};


	//////////////////////////////////////////////////////////////////////////
	// Typedefs
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(typedef, node)
		typedef_node(const_type_node* base_type_, variable_dim_list_node *names_) : 
			node(node::t_typedef), base_type(base_type_), names(names_) {}

		VISITOR_BEGIN
			VISIT(base_type)
			VISIT(names)
		VISITOR_END

		const_type_node *base_type;
		variable_dim_list_node *names;
	};


	//////////////////////////////////////////////////////////////////////////
	// Function declarations
	//////////////////////////////////////////////////////////////////////////

	AST_NODE(function_decl, node)
		function_decl_node(function_node *function_, annotation_list_node *annotations_, statement_list_node *stmts_) :
			node(node::t_function_decl),
			function(function_),
			annotations(annotations_),
			statements(stmts_)
			{}

		VISITOR_BEGIN
			VISIT(function)
			VISIT(annotations)
			VISIT(statements)
		VISITOR_END

		function_node *function;
		annotation_list_node *annotations;
		statement_list_node *statements;
	};


	//////////////////////////////////////////////////////////////////////////
	// D3D9 Techniques
	//////////////////////////////////////////////////////////////////////////
	
	AST_NODE(pass_decl, node)
		pass_decl_node(id_node *id_, annotation_node *annotation_, state_list_node *states_) :
			node(node::t_pass_decl),
			id(id_),
			annotation(annotation_),
			states(states_) {}
			
		VISITOR_BEGIN
			VISIT(id)
			VISIT(annotation)
			VISIT(states)
		VISITOR_END
		
		id_node *id;
		annotation_node *annotation;
		state_list_node *states;
	};
	
	typedef node_list<pass_decl_node, node::t_pass_decl_list> pass_decl_list_node;
	
	AST_NODE(technique, node)
		technique_node(pass_decl_list_node *decls_) :
			node(node::t_technique),
			decls(decls_)
		{}
		
		VISITOR_BEGIN
			VISIT(decls)
		VISITOR_END
		
		pass_decl_list_node *decls;
	};

	AST_NODE(technique_decl, node)
		technique_decl_node(id_node *id_, annotation_node *annotation_, technique_node *technique_) :
			node(node::t_technique_decl),
			id(id_),
			annotation(annotation_),
			technique(technique_)
		{			
		}
		
		VISITOR_BEGIN
			VISIT(id)
			VISIT(annotation)
			VISIT(technique)
		VISITOR_END
		
		id_node *id;
		annotation_node *annotation;
		technique_node *technique;
	};

	//////////////////////////////////////////////////////////////////////////
	// D3D10 Techniques
	//////////////////////////////////////////////////////////////////////////
	
	AST_NODE(pass10_decl, node)
		pass10_decl_node(id_node *id_, annotation_node *annotation_, state10_list_node *states_) :
			node(node::t_pass10_decl),
			id(id_),
			annotation(annotation_),
			states(states_) {}
			
		VISITOR_BEGIN
			VISIT(id)
			VISIT(annotation)
			VISIT(states)
		VISITOR_END
		
		id_node *id;
		annotation_node *annotation;
		state10_list_node *states;
	};
	
	typedef node_list<pass10_decl_node, node::t_pass_decl_list> pass10_decl_list_node;
	
	AST_NODE(technique10, node)
		technique10_node(pass10_decl_list_node *decls_) :
			node(node::t_technique10),
			decls(decls_)
		{}
		
		VISITOR_BEGIN
			VISIT(decls)
		VISITOR_END
		
		pass10_decl_list_node *decls;
	};

	AST_NODE(technique10_decl, node)
		technique10_decl_node(id_node *id_, annotation_node *annotation_, technique10_node *technique_) :
			node(node::t_technique10_decl),
			id(id_),
			annotation(annotation_),
			technique(technique_)
		{			
		}
		
		VISITOR_BEGIN
			VISIT(id)
			VISIT(annotation)
			VISIT(technique)
		VISITOR_END
		
		id_node *id;
		annotation_node *annotation;
		technique10_node *technique;
	};



	//////////////////////////////////////////////////////////////////////////
	// Program
	//////////////////////////////////////////////////////////////////////////

	/// base node of the program graph
	AST_NODE(program, node)
		program_node(declaration_list_node *decls) 
			: node(node::t_program),
			declarations(decls)
		{}
	
		VISITOR_BEGIN
			VISIT(declarations)
		VISITOR_END

		/// in same order as declared in the file
		declaration_list_node *declarations;
	};
				
	/// adapter template to dispatch node visitation to type safe virtual function
	template<class Visitor>
	class virtual_dispatcher : public Visitor
	{
	public:
		virtual_dispatcher(visitor::flags flags) :	Visitor(flags) {}
		
		// unhandled node callback, this is called for every node the user has not overridden.
		virtual visitor::result unhandled_node(node *, node *) 
		{ 
			return visitor::visit_abort_subtree; 
		}
		
		// virtual dispatch interface for all ast node types
#define DEF_VISIT(_n) virtual visitor::result visit(_n##_node *, node *) { handled = false; return visitor::visit_continue; }			
		DEF_VISIT(program);
		DEF_VISIT(int_literal);
		DEF_VISIT(float_literal);
		DEF_VISIT(double_literal);
		DEF_VISIT(bool_literal);
		//DEF_VISIT(string_literal);
		DEF_VISIT(id_literal);
		DEF_VISIT(function);
		DEF_VISIT(type_id);
		DEF_VISIT(non_type_id);
		DEF_VISIT(typedef);
		DEF_VISIT(const_type);
		DEF_VISIT(const_type_dim);
		DEF_VISIT(scalar_type);
		DEF_VISIT(vector_type);
		DEF_VISIT(matrix_type);
		DEF_VISIT(identifier_list);
		DEF_VISIT(declaration_list);
		DEF_VISIT(usage);
		DEF_VISIT(usage_list);
		DEF_VISIT(usage_type);
		DEF_VISIT(object_type);
		DEF_VISIT(variable_decl);
		DEF_VISIT(variable_decl_list);
		DEF_VISIT(struct);
		DEF_VISIT(cbuffer);
		DEF_VISIT(var_struct_decl);
		DEF_VISIT(variable_dim);
		DEF_VISIT(variable);
		DEF_VISIT(variable_list);
		DEF_VISIT(initialiser);
		DEF_VISIT(semantic_list);
		DEF_VISIT(annotation_list);
		DEF_VISIT(id_semantic);
		//DEF_VISIT(register_semantic);
		DEF_VISIT(annotation);
		DEF_VISIT(function_decl);
		DEF_VISIT(parameter);
		DEF_VISIT(statement);
		DEF_VISIT(statement_list);
		DEF_VISIT(empty_statement);
		DEF_VISIT(return_statement);
		DEF_VISIT(expression);
		DEF_VISIT(expression_list);
		//DEF_VISIT(constant_expression);
		//DEF_VISIT(constant_expression_list);
		DEF_VISIT(logical_and_expression);
		DEF_VISIT(logical_or_expression);
		DEF_VISIT(comparision_ge_expression);
		DEF_VISIT(comparision_le_expression);
		DEF_VISIT(comparision_lt_expression);
		DEF_VISIT(comparision_gt_expression);
		DEF_VISIT(comparision_eq_expression);
		DEF_VISIT(comparision_ne_expression);
		DEF_VISIT(divide_expression);
		DEF_VISIT(add_expression);
		DEF_VISIT(minus_expression);
		DEF_VISIT(times_expression);
		DEF_VISIT(mod_expression);
		DEF_VISIT(assign_expression);
		DEF_VISIT(times_assign_expression);
		DEF_VISIT(divide_assign_expression);
		DEF_VISIT(add_assign_expression);
		DEF_VISIT(minus_assign_expression);
		DEF_VISIT(mod_assign_expression);
		DEF_VISIT(expression_statement);
		DEF_VISIT(do_while_statement);
		DEF_VISIT(while_statement);
		DEF_VISIT(discard_statement);
		DEF_VISIT(declaration_statement);
		DEF_VISIT(for_statement);
		DEF_VISIT(if_else_statement);
		DEF_VISIT(ternary_expression);
		DEF_VISIT(cast_expression);
		DEF_VISIT(constructor_expression);
		DEF_VISIT(variable_dim_list);
		DEF_VISIT(prefix_inc_expression);
		DEF_VISIT(prefix_dec_expression);
		DEF_VISIT(prefix_not_expression);
		DEF_VISIT(prefix_plus_expression);
		DEF_VISIT(prefix_minus_expression);
		DEF_VISIT(postfix_inc_expression);
		DEF_VISIT(postfix_dec_expression);
		DEF_VISIT(postfix_member_expression);
		DEF_VISIT(postfix_member_call_expression);
		DEF_VISIT(postfix_array_expression);
		DEF_VISIT(function_call);
		DEF_VISIT(state);
		DEF_VISIT(state_list);
		DEF_VISIT(sampler_state_expression);
		DEF_VISIT(stateblock_state_expression);
		DEF_VISIT(state_variable_expression);
		DEF_VISIT(sampler_state10_expression);
		//DEF_VISIT(state10);
		DEF_VISIT(state10_list);
		DEF_VISIT(pass_decl);
		DEF_VISIT(pass_decl_list);
		DEF_VISIT(technique);
		DEF_VISIT(technique_decl);
		DEF_VISIT(pass10_decl);
		DEF_VISIT(pass10_decl_list);
		DEF_VISIT(technique10);
		DEF_VISIT(technique10_decl);
		DEF_VISIT(compile_expression);
		DEF_VISIT(compile_fragment_expression);
		#undef DEF_VISIT
				
	private:
		virtual visitor::result visit(ast::node *n, ast::node *parent)
		{
			#define DEF_VISIT(_n) case node::t_##_n : res = visit((_n##_node*)n, parent); break;
			visitor::result res = visitor::visit_continue;
			handled = true;
			switch(n->get_type())
			{
				DEF_VISIT(program);
				DEF_VISIT(int_literal);
				DEF_VISIT(float_literal);
				DEF_VISIT(double_literal);
				DEF_VISIT(bool_literal);
				//DEF_VISIT(string_literal);
				DEF_VISIT(id_literal);
				DEF_VISIT(function);
				DEF_VISIT(type_id);
				DEF_VISIT(non_type_id);
				DEF_VISIT(typedef);
				DEF_VISIT(const_type);
				DEF_VISIT(const_type_dim);
				DEF_VISIT(scalar_type);
				DEF_VISIT(vector_type);
				DEF_VISIT(matrix_type);
				DEF_VISIT(identifier_list);
				DEF_VISIT(declaration_list);
				DEF_VISIT(usage);
				DEF_VISIT(usage_list);
				DEF_VISIT(usage_type);
				DEF_VISIT(object_type);
				DEF_VISIT(variable_decl);
				DEF_VISIT(variable_decl_list);
				DEF_VISIT(struct);
				DEF_VISIT(cbuffer);
				DEF_VISIT(var_struct_decl);
				DEF_VISIT(variable_dim);
				DEF_VISIT(variable);
				DEF_VISIT(variable_list);
				DEF_VISIT(initialiser);
				DEF_VISIT(semantic_list);
				DEF_VISIT(annotation_list);
				DEF_VISIT(id_semantic);
				//DEF_VISIT(register_semantic);
				DEF_VISIT(annotation);
				DEF_VISIT(function_decl);
				DEF_VISIT(parameter);
				DEF_VISIT(statement);
				DEF_VISIT(statement_list);
				DEF_VISIT(empty_statement);
				DEF_VISIT(return_statement);
				DEF_VISIT(expression);
				DEF_VISIT(expression_list);
				//DEF_VISIT(constant_expression);
				//DEF_VISIT(constant_expression_list);
				DEF_VISIT(logical_and_expression);
				DEF_VISIT(logical_or_expression);
				DEF_VISIT(comparision_ge_expression);
				DEF_VISIT(comparision_le_expression);
				DEF_VISIT(comparision_lt_expression);
				DEF_VISIT(comparision_gt_expression);
				DEF_VISIT(comparision_eq_expression);
				DEF_VISIT(comparision_ne_expression);
				DEF_VISIT(divide_expression);
				DEF_VISIT(add_expression);
				DEF_VISIT(minus_expression);
				DEF_VISIT(times_expression);
				DEF_VISIT(mod_expression);
				DEF_VISIT(assign_expression);
				DEF_VISIT(times_assign_expression);
				DEF_VISIT(divide_assign_expression);
				DEF_VISIT(add_assign_expression);
				DEF_VISIT(minus_assign_expression);
				DEF_VISIT(mod_assign_expression);
				DEF_VISIT(expression_statement);
				DEF_VISIT(do_while_statement);
				DEF_VISIT(while_statement);
				DEF_VISIT(discard_statement);
				DEF_VISIT(declaration_statement);
				DEF_VISIT(for_statement);
				DEF_VISIT(if_else_statement);
				DEF_VISIT(ternary_expression);
				DEF_VISIT(cast_expression);
				DEF_VISIT(constructor_expression);
				DEF_VISIT(variable_dim_list);
				DEF_VISIT(prefix_inc_expression);
				DEF_VISIT(prefix_dec_expression);
				DEF_VISIT(prefix_not_expression);
				DEF_VISIT(prefix_plus_expression);
				DEF_VISIT(prefix_minus_expression);
				DEF_VISIT(postfix_inc_expression);
				DEF_VISIT(postfix_dec_expression);
				DEF_VISIT(postfix_member_expression);
				DEF_VISIT(postfix_member_call_expression);
				DEF_VISIT(postfix_array_expression);
				DEF_VISIT(function_call);
				DEF_VISIT(state);
				DEF_VISIT(state_list);
				DEF_VISIT(sampler_state_expression);
				DEF_VISIT(stateblock_state_expression);
				DEF_VISIT(state_variable_expression);
				DEF_VISIT(sampler_state10_expression);
				//DEF_VISIT(state10);
				DEF_VISIT(state10_list);
				DEF_VISIT(pass_decl);
				DEF_VISIT(pass_decl_list);
				DEF_VISIT(technique);
				DEF_VISIT(technique_decl);
				DEF_VISIT(pass10_decl);
				DEF_VISIT(pass10_decl_list);
				DEF_VISIT(technique10);
				DEF_VISIT(technique10_decl);
				DEF_VISIT(compile_expression);
				DEF_VISIT(compile_fragment_expression);

				
				default :
					res = unhandled_node(n, parent);   					
			}
			if(!handled)
				res = unhandled_node(n, parent);
			return res;
		}
		
	private:
		bool handled;
	};


} // end namespace
} // end namespace
} // end namespace

#endif // __AST_H_0BD3E857_15CB_4C9C_8A41_8012D32AB2EC_
