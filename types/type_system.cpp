//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:27 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "type_system.h"
#include "slang/compiler/symbol_table.h"
#include "slang/types/type_factory.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace types
{

	//--------------------------------------------------------------------

	const type_info* type_system::get_arithmetic_conversion_type(const type_info *t1, const type_info *t2)
	{
		// Implements subset of C99 : 6.3.1.8 Usual arithmetic conversions.
		if(!t1->has_flag(type_info::f_supports_arithmetic) || !t2->has_flag(type_info::f_supports_arithmetic))
		{
			return 0;
		}
		
		if(t1->equals(t2))
		{
			return t1;
		}						
		
		const type_info *ptype = 0;
					
		// if any operand is of a floating point type then the conversion type
		// is the type that has the greater precision
		if(t1->get_type() == type_info::t_double)
		{
			ptype = t1;
		}
		else if(t2->get_type() == type_info::t_double)
		{
			ptype = t2;
		}
		else if(t1->get_type() == type_info::t_float)
		{
			ptype = t1;
		}
		else if(t2->get_type() == type_info::t_float)
		{
			ptype = t2;
		}
		
		// not floating point so handle integral conversion
		else if(t1->get_type() == type_info::t_uint)
		{
			ptype = t1;
		}
		else if(t2->get_type() == type_info::t_uint)
		{
			ptype = t2;
		}
		
		if(ptype == 0)
		{
			return 0;
		}
		
		if(!ptype->can_convert_from(ptype == t1 ? t2 : t1))
			return 0;
			
		return ptype;
	}		


    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
