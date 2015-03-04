//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:27 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "front_end.h"
#include "slang/compiler/program.h"
#include "slang/parsing/parser_factory.h"
#include "slang/parsing/lex.h"
#include "slang/parsing/parser.h"
#include "slang/ast/analyzer.h"
#include "core/memory.h"
#include "io/memory_stream.h"
#include <fstream>
#include <istream>
//#include <istringstream>


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
	using namespace parsing;

namespace compiler
{

    //--------------------------------------------------------------------

	program* front_end::compile(program *np, const char *filename, const char *buffer, language_t lang, const char * /*entrypoint*/, int compile_flags)
	{
		// parse the source text
		parser *p = parser_factory::get_parser(lang);

		if(!p)
		{
			np->add_message(mt_parser_not_implemented, token_pos(), get_language_as_string(lang));
			return np;
		}

		if(p)
		{
			if(!buffer)
			{
				np->add_message(mt_file_not_found, token_pos(), filename);
				return np;
			}
			
			//TODO : Remove the copy
			io::memory_stream_ptr istr(new io::memory_stream(buffer, strlen(buffer)));
			lexer lex(np->get_allocator(), istr, filename);
			p->parse(np, &lex, (compile_flags & comp_trace_parser) != 0);
			if(np->get_ast())
			{
				// perform semantic checking on the tree
				ast::ast_analyser analyser(np);
			}
		}

		return np;			
	}	
	
    //--------------------------------------------------------------------

	program* front_end::compile_file(const char *filename, language_t lang, const char *entrypoint, int compile_flags)
	{
		program *np = new program;
		FILE *f = fopen(filename, "rt");
		if(!f)
			return 0;
		fseek(f, 0L, SEEK_END);
		long len = ftell(f);
		fseek(f, 0L, SEEK_SET);
		char *buffer = HLSL_ALLOC_ARRAY(np->get_allocator(), char, len+1);
		core::mem_set(buffer, 0L, len+1);
		fread(buffer, len, 1, f);
		filename = slang::clone_string(np->get_allocator(), filename);
		np->add_source_text(filename, buffer);
		return compile(np, filename, buffer, lang, entrypoint, compile_flags);
	}
				
    //--------------------------------------------------------------------

	program* front_end::compile_string(const char *string, language_t lang, const char *entrypoint, int compile_flags)
	{
		const char *filename = "<string>";
		program *np = new program;
		string = slang::clone_string(np->get_allocator(), string);
		filename = slang::clone_string(np->get_allocator(), filename);
		np->add_source_text(filename, string);
		return compile(np, filename, string, lang, entrypoint, compile_flags);
	}
	

    //--------------------------------------------------------------------

	void front_end::optimise(int /*flags*/)
	{
	
	}

    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
