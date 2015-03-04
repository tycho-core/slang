//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:25 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __PRETTY_PRINT_H_BAA09749_DEDE_4E81_8487_19CF236F5B35_
#define __PRETTY_PRINT_H_BAA09749_DEDE_4E81_8487_19CF236F5B35_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/ast/ast.h"
#include "slang/utils/utils.h"
#include "core/colour/rgba.h"
#include "core/printing/text/formatter.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace printing
{

	/// pretty print the program in shader model 4.0 syntax
	class TYCHO_SLANG_ABI pretty_printer
	{
	public:
		/// configuration options
		enum option
		{
			opt_tab_size,
			opt_newline_after_opening_brace,
			opt_newline_before_opening_brace,
			opt_newline_after_closing_brace,
			opt_newline_before_closing_brace,
			
			opt_count
		};
		
		/// highlight description
		struct highlight
		{
			highlight() : 
				use_default_fore(true), 
				use_default_back(true),
				bold(false),
				italic(false) {}
			
			highlight(core::rgba  fore) : 
				use_default_fore(false), 
				use_default_back(true), 
				foreground(fore), 
				bold(false), 
				italic(false) {}

			highlight(core::rgba  fore, core::rgba  back) : 
				use_default_fore(false), 
				use_default_back(false), 
				foreground(fore), 
				background(back),
				bold(false), 
				italic(false) {}
			
			bool use_default_fore;
			bool use_default_back;
			core::rgba foreground;
			core::rgba background;
			bool  bold;
			bool  italic;
		};
						
		enum token
		{
			t_keyword,
			t_type_id,
			t_non_type_id,
			t_literal,
			t_operator,
			t_punctuation,
			t_semicolon,
			t_rbrace,
			t_lbrace,
			t_count
		};
		
	public:
		/// constructor
		pretty_printer();
		
		/// format the passed program tree. writes output to the passed formatter.
		void format(const ast::program_node &program, core::printing::text::formatter &formatter);				
				
		/// set a control option
		void set_option(option o, int value) { m_options[o] = value; }

		/// get a control option
		int  get_option(option o) { return m_options[o]; }
		
		/// set a highlight for a token
		void set_highlight(token t, const highlight &h) { m_highlights[t] = h;}

		/// get a highlight for a token
		const highlight& get_highlight(token t) { return m_highlights[t]; }
		
		/// set default background color
		void set_default_background_color(core::rgba clr) { m_default_background = clr; }

		/// set default foreground color
		void set_default_foreground_color(core::rgba clr) { m_default_foreground = clr; }
		
		/// set default colors and options
		void set_defaults();
		
		/// internal. increment indent depth
		void inc_depth();
		
		/// internal. decrement indent depth
		void dec_depth();
		
		/// internal. write out a string.
		void write(const char *, token t);
		
		/// internal. write a newline character and indent
		void write_newline();
		
	private:
	
		enum kerning_trail
		{
			kt_none,
			kt_space,
			kt_newline
		};
		
		/// kerning information between tokens
		struct kerning
		{
			kerning_trail trail;
		};
			
	private:
		static kerning ms_kerning_table[t_count][t_count];

		int					m_options[opt_count];
		highlight			m_highlights[t_count];		
		token				m_last_token;
		core::printing::text::formatter*	m_formatter;
		core::rgba			m_default_foreground;
		core::rgba			m_default_background;
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __PRETTY_PRINT_H_BAA09749_DEDE_4E81_8487_19CF236F5B35_
