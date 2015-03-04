//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:28:57 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "context_check_dx10.h"
#include "slang/types/structure_type.h"

//TODO : Technique checking
//TODO : Pass checking
//TODO : Constant buffer checking
//TODO : Default function parameter support
//TODO : Constructor support
//TODO : lvalue checks
//TODO : function semantics
//TODO : parameter semantics

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace compiler
{
	using namespace ast;
	using namespace types;

    //--------------------------------------------------------------------
	
	/// builds a succinct description of all parameters in the passed list
	bool context_checker_dx10::make_param_list(program *p, parameter_list_node *params, types::function_type::param_list &list, const char **names, int max_names)
	{		
		bool ok = true;	
		if(params)
		{
			parameter_list_node::list_type::iterator it = params->begin();
			parameter_list_node::list_type::iterator end = params->end();

			for(; it != end; ++it)
			{
				parameter_node *param = *it;
				TYCHO_ASSERT(param);
				const char *name = 0;
				if(param->var)
				{
					name = param->var->var->id->name;;
				}
				
				// parameters must be named
				if(!name)
				{
					p->add_message(mt_function_param_not_named, param->get_token().pos);
				}
				
				usage_t usage = usage_none;
				usage_list_node *usages = param->type->usages;
				if(usages)
				{			
					usage_list_node::list_type::iterator it = usages->begin();
					usage_list_node::list_type::iterator end = usages->end();

					for(; it != end; ++it)
					{
						const usage_node *usage_node = *it;
						usage |= usage_node->usage;
					}					
				}
				
				// check we don't have conflicting usages
				//TODO : more rigorously check parameter usages to check for invalid ones in this context and for conflicting ones
				if(usage & usage_param_in && usage & usage_param_out)
				{
					p->add_message(mt_conflicting_param_usage, params->get_token().pos, name);
					ok = false;
				}
				
				// get the type of this parameter
				const types::type_info *t = evaluate_type(p, param);
				if(t)
				{
					function_type::param *param = HLSL_ALLOC(p->get_allocator(), function_type::param);
					param->m_type = t;
					param->m_usage = usage;
					param->m_semantic = semantic_none;					
					list.push_back(param);
					*names++ = name;
				}					
			}								
		}		
		
		return ok;
	}		
	
	/// check a typedef node and add all names to the symbol table
	/// in the current scope
	bool context_checker_dx10::check_typedef(program *p, node *n_)
	{
		if(!n_->isa(node::t_typedef))
			return false;
			
		typedef_node *n = (typedef_node*)n_;
						
		// find basic type			
		const types::type_info *basic_type = basic_type_checker::evaluate(p, n->base_type->type);
		if(basic_type)
		{
			// add all names to the symbol table
			variable_dim_list_node *list = n->names;
			variable_dim_list_node::list_type::iterator it = list->begin();
			variable_dim_list_node::list_type::iterator end = list->end();

			for(; it != end; ++it)
			{	
				variable_dim_node *n =  *it;		
				const char *name = n->id->name;
				
				// evaluate the full type for this name
				const types::type_info* full_type = basic_type;
				
				// if we're an array type then validate all dimensions evaluate to ICE's and instantiate a full type
				const size_t num_dimensions = n->dimensions->size();
				if(num_dimensions)
				{
					// validate all dimension initialisers evaluate to a ICE and get their values
					int *dimensions = HLSL_ALLOC_STACK_ZERO(int, num_dimensions);
					bool ok = evaluate_constant_tree(p, n->dimensions, dimensions);				
					if(ok)
					{
						full_type = p->get_symbol_table()->instantiate_array_type(basic_type, dimensions, (int)num_dimensions);						
					}							
				}
				
				
				// check for duplicate definitions
				if(p->get_symbol_table()->identify(name, true))
				{
					p->add_message(mt_duplicate_type_definition, n->get_token().pos, name);
				}
				else
				{
					identifier_info *iinfo = p->get_symbol_table()->add_type(name, full_type, n);
					TYCHO_ASSERT(iinfo);
					n->set_type_info(iinfo->type_id);
					n->set_id_info(iinfo);
					n->id->set_id_info(iinfo);
					n->id->set_type_info(iinfo->type_id);
				}
			}								
		}
					
		return true;		
	}
	

	bool context_checker_dx10::check_var_struct_decl(program *p, node *n_)
	{
		TYCHO_ASSERT(n_->isa(node::t_var_struct_decl));
		symbol_table *st = p->get_symbol_table();
		var_struct_decl_node *n = (var_struct_decl_node*)n_;

		// check the struct definition
		context_checker_dx10::struct_checker(p, n->s, n);
		const types::type_info *type = n->s->get_type_info();
		if(type)
		{
			if(n->vars)
			{
				variable_list_node::list_type::iterator it = n->vars->begin();
				variable_list_node::list_type::iterator end = n->vars->end();
				for(; it != end; ++it)
				{
					variable_node *var = *it;
					if(var)
					{
						const types::type_info* var_type = evaluate_type(p, var, type);
						const char* var_name = var->var->id->name;
						if(var_type && var_name)
						{
							st->add_variable(var_name, var_type, var);
						}
					}
				}			
			}
		}
		return true;
	}


	const types::type_info* context_checker_dx10::evaluate_type(program *p, ast::parameter_node *n)
	{
		if(n->get_type_info())
			return n->get_type_info();
			
		if(n->type && n->type->type)
		{
			// evaluate basic type
			const types::type_info *full_type = basic_type_checker::evaluate(p, n->type->type);
			if(full_type && n->var)
			{
				evaluate_type(p, n->var, full_type);
				full_type = n->var->get_type_info();
			}
			n->set_type_info(full_type);
		}
		return n->get_type_info();
	}
			
	const types::type_info* context_checker_dx10::evaluate_type(program *p, ast::variable_dim_node *n, const types::type_info *base_type)
	{
		// check base type
		const types::type_info *component_type = base_type;
		if(!component_type)
			return 0;
						
		// if this is an array type check the dimensions are within bounds
		const size_t num_dimensions = n->dimensions->size();
		if(num_dimensions)                                
		{								
			// validate all dimension initialisers evaluate to a ICE and get their values
			int *dimensions = HLSL_ALLOC_STACK_ZERO(int, num_dimensions);
			bool ok = evaluate_constant_tree(p, n->dimensions, dimensions);				
			if(ok)
			{
				n->set_type_info(p->get_symbol_table()->instantiate_array_type(component_type, dimensions, (int)num_dimensions));						
			}
		}
		else
		{
			n->set_type_info(base_type);				
		}				
		return n->get_type_info();
	}	

	const types::type_info* context_checker_dx10::evaluate_type(program *p, ast::variable_node *n, const types::type_info *base_type)
	{
		const types::type_info *lhs_type = evaluate_type(p, n->var, base_type);			
		if(!lhs_type)
		{
			n->set_type_info(0);
			return n->get_type_info();
		}
						
		// check that initialiser can be converted to this type				
		if(n->initialiser)
		{
			expression_list_checker(p, n->initialiser, n);
			const types::type_info *rhs_type = n->initialiser->get_type_info();
			
			// something really bad failed, ICE time
			if(rhs_type == 0)
			{
				p->add_message(mt_type_deduction_failed, n->initialiser->get_token().pos, n->initialiser->get_token().str);
				n->set_type_info(0);
				return 0;
			}
			
			if(rhs_type->get_basic_type() == lhs_type->get_basic_type())
			{
				n->set_type_info(lhs_type);
				return n->get_type_info();
			}
			
			if(!compiler::type_system::get_arithmetic_conversion_type(lhs_type, rhs_type))
			{
				p->add_message(mt_assign_invalid_type, n->initialiser->get_token().pos, lhs_type->get_name(), rhs_type->get_name());
				n->set_type_info(0);
				return 0;
			}
			
			// implicit conversion exists, if expression is not cast expression then warn but allow			
			if(n->initialiser->get_type() != ast::node::t_cast_expression)
			{
				p->add_message(mt_warn_implicit_conversion, 
					n->initialiser->get_token().pos,
					rhs_type->get_name(), 
					lhs_type->get_name());				
			}
			
			n->initialiser = ast::make_cast_expression(p, lhs_type, n->initialiser);
			n->initialiser->set_type_info(lhs_type);
		}	
		n->set_type_info(lhs_type);				
		return n->get_type_info();		
	}
	
	bool context_checker_dx10::check_variable_decl(program *p, ast::node *n)
	{
		if(!n->isa(node::t_variable_decl))
			return false;
			
		ast::variable_decl_node *node = (ast::variable_decl_node*)n;
		
		// evaluate the base type of all the declarations					
		const types::type_info *base_type = basic_type_checker::evaluate(p, node->type->type);
		if(base_type)
		{
			// evaluate all the variables types add all names to the symbol table
			variable_list_node *list = node->variables;
			variable_list_node::list_type::iterator it = list->begin();
			variable_list_node::list_type::iterator end = list->end();
			for(; it != end; ++it)
			{	
				variable_node *n =  *it;
				const char *name = n->var->id->name;
				evaluate_type(p, n, base_type);
				if(!n->get_type_info())
				{
					p->add_message(mt_type_deduction_failed, n->get_token().pos, name);
					continue;
				}
				// check to see if this name is already defined in the current scope
				if(p->get_symbol_table()->identify(name, true))
				{
					p->add_message(mt_duplicate_definition, n->get_token().pos, name);
				}
				else
				{
					identifier_info *iinfo = p->get_symbol_table()->add_variable(name, n->get_type_info(), n);					
					n->set_id_info(iinfo);			
					node->set_id_info(iinfo);		
				}				
			}
		}
		return false;
	}

	bool context_checker_dx10::evaluate_constant_tree(compiler::program *p, ast::expression_list_node *n, int *results)
	{
		bool ok = true;
		expression_list_node::list_type::iterator it = n->begin();
		expression_list_node::list_type::iterator end = n->end();
		int i = 0;
		for(; it != end; ++it, ++i)
		{
			expression_node *cur_expr = *it;
			compiler::context_checker_dx10::expression_list_checker(p, cur_expr, n);
			if(cur_expr->get_type_info() != 0)
			{
				if(!ast::evaluate_constant_tree(p, cur_expr, &results[i]))
					ok = false;
			}
		}
		return ok;		
	}
			
	bool context_checker_dx10::check_convertable_to_bool(program *p, const types::type_info *type)
	{
		if(!type)
			return false;
			
		//OPT : avoid looking this up all the time
		const types::type_info *bool_type = p->get_symbol_table()->identify_type("bool");
		TYCHO_ASSERT(bool_type);
		return !bool_type->can_convert_from(type);
	}
				
	context_checker_dx10::context_checker_dx10(program *p) :
		m_program(p)
	{
		// clear the symbol table as a complete version will be generated during checking
		p->get_symbol_table()->clear();
		p->get_symbol_table()->push_scope("$Global");
		global_checker checker(p);
		declaration_list_node *decls = p->get_ast()->declarations;
		declaration_list_node::list_type::iterator it = decls->begin();
		declaration_list_node::list_type::iterator end = decls->end();
		for(; it != end; ++it)
			checker.accept(*it, 0);
	}

	//////////////////////////////////////////////////////////////////////////
	/// Basic Type checker
	//////////////////////////////////////////////////////////////////////////
	context_checker_dx10::basic_type_checker::basic_type_checker(program *p, ast::node *n) :
		checker(p)
	{
		n->accept(*this, 0);
	}

	const types::type_info* context_checker_dx10::basic_type_checker::evaluate(program *p, ast::node *n)
	{
		basic_type_checker(p, n);
		return n->get_type_info();
	}

	visitor::result context_checker_dx10::basic_type_checker::unhandled_node(node *n, ast::node *)
	{
		m_program->add_message(mt_invalid_basic_type_nodes, n->get_token().pos, n->get_node_name());		
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::basic_type_checker::visit(ast::type_id_node *n, ast::node*)
	{
		identifier_info *iinfo = m_program->get_symbol_table()->identify(n->type_name);
		if(iinfo)
		{
			n->set_id_info(iinfo);
			n->set_type_info(iinfo->type_id);			
		}
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::basic_type_checker::visit(ast::scalar_type_node *n, ast::node*)
	{
		identifier_info *iinfo = m_program->get_symbol_table()->identify(n->string());
		if(iinfo)
		{
			n->set_id_info(iinfo);
			n->set_type_info(iinfo->type_id);			
		}
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::basic_type_checker::visit(ast::matrix_type_node *mn, ast::node*)
	{
		matrix_type_node *rn = mn;
		expression_node *n = rn->num_rows;
		expression_node *n1 = rn->num_columns;
		scalar_type_node *t = rn->type;
		int num_rows = 0, num_cols = 0;
		if(ast::evaluate_constant_tree(m_program, n, &num_rows) && ast::evaluate_constant_tree(m_program, n1, &num_cols))
		{
			//TODO : is there a valid range for the size?
			const types::type_info *component_type = m_program->get_symbol_table()->identify_type(t->query_type_name());		
			if(component_type)
			{
				const types::type_info *type = m_program->get_symbol_table()->instantiate_matrix_type(component_type, num_rows, num_cols);					
				if(type)
				{
					mn->set_type_info(type);
				}
				else
				{
					//TODO: ERROR
				}
			}
			else
			{
				//TODO: ERROR
			}
		}			

		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::basic_type_checker::visit(ast::vector_type_node *vn, ast::node*)
	{
		expression_node *n = vn->size_expr;
		scalar_type_node *t = vn->type;
		int size = 0;
		if(ast::evaluate_constant_tree(m_program, n, &size))
		{
			//TODO : is there a valid range for the size?
			vn->size = size;
			const types::type_info *component_type = m_program->get_symbol_table()->identify_type(t->query_type_name());		
			if(component_type)
			{
				const types::type_info *type = m_program->get_symbol_table()->instantiate_vector_type(component_type, size);
				if(type)
				{
					vn->set_type_info(type);
				}
				else
				{
					//TODO: ERROR
				}
			}
			else
			{
				//TODO: ERROR
			}
		}
	
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::basic_type_checker::visit(ast::object_type_node*, ast::node*)
	{
		return visit_abort_subtree;
	}
	
	//////////////////////////////////////////////////////////////////////////
	/// Global checker
	//////////////////////////////////////////////////////////////////////////
	
	visitor::result context_checker_dx10::global_checker::unhandled_node(node *n, ast::node *)
	{
		// generate an error, this type of definition not allowed in the global scope
		m_program->add_message(mt_invalid_node_at_global_scope, n->get_token().pos, n->get_node_name());		
		return visit_abort_subtree;
	}
	
	visitor::result context_checker_dx10::global_checker::visit(function_decl_node *n, node *parent)
	{
		function_checker(m_program, n, parent);
		return visit_abort_subtree;
	}
	
	visitor::result context_checker_dx10::global_checker::visit(ast::var_struct_decl_node *n, ast::node *)
	{
		check_var_struct_decl(m_program, n);
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::global_checker::visit(ast::variable_decl_node *n, ast::node *)
	{
		check_variable_decl(m_program, n);
		return visit_abort_subtree;
	}
	
	visitor::result context_checker_dx10::global_checker::visit(ast::typedef_node *n, ast::node *)
	{	
		check_typedef(m_program, n);
		return visit_abort_subtree;
	}
	
	visitor::result context_checker_dx10::global_checker::visit(ast::cbuffer_node *, ast::node *)
	{
		//TODO : add cbuffer context checker
		return visit_abort_subtree;
	}

	//////////////////////////////////////////////////////////////////////////
	/// Structure checker
	//////////////////////////////////////////////////////////////////////////
	context_checker_dx10::struct_checker::struct_checker(program *p, ast::struct_node *fn, ast::node *) :
		checker(p)
	{
		TYCHO_ASSERT(p);
		TYCHO_ASSERT(fn);
		
		if(fn->id)
		{
			const char* name = fn->id->name;
			if(name)
			{						
				// validate name and add to symbol table					
				m_iinfo = m_program->get_symbol_table()->identify(name, true);
				if(m_iinfo)						
				{
					// error : already defined. make up a name and carry on trying to find more
					// errors.
					m_program->add_message(mt_duplicate_struct_definition, fn->get_token().pos, name);
				}
				else
				{
					// add the structure name to the symbol table
					m_iinfo = m_program->get_symbol_table()->add_structure(name, fn);
					fn->set_type_info(m_iinfo->type_id);
				}
				// check the declaration within the struct
				if(fn->decls)
				{
					variable_decl_list_node::list_type::iterator it = fn->decls->begin();
					variable_decl_list_node::list_type::iterator end = fn->decls->end();
					for(; it != end; ++it)
						visit(*it, fn);
				}
			}
		}						
	}
	
	visitor::result context_checker_dx10::struct_checker::visit(ast::variable_decl_node *node, ast::node *)
	{
		symbol_table *st = m_program->get_symbol_table();
		structure_type *structure = const_cast<structure_type*>(compiler::safe_cast<structure_type>(m_iinfo->type_id));

		// add all names to the symbol table
		variable_list_node *list = node->variables;
		check_variable_decl(m_program, node);
		variable_list_node::list_type::iterator it = list->begin();
		variable_list_node::list_type::iterator end = list->end();
		for(; it != end; ++it)
		{	
			variable_node *n = *it;
			
			// n may be null due to errors encountered passing the variables
			if(n && n->var)
			{
				const char *name = n->var->id->name;
				
				// check to see if the name is already defined in this structure
				if(structure->validate_accessor(*st, name))
				{
					m_program->add_message(mt_duplicate_definition, n->var->id->get_token().pos, name);
				}
				else
				{
					const type_info* type = n->get_type_info();
					if(!type)
					{
						m_program->add_message(mt_invalid_type, n->get_token().pos, n->var->id->name);
					}
					else
					{
						structure->add_member(name, type);																	
						//TODO : validate semantics
					}
				}
			}
		}	
		return visit_abort_subtree;
	}
		
	visitor::result context_checker_dx10::struct_checker::unhandled_node(ast::node *n, ast::node *)
	{			
		// generate an error, this type of definition not allowed in the structure scope
		m_program->add_message(mt_invalid_node_at_structure_scope, n->get_token().pos, n->get_node_name());
		return visit_abort_subtree;
	}
		
	//////////////////////////////////////////////////////////////////////////
	/// Function checker
	//////////////////////////////////////////////////////////////////////////
			
	context_checker_dx10::function_checker::function_checker(program *p, function_decl_node *fdn, ast::node *) : 
		checker(p) 
	{					
		symbol_table *st = p->get_symbol_table();
		
		// check to see if the function is already defined, this is more complex than
		// just checking the name as we can have overloaded functions so we need to find
		// all functions with this name and check the parameter types are different.
		function_node *fn = fdn->function;
		function_type::param_list *params = HLSL_ALLOC(p->get_allocator(), function_type::param_list);
		const char *param_names[compiler::max_parameters] = { 0 };
		if(context_checker_dx10::make_param_list(p, fn->params, *params, param_names, compiler::max_parameters))
		{
			const types::type_info *return_type = basic_type_checker::evaluate(p, fn->usage->type);
			if(return_type)
			{
				const function_type *type = p->get_symbol_table()->instantiate_function_type(return_type, params);
				if(type)
				{
					identifier_info *iinfo = p->get_symbol_table()->identify(fn->name->name);
					bool has_errors = false;
					if(iinfo)
					{
						bool is_dup = false;
						if(iinfo->type_id == type)
						{
							is_dup = true;
						}
						else
						{
							identifier_info *sinfo = iinfo->sibling;
							while(sinfo)
							{
								if(sinfo->type_id == type)
								{
									is_dup = true;
									break;	
								}
								sinfo = sinfo->next;
							}								
						}	
						if(is_dup)
						{
							p->add_message(mt_duplicate_function_definition, fn->get_token().pos, fn->name->name);
							has_errors = true;
							// carry on and try and find as many errors as we can in the body
						}												
					}										
					
					if(!has_errors)
					{
						// add the function to the symbol table before processing body in case it is 
						// referenced recursively.
						symbol_table *st = p->get_symbol_table();
						iinfo = st->add_function(iinfo, fn->name->name, fn);							
						iinfo->type_id = type;							
						fn->set_type_info(type);
						fn->set_id_info(iinfo);
						fdn->set_type_info(type);
						fdn->set_id_info(iinfo);
					}

					//TODO : validate return type
					
					//TODO : validate storage class (static, inline, target)
					if(fn->usage && fn->usage->usages)
					{
						usage_list_node *storage_types = fn->usage->usages;
						usage_list_node::list_type::iterator it = storage_types->begin();
						usage_list_node::list_type::iterator end = storage_types->end();
						for(; it != end; ++it)
						{
							usage_node *storage_type = *it;
							TYCHO_ASSERT(storage_type);
							switch(storage_type->usage)
							{
								case usage_inline : break;
								case usage_static : break;
								case usage_target : break;
								
								default :
									p->add_message(mt_invalid_function_storage_type, fn->get_token().pos, fn->name->name, storage_type->string());
							}
						}				
					}
			
					// push a new scope onto the symbol table stack
					st->push_scope(fn->name->name);
					
					// add all parameters to the symbol table
					if(params)
					{
						function_type::param_list::iterator it = params->begin(),
															end = params->end();
															
						for(int i = 0; it != end; ++it, ++i)
						{
							if(param_names[i])
							{
								st->add_variable(param_names[i], (*it)->m_type, 0);
							}								
						}
					}			
					
					// validate body
					if(fdn->statements)
						statement_list_checker(m_program, fdn->statements, fdn, fdn, false);

					// pop scope from the symbol table
					st->pop_scope();				
				}	
			}				
		}			
	}		

	visitor::result context_checker_dx10::function_checker::visit(ast::node *, ast::node *)
	{			
		return visit_continue;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// statement_list checker
	//////////////////////////////////////////////////////////////////////////

	context_checker_dx10::statement_list_checker::statement_list_checker(program *p, 
		
		ast::statement_list_node *sln, 
		ast::function_decl_node *func,
		ast::node *parent, bool supress_scope) :
		checker(p),
		m_function(func)
	{
		if(sln)
		{
			// each statement list opens a new scope
			if(!supress_scope)
				p->get_symbol_table()->push_scope("$StatementBlock");

			statement_list_node::list_type::iterator it = sln->begin();
			statement_list_node::list_type::iterator end = sln->end();
			for(; it != end; ++it)
				visitor_type::accept(*it, parent);		
			if(!supress_scope)
				p->get_symbol_table()->pop_scope();			
		}
	}
	
	context_checker_dx10::statement_list_checker::statement_list_checker(program *p, 
		ast::statement_node *sn, ast::function_decl_node *func, ast::node *parent, bool supress_scope) :
		checker(p),
		m_function(func)
	{
		// each statement list opens a new scope
		if(!supress_scope)
			p->get_symbol_table()->push_scope("$StatementBlock");
			
		// bit of a hack but avoid creating a new check if this is a statement_list_node as not doing
		// so will open a new scope and does not honour the supress_scope parameter
		if(sn->get_type() == node::t_statement_list)
		{				
			statement_list_node *sln = (statement_list_node*)(sn);
			statement_list_node::list_type::iterator it = sln->begin();
			statement_list_node::list_type::iterator end = sln->end();
			for(; it != end; ++it)
				visitor_type::accept(*it, parent);					
		}
		else
		{
			visitor_type::visit(sn, parent);					
		}
		
		if(!supress_scope)
			p->get_symbol_table()->pop_scope();
	}
			
	visitor::result context_checker_dx10::statement_list_checker::visit(ast::empty_statement_node *, ast::node *)
	{
		// no - op
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::for_statement_node *fn, ast::node *)
	{
		// open a new scope so initialisers do not leak
		m_program->get_symbol_table()->push_scope("$ForLoop");
				
		// validate parts
		if(fn->initialiser)
		{
			expression_list_checker(m_program, fn->initialiser, fn);
		}
		if(fn->step)
		{
			expression_list_checker(m_program, fn->step, fn);			
		}
					
		if(fn->condition)
		{
			expression_list_checker(m_program, fn->condition, fn);
			check_convertable_to_bool(m_program, fn->condition->get_type_info());			
		}
		
		// validate body
		statement_list_checker(m_program, fn->statement, m_function, fn, true);
		m_program->get_symbol_table()->pop_scope();
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::do_while_statement_node *n, ast::node *)
	{		
		expression_list_checker(m_program, n->expression, n);
		check_convertable_to_bool(m_program, n->get_type_info());
		
		// validate body
		statement_list_checker(m_program, n->statement, m_function, n, false);
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::while_statement_node *n, ast::node *)
	{
		// validate condition
		
		// validate body
		statement_list_checker(m_program, n->statement, m_function, n, false);
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::if_else_statement_node *n, ast::node *parent)
	{
		expression_list_checker(m_program, n->condition, parent);
		check_convertable_to_bool(m_program, n->get_type_info());
		
		// validate if / else statements
		statement_list_checker(m_program, n->if_statement, m_function, n, false);
		if(n->else_statement)
			statement_list_checker(m_program, n->else_statement, m_function, n, false);			
		
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::return_statement_node *n, ast::node *)
	{
		//TODO : Diagnose when a function has not returned a value when it should
		
		// validate return expression if present
		const types::type_info *return_type = 0;
		if(n->expression)
		{
			expression_list_checker(m_program, n->expression, n);				
			return_type = n->expression->get_type_info();
		}

		const function_type *ft = compiler::safe_cast<function_type>(m_function->get_type_info());
		const types::type_info *func_return_type = ft->return_type();

		if(func_return_type->get_type() == type_info::t_void)				
		{
			if(n->expression)
			{
				m_program->add_message(mt_void_return, n->get_token().pos, m_function->function->name->string());
			}
		}
		else
		{						
			// get the function type of the function we are returning from
			if(return_type)
			{					
				// validate expression converts to function return type, if the function
				// returns void then check there is no expression.
				if(!func_return_type->can_convert_from(return_type))
				{
					m_program->add_message(mt_illegal_cast, n->get_token().pos, func_return_type->get_name(), return_type->get_name());
				}
			}
			else
			{
				m_program->add_message(mt_function_must_return_a_value, n->get_token().pos, m_function->get_id_info()->name);
			}
		}
		
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::declaration_statement_node *, ast::node *)
	{
		// just burrow into the declaration statement
		return visit_continue;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::typedef_node *n, ast::node *)
	{
		check_typedef(m_program, n);
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::variable_decl_node *n, ast::node *)
	{
		check_variable_decl(m_program, n);
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::var_struct_decl_node *n, ast::node *)
	{
		check_var_struct_decl(m_program, n);
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::expression_statement_node *n, ast::node *)
	{
		expression_list_checker(m_program, n->expression, n);
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::statement_list_checker::visit(ast::statement_list_node *n, ast::node *parent)
	{
		statement_list_checker(m_program, n, m_function, parent, false);
		return visit_abort_subtree;
	}
	
	visitor::result context_checker_dx10::statement_list_checker::unhandled_node(ast::node *n, ast::node *)
	{
		// generate an error, this type of definition not allowed here
		m_program->add_message(mt_invalid_node_in_statement_list, n->get_token().pos, n->get_node_name());		
		return visit_abort_subtree;
	}

	//////////////////////////////////////////////////////////////////////////
	// expression_list checker
	//////////////////////////////////////////////////////////////////////////

	context_checker_dx10::expression_list_checker::expression_list_checker(program *p, ast::node *sln, ast::node *parent) :
		checker(p)
	{		
		visitor_type::accept(sln, parent);
	}

	visitor::result context_checker_dx10::expression_list_checker::unhandled_node(ast::node *n, ast::node *)
	{			
		// generate an error, this type of definition not allowed here
		m_program->add_message(mt_invalid_node_evaluating_expression, n->get_token().pos, n->get_node_name());
		return visit_abort_subtree;
	}		

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::expression_list_node *n, ast::node *parent)
	{
		expression_list_node *el = (expression_list_node*)n;
		expression_list_node::list_type::iterator it = el->begin();
		expression_list_node::list_type::iterator end = el->end();
		for(; it != end; ++it)
			accept(*it, parent);
			
		// set type and identifier info of the list to that of the first element
		if(!el->empty())
		{
			n->set_type_info((*el->begin())->get_type_info());
			n->set_id_info((*el->begin())->get_id_info());
		}			
							
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::postfix_member_expression_node *rn, ast::node *)
	{
		// depending on the type this could either be a structure member access or a swizzle
		// we need to detect which and check appropriately so we need to figure whether this
		// converts to something we can swizzle or something we can get a member of.
		expression_node *expression = rn->expression;
		non_type_id_node *id = rn->member;

		expression_list_checker(m_program, expression, rn);
		const types::type_info *t = expression->get_type_info();
		if(t)
		{
			const types::type_info *type = t->validate_accessor(*m_program->get_symbol_table(), id->name);
			if(type)
			{
				id->set_type_info(type);
				rn->set_type_info(type);
			}
			else
			{
				m_program->add_message(mt_invalid_accessor, id->get_token().pos, id->name, t->get_name());
			}
		}
		else
		{
			m_program->add_message(mt_invalid_accessor, id->get_token().pos, id->name, "");
		}
				
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::function_call_node *rn, ast::node *)
	{
		// lookup the function name in the symbol table 
		non_type_id_node *id = rn->id;
		identifier_info *ii = m_program->get_symbol_table()->identify(id->name);
		if(!ii)
		{
			m_program->add_message(mt_undeclared_function, id->get_token().pos, id->name);
			return visit_abort_subtree;
		}	
		else if(ii->type != it_function && ii->type != it_intrinsic_function)
		{
			m_program->add_message(mt_identifier_not_a_function, id->get_token().pos, id->name);
			return visit_abort_subtree;
		}		
		id->set_id_info(ii);
										
		// get types of all parameters 
		bool ok = true;
		const types::type_info* arg_types[compiler::max_parameters];
		int num_src_params = 0;
		if(rn->args)
		{
			expression_list_node::list_type::iterator it = rn->args->begin();
			expression_list_node::list_type::iterator end = rn->args->end();
			for(; it != end; ++it, ++num_src_params)
			{
				expression_list_checker(m_program, *it, rn->args);
				arg_types[num_src_params] = (*it)->get_type_info();
				if(!arg_types[num_src_params])
				{
					ok = false;
				}
			}			
		}

		// arguments didn't check out ok so don't try to resolve the function.
		if(!ok)
			return visitor::visit_abort_subtree;
		
		// resolve call to a single function or error if not applicable function or if it is ambiguous
		// we keep a running track of the current best match as we iterate over the functions, if we encounter
		// a situation where it is ambiguous we remember that until we get a better match or finish iterating
		// and then issue a diagnostic.			
		identifier_info *func_id = ii;
		int best_num_promotions = -1;
		int best_num_exact = -1;
		int best_num_conversions = -1;
		identifier_info *best_func = 0;
		const function_type *best_ft = 0;
		bool ambiguous = false;
		while(func_id)
		{
			const function_type *ft = compiler::safe_cast<function_type>(func_id->type_id);
			const function_type::param_list *params = ft->get_params();
			int num_params = 0;
			if(params)
				num_params = (int)params->size();
			bool candidate = true;
			if(num_params == num_src_params)
			{
				int num_promotions = 0;
				int num_exact = 0;
				int num_conversions = 0;
				
				if(num_params)
				{
					// check types, we track how each param converts and use it to reduce the 
					// applicable functions until only one remains, if more than one is applicable
					// then a ambiguous function error is issued.
					function_type::param_list::const_iterator it = params->begin();
					for(int i = 0; i < ft->num_params(); ++i, ++it)
					{		
						const types::type_info *ptype = (*it)->m_type;
						
						// check for exact match
						if(ptype == arg_types[i])
							++num_exact;
						
						// check for promotion							
						else if(ptype->promotion_exists_from(arg_types[i]))
							++num_promotions;														
														
						// check for standard conversion
						else if(ptype->standard_conversion_exists_from(arg_types[i]))
							++num_conversions;
							
						// no conversion exists, disregard this function
						else
						{
							candidate = false;
							break;
						}
					}									
				}
				
				if(candidate)
				{
					if(best_func == 0)
					{
						best_num_exact = num_exact;
						best_num_promotions = num_promotions;
						best_num_conversions = num_conversions;
						best_func = func_id;
						best_ft = ft;
					}
					else
					{
						// remember if the last match was ambiguous
						bool is_ambiguous = false;
						
						// check standard conversions
						bool replace = false;
						if(best_num_conversions > num_conversions)
						{
							replace = true;
						}
						else if(best_num_conversions == num_conversions)
						{
							// check promotions
							if(best_num_promotions > num_promotions)
							{
								replace = true;
							}
							else if(best_num_promotions == num_promotions)
							{
								// must be ambiguous
								replace = true;
								is_ambiguous = true;
							}
						}
						
						if(replace)
						{
							best_num_exact = num_exact;
							best_num_promotions = num_promotions;
							best_num_conversions = num_conversions;
							best_func = func_id;
							ambiguous = is_ambiguous;
							
							// early out on exact match
							if(num_exact == num_params)
								break;
						}
					}
				}
			}				
			func_id = func_id->sibling;
		}
		
		if(ambiguous)
		{
			m_program->add_message(mt_ambiguous_function_call, rn->get_token().pos, id->name);
		}
		else if(best_func)
		{
			// remember the function we resolved to in the function_call node
			rn->resolved_call = best_func;
			
			// our type is that of the function return type				
			rn->set_type_info(best_ft->return_type());
			rn->set_id_info(best_func);
			id->set_type_info(best_func->type_id);
			//TODO : update the caller / callee graphs for this function
			
		}
		else
		{
			m_program->add_message(mt_no_function_overload, rn->get_token().pos, id->name);			
		}
							
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::constructor_expression_node *n, ast::node *)
	{
		// evaluate all parameter expressions
		const type_info* arg_types[compiler::max_parameters];
		size_t num_args = 0;
		bool error = false;
		if(n->args)
		{
			expression_list_node::list_type::iterator it = n->args->begin();
			expression_list_node::list_type::iterator end = n->args->end();
			for(; it != end; ++it, ++num_args)
			{
				expression_node *expr = *it;
				TYCHO_ASSERT(expr);
				expression_list_checker(m_program, expr, n->args);
				if(!expr->get_type_info())
					error = true;
				else
					arg_types[num_args] = expr->get_type_info();
			}
			
			//TODO : validate constructor call
		}
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::assign_expression_node *n, ast::node *)
	{
		check_assign_expression(n);							
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::ternary_expression_node *n, ast::node *)
	{
		// implements subset of C99 : 6.5.15 Conditional operator
		expression_list_checker(m_program, n->if_expression, n);
		expression_list_checker(m_program, n->else_expression, n);			
		expression_list_checker(m_program, n->condition, n);			
		check_convertable_to_bool(m_program, n->condition->get_type_info());			
		
		const types::type_info *lhs = n->if_expression->get_type_info();
		const types::type_info *rhs = n->else_expression->get_type_info();
		
		// one of the following rules apply for the second and third operands
		//  - both have arithmetic type
		//  - both have the same structure or union type
		//  - both have void type			
		if(lhs && rhs)
		{
			if(lhs->has_flag(type_info::f_supports_arithmetic) && rhs->has_flag(type_info::f_supports_arithmetic))
			{
				// check conversion type
				const types::type_info *ctype = type_system::get_arithmetic_conversion_type(lhs, rhs);
				if(!ctype)
				{
					m_program->add_message(mt_illegal_cast, n->get_token().pos, lhs->get_name(), rhs->get_name());
				}
				else
				{
					n->set_type_info(ctype);
				}
			}
			else if(lhs->get_type() == type_info::t_void && rhs->get_type() == type_info::t_void)
			{
				// all good
				n->set_type_info(lhs);
			}
			else if(lhs == rhs && lhs->get_type() == type_info::t_structure)
			{
				// also all good
				n->set_type_info(lhs);
			}
			else
			{
				m_program->add_message(mt_ternary_expressions_invalid_operand, n->get_token().pos);
			}
		}
			
		return visit_abort_subtree;
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::non_type_id_node *id, ast::node *)
	{
		if(id)
		{
			identifier_info *ii = m_program->get_symbol_table()->identify(id->name);
			bool report_undeclared = true;
			if(ii && ii->type == it_function)
			{
				m_program->add_message(mt_function_used_as_variable, id->get_token().pos, id->name);
				report_undeclared = false;
			}
			if(report_undeclared && (!ii || ii->type != it_variable))
			{
				m_program->add_message(mt_undeclared_variable, id->get_token().pos, id->name);
			}
			else
			{
				id->set_id_info(ii);
				id->set_type_info(ii->type_id);							
			}
		}	
		return visit_abort_subtree;
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::cast_expression_node *n, ast::node *)
	{
		// evaluate full destination type
		const types::type_info* dst_type = basic_type_checker::evaluate(m_program, n->type->type->type);
		size_t num_dimensions = 0;
		if(n->type->dimensions && !n->type->dimensions->empty()) 
			num_dimensions = n->type->dimensions->size();
		
		if(num_dimensions)
		{
			int* dimensions = HLSL_ALLOC_STACK_ZERO(int, num_dimensions);
			bool ok = evaluate_constant_tree(m_program, n->type->dimensions, dimensions);				
			if(ok)
			{
				dst_type = m_program->get_symbol_table()->instantiate_array_type(dst_type, dimensions, static_cast<int>(num_dimensions));						
			}											
		}
		
		expression_list_checker(m_program, n->expr, n);
		const types::type_info* src_type = n->expr->get_type_info();
		
		if(!src_type || !dst_type)
		{
			n->set_type_info(0);
		}			
		// check there is an actual conversion between the two types
		else if(!compiler::type_system::get_arithmetic_conversion_type(src_type, dst_type))
		{
			m_program->add_message(mt_illegal_cast, n->get_token().pos, src_type->get_name(), dst_type->get_name());
			n->set_type_info(0);
		}
		else 
		{
			n->set_type_info(dst_type);
		}
		
		return visit_abort_subtree;
	}
	

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::variable_decl_node *n, ast::node *)
	{
		check_variable_decl(m_program, n);
		return visit_abort_subtree;
	}
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::postfix_array_expression_node *n, ast::node *)
	{			
		// our type is just the type of the identifier 
		expression_list_checker(m_program, n->expr, n);
		const types::type_info *type = n->expr->get_type_info();
		if(type)		
		{
			// if this is an array type check the dimensions are within bounds
			if(!n->dimensions.empty())                                
			{								
				// evaluate the types of all expressions and check they convert to an 
				// integral type			
				utils::list<expression_node>::iterator it = n->dimensions.begin(), end = n->dimensions.end();		
				const types::type_info* int_type = m_program->get_symbol_table()->identify_type("int");
				for(;it != end; ++it)
				{
					expression_list_checker(m_program, *it, n);
					const types::type_info* t = (*it)->get_type_info();
					if(!int_type->can_convert_from(t))
					{
						m_program->add_message(mt_array_access_not_integral, n->get_token().pos);
					}
				}
				
				// check that the access is valid for this array
				const types::type_info *element_type = type->validate_array_access(m_program, 0, (int)n->dimensions.size());
				if(element_type)
				{
					// type of this expression is that of a single array element
					n->set_type_info(element_type);					
				}
			}
			else
			{
				// empty dimensions illegal, but shouldn't ever get here as that is a syntax error, might be nicer to
				// allowed then fail here with nice error message	
				if(n->expr)
					n->set_type_info(n->expr->get_type_info());
			}				
		}
		
		return visit_abort_subtree;
	}

	//////////////////////////////////////////////////////////////////////////				
	// unary expressions
	//////////////////////////////////////////////////////////////////////////				
	
	void context_checker_dx10::expression_list_checker::check_unary_arithmetic(ast::unary_expression_node *expr)
	{
		// check that the target type supports increment and decrement
		expression_list_checker(m_program, expr->value, expr);
		const types::type_info *target_type = expr->value->get_type_info();
		if(target_type)
		{
			if(!target_type->has_flag(type_info::f_supports_arithmetic))
			{
				m_program->add_message(mt_arithmetic_not_supported, expr->value->get_token().pos, target_type->get_name());
			}
			//TODO : Check target is an lvalue
		}
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::prefix_inc_expression_node *n, ast::node *)
	{
		check_unary_arithmetic(n);
		return visit_abort_subtree;		
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::prefix_dec_expression_node *n, ast::node *)
	{
		check_unary_arithmetic(n);
		return visit_continue;		
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::prefix_not_expression_node *n, ast::node *)
	{
		check_unary_arithmetic(n);
		return visit_continue;		
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::prefix_plus_expression_node *n, ast::node *)
	{
		check_unary_arithmetic(n);
		return visit_continue;		
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::prefix_minus_expression_node *n, ast::node *)
	{
		check_unary_arithmetic(n);
		return visit_continue;		
	}
	
	
	//////////////////////////////////////////////////////////////////////////				
	// binary expressions
	//////////////////////////////////////////////////////////////////////////			
	
	void context_checker_dx10::expression_list_checker::check_logical_expression(ast::binary_expression_node *n)
	{
		expression_list_checker(m_program, n->lhs, n);
		expression_list_checker(m_program, n->rhs, n);
		const types::type_info *lhs_type = n->lhs->get_type_info();
		const types::type_info *rhs_type = n->rhs->get_type_info();
		
		if(lhs_type && rhs_type)
		{
			// check both sides can evaluate to a bool				
			const types::type_info *bool_type = m_program->get_symbol_table()->identify_type("bool");
			TYCHO_ASSERT(bool_type);
			if(!bool_type->can_convert_from(lhs_type))
				m_program->add_message(mt_type_conversion, n->lhs->get_token().pos, lhs_type->get_name(), "bool");
			if(!bool_type->can_convert_from(rhs_type))
				m_program->add_message(mt_type_conversion, n->rhs->get_token().pos, rhs_type->get_name(), "bool");				
		}		
	}
	
	void context_checker_dx10::expression_list_checker::check_arithmetic_expression(ast::binary_expression_node *n)
	{
		expression_list_checker(m_program, n->lhs, n);
		expression_list_checker(m_program, n->rhs, n);
		const types::type_info *lhs_type  = n->lhs->get_type_info();
		const types::type_info *rhs_type  = n->rhs->get_type_info();
		
		if(lhs_type && rhs_type)
		{			
			// check both sides supports arithmetic operations
			if(!lhs_type->has_flag(type_info::f_supports_arithmetic))
			{
				m_program->add_message(mt_type_does_not_support_arithmetic, 
					n->lhs->get_token().pos,
					lhs_type->get_name());
			}
			else if(!rhs_type->has_flag(type_info::f_supports_arithmetic))
			{
				m_program->add_message(mt_type_does_not_support_arithmetic, 
					n->rhs->get_token().pos,
					rhs_type->get_name());
			}
			else
			{
				const types::type_info *conversion_type = type_system::get_arithmetic_conversion_type(lhs_type, rhs_type);
				if(!conversion_type)
				{
					// no conversion available
					m_program->add_message(mt_no_conversion, 
						n->lhs->get_token().pos,
						lhs_type->get_name(), 
						rhs_type->get_name());
				}
				else
				{
					// check to see if either need casting to this type						
					if(conversion_type == lhs_type)
					{
						n->rhs = ast::make_cast_expression(m_program, conversion_type, n->rhs);							
						n->rhs->set_type_info(conversion_type);
					}
					else
					{
						n->lhs = ast::make_cast_expression(m_program, conversion_type, n->lhs);													
						n->lhs->set_type_info(conversion_type);
					}
					n->set_type_info(conversion_type);
				}
			}

		}		
	}
	
	void context_checker_dx10::expression_list_checker::check_comparison_expression(ast::binary_expression_node *n)
	{
		// same requirements as an arithmetic expression
		check_arithmetic_expression(n); 
	}
			
	void context_checker_dx10::expression_list_checker::check_assign_expression(ast::binary_expression_node *rn)
	{
		// evaluate lhs and rhs
		expression_list_checker(m_program, rn->lhs, rn);
		expression_list_checker(m_program, rn->rhs, rn);
		
		const types::type_info *lhs_type = rn->lhs->get_type_info();
		const types::type_info *rhs_type = rn->rhs->get_type_info();
		
		if(lhs_type && rhs_type)
		{
			if(!lhs_type->can_convert_from(rhs_type))
			{
				m_program->add_message(mt_illegal_cast, rn->rhs->get_token().pos, rhs_type->get_name(), lhs_type->get_name());
			}
			
			//TODO : check that lhs is an lvalue			
		}		
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::logical_or_expression_node *n, ast::node *)
	{			
		check_logical_expression(n);			
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::logical_and_expression_node *n, ast::node *)
	{
		check_logical_expression(n);			
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::comparision_ge_expression_node *n, ast::node *)
	{
		check_comparison_expression(n);
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::comparision_le_expression_node *n, ast::node *)
	{
		check_comparison_expression(n);
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::comparision_lt_expression_node *n, ast::node *)
	{
		check_comparison_expression(n);
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::comparision_gt_expression_node *n, ast::node *)
	{
		check_comparison_expression(n);
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::comparision_eq_expression_node *n, ast::node *)
	{
		check_comparison_expression(n);
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::comparision_ne_expression_node *n, ast::node *)
	{
		check_comparison_expression(n);
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::add_expression_node *n, ast::node *)
	{
		check_arithmetic_expression(n);			
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::minus_expression_node *n, ast::node *)
	{
		check_arithmetic_expression(n);			
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::times_expression_node *n, ast::node *)
	{
		check_arithmetic_expression(n);			
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::divide_expression_node *n, ast::node *)
	{
		check_arithmetic_expression(n);			
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::mod_expression_node *n, ast::node *)
	{
		check_arithmetic_expression(n);			
		return visit_continue;		
	}

	
	//////////////////////////////////////////////////////////////////////////				
	// postfix expressions
	//////////////////////////////////////////////////////////////////////////				
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::postfix_inc_expression_node *n, ast::node *)
	{
		check_unary_arithmetic(n);
		return visit_continue;		
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::postfix_dec_expression_node *n, ast::node *)
	{
		check_unary_arithmetic(n);
		return visit_continue;		
	}
			
	//////////////////////////////////////////////////////////////////////////				
	// assignment expressions
	//////////////////////////////////////////////////////////////////////////				
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::add_assign_expression_node *n, ast::node *)
	{
		check_assign_expression(n);
		return visit_continue;
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::minus_assign_expression_node *n, ast::node *)
	{
		check_assign_expression(n);
		return visit_continue;
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::times_assign_expression_node *n, ast::node *)
	{
		check_assign_expression(n);
		return visit_continue;
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::divide_assign_expression_node *n, ast::node *)
	{
		check_assign_expression(n);
		return visit_continue;
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::mod_assign_expression_node *n, ast::node *)
	{
		check_assign_expression(n);
		return visit_continue;
	}							
			
	//////////////////////////////////////////////////////////////////////////				
	// literal expressions
	//////////////////////////////////////////////////////////////////////////				
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::float_literal_node *n, ast::node *)
	{ 
		identifier_info *iinfo = m_program->get_symbol_table()->identify("float");
		TYCHO_ASSERT(iinfo);
		n->set_type_info(iinfo->type_id);
		n->set_id_info(iinfo);
		return visitor::visit_continue; 
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::double_literal_node *n, ast::node *)
	{ 
		identifier_info *iinfo = m_program->get_symbol_table()->identify("double");
		TYCHO_ASSERT(iinfo);
		n->set_type_info(iinfo->type_id);
		n->set_id_info(iinfo);
		return visitor::visit_continue; 
	}

	visitor::result context_checker_dx10::expression_list_checker::visit(ast::int_literal_node *n, ast::node *)
	{ 
		identifier_info *iinfo = m_program->get_symbol_table()->identify("int");
		TYCHO_ASSERT(iinfo);
		n->set_type_info(iinfo->type_id);
		n->set_id_info(iinfo);
		return visitor::visit_continue; 
	}
	
	visitor::result context_checker_dx10::expression_list_checker::visit(ast::bool_literal_node *n, ast::node *) 
	{ 
		identifier_info *iinfo = m_program->get_symbol_table()->identify("bool");
		TYCHO_ASSERT(iinfo);
		n->set_type_info(iinfo->type_id);
		n->set_id_info(iinfo);
		return visitor::visit_continue; 
	}
			
	//////////////////////////////////////////////////////////////////////////
	//
	// Pass checker
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	// Technique checker
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	// Constant buffer checker
	//
	//////////////////////////////////////////////////////////////////////////

	context_checker_dx10::cbuffer_checker::cbuffer_checker(program *p, ast::node *) :
		checker(p)
	{}
	
	visitor::result context_checker_dx10::cbuffer_checker::visit(ast::variable_decl_node *, ast::node *)
	{
		return visit_abort_subtree;
	}



    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
