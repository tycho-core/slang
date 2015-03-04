//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:29:50 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "parser_factory.h"
#include "slang/parsing/parser.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace parsing
{

    //--------------------------------------------------------------------

	parser* parser_factory::get_parser(compiler::language_t lang)
	{
		static parsing::parser parser;
		
		switch(lang)
		{
			case compiler::lang_hlsl_dx9	: return &parser;
			case compiler::lang_hlsl_dx10	: return &parser;
			case compiler::lang_cg			: TYCHO_ASSERT(!"Not implemented"); break;
			case compiler::lang_v			: TYCHO_ASSERT(!"Not implemented"); break;
		}
		
		return 0;
	}

    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
