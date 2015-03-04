//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:29:40 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __LEX_H_37187762_86E6_4A10_A7C1_8CBF92557F46_
#define __LEX_H_37187762_86E6_4A10_A7C1_8CBF92557F46_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/parsing/tokens.h"
#include "slang/parsing/token_def.h"
#include "io/forward_decls.h"

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
	class yyFlexLexer;
} // end namespace

	// performs lexical analysis of the passed source file, using the flex
	// c++ interface as it is fully reentrant.
	class lexer
	{
	public:
		lexer(allocator *, io::stream_ptr, const char *);
		~lexer();
		token_def lex();
		const char* token_str();
		int current_line() const;

	private:
		detail::yyFlexLexer *m_lexer;
		lexer(const lexer &); // disable
		lexer operator=(const lexer &); // disable
		
		const char* m_current_filename;
		allocator *m_allocator;
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __LEX_H_37187762_86E6_4A10_A7C1_8CBF92557F46_
