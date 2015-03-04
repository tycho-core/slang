//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:29:46 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "parser.h"
#include "slang/parsing/lex.h"
#include "io/memory_stream.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
	using namespace compiler;
	using namespace types;

namespace parsing
{

    //--------------------------------------------------------------------
#if TYCHO_PC
#pragma warning(push, 3)
#pragma warning(disable : 4065 4311)
#endif 

#include "slang_grammar.inl"

#if TYCHO_PC
#pragma warning(pop)
#endif

    //--------------------------------------------------------------------

	namespace detail
	{
		#include "default_dx10.inl"
		const char default_dx10_precompile_text_name[] = "dx10_precompile.internal";
		static parser_token_def null_token = { {NULL_TOKEN, {0, 0}} };
	}

    //--------------------------------------------------------------------

	void parser::push_scope(parser_state *state, const char *name)
	{
		state->m_program->get_symbol_table()->push_scope(name);
	}

    //--------------------------------------------------------------------

	void parser::pop_scope(parser_state *state)
	{
		state->m_program->get_symbol_table()->pop_scope();
	}

    //--------------------------------------------------------------------

	bool parser::parse(program *p, lexer *lex, bool trace)
	{
		parser_state state;

		// initialise parser
		state.m_parser = this;
		state.m_lemon_state = HlslParseAlloc(slang::malloc);
		state.m_trace = trace;
		state.m_lexer = lex;
		state.m_program = p;
		state.m_syntax_error = false;
		state.m_current_structure_type_info = 0;
#ifndef NDEBUG
		// disable tracing around compiler code
		HlslParseTrace(0, 0);
#endif 

		// before parsing user code we need to parse the compiler code that 
		// sets up some well know type aliases. these go into the super global
		// scope		
		state.m_parse_internal = true;
        io::memory_stream_ptr istr(new io::memory_stream((char*)&detail::default_dx10[0], core::strlen(detail::default_dx10)));
		parsing::lexer lexer(&get_allocator(&state), istr, &detail::default_dx10_precompile_text_name[0]);
		state.m_program->add_source_text(detail::default_dx10_precompile_text_name, detail::default_dx10);
		state.m_lexer = &lexer;
		parse_aux(state);
		
		// hack to allow us to track when we are compiling internal code instead of users. we have to do this
		// to reduce the last internal definition
		HlslParse(state.m_lemon_state, NULL_TOKEN, detail::null_token, &state);
		
#ifndef NDEBUG
		if(trace)
			HlslParseTrace(stdout, 0);
#endif		
		// now push global scope and compiler users code
		state.m_parse_internal = false;
		state.m_lexer = lex;
		push_scope(&state, "$Global");
		parse_aux(state);
		
		// push empty token to finish off parsing and cleanup
		HlslParse(state.m_lemon_state, 0, parser_token_def(), &state);
		HlslParseFree(state.m_lemon_state, slang::free);
		state.m_parser = 0;
		
		if(state.m_syntax_error)
			return 0;
			
		post_process_ast(state.m_ast);
		state.m_program->set_ast(state.m_ast);	
		
		return state.m_ast != 0;		
	}

	//--------------------------------------------------------------------

	void parser::parse_aux(parser_state &state)
	{
		token_def t;
		
		// parse the token stream. while doing this we need to keep track of type and non type 
		// identifiers and emit special tokens to the parer to signal what kind of identifier it 
		// is getting. this means we have to keep a list of all type identifiers we have seen in 
		// the current scope.	
		while((t = state.m_lexer->lex()) != END_OF_FILE)
		{
			// ignore whitespace and comments
			if(t != WSPC && t != COMMENT)
			{
				parser_token_def def;
				def.token = t;
				def.ast_node = 0;

				if(state.m_trace && !state.m_parse_internal)
					printf("TOK : %d : %s : %d(%d) : %s\n", t.token, to_string(t.token), t.pos.line, t.pos.col, state.m_lexer->token_str() ? state.m_lexer->token_str() : "<null>");

				def.str = HLSL_ALLOC_ARRAY(&get_allocator(&state), char, core::strlen(state.m_lexer->token_str())+1);
				core::strcpy(def.str, state.m_lexer->token_str());

				// if this is an identifier check to see if its a type or non type, we only add 
				// types to the symbol table during passing
				if(t == IDSYM)
				{
					if(get_symbol_table(&state).identify(def.str))
					{
						def.token.token = TYPE_ID;
						t.token = TYPE_ID;			
					}
					else
					{
						def.token.token = NON_TYPE_ID;
						t.token = NON_TYPE_ID;
					}
				}
				state.m_current_token = t;
				HlslParse(state.m_lemon_state, t.token, def, &state);

				// we need to force the rule to reduce as soon as possible after seeing a closing brace as this 
				// can signal the closing of a scope. Lemon only progresses the parser one token behind so if we
				// don't do this then a IDSYM can be added as a type when it isn't any more as it was declared in
				// the previous scope.
				if(t == RBRACE)
				{
					HlslParse(state.m_lemon_state, NULL_TOKEN, detail::null_token, &state);
				}
			}
		}
	}

    //--------------------------------------------------------------------

	void parser::report_syntax_error(parser_state *state)
	{
		state->m_syntax_error = true;
		state->m_program->add_message(mt_syntax_error, state->m_current_token.pos, state->m_current_token.str);
	}

    //--------------------------------------------------------------------

	void parser::parse_complete(parser_state *state, ast::program_node *program)
	{
 		state->m_ast = program;
	}

    //--------------------------------------------------------------------

	void parser::node_post_create_hook_aux(parser_state *state, ast::node *n)
	{
		if(state->m_parse_internal)
			n->add_annotation(node::annotation_internal);
	}

    //--------------------------------------------------------------------

	/// for each node in subtree T recursively set either
	///     attribute internal on IF all children are internal
	///     attribute internal off IF at least one child is not internal
	struct propogate_internal : ast::visitor_postorder
	{	
		propogate_internal() :
			visitor_postorder(visitor::visit_internal)
		{}
		
		virtual result visit(ast::node *n, ast::node *parent)
		{
			if(parent)
			{
				if(parent->has_annotation(node::annotation_internal) &&
					!n->has_annotation(node::annotation_internal))
				{
					parent->clear_annotation(node::annotation_internal);				
				}
			}
			return visit_continue;
		}
	};
	
    //--------------------------------------------------------------------

	void parser::post_process_ast(ast::program_node *p)
	{
		// propagate internal node attribute up the tree so we are left with it only on subtrees 
		// that contain only other internal nodes
		{
			propogate_internal visitor;
			p->accept(visitor, 0);
		}
	}

    //--------------------------------------------------------------------

	symbol_table& parser::get_symbol_table(parser_state *state) 
	{ 
		return *state->m_program->get_symbol_table(); 
	}
	
    //--------------------------------------------------------------------

	allocator& parser::get_allocator(parser_state *state) 
	{ 
		return *state->m_program->get_allocator(); 
	}	

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
