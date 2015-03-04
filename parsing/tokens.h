//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:07 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __TOKENS_H_5E815B15_6D5C_4E67_8765_58678B5EC0BF_
#define __TOKENS_H_5E815B15_6D5C_4E67_8765_58678B5EC0BF_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace parsing
{

	enum tokens
	{
		END_OF_FILE = 0,
		
		// Token definitions, some are not implemented yet in the parser so it does not
		// generate some we user in the lexer, so they are added here, when the parse is fixed
		// duplicate define warnings will show which one to kill
		#include "slang_grammar.h"
		
		WSPC = TOKEN_COUNT, // end of tokens defined in the grammar
		RETURN_TOKEN,
		COMMENT,
		PREPROC,
		PTR,
		BAND,
		BNOT,
		LSHIFT,
		RSHIFT,
		BXOR,
		ELLIPSIS,
		ANDASSIGN,
		XORASSIGN,
		ORASSIGN,
		SHLASSIGN,
		SHRASSIGN,
		CHAR_CONST,
		WIDE_CHAR_CONST,	
		STR_CONST,
		WIDE_STR_CONST,
		AUTO,
		BREAK,
		CASE,
		CLANGCHAR,
		CONTINUE,
		DEFAULT,
		ENUM,
		cEXTERN,
		GOTO,
		RESTRICT,
		SIGNED,
		SIZEOF,
		SWITCH,
		UNION,
		UNSIGNED,
		_BOOL,
		_COMPLEX,
		_IMAGINARY,
		DIV,
		ERROR_ACTION,
		CLANGSHORT,
		IDSYM,

		TOTAL_TOKEN_COUNT
	};

	TYCHO_SLANG_ABI const char* to_string(tokens);

} // end namespace
} // end namespace
} // end namespace

#endif // __TOKENS_H_5E815B15_6D5C_4E67_8765_58678B5EC0BF_
