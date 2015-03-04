//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:29:59 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __PARSER_INC_H_42680298_F0A3_4960_B073_8AFEE8BBB6E3_
#define __PARSER_INC_H_42680298_F0A3_4960_B073_8AFEE8BBB6E3_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/ast/node.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

struct parser_token_def
{
	token_def token;
	char *str;
	slang::ast::node *ast_node;
};

template<class T>
T* safe_cast(ast::node *n)
{
	return dynamic_cast<T*>(n);
}

template<class T>
T* checked_safe_cast(ast::node *n)
{
	T *r = safe_cast<T>(n);
#ifndef NDEBUG
	if(n)		
	{
		TYCHO_ASSERT(r);
	}
#endif
	return r;	
}
static parser_token_def null_token;

#define PARSER state->m_parser
#define PROGRAM state->m_program
#define SYMBOL_TABLE PARSER->get_symbol_table(state)
#define IDENTIFY(_id) SYMBOL_TABLE.identify(_id)
#define IDENTIFY_THIS_SCOPE(_id) SYMBOL_TABLE.identify(_id, true)
#define SAVE_TOKEN(A, B) GET_NODE(A)->set_token(B.ast_node ? B.ast_node->get_token() : B.token)
#define GET_NODE_TOKEN(A) GET_NODE(A)->get_token()		
		
#define ALLOC(_size) state->m_parser->get_allocator(state).allocate(_size)		
#define MAKE_NODE_PTR(_type, ...) state->m_parser->node_post_create_hook(new(ALLOC(sizeof(ast::_type##_node))) ast::_type##_node(__VA_ARGS__), state)
#define MAKE_NODE(_type, _r, _tk, ...) _r.ast_node = MAKE_NODE_PTR(_type, __VA_ARGS__); SAVE_TOKEN(_r, _tk);
#define COPY_NODE(_dst, _src) _dst.ast_node = _src.ast_node
#define GET_NODE(_s) _s.ast_node
#define SAFE_CAST(_type, A) checked_safe_cast<ast::_type##_node>(GET_NODE(A))

#define ADD_NODE_TO_LIST_0(_list_type, B, C) if(GET_NODE(C)) { SAFE_CAST(_list_type,B)->add_node(checked_safe_cast<ast::_list_type##_node::type>(GET_NODE(C))); }
#define ADD_NODE_TO_LIST(_list_type, A, B, C) if(GET_NODE(C)) { ADD_NODE_TO_LIST_0(_list_type, B, C); } COPY_NODE(A, B)

#define MAKE_BINARY_EXPR(_name, A, _lhs, _rhs, _tkn) MAKE_NODE(_name##_expression, A, _lhs, SAFE_CAST(expression, _lhs), SAFE_CAST(expression, _rhs)); SAVE_TOKEN(A, _tkn);
#define MAKE_UNARY_EXPR(_name, A, _value, _tkn) MAKE_NODE(_name##_expression, A, _value, SAFE_CAST(expression, _value));SAVE_TOKEN(A, _tkn);

#define ERROR(_type, _token,  ...) PROGRAM->add_message(_type, _token.pos, __VA_ARGS__)

#define MAKE_NON_TYPE_ID(_name) HLSL_ALLOC(PROGRAM->get_allocator(), non_type_id_node, _name);
#define SET_TYPE_INFO(A, _name) GET_NODE(A)->set_type_info(SYMBOL_TABLE.identify_type(_name))

#endif // __PARSER_INC_H_42680298_F0A3_4960_B073_8AFEE8BBB6E3_
