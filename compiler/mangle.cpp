//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:36 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "mangle.h"
#define TEST_MANGLE 0

#if TEST_MANGLE
#	include "slang/compiler/hlsl_program.h"
#	include "slang/types/array_type.h"
#	include "slang/types/vector_type.h"
#	include "slang/types/matrix_type.h"
#	include "slang/types/component_types.h"
#endif



//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace compiler
{
namespace mangler
{

    //--------------------------------------------------------------------


#if TEST_MANGLE
	TYCHO_SLANG_ABI void test_mangle()
	{
		using namespace compiler;

		typedef list<component_type> components;
		components component_types;
		program p;
				
		component_types.push_back(new half_type());
		component_types.push_back(new float_type());
		component_types.push_back(new double_type());
		component_types.push_back(new int_type());
		component_types.push_back(new uint_type());
		component_types.push_back(new bool_type());
		
		// array
		{
			int dimensions[] = { 1, 2, 3 };
			array_type at(&p, *component_types.begin(), dimensions, 3);
			printf("%s : %s\n", at.get_name(), at.get_mangled_name());
		}
		
		// vectors
		{
			components::iterator it = component_types.begin();
			components::iterator end = component_types.end();
			
			for(; it != end; ++it)
			{
				for(int i = 1; i < 5; ++i)
				{				
					vector_type vt(&p, *it, i);
					printf("%s : %s\n", vt.get_name(), vt.get_mangled_name());
				}			
			}
		}
		
		// matrices
		{
			components::iterator it = component_types.begin();
			components::iterator end = component_types.end();
			
			for(; it != end; ++it)
			{
				for(int i = 1; i < 5; ++i)
				{				
					for(int j = 1; j < 5; ++j)
					{				
						matrix_type t(&p, *it, i, j);
						printf("%s : %s\n", t.get_name(), t.get_mangled_name());
					}
				}			
			}		
		}		
	}
#endif 

    //--------------------------------------------------------------------

} // end namespace
} // end namespace
} // end namespace
} // end namespace
