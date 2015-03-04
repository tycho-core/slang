//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:06 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "tools.h"
#include "slang/ast/visitor.h"
#include "slang/ast/ast.h"
#include "slang/compiler/program.h"
#include "slang/types/type_system.h"
#include <stack> 

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace ast
{

    //--------------------------------------------------------------------

	namespace detail
	{
		struct annotating_visitor : ast::visitor
		{
			annotating_visitor(node::annotation ann) : ast::visitor(visit_internal), m_ann(ann) {}
			
			virtual result visit(ast::node *n, ast::node *)
			{
				if(n)
					n->add_annotation(m_ann);
				return visit_continue;
			}
		
		private:
			node::annotation m_ann;
		};
	} // end namespace
	
    //--------------------------------------------------------------------

	void annotate_subtree(node *n, node::annotation ann)
	{
		detail::annotating_visitor visitor(ann);
		n->accept(visitor, 0);
	}
	
    //--------------------------------------------------------------------

	cast_expression_node* make_cast_expression(compiler::program *p, const types::type_info *type, expression_node *expr)
	{
		type_id_node *type_id_n = HLSL_ALLOC(p->get_allocator(), type_id_node, type->get_basic_type()->get_name());
		type_id_n->set_type_info(type);
		
		const_type_node *type_n = HLSL_ALLOC(p->get_allocator(), const_type_node, type_id_n, 0);
		const_type_dim_node *type_dim_n = HLSL_ALLOC(p->get_allocator(), ast::const_type_dim_node, type_n);
		cast_expression_node *cast = HLSL_ALLOC(p->get_allocator(), ast::cast_expression_node, type_dim_n, expr);
		return cast;
	}

    //--------------------------------------------------------------------

	class constant_expression_evaluator : ast::visitor_postorder
	{
	public:
		constant_expression_evaluator(compiler::program *p, expression_node *expr) :
			m_valid(true),
            m_program(p)
		{
			expr->accept(*this, 0);
		}
		
		virtual result visit(ast::node *n, ast::node *parent)
		{			
			switch(n->get_type())
			{
				case node::t_int_literal : {
					int_literal_node *rd = (int_literal_node*)n;
					push(rd->value);
				} break;
				
				case node::t_cast_expression : {
					cast_expression_node *rd = (cast_expression_node*)n;					
					rd->expr->accept(*this, parent);
					return visit_abort_subtree;
				} break;

			#define BINARY_OP(_type, _token) \
				case node::t_##_type##_expression : { \
					int v0 = pop(); \
					int v1 = pop(); \
					int v2 = v1 _token v0; \
					push(v2); \
				} break; 

			#define UNARY_OP(_type, _token) \
				case node::t_##_type##_expression : { \
					int v0 = pop(); \
					int v2 = _token v0; \
					push(v2); \
				} break; 
				
				BINARY_OP(logical_or, ||)
				BINARY_OP(logical_and, &&)
				BINARY_OP(comparision_ge, >=)
				BINARY_OP(comparision_le, <=)
				BINARY_OP(comparision_lt, <)
				BINARY_OP(comparision_gt, >)
				BINARY_OP(comparision_eq, ==)
				BINARY_OP(comparision_ne, !=)
				BINARY_OP(add, +)
				BINARY_OP(minus, -)
				BINARY_OP(times, *)
				BINARY_OP(divide, /)
				BINARY_OP(mod, %)
				UNARY_OP(prefix_inc, ++);
				UNARY_OP(prefix_dec, --);
				UNARY_OP(prefix_not, !);
				UNARY_OP(prefix_plus, -);
				UNARY_OP(prefix_minus, +);

			#undef BINARY_OP
			#undef UNARY_OP
			
				default:
					// invalid ICE expression
					m_program->add_message(compiler::mt_invalid_ice_expression, n->get_token().pos, n->get_token().str);
					m_valid = false;
					return visit_abort;
			}
			
			return visit_continue;
		}
		
		int get_result() { return peek(); }
		bool valid() { return m_valid; }
	private:
		int  peek() { return m_stack.top(); }
		void push(int a) { m_stack.push(a); }
		int  pop() 
		{ 
			int val = m_stack.top(); 
			m_stack.pop(); 
			return val;
		}
		
	private:
		bool				m_valid;
		compiler::program*	m_program;
		std::stack<int>		m_stack;
	};
	
    //--------------------------------------------------------------------

	bool evaluate_constant_tree(compiler::program *p, expression_node *expr, int *result)
	{
		TYCHO_ASSERT(result);
		TYCHO_ASSERT(expr);
		TYCHO_ASSERT(p);
		constant_expression_evaluator evaluator(p, expr);
		if(!evaluator.valid())
			return false;
		*result = evaluator.get_result();
		return true;
	}

    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
