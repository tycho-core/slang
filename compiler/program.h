//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:43 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __PROGRAM_H_5F0D752D_54AB_4E2F_A9EA_9177CF33E3AD_
#define __PROGRAM_H_5F0D752D_54AB_4E2F_A9EA_9177CF33E3AD_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/utils/allocator.h"
#include "slang/compiler/symbol_table.h"
#include "slang/compiler/message.h"
#include "slang/parsing/tokens.h"
#include "slang/compiler/slang.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace ast { struct program_node; }

namespace compiler
{
	struct token_def;

	/// a program contains all stages of the compilation process 
	/// for one translation unit and manages all the memory allocated 
	/// during processing and will release it all on destruction. If 
	/// you modify its tree in any way you either need to use the allocator
	/// or track and release all memory you attach manually.
	class TYCHO_SLANG_ABI program
	{
	public:		
		struct source_text : utils::intrusive_list<source_text>
		{
			const char *filename;
			const char *text;
		};			
		typedef utils::list<source_text> sources;

	public:
		/// constructor 
		program();
		  
		/// \returns the allocator for this program
		allocator* get_allocator() { return &m_allocator; }
		
		/// \returns the symbol table for this program
		symbol_table* get_symbol_table() { return &m_symbol_table; }
	
		/// set the ast for this program
		void set_ast(ast::program_node *p) { m_ast = p;}
		
		/// \returns the root of the program ast
		ast::program_node* get_ast() { return m_ast; }
		
		/// add a compile message to the program
		void add_message(message_type type, const parsing::token_pos &pos, const char *extra1 = 0, const char *extra2 = 0, const char *extra3 = 0);
		
		/// print messages
		void print(core::printing::text::formatter*);
		
		/// \returns true if this is a valid program
		bool is_valid() const;
		
		/// \returns the number of errors encountered parsing the source
		int get_message_count(message_level mt) const { return m_msg_count[mt]; }
		
		/// set the current token being processed during tree processing so we can retrieve it
		/// for more detailed error messages
		void set_current_token(const parsing::token_def &t) { m_current_token = t; }
		
		/// clear the current token
		void clear_current_token() { m_current_token.pos = parsing::make_token_pos(0,0,0,0); m_current_token.str = 0; m_current_token.token = parsing::END_OF_FILE; }
		
		/// get the current token being processed
		const parsing::token_def& get_current_token() const { return m_current_token; }

		/// store the source code for a file
		void add_source_text(const char *filename, const char *src_text);
		
		/// \returns list of source files and their contents used to generate this program
		const sources& get_source_files() const { return m_source_files; }
		
		/// \returns list of messages generate during compilation
		const message_list& get_messages() const { return m_messages; }

		/// format message description			
		void format_description(char *output, const char *desc, const char *extra1 = 0, const char *extra2 = 0, const char *extra3 = 0);
			
	private:
		/// allocator for all parts of the program 
		grow_only_allocator  m_allocator;			
		symbol_table         m_symbol_table;
		ast::program_node	*m_ast;
		message_list		 m_messages;
		int					 m_msg_count[mt_count];
		parsing::token_def  m_current_token;
		sources				 m_source_files;
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __PROGRAM_H_5F0D752D_54AB_4E2F_A9EA_9177CF33E3AD_
