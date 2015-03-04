//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:45 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "allocator.h"
#include "slang/printing/pretty_print.h"
#include "core/memory/new.h"
#include "core/string.h"
#include "core/memory.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
    //--------------------------------------------------------------------

	void set_new_handler(malloc_func *m, free_func *f)
	{
		malloc = m;
		free = f;
	}

    //--------------------------------------------------------------------

	void* default_malloc(size_t size)
	{
		return core::malloc(size);
	}
	
    //--------------------------------------------------------------------

	void default_free(void*p)
	{
		core::free(p);
	}

    //--------------------------------------------------------------------

	malloc_func *malloc = default_malloc;
	free_func* free = default_free;

    //--------------------------------------------------------------------

	const char* clone_string(allocator *a, const char *str)
	{
		char *nstr = HLSL_ALLOC_ARRAY(a, char, core::strlen(str) + 1);
		core::strcpy(nstr, str);
		return nstr;
	}

    //--------------------------------------------------------------------

	void* zero_memory(void *ptr, size_t size)
	{
		core::mem_set(ptr, 0L, size);
		return ptr;
	}

    //--------------------------------------------------------------------

	void grow_only_allocator::print_usage_stats(core::printing::text::formatter*printer)
	{
		size_t num_pages = 0;
		size_t total_mem = 0;
		size_t used_mem = 0;
		
		page_header *page = m_current_page;
		while(page)
		{
			++num_pages;
			total_mem += page->page_size;
			used_mem += page->used + sizeof(page_header);
			page = page->next_page;
		}
		
		printer->format("Memory usage : %d pools : %d kb total : %d kb used : %d kb free\n", num_pages, total_mem / 1024, used_mem / 1024, (total_mem-used_mem) / 1024);
	}


    //--------------------------------------------------------------------


} // end namespace
} // end namespace
