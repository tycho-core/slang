//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:00 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "analyzer.h"
#include "slang/ast/ast.h"
#include "slang/ast/tools.h"
#include "slang/compiler/program.h"
#include "slang/compiler/context_check_dx10.h"

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

	/// construct a new object and perform all checking on passed program.
	ast_analyser::ast_analyser(compiler::program *p)
	{
		m_program = p;

		//TODO : check source language and instantiate the appropriate context checker
		compiler::context_checker_dx10 checker(p);			
	}		

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
