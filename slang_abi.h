//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Sunday, 7 December 2008 9:46:31 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef _TEMPLATE_ABI_H_245B5767_FFA2_4D01_81B7_AE51D8FFF9B7
#define _TEMPLATE_ABI_H_245B5767_FFA2_4D01_81B7_AE51D8FFF9B7

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "core/core_abi.h"
#include "core/forward_decls.h"

#if TYCHO_PC

// DLL interface
#ifdef TYCHO_SLANG_EXPORTS
#define TYCHO_SLANG_ABI __declspec(dllexport)
#else
#define TYCHO_SLANG_ABI __declspec(dllimport)
#endif 

// disable a few warnings globally. should move these into specific cpp's to avoid polluting
// user header files
#pragma warning(disable : 4251) // class needs to have dll-interface to be used by clients of class ''
#pragma warning(disable : 4355) // 'this' : used in base member initializer list

#else // TYCHO_PC

#define TYCHO_SLANG_ABI

#endif // TYCHO_GC

#ifdef __cplusplus
#include "core/memory/new.h"
#include "core/memory.h"
#include "core/debug/assert.h"
#endif 

#endif // _TEMPLATE_ABI_H_245B5767_FFA2_4D01_81B7_AE51D8FFF9B7
