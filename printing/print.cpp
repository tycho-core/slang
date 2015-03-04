//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:28 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "print.h"
#include "slang/ast/visitor.h"
#include "slang/ast/ast.h"
#include "slang/types/type_system.h"
#include "core/console.h"
#include <typeinfo>

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
	using namespace ast;

namespace printing
{

    //--------------------------------------------------------------------

	namespace detail
	{
		typedef ast::visitor_preorder print_visitor_t;
		struct print_visitor : print_visitor_t
		{
			typedef print_visitor_t base;

			print_visitor() : 
				base(visitor::visit_default),
				depth(0) {}

			virtual result pre_visit(node *n, node *parent)
			{
				++depth;
				return base::pre_visit(n, parent);
			}
			virtual result post_visit(node *n, node *parent)
			{
				result r = base::post_visit(n, parent);
				--depth;
				return r;
				
			}
			virtual result visit(node *n, node *parent)
			{
				for(int i = 1; i < depth; ++i)
					core::console::write("    ");
				if(!n)
					core::console::write("<null>\n");
				else
				{
					const parsing::token_def &token = n->get_token();
					core::console::write("%s : %s : %s : %s : %p : %p : str(%s) : file(%s) : line(%d) : column(%d)\n", 
						typeid(*n).name(), 
						n->string(), 
						n->has_annotation(node::annotation_internal) ? "<internal>" : "<none>",
						n->get_type_info() ? n->get_type_info()->get_name() : "<typeless>",
						(void*)n, 
						(void*)parent,
						token.str ? token.str : "<null>",
						token.pos.file ? token.pos.file : "<null>",
						token.pos.line,
						token.pos.col
						);
				}
				return visit_continue;
			}

			int depth;
		};
	}

    //--------------------------------------------------------------------

	//TODO make const correct visitor
	void print_tree(ast::program_node &program)
	{
		detail::print_visitor visitor;
		program.accept(visitor, 0);
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
