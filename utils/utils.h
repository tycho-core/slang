//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:32:30 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __UTILS_H_6D02DD9F_25B7_45D4_870C_F6C314C05E8C_
#define __UTILS_H_6D02DD9F_25B7_45D4_870C_F6C314C05E8C_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace utils
{
    
	/// primitive lexical cast operator
	template<class DestType> struct TYCHO_SLANG_ABI lexical_cast
	{
		template<class SrcType>
		lexical_cast(const SrcType &val)
		{
			// generic interface
			std::ostringstream ostr;
			ostr << val;
			std::istringstream istr(ostr.str());
			istr >> str;				
		}

		operator DestType() { return str; }
		DestType str;
	};
	
} // end namespace
} // end namespace
} // end namespace

#endif // __UTILS_H_6D02DD9F_25B7_45D4_870C_F6C314C05E8C_
