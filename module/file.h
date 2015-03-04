//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2013 Martin Slater
// Created : Wednesday, 10 September 2014 3:56:35 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __FILE_H_9A9CEA66_3442_48D6_B28F_C105FD9BE22F_
#define __FILE_H_9A9CEA66_3442_48D6_B28F_C105FD9BE22F_

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
namespace module
{
	// Constants
	const char FileMagic[] = "TYSM";
	const int SectionNameMax = 64;

	// Sections
	const char FileSectionSymbolTable[] = "SymbolTable";
	const char FileSectionConstData[]	= "ConstantData";
	const char FileSectionDebugInfo[]	= "DebugInfo";
	const char FileSectionImportTable[] = "ImportTable";
	const char FileSectionExportTable[] = "ExportTable";
	const char FileSectionCode[]		= "Code";


	/// Header at the beginning of all module files
	struct file_header
	{
		core::uint8  magic[4];			///< File type magic 'TYSM'
		core::uint32 file_version;		///< Version of this file
		core::uint32 num_sections;		///< Number of sections in the file
		core::uint32 sections_offset;	///< Offset from start of file in bytes to section headers
		core::uint32 flags;
	};

	/// Section header
	struct section_header
	{
		char name[SectionNameMax];	///< Name of the section 
		core::uint32 version;		///< Version of the section
		core::uint32 file_offset;	///< Offset from start of file to this section
		core::uint32 section_len;	///< Length in bytes of this section
		core::uint32 flags;
	};


	enum class symbol_table_flags : core::uint16
	{
		invalid = 0,	///< invalid flags
		none,			///< no flags specified
	};

	/// Symbol table header
	struct symbol_table_header
	{
		core::uint16 type;			///< @see tycho::slang::il::types
		core::uint16 num_symbols;	///< number of symbols in the table
		core::uint16 type_len;		///< length of the basic type in bytes
		core::uint16 flags;			///< @see tycho::slang::module::symbol_type_flags
	};

	struct symbol
	{
		
	};


} // end namespace
} // end namespace
} // end namespace

#endif // __FILE_H_9A9CEA66_3442_48D6_B28F_C105FD9BE22F_
