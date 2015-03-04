//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:32:16 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __HASH_TABLE_H_B7B6CA0B_D206_423E_A385_116D660CBC58_
#define __HASH_TABLE_H_B7B6CA0B_D206_423E_A385_116D660CBC58_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/utils/allocator.h"
#include "core/noncopyable.h"
#include "core/string.h"
#include "core/debug/assert.h"
#include "core/memory.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace utils
{
	inline unsigned int get_hash(const char *str)
	{
		// JSHash
		unsigned int hash = 1315423911;
		unsigned int i    = 0;

		for(i = 0; *str; str++, i++)
		{
			hash ^= ((hash << 5) + (*str) + (hash >> 2));
		}

		return hash;
	}
	inline bool hash_compare(const char *lhs, const char *rhs)
	{
		return core::strcmp(lhs, rhs) == 0;
	}
	
	//TODO : finish, no copy constructor / assignment
	template<class Key, class Value, int Size>
	class TYCHO_SLANG_ABI hash_table : core::noncopyable
	{
	public:
		typedef hash_table<Key, Value, Size> this_type;
		
	public:
		hash_table()
		{
			core::mem_set(m_buckets, 0L, sizeof(m_buckets));
		}
		
		~hash_table()
		{
			clear();
		}
		
		/// remove all elements from the hash table
		void clear()
		{
			// delete all buckets
			for(int i = 0; i < Size; ++i)
			{
				// delete all entries in this bucket
				entry *e = m_buckets[i];
				while(e)
				{
					entry *cur = e;
					e = cur->m_next;
					HLSL_DELETE(cur);
				}
			}
			core::mem_set(m_buckets, 0L, sizeof(m_buckets));
		}
		
		Value* find(const Key &key)
		{
			unsigned int b = get_hash(key) % Size;
			entry *e = find_entry(m_buckets[b], key);
			if(e)
				return &e->m_value;
			return 0;
		}
		bool insert(const Key &key, const Value &value)
		{
			unsigned int bucket = get_hash(key) % Size;
			entry* e = find_entry(m_buckets[bucket], key);
			if(e)
				return false;
			e = HLSL_NEW(entry);
			e->m_key = key;
			e->m_value = value;
			if(m_buckets[bucket])
				e->m_next = m_buckets[bucket];
			
			m_buckets[bucket] = e;
			return true;
		}
		
		bool erase(const Key &key)
		{
			unsigned int bi = get_hash(key) % Size;
			entry *e = m_buckets[bi], *prev = 0;
			while(e)
			{
				if(hash_compare(e->m_key, key))
					break;
				prev = e;
				e = e->m_next;
			}
			if(e == 0)
			{
				// doesn't exist
				return false;
			}
			if(prev == 0)
			{
				// at head of bucket
				m_buckets[bi] = 0;
			}
			else
			{
				// relink previous to next
				prev->m_next = e->m_next;
			}
			
			delete e;
			return true;
		}
	
	private:
		struct entry;
		entry* find_bucket(const Key &key)
		{
			int hash = get_hash(key);
			int bucket = hash % Size;
			TYCHO_ASSERT(bucket >= 0);
			TYCHO_ASSERT(bucket < Size);
			return m_buckets[bucket];
		}
		
		entry* find_entry(entry *bucket, const Key &key)
		{
			while(bucket)
			{
				if(hash_compare(bucket->m_key, key))
					return bucket;
				bucket = bucket->m_next;
			}
			return 0;
		}
		
	private:
		void operator=(const this_type &);
		
	private:
		struct entry
		{
			entry() : m_next(0) {}
			Key    m_key;
			Value  m_value;
			entry *m_next;
		};
		
		entry *m_buckets[Size];
	};

	TYCHO_SLANG_ABI int test_hash_table();

} // end namespace
} // end namespace
} // end namespace

#endif // __HASH_TABLE_H_B7B6CA0B_D206_423E_A385_116D660CBC58_
