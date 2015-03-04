//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:29:40 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "lex.h"
#include "slang/utils/allocator.h"
#include "slang/parsing/tokens.h"
#include "io/stream.h"
#include <cstdio>
#include <cerrno>
#include <cstdlib>

using namespace tycho;
using namespace tycho::slang::compiler;
using namespace tycho::slang::parsing;

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace parsing
{
namespace detail
{

#if TYCHO_PC
#pragma warning(push, 3) 
#pragma warning(disable : 4065 4244 4102)
#endif

#define RETURN_TOKEN(_tok)	return _tok

#include "core/flex_lexer.h"
#include "slang_lex.inl"

#if TYCHO_PC
#pragma warning(pop)
#endif

} // end namespace

    //--------------------------------------------------------------------

	lexer::lexer(allocator *a, io::stream_ptr istr, const char *filename) :
		m_lexer(new detail::yyFlexLexer(istr.get(), 0)),
		m_current_filename(filename),
        m_allocator(a)
	{
		//TODO incorporate colume support
		//tocol = frcol = 0;		
	}

    //--------------------------------------------------------------------

	token_def lexer::lex()
	{
		int tok = m_lexer->yylex();
		return make_token_def(static_cast<tokens>(tok), 
						 make_token_pos(m_current_filename, 
							m_lexer->lineno(), 
							0, 0),
							//TODO
							//frcol, 
							//tocol - frcol), 
							slang::clone_string(m_allocator, token_str()));
	}

    //--------------------------------------------------------------------

	const char* lexer::token_str()
	{
		return m_lexer->YYText();
	}

    //--------------------------------------------------------------------

	lexer::~lexer()
	{
		core::safe_delete(m_lexer);
	}

    //--------------------------------------------------------------------

	int lexer::current_line() const
	{
		return m_lexer->lineno();
	}

    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
