//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:11 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __NODE_LIST_H_EFF3AE82_371C_4A5B_877F_229759123BE9_
#define __NODE_LIST_H_EFF3AE82_371C_4A5B_877F_229759123BE9_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/ast/visitor.h"
#include "slang/ast/node.h"
#include "slang/utils/list.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace ast
{

	template<class T, node::type NType, class Base = node>
	class TYCHO_SLANG_ABI node_list : public Base
	{
	public:
		typedef T type;
		typedef utils::list<T> list_type;

		node_list(T *n) : Base(NType) { /* CORE_ASSERT(n); */add_node(n); }
		node_list() : Base(NType) {}
		
		void add_node(T* n) 
			{ nodes.push_back(n); }
		
		bool empty() const
			{ return nodes.empty(); }

		int size() const 
			{	return nodes.size(); }

		typename list_type::const_iterator begin() const 
			{ return nodes.begin(); }
		typename  list_type::const_iterator end() const 
			{ return nodes.end(); }

		typename list_type::iterator begin()
			{ return nodes.begin(); }
		typename  list_type::iterator end()
			{ return nodes.end(); }

		VISITOR_BEGIN
			VISIT(nodes)
		VISITOR_END
					
	private:
		utils::list<T> nodes;
	};


} // end namespace
} // end namespace
} // end namespace

#endif // __NODE_LIST_H_EFF3AE82_371C_4A5B_877F_229759123BE9_
