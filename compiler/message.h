//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:39 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __MESSAGE_H_D48D9305_29BD_48A5_9A4C_E50488B6E33F_
#define __MESSAGE_H_D48D9305_29BD_48A5_9A4C_E50488B6E33F_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/compiler/slang.h"
#include "slang/utils/list.h"
#include "slang/parsing/token_def.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace compiler
{

	enum message_level
	{
		mt_info,
		mt_warning,
		mt_error,
		mt_internal,
		mt_count
	};
	
	enum message_type
	{
#include "messages.types"
	};
	
	struct TYCHO_SLANG_ABI message_info
	{
		message_level level;
		message_type  type;
		const char*   short_desc;
		const char*   long_desc;
	};	
	
	
	/// message generated during compilation
	struct TYCHO_SLANG_ABI message : utils::intrusive_list<message>
	{ 
		const message_info *info;
		const char* context;
		parsing::token_pos   pos;
		const char* extra[3];
	};
	
	typedef utils::list<message> message_list;
		
	TYCHO_SLANG_ABI const message_info& get_message_info(message_type);
	TYCHO_SLANG_ABI const char* get_message_level_string(message_level);
	
} // end namespace
} // end namespace
} // end namespace

#endif // __MESSAGE_H_D48D9305_29BD_48A5_9A4C_E50488B6E33F_
