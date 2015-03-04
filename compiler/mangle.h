//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:36 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __MANGLE_H_90C2FFCC_5A58_4B28_BAE8_5AC347DEBC82_
#define __MANGLE_H_90C2FFCC_5A58_4B28_BAE8_5AC347DEBC82_

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
namespace compiler
{
namespace mangler
{
	
	// Name and type mangling support based on a subset of the Itanium C++ abi 
	// http://www.codesourcery.com/cxx-abi/abi.html#mangling
	//
	// Grammar
	// --------------------------------------------------------
	//
	//  <mangled-name>	::= _Z <encoding>
	//
	//	<encoding>		::= <function name> <bare-function-type>
	//					::= <data name>
	//					::= <special-name>
	//
	//	<name>			::= <nested-name>
	//					::= <unscoped-name>
	//					::= <unscoped-template-name> <template-args>
	//					::= <local-name>	# See Scope Encoding below
	// 
	//	<unscoped-name> ::= <unqualified-name>
	//					::= St <unqualified-name>   # ::std::
	// 
	// <unscoped-template-name> ::= <unscoped-name>
	// 							::= <substitution>		
	//
	// 	<nested-name> ::= N [<CV-qualifiers>] <prefix> <unqualified-name> E
	// 		  ::= N [<CV-qualifiers>] <template-prefix> <template-args> E
	// 
	// 	<prefix> ::= <prefix> <unqualified-name>
	// 		 ::= <template-prefix> <template-args>
	// 			 ::= <template-param>
	// 		 ::= # empty
	// 		 ::= <substitution>
	// 
	// 	<template-prefix> ::= <prefix> <template unqualified-name>
	// 					  ::= <template-param>
	// 					  ::= <substitution>
	// 	<unqualified-name> ::= <operator-name>
	// 					   ::= <ctor-dtor-name>  
	// 					   ::= <source-name>   
	// 
	// 	<source-name> ::= <positive length number> <identifier>
	// 	<number> ::= [n] <non-negative decimal integer>
	// 	<identifier> ::= <unqualified source code identifie    		
	//
	// 
	// 	<type>	::= <builtin-type>
	//			::= <vector-builtin-type>
	//			::= <matrix-builtin-type>
	// 			::= <function-type>
	// 			::= <array-type>
	// 			::= U <source-name> <type>	# vendor extended type qualifier
	// 
	// 	<CV-qualifiers> ::= [V] [K] 	# volatile, const	
	//
	// 	<builtin-type>	::= v	# void
	// 					::= b	# bool
	// 					::= c	# char
	// 					::= a	# signed char
	// 					::= h	# unsigned char
	// 					::= i	# int
	// 					::= j	# unsigned int
	// 					::= f	# float
	// 					::= d	# double
	// 					::= u <source-name>	# vendor extended type		
	//
	//	<dimension>		::=	<unsigned integer>
	//
	//	<vector-builtin-type> ::= u V @ <dimension> _ <builtin-type> 
	//  <matrix-builtin-type> ::= u M @ <dimension> @ <dimension> _ <builtin-type>
	//
	// 	<array-type> ::= A <positive dimension number> _ <element type>
	// 		   ::= A [<dimension expression>] _ <element types
	//
	// 	<function-type> ::= F [Y] <bare-function-type> E
	// 	<bare-function-type> ::= <signature type>+
	// 
	// 	<expression> ::= <unary operator-name> <expression>
	// 		   ::= <binary operator-name> <expression> <expression>
	// 		   ::= <trinary operator-name> <expression> <expression> <expression>
	// 			   ::= st <type>
	// 			   ::= <template-param>
	// 			   ::= sr <type> <unqualified-name>                   # dependent name
	// 			   ::= sr <type> <unqualified-name> <template-args>   # dependent template-id
	// 		   ::= <expr-primary>
	// 
	// 	<expr-primary> ::= L <type> <value number> E                   # integer literal
	// 				 ::= L <type <value float> E                     # floating literal
	// 				 ::= L <mangled-name> E                           # external na		
		
	// type identifying characters
	
	const char int_type[] = "_Zi";
	const char uint_type[] = "_Zj";
	const char float_type[] = "_Zf";
	const char half_type[] = "_Zh";
	const char double_type[] = "_Zd";
	const char bool_type[] = "_Zb";
	const char array_type[] = "_ZA";
	const char function_type[] = "F";
	
	/// prefix that is appended to all mangled names so they cannot conflict with names defined by
	/// the user
	const char prefix[] = "_Z";
	
	/// length of above prefix
	const int prefix_length = 2;

	TYCHO_SLANG_ABI void test_mangle();

} // end namespace
} // end namespace
} // end namespace
} // end namespace

#endif // __MANGLE_H_90C2FFCC_5A58_4B28_BAE8_5AC347DEBC82_
