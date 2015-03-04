//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:29:46 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __PARSER_H_DBC32D58_34CC_456A_86CC_9FC2448E63DF_
#define __PARSER_H_DBC32D58_34CC_456A_86CC_9FC2448E63DF_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/ast/ast.h"
#include "slang/utils/allocator.h"
#include "slang/compiler/symbol_table.h"
#include "slang/compiler/message.h"
#include "slang/types/structure_type.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace parsing
{

	/// hlsl compiler front end. this generates an abstract syntax tree
	/// from a stream of lexical tokens. the generated tree is guaranteed free
	/// of all grammatical errors and some type errors but may still be semantically 
	/// incorrect so post processing is required to ensure a valid program.
	class TYCHO_SLANG_ABI parser
	{	
	public:		
		/// parse a program
		bool parse(compiler::program *, lexer *, bool trace = false);
		
	//TODO: should all be private but need access from parser.
	public:			
		struct parser_state;
		
		/// internal. called when a syntax error is encountered
		void report_syntax_error(parser_state *s);
		
		/// internal. add a type definition to the current scope
		compiler::symbol_table& get_symbol_table(parser_state *s);
		
		/// internal. push a new scope on the stack
		void push_scope(parser_state *, const char *name = 0);
		
		/// internal. pop a scope from the stack
		void pop_scope(parser_state *);
		
		/// internal. called by the parser after a successful parse
		void parse_complete(parser_state *, ast::program_node *);

		/// internal. called from the parser immediately after every node creation.
		/// template here to preserve the type info on the pointer as is
		template<class T>
		T* node_post_create_hook(T *t, parser_state *state)	{ node_post_create_hook_aux(state, t); return t; }
		
		/// \returns the allocator being used
		allocator& get_allocator(parser_state *);
		
		struct parser_state
		{
			/// parser object being used
			parser *m_parser;
			
			/// program to store all parsed data in
			compiler::program *m_program;
			
			/// true if the parser encountered a syntax error
			bool m_syntax_error;

			/// root of the generated ast
			ast::program_node *m_ast;		
			
			/// true if we are parsing our own code not users
			bool m_parse_internal;

			/// current lexer we are getting tokens from
			lexer *m_lexer;							
			
			/// lemon parser state
			void *m_lemon_state;
			
			/// parser debug tracing
			bool m_trace;
							
			/// current token being passed
			token_def m_current_token;
			
			/// current structure being passed
			types::structure_type* m_current_structure_type_info;
		};												
		
	private:
		/// internal helper function
		void parse_aux(parser_state &state);			

		/// called from the parser immediately after every node creation.
		void node_post_create_hook_aux(parser_state *, ast::node *n);

		/// called after parsing to do initial processing on the resulting tree
		/// before returning it to the user
		void post_process_ast(ast::program_node *p);
				
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __PARSER_H_DBC32D58_34CC_456A_86CC_9FC2448E63DF_
