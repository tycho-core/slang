//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:58 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __COMPONENT_TYPES_H_D264A832_09CC_476D_92FC_552AC3DA4CF4_
#define __COMPONENT_TYPES_H_D264A832_09CC_476D_92FC_552AC3DA4CF4_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/types/type_system.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace types
{

	/// component types are allowed to be used in vector / matrix declarations	
	class TYCHO_SLANG_ABI component_type : public builtin_type
	{
	public:
		component_type(const char *name, const char *mangled_name, int size, int promotion_types, int standard_conversion_types) :
			builtin_type(f_supports_arithmetic | f_supports_comparison),
			m_name(name),
			m_mangled_name(mangled_name),
			m_size(size),
			m_ptypes(promotion_types),
			m_sctypes(standard_conversion_types) {}
			
		virtual const char* get_name() const 
			{ return m_name; }

		virtual const char* get_mangled_name() const 
			{ return m_mangled_name; }

		virtual int get_byte_size() const
			{ return m_size; }			
			
		virtual bool promotion_exists_from(const type_info *type) const
			{ return (m_ptypes & type->get_type()) > 0; }
			
		virtual bool standard_conversion_exists_from(const type_info *type) const
			{ return (m_sctypes & type->get_type()) > 0; }
					
	private:
		const char *m_name;
		const char *m_mangled_name;
		int			m_size;
		int			m_ptypes; 	///< types this type can be promoted from 
		int			m_sctypes; 	///< types this type has a standard conversion from 
	};

			
	/// 16 bit floating point
	struct TYCHO_SLANG_ABI half_type : component_type
	{
		half_type() : 
			component_type("half", 
				compiler::mangler::half_type, 
						   2, // byte size 
						   t_invalid,
						   t_integral_types | t_floating_types
						   ) {}
		virtual type get_type() const { return t_half; }
	};
	
	/// 32 bit floating point
	struct TYCHO_SLANG_ABI float_type : component_type
	{
		float_type() : 
			component_type("float", 
							compiler::mangler::float_type, 
							4, // byte size
							t_half,
							t_integral_types | t_floating_types
							) {}
							
		virtual type get_type() const { return t_float; }
	};

	/// 64 bit floating point
	struct TYCHO_SLANG_ABI double_type : component_type
	{
		double_type() : 
			component_type("double", 
							compiler::mangler::double_type, 
							8,
							t_half | t_float,
							t_integral_types | t_floating_types
							) {}
		virtual type get_type() const { return t_double; }
	};
	
	/// 32 bit integer 
	struct TYCHO_SLANG_ABI int_type : component_type
	{
		int_type() : component_type("int", 
									compiler::mangler::int_type, 
									4,
									t_bool,
									t_integral_types | t_floating_types
									) {}
		virtual type get_type() const { return t_int; }
	};
	
	/// 32 bit unsigned integer
	struct TYCHO_SLANG_ABI uint_type : component_type
	{
		uint_type() : 
			component_type("uint", 
							compiler::mangler::uint_type,  
							4,
							t_invalid,
							t_integral_types | t_floating_types
							) {}
		virtual type get_type() const { return t_uint; }
	};

	/// boolean type
	struct TYCHO_SLANG_ABI bool_type : component_type
	{
		bool_type() : 
			component_type("bool", 
							compiler::mangler::bool_type, 
							4,
							t_invalid,
							t_integral_types | t_floating_types
							) {}
		virtual type get_type() const { return t_bool; }
	};		
				

} // end namespace
} // end namespace
} // end namespace

#endif // __COMPONENT_TYPES_H_D264A832_09CC_476D_92FC_552AC3DA4CF4_
