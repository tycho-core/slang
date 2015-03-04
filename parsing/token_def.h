//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:05 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __TOKEN_DEF_H_5A33EE1C_7008_40EA_8090_25C32F8D2524_
#define __TOKEN_DEF_H_5A33EE1C_7008_40EA_8090_25C32F8D2524_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/parsing/tokens.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace parsing
{

	// used in a union in the parser so cannot have a constructor
	struct TYCHO_SLANG_ABI token_pos
	{
		const char*	file;
		int		line;
		int		col;
		int		length;
	};
			
	
	inline token_pos make_token_pos(const char *file, int line, int col, int length)
	{
		token_pos pos;
		pos.file = file;
		pos.line = line;
		pos.col = col;
		pos.length = length;
		return pos;
	}			

	// used in a union in the parser so cannot have a constructor
	struct TYCHO_SLANG_ABI token_def
	{			
		tokens		token;
		token_pos	pos;
		const char*	str;
	};

	inline token_def make_token_def(tokens t, const token_pos &p, const char *str)
	{
		token_def token;				
		token.token = t;
		token.pos = p;
		token.str = str;
		return token;
	}
	inline bool operator!=(const token_def &def, tokens ot) { return def.token != ot; }
	inline bool operator==(const token_def &def, tokens ot) { return def.token == ot; }

} // end namespace
} // end namespace
} // end namespace

#endif // __TOKEN_DEF_H_5A33EE1C_7008_40EA_8090_25C32F8D2524_
