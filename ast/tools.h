//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:06 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __TOOLS_H_FE2BDE95_F495_487E_AFAB_413AC94703A4_
#define __TOOLS_H_FE2BDE95_F495_487E_AFAB_413AC94703A4_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/ast/node.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace ast
{

	struct cast_expression_node;
	struct expression_node;
	
	/// apply an annotation to all nodes in a subtree
	TYCHO_SLANG_ABI void annotate_subtree(node *, node::annotation);
			
	/// construct a cast expression to convert to the specified type
	TYCHO_SLANG_ABI cast_expression_node* make_cast_expression(compiler::program *p, const types::type_info *type, expression_node *expr);
		
	/// evaluate a constant expression tree, returns true if the tree could be fully evaluated
	TYCHO_SLANG_ABI bool evaluate_constant_tree(compiler::program *p, expression_node *expr, int *result);
	
} // end namespace
} // end namespace
} // end namespace

#endif // __TOOLS_H_FE2BDE95_F495_487E_AFAB_413AC94703A4_
