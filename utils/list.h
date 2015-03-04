//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:32:19 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __LIST_H_F7AF67D2_8A0C_4FE2_9107_D3030BE2864C_
#define __LIST_H_F7AF67D2_8A0C_4FE2_9107_D3030BE2864C_

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
namespace utils
{

	template<class T>
	class TYCHO_SLANG_ABI intrusive_list
	{
	public:
		intrusive_list() : m_next(0) {}
		
		void intrusive_list_set_next(T*t) { m_next = t; }
		T*   intrusive_list_get_next() { return m_next; }
		const T*   intrusive_list_get_next() const { return m_next; }
		
	private:
		T *m_next;
	};
	
	template<class T>
	class TYCHO_SLANG_ABI list
	{
	public:
		typedef T* value_type;
		 
	public:
		struct iterator
		{
			iterator(T *t) : m_t(t) {}
			
			T* operator*() 
			{ 
				return m_t; 
			}
			
			iterator operator++(int)
			{
				iterator tmp = *this;
				m_t = m_t->intrusive_list_get_next();
				return tmp;
			}
			
			iterator operator++()
			{
				m_t = (T*)m_t->intrusive_list_get_next();
				return *this;
			}
			
			bool operator==(iterator lhs)
			{
				return m_t == lhs.m_t;
			}
			
			bool operator!=(iterator lhs)
			{
				return !(*this == lhs);
			}
			
		private:
			T* m_t;
		};
	
		struct const_iterator
		{
			const_iterator(const T *t) : m_t(t) {}
			const_iterator(iterator it)
			{
				m_t = *it;
			}
			const_iterator& operator=(const iterator &it)
			{
				m_t = it->m_t;
				return *this;
			}
			
			const T* operator*() 
			{ 
				return m_t; 
			}
			
			const_iterator operator++(int)
			{
				iterator tmp = *this;
				m_t = m_t->get_next();
				return tmp;
			}
			
			const_iterator operator++()
			{
				m_t = (const T*)m_t->intrusive_list_get_next();
				return *this;
			}
			
			bool operator==(const_iterator lhs)
			{
				return m_t == lhs.m_t;
			}
			
			bool operator!=(const_iterator lhs)
			{
				return !(*this == lhs);
			}
			
		private:
			const T* m_t;
		};
		
		list()
		{
			m_head = m_tail = 0;
		}
		
		void push(T*t)
		{
			if(m_head == 0)
			{
				m_head = m_tail = t;
				return;
			}
			t->intrusive_list_set_next(m_head);
			m_head = t;
		}
		
		void push_back(T*t)
		{
			TYCHO_ASSERT(t);
			if(m_head == 0)
			{
				m_head = m_tail = t;
				return;
			}
			m_tail->intrusive_list_set_next(t);
			m_tail = t;
		}
		
		void clear()
		{
			m_head = m_tail = 0;
		}
		
		bool empty() const
		{
			return m_head == 0;
		}
		
		int size() const
		{
			int s = 0;
			T* t = m_head;
			while(t)
			{
				++s;
				t = static_cast<T*>(t->intrusive_list_get_next());
			}
			return s;
		}
		
		iterator begin()
			{ return iterator(m_head); }
		
		iterator end()
			{ return iterator(0); }

		const_iterator begin() const
			{ return const_iterator(m_head); }
		
		const_iterator end() const
			{ return const_iterator(0); }
		
	private:
		T* m_head;
		T* m_tail;
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __LIST_H_F7AF67D2_8A0C_4FE2_9107_D3030BE2864C_
