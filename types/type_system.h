//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:27 PM
//////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER

#ifndef __TYPE_SYSTEM_H_C5C1C782_7518_411D_9399_445EEFD5AE46_
#define __TYPE_SYSTEM_H_C5C1C782_7518_411D_9399_445EEFD5AE46_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "slang/slang_abi.h"
#include "slang/forward_decls.h"
#include "slang/utils/list.h"
#include "slang/utils/allocator.h"
#include "slang/compiler/mangle.h"
#include "core/string.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////

namespace tycho
{
namespace slang
{
namespace types
{

	/// all things type related should live here
	//TODO : refactor type system
	class  TYCHO_SLANG_ABI type_system
	{
	public:
		static const type_info* get_arithmetic_conversion_type(const type_info *, const type_info *);		
	};
	
	/// base class of all types 
	class TYCHO_SLANG_ABI type_info : public utils::intrusive_list<type_info>
	{
	public:

		/// types available, bitfield to allow easy specification of multiple types
		enum type
		{
			t_invalid 		= 0,
			t_bool	  		= (1 << 0),
			t_int	  		= (1 << 1),
			t_uint	  		= (1 << 2),
			t_half	  		= (1 << 3),
			t_double	  	= (1 << 4),	
			t_float	  		= (1 << 5),
			t_vector	  	= (1 << 6),
			t_matrix	  	= (1 << 7),
			t_function	  	= (1 << 8),
			t_stateblock	= (1 << 9),
			t_cbuffer	  	= (1 << 10),
			t_pixelshader	= (1 << 11),
			t_vertexshader	= (1 << 12),
			t_shaderprofile	= (1 << 13),
			t_null	  		= (1 << 14),
			t_string	  	= (1 << 15),
			t_structure		= (1 << 16),
			t_texture1d		= (1 << 17),
			t_texture2d		= (1 << 18),
			t_texture3d		= (1 << 19),
			t_texture_cube	= (1 << 20),
			t_sampler1d	    = (1 << 21),
			t_sampler2d	    = (1 << 22),
			t_sampler3d	    = (1 << 23),
			t_sampler_cube  = (1 << 24),
			t_array			= (1 << 25),
			t_void			= (1 << 26),
			

			// useful groupings				
			t_integral_types = t_int | t_uint | t_bool,
			t_floating_types = t_half | t_float | t_double,
			t_texture_types  = t_texture1d | t_texture2d | t_texture3d | t_texture_cube,
			t_sampler_types  = t_sampler1d | t_sampler2d | t_sampler3d | t_sampler_cube
		};

		// type flags
		enum flag
		{
			f_none				  = 0,
			f_supports_arithmetic = (1 << 0),
			f_supports_comparison = (1 << 1)
		};
		
	public:
		/// constructor
		type_info(int f = f_none) : m_flags(f) {}
		
		/// \returns the type of this type_info. \ref type
		virtual type get_type() const
			{ TYCHO_ASSERT(!"type not specified for type_info"); return t_invalid; }
			
		/// get the name of the type, this is in valid hlsl syntax
		virtual const char* get_name() const = 0;
		
		/// get the mangled name of this type, by default just returns the name
		virtual const char* get_mangled_name() const
			{ return get_name(); }
									
		/// get the number of bytes this consumes
		virtual int get_byte_size() const
			{ return 0; }
		
		/// get the alignment in bytes this type requires
		virtual int get_alignment() const
			{ return 0; }
		
		/// returns non zero size if this is an array type
		virtual int get_array_size() const
			{ return 1; }
		
		/// check that an accessor is valid for this type.
		/// (i.e. foo.i, where i is a structure member or a swizzle .xyzw, .xxxx, etc)
		virtual const type_info* validate_accessor(compiler::symbol_table &, const char *) const
            { return nullptr; }
			
		/// check that an array style access is valid for this type
		/// \returns the element type of the array if access is successful or 0 otherwise
		virtual const type_info* validate_array_access(compiler::program *, const int*, int) const
            { return nullptr; }
		
		/// \returns true if this type can be converted to the passed type
		virtual bool can_convert_from(const type_info *type) const 
			{ return type->equals(this) || promotion_exists_from(type) || standard_conversion_exists_from(type); }
			
		/// \returns true if the passed type can be promoted to this type
		virtual bool promotion_exists_from(const type_info *) const
			{ return false; }
			
		/// \returns true if the passed type has a standard conversion to this type
		virtual bool standard_conversion_exists_from(const type_info *) const
			{ return false; }
			
		/// \returns true if the types are identical
		virtual bool equals(const type_info *t) const
			{ return t->get_type() == get_type(); }
					
		/// \returns true if this is a built in type
		virtual bool is_builtin() const 
			{ return false; }
			
		/// \returns true if this is a template type
		virtual bool is_template() const
			{ return false; }
			
		/// \returns true if this is an object type
		virtual bool is_object() const
			{ return false;	}
			
		/// \returns the most basic type, only different from default if this is a typedef node
		virtual const type_info* get_basic_type() const
			{ return this; }										
			
		/// \returns the mangled name without the prefix
		const char* get_undecorated_mangled_name() const
			{
				const char *mn = get_mangled_name();
				if(mn)
				{
					TYCHO_ASSERT(core::strlen(mn) > 2);
				}
				// just remove the preceding prefix
				return &mn[compiler::mangler::prefix_length];
			}
			
		/// \returns true if the type supports the requested flag
		bool has_flag(flag f) const
		{
			return (m_flags & f) > 0;				
		}				
		
	private:
		int m_flags;
	};
	
	/// debug checked cast operator
	template<class Dst>
	const Dst* safe_cast(const type_info *t)
	{
#if _DEBUG
		//TODO: check cast is valid in debug
#endif			
		return static_cast<const Dst*>(t);
	}

	/// debug checked cast operator
	template<class Dst>
	const Dst* safe_cast(type_info *t)
	{
#if _DEBUG
		//TODO: check cast is valid in debug
#endif			
		return static_cast<Dst*>(t);
	}

	/// void type
	struct TYCHO_SLANG_ABI void_type : public type_info
	{
		virtual const char* get_name() const { return "void"; }
		virtual type get_type() const { return t_void; }

		/// \returns void is special in that it can't even convert to itself.
		virtual bool can_convert_from(const type_info *) const 
			{ return false; }
	};
	
	/// basic builtin type
	class TYCHO_SLANG_ABI builtin_type : public type_info
	{
	public:
		builtin_type(int flags = f_none) : type_info(flags) {}
		virtual bool is_builtin() const 
			{ return true; }
	};	

	/// template type.
	class TYCHO_SLANG_ABI template_type : public builtin_type
	{
	public:
		/// constructor
		template_type(int flags = f_none) : builtin_type(flags) {}
		
		/// \returns true if this is a template type
		virtual bool is_template() const
			{ return true; }	
	};
	
	
	
	/// typedef of another type
	class TYCHO_SLANG_ABI typedef_type : type_info
	{
	public:
		typedef_type(const char *name, type_info *t) :
			m_name(name),
			m_type(t)
		{}
		
		virtual const char* get_name() const 
			{ return m_name; }
		
		virtual int get_byte_size() const
		 { return m_type->get_byte_size(); }

		virtual const type_info* get_basic_type() const
			{ return m_type->get_basic_type(); }
		
	private:
		const char* m_name;
		type_info*		m_type;
	};
	
	/// base of all object types
	class TYCHO_SLANG_ABI object_type : public type_info
	{
	public:
		object_type(const char *name) :
			m_name(name)
		{}
		
		/// \returns true if this is an object type
		virtual bool is_object() const
			{ return true;	}
	
		const char* get_name() const { return m_name; }			
	
	private:
		const char *m_name;
	};
	

	
	struct TYCHO_SLANG_ABI string_type : object_type
	{
		string_type() : object_type("string") {}
		virtual type get_type() const { return t_string; }
	};
	
	struct TYCHO_SLANG_ABI pixel_shader_type : object_type
	{
		pixel_shader_type() : object_type("pixelshader") {}			
		virtual type get_type() const { return t_pixelshader; }
	};

	struct TYCHO_SLANG_ABI vertex_shader_type : object_type
	{
		vertex_shader_type() : object_type("vertexshader") {}
		virtual type get_type() const { return t_vertexshader; }
	};

	struct TYCHO_SLANG_ABI state_block_type : object_type
	{
		state_block_type() : object_type("stateblock") {}
		virtual type get_type() const { return t_stateblock; }
	};

	struct TYCHO_SLANG_ABI null_type : type_info
	{
		virtual const char* get_name() const
			{ return "NULL"; }
		virtual type get_type() const { return t_null; }				
	};
	
	struct TYCHO_SLANG_ABI shader_profile_type : type_info
	{
		virtual const char* get_name() const
			{ return "shader_profile"; }		
		virtual type get_type() const { return t_shaderprofile; }
	};

} // end namespace
} // end namespace
} // end namespace

#endif // __TYPE_SYSTEM_H_C5C1C782_7518_411D_9399_445EEFD5AE46_
