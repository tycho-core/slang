//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:31:22 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "type_factory.h"
#include "slang/compiler/symbol_table.h"
#include "slang/compiler/program.h"
#include "core/string.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
	using namespace compiler;
	using namespace utils;

namespace types
{

	//--------------------------------------------------------------------

	TYCHO_SLANG_ABI const type_factory& get_type_factory(language_t lang)
	{
		static type_factory dx9(lang_hlsl_dx9);
		static type_factory dx10(lang_hlsl_dx10);
		switch(lang)
		{
			case lang_hlsl_dx9 : return dx9;
			case lang_hlsl_dx10 : return dx10;
			default : TYCHO_ASSERT(!"not implemented");
		}
		return dx9;
	}

    //--------------------------------------------------------------------

	type_factory::type_factory(language_t /*lang*/)
	{
		m_program = HLSL_NEW(program);
		
		// component types
		m_component_types.push_back(new half_type());
		m_component_types.push_back(new float_type());
		m_component_types.push_back(new double_type());
		m_component_types.push_back(new int_type());
		m_component_types.push_back(new uint_type());
		m_component_types.push_back(new bool_type());
		
		// object types
		m_object_types.push_back(new pixel_shader_type());
		m_object_types.push_back(new vertex_shader_type());
		m_object_types.push_back(new state_block_type());
		m_object_types.push_back(new texture_type());
		m_object_types.push_back(new texture_1d_type());
		m_object_types.push_back(new texture_2d_type());
		m_object_types.push_back(new texture_3d_type());
		m_object_types.push_back(new texture_cube_type());
		m_object_types.push_back(new sampler_type());
		m_object_types.push_back(new sampler_1d_type());
		m_object_types.push_back(new sampler_2d_type());
		m_object_types.push_back(new sampler_3d_type());
		m_object_types.push_back(new sampler_cube_type());
		m_object_types.push_back(new string_type());			
		
		// vector types
		{
			components::iterator it = m_component_types.begin();
			components::iterator eit = m_component_types.end();		
			for(; it != eit; ++it)
			{
				for(int i = 1; i < 5; ++i)
				{
					m_vector_types.push_back(new vector_type(m_program, *it, i));
				}
			}			
		}
					
		// matrix types
		{
			components::iterator it = m_component_types.begin();
			components::iterator eit = m_component_types.end();		
			for(; it != eit; ++it)
			{
				for(int i = 1; i < 5; ++i)
				{
					for(int j = 1; j < 5; ++j)
					{
						m_matrix_types.push_back(new matrix_type(m_program, *it, i, j));
					}
				}
			}			
		}

	}
	
    //--------------------------------------------------------------------
	
	type_factory::~type_factory()
	{
		HLSL_DELETE(m_program);
	}
	
    //--------------------------------------------------------------------

	const type_info* type_factory::find_type(const char *name) const
	{
		// special case for void
		if(core::strcmp(name, m_void_type.get_name()) == 0)
		{
			return &m_void_type;
		}
		
		// search component types
		{
			components::const_iterator it = m_component_types.begin();
			components::const_iterator eit = m_component_types.end();		
			for(; it != eit; ++it)
			{
				if(core::strcmp((*it)->get_name(), name) == 0)
					return *it;
			}			
		}

		// search object types
		{
			objects::const_iterator it = m_object_types.begin();
			objects::const_iterator eit = m_object_types.end();		
			for(; it != eit; ++it)
			{
				if(core::strcmp((*it)->get_name(), name) == 0)
					return *it;
			}			
		}
	
		return 0;
	}
	
    //--------------------------------------------------------------------

	const vector_type* type_factory::find_vector_type(const type_info *component_type, int size) const
	{
		//TODO: optimise, compute index in list
		vectors::const_iterator it = m_vector_types.begin();
		vectors::const_iterator eit = m_vector_types.end();		
		for(; it != eit; ++it)
		{
			const vector_type *v = *it;
			if(v->equals(component_type, size))
				return v;
		}
		return 0;		
	}
	
    //--------------------------------------------------------------------

	const matrix_type* type_factory::find_matrix_type(const type_info *component_type, int num_rows, int num_cols) const
	{
		//TODO: optimise, compute index in list
		matrices::const_iterator it = m_matrix_types.begin();
		matrices::const_iterator eit = m_matrix_types.end();		
		for(; it != eit; ++it)
		{
			const matrix_type *m = *it;
			if(m->equals(component_type, num_rows, num_cols))
				return m;
		}
		return 0;		
	}
	
    //--------------------------------------------------------------------

	template<class T>
	void create_identifiers_aux(const list<T> &l, symbol_table &table)
	{
		typename list<T>::const_iterator it = l.begin();
		typename list<T>::const_iterator end = l.end();
		while(it != end)
		{
			table.add_builtin_type(*it);
			++it;
		}
	}
	
    //--------------------------------------------------------------------

	void type_factory::create_identifiers(symbol_table &table) const
	{
		create_identifiers_aux(m_component_types, table);
		create_identifiers_aux(m_vector_types, table);
		create_identifiers_aux(m_matrix_types, table);
		create_identifiers_aux(m_object_types, table);
		table.add_builtin_type(&m_void_type);
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
