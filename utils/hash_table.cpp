//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:32:16 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "hash_table.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace utils
{

    //--------------------------------------------------------------------

	TYCHO_SLANG_ABI int test_hash_table()
	{
		hash_table<const char*, int, 16> ht;
		ht.insert("foo", 1);
		ht.insert("bar", 2);
		int* r = ht.find("foo");
//		TYCHO_ASSERT(<#_expr#>)(r);
		TYCHO_ASSERT(*r == 1);
		r = ht.find("bar");
		TYCHO_ASSERT(r);
		TYCHO_ASSERT(*r == 2);
		ht.erase("foo");
		TYCHO_ASSERT(!ht.find("foo"));
		return *ht.find("bar");
	}
	
    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
