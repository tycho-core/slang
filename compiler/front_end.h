//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:27 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __FRONT_END_H_62383B9C_AC9A_4802_B751_CD770A80FAE2_
#define __FRONT_END_H_62383B9C_AC9A_4802_B751_CD770A80FAE2_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/compiler/slang.h"
#include <iosfwd>

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace compiler
{

	struct TYCHO_SLANG_ABI include_handler
	{
		void read_file(const char *filename);
	};
	
	/// simple interface to compiler front end. trees created through here
	/// are guaranteed valid programs that can be processed further for 
	/// tree based optimisation and other transforms.
	class TYCHO_SLANG_ABI front_end
	{
	public:
		/// compile a program in a file
		program* compile_file(const char *, language_t lang, const char *entrypoint, int compile_flags);
		
		/// compile a program in a char array
		program* compile_string(const char *, language_t lang, const char *entrypoint, int compile_flags);
		
		/// compile a program from a std::istream
		program* compile(program *np, const char *filename, const char *buffer, language_t lang, const char *entrypoint, int compile_flags);
					
		/// apply a group of optimisations to a program
		void optimise(int flags);
					
	private:			
	
	};
	
} // end namespace
} // end namespace
} // end namespace

#endif // __FRONT_END_H_62383B9C_AC9A_4802_B751_CD770A80FAE2_
