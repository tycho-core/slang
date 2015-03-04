//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:00 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __ANALYZER_H_5877ABF6_AB69_4D1C_B54E_474C0582D2D8_
#define __ANALYZER_H_5877ABF6_AB69_4D1C_B54E_474C0582D2D8_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace ast
{

		
	///	performs all post parsing semantic tree analysis. this includes :
	///		kind checking
	///		type checking
	///		function checking
	class TYCHO_SLANG_ABI ast_analyser
	{
	public:
		/// construct a new object and perform all checking on passed program.
		/// the passed tree is modified during checking.
		ast_analyser(compiler::program *p);
		
		/// \returns true if the program is syntactically and semantically valid
		bool valid() const { return m_valid; }

	private:
		compiler::program*	m_program;	
		bool		m_valid;	
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __ANALYZER_H_5877ABF6_AB69_4D1C_B54E_474C0582D2D8_
