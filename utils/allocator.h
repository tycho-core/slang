//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:45 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __ALLOCATOR_H_BA03721C_BCFE_4D4A_9F20_44445E04BA27_
#define __ALLOCATOR_H_BA03721C_BCFE_4D4A_9F20_44445E04BA27_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "core/printing/text/formatter.h"
#include "core/memory/grow_only_pod_allocator.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{

	#define HLSL_ALLOC(_allocator, _type, ...) new((_allocator)->allocate(sizeof(_type))) _type(__VA_ARGS__)
	#define HLSL_ALLOC_ARRAY(_allocator, _type, _size) new((_allocator)->allocate(sizeof(_type) * _size)) _type[_size]
	#define HLSL_NEW(_type, ...) new(slang::malloc(sizeof(_type))) _type(__VA_ARGS__)
	#define HLSL_DELETE(_ptr) slang::destruct(_ptr); _ptr = 0
    #define HLSL_ALLOC_STACK(_type, _count) (_type*)TYCHO_ALLOCA(sizeof(_type) * _count)
	#define HLSL_ALLOC_STACK_ZERO(_type, _count) (_type*)zero_memory(HLSL_ALLOC_STACK(_type, _count), sizeof(_type) * _count)
	
	class allocator;
	
	/// make a copy of the string
	const char* clone_string(allocator *, const char *);
	
	/// clear a piece of memory to zero
	void* zero_memory(void *ptr, size_t size);
	
	/// malloc/free function types
	typedef void*(malloc_func)(size_t);
	typedef void(free_func)(void*);
	
	/// override the default heap allocation functions
	void set_new_handler(malloc_func *, free_func *);
	
	/// all user heap allocations go through these which can be overridden via the set_new_handler() function
	extern malloc_func *malloc;
	extern free_func* free;
	
	/// destruct a c++ object create via HLSL_NEW, use the HLSL_DELETE instead of calling this directly
	template<class T> void destruct(T *t)
	{
		t->~T();
		slang::free(t);
	}
	
	/// allocator interface. all classes that allocate memory take an allocator to use.
	class allocator
	{
	public:
		virtual void *allocate(size_t) = 0;
		virtual void deallocate(void*) = 0;
		virtual void print_usage_stats(core::printing::text::formatter *) = 0;
	};
	
	/// simple grow only allocator, very fast but all elements must be free'd together 
	/// and be pod type. this is useful when there are many small allocations and few
	/// deallocations. 
	class grow_only_allocator : public allocator
	{
	public:
		grow_only_allocator(size_t page_size) :
			m_page_size(page_size),
			m_current_page(0)
			{
				
			}
	
		~grow_only_allocator()
		{
			release_all();
		}		
		
		
		// cleanup all allocated memory
		void release_all()
		{
			page_header *page = m_current_page;
			while(page)
			{
				page_header *next = page->next_page;
				slang::free(page);
				page = next;
			}
			m_current_page = 0;
		}
		
		virtual void* allocate(size_t size)
		{
			// check to see if we have room in current page
			if(!m_current_page || ((m_current_page->used + size + sizeof(page_header)) > m_current_page->page_size))
				add_page();
			
			void *ptr = (char*)m_current_page->user + m_current_page->used;
			m_current_page->used += size;				
			return ptr;
		}
		
		virtual void deallocate(void*) { /* no-op */ }		
		virtual void print_usage_stats(core::printing::text::formatter *printer);
		
	private:
		/// per page header
		struct page_header
		{
			size_t		page_size;
			size_t		used;
			char*		user;
			page_header *next_page;
		};

	private:
		void operator=(const grow_only_allocator &); // undefined
		
		void add_page()
		{
			// allocate space and initialise
			page_header *new_page = (page_header *)slang::malloc(m_page_size);
			new_page->page_size = m_page_size;
			new_page->used		= 0;
			new_page->user		= (char*)new_page + sizeof(page_header);
			
			// link to front of list
			new_page->next_page = m_current_page;
			m_current_page = new_page;
		}
					
	private:
		size_t		 m_page_size;
		page_header *m_current_page;
	};

} // end namespace
} // end namespace

#endif // __ALLOCATOR_H_BA03721C_BCFE_4D4A_9F20_44445E04BA27_
