//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:43 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "program.h"
#include "slang/printing/pretty_print.h"
#include "core/string.h"
#include "core/memory.h"

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

	program::program() :
		m_allocator(8 * 1024),
		m_symbol_table(this, lang_hlsl_dx10),
		m_ast(0)
	{
		core::mem_set(m_msg_count, 0L, sizeof(m_msg_count));
	}

	//--------------------------------------------------------------------

		void program::add_message(message_type type, const parsing::token_pos &pos, const char *extra1, const char *extra2, const char *extra3)
	{
		// check to see if we already have this message
		message_list::iterator it = m_messages.begin();
		message_list::iterator end = m_messages.end();
		for(;it != end; ++it)
		{				
			message *m = *it;
			if(m->pos.file == pos.file && m->pos.line == pos.line && m->pos.col == pos.col && m->info->type == type)
				return;
		}
		
		message *m = HLSL_ALLOC(&m_allocator, message);			
		const message_info &info = get_message_info(type);
		m->pos = pos;
		m->extra[0] = extra1;			
		m->extra[1] = extra2;			
		m->extra[2] = extra3;			
		m->info = &info;
		m_messages.push_back(m);
		m_msg_count[info.level]++;
	}

	//--------------------------------------------------------------------

	void program::format_description(char *output, const char *desc, const char *extra1, const char *extra2, const char *extra3)
	{
		const size_t len = core::strlen(desc);
		size_t i = 0;
		for(; i < len-1; ++i)
		{
			if(desc[i] != '%')
				*output++ = desc[i];
			else
			{
				int param = desc[i+1] - '1';
				if(param >= 0 && param <= 9)
				{
					const char *extra[3] = {
						extra1, extra2, extra3 };

					const char *text = 0;
					if(param > 2)		
						text = "<out_of_range>";
					else if(extra[param] == 0)
						text = "<missing>";
					else
						text = extra[param];
					
					core::strcpy(output, text);
					output += core::strlen(text);
				}
				i += 1;
			}				 
		}			
		if(i < len)
			*output++ = desc[len-1];
		*output = 0;
	}
	
	//--------------------------------------------------------------------

	void program::print(core::printing::text::formatter *printer)
	{
		message_list::iterator it = m_messages.begin();
		message_list::iterator end = m_messages.end();
		for(;it != end; ++it)
		{				
			message *msg = *it;
			static char buffer[1024];
			format_description(buffer, msg->info->short_desc, msg->extra[0], msg->extra[1], msg->extra[2]);
			printer->format("%s(%d)(%d) : %s : %s \n", 
					msg->pos.file, msg->pos.line, msg->pos.col, 
					get_message_level_string(msg->info->level),
					buffer);						
		}
	}

	//--------------------------------------------------------------------
	
	bool program::is_valid() const
	{
		return m_ast && m_messages.size() == 0;
	}

	//--------------------------------------------------------------------

	void program::add_source_text(const char *filename, const char *src_text)
	{
		source_text *st = HLSL_ALLOC(get_allocator(), source_text);
		st->filename = filename;
		st->text = src_text;
		m_source_files.push_back(st);
	}

    //--------------------------------------------------------------------
	
} // end namespace
} // end namespace
} // end namespace
