//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 7 December 2008 10:50:37 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef _FORWARD_DECLS_H_842E9E02_3708_4F8D_8DCE_CAF66EFD09F9
#define _FORWARD_DECLS_H_842E9E02_3708_4F8D_8DCE_CAF66EFD09F9

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{

	class allocator;

namespace compiler
{
	class program;
	struct intrinsic_function;
	struct identifier_info;
	struct scope;
	class symbol_table;

} // end namespace

namespace ast
{
	class visitor;
	class node;
	class annotation;
	struct program_node;

} // end namespace


namespace types
{
	class type_info;
	class type_factory;

	class vector_type;
	class matrix_type;
	class array_type;
	class function_type;
	class structure_type;

} // end namespace

namespace parsing
{

	class parser;
	struct token_def;
	struct token_pos;
	class lexer;

} // end namespace

namespace printing
{
	class pretty_print;

} // end namespace

namespace utils
{

} // end namespace

} // end namespace
} // end namespace

#endif // _FORWARD_DECLS_H_842E9E02_3708_4F8D_8DCE_CAF66EFD09F9
