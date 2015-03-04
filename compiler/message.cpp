//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:39 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "message.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace compiler
{

    //--------------------------------------------------------------------

	namespace detail
	{
	
		message_info message_infos[] = 
		{
#include "messages.defs"
		};
	
		const char *message_level_strings[] = 
		{
			"info",
			"warning",
			"error",
			"internal"
		};
		
	} // end namespace

    //--------------------------------------------------------------------

	const message_info& get_message_info(message_type t)
	{
		return detail::message_infos[t];
	}

	//--------------------------------------------------------------------
	
	const char* get_message_level_string(message_level l)
	{
		return detail::message_level_strings[l];
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
