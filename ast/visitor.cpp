//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:17 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "visitor.h"
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

    //--------------------------------------------------------------------

	/// visit a node
	visitor::result visitor::accept(ast::node *n, ast::node *parent)
	{
		// exclude internal nodes if required
		if(flag_off(visit_internal) && n && n->has_annotation(node::annotation_internal))
			return visit_continue;
		
		if(!n)
			return visit_continue;
			
		return n->accept(*this, parent);
	}

    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
