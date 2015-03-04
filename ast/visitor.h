//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:17 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __VISITOR_H_FD7F812C_6D54_42E9_B6C6_FA82906C6335_
#define __VISITOR_H_FD7F812C_6D54_42E9_B6C6_FA82906C6335_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"


//////////////////////////////////////////////////////////////////////////////
// MACROS
//////////////////////////////////////////////////////////////////////////////

//TODO : prefix macors with TYCHO_

#define VISITOR_BEGIN \
	virtual visitor::result accept(visitor &v, node *parent) { \
	visitor::result r = v.pre_visit((node*)this, parent); \
	if(r == visitor::visit_abort_subtree) return visitor::visit_continue; \
	if(r == visitor::visit_abort) return visitor::visit_abort;

#define VISIT(_node) \
	r = v.accept(_node, this); \
	if(r == visitor::visit_abort) return visitor::visit_abort; \
	if(r == visitor::visit_abort_subtree) return visitor::visit_continue;

#define VISITOR_END \
	return v.post_visit((node*)this, parent); }

#define VISITOR_LEAF VISITOR_BEGIN VISITOR_END

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace ast
{
	class TYCHO_SLANG_ABI visitor
	{
	public:
		/// bit flags defining which nodes to visit
		enum flags
		{
			/// defaults to visiting all except internal nodes
			visit_default  = 0,
			
			/// even visit internally generated nodes
			visit_internal = (1 << 0)
		};

		enum result
		{
			/// continue visiting 
			visit_continue,
			
			/// abort visiting the current subtree, only makes sense for a preorder traversal
			visit_abort_subtree,
			
			/// abort visiting completely
			visit_abort				
		};
		
	public:
		/// constructor.
		visitor(core::uint32 flags) : m_flags(flags) {}
		
		/// called after visiting all nodes children
		virtual result pre_visit(node *, node* /*parent*/) { return visit_continue; }

		/// called before visiting all nodes children
		virtual result post_visit(node *, node* /*parent*/) { return visit_continue; }

		/// visit a single node
		virtual result visit(ast::node *, ast::node *parent) = 0;

		/// visit a node
		result accept(ast::node *n, ast::node *parent);

		// visit a container of nodes
		template<class Container>
		result accept(Container &cont, ast::node *parent, typename Container::value_type = 0)
		{
			for(typename Container::iterator it = cont.begin(); it != cont.end(); ++it)
			{
				result r = accept(*it, parent);
				if(r == visit_abort)
					return visit_abort;
				if(r == visit_abort_subtree)
					return visit_continue;
			}
			
			return visit_continue;
		}
		
	private:
		bool flag_on(flags f) const { return (m_flags & f) != 0; }
		bool flag_off(flags f) const { return !flag_on(f); }
		
	private:
		core::uint32 m_flags;
	};


	/// visit nodes after children
	struct TYCHO_SLANG_ABI visitor_postorder : visitor
	{
		visitor_postorder(flags f = visitor::visit_default) : 
			visitor(f) {}
			
		virtual result post_visit(node *n, node *parent)  { return visit(n, parent); }
	};

	/// visit nodes before children
	struct TYCHO_SLANG_ABI visitor_preorder : visitor
	{
		visitor_preorder(flags f = visitor::visit_default) : 
			visitor(f) {}

		virtual result pre_visit(node *n, node *parent)  { return visit(n, parent); }
	};				


} // end namespace
} // end namespace
} // end namespace

#endif // __VISITOR_H_FD7F812C_6D54_42E9_B6C6_FA82906C6335_
