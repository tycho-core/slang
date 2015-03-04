//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:25 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "pretty_print.h"
#include "core/colour/constants.h"
#include "core/string.h"

//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
	using namespace ast;

namespace printing
{

    //--------------------------------------------------------------------

	namespace detail
	{
					
		struct pretty_printer_visitor : ast::visitor_preorder
		{	
			typedef ast::visitor_preorder base;
			typedef pretty_printer pp;
			
			pretty_printer_visitor(pretty_printer *printer) : m_printer(printer) {}
			
			void write_keyword(const char *str) { m_printer->write(str, pretty_printer::t_keyword); }
			void write_literal(const char *str) { m_printer->write(str, pretty_printer::t_literal); }
			void write_type_id(const char *str) { m_printer->write(str, pretty_printer::t_type_id); }
			void write_non_type_id(const char *str) { m_printer->write(str, pretty_printer::t_non_type_id); }
			void write_operator(const char *str) { m_printer->write(str, pretty_printer::t_operator); }
			
			void write_lbrace() { m_printer->write("{", pretty_printer::t_lbrace); m_printer->inc_depth(); }
			void write_rbrace() { m_printer->dec_depth(); m_printer->write("}", pretty_printer::t_rbrace); }
			void write_lparen() { m_printer->write("(", pretty_printer::t_punctuation); }
			void write_rparen() { m_printer->write(")", pretty_printer::t_punctuation);	}
			void write_lbrack() { m_printer->write("[", pretty_printer::t_punctuation); }
			void write_rbrack() { m_printer->write("]", pretty_printer::t_punctuation);	}
			void write_colon()  { m_printer->write(":", pretty_printer::t_punctuation);	}
			void write_semi_colon()  { m_printer->write(";", pretty_printer::t_semicolon);	}

			template<class T>
			void write_seperated(T *list, const char *sep, bool exclude_last, pretty_printer::token t = pretty_printer::t_operator)
			{
				if(list)
				{
					typename T::list_type::iterator it = list->begin();
					typename T::list_type::iterator end = list->end();
					size_t list_size = list->size();
					size_t i = 0;
					for(; it != end; ++it, ++i)
					{
						if(*it)
							(*it)->accept(*this, 0);
						else
							write_literal("0");
							
						if(!exclude_last || i != list_size-1)
							m_printer->write(sep, t);
					}							
				}
			}
			
			template<class T>
			void write_comma_seperated(T *list)
			{
				write_seperated(list, ",", true);
			}
			
			void accept(node *n)
			{
				if(n)
					n->accept(*this, 0);
			}
			
			virtual result visit(ast::node *n, ast::node *) 
			{
				#define BINARY_OP(_type, _token) \
					case node::t_##_type##_expression : { \
						typedef _type##_expression_node type; \
						type *rd = (type*)n; \
						write_lparen(); \
						accept(rd->lhs); \
						m_printer->write(_token, pretty_printer::t_operator); \
						accept(rd->rhs); \
						write_rparen(); \
						return visit_abort_subtree; \
					} break; 

				#define UNARY_OP(_type, _token) \
					case node::t_##_type##_expression : { \
						typedef _type##_expression_node type; \
						type *rd = (type*)n; \
						m_printer->write(_token, pretty_printer::t_operator); \
						accept(rd->value); \
						return visit_abort_subtree; \
					} break; 
					
				switch(n->get_type())
				{					
					BINARY_OP(logical_or, "||")
					BINARY_OP(logical_and, "&&")
					BINARY_OP(comparision_ge, ">=")
					BINARY_OP(comparision_le, "<=")
					BINARY_OP(comparision_lt, "<")
					BINARY_OP(comparision_gt, ">")
					BINARY_OP(comparision_eq, "==")
					BINARY_OP(comparision_ne, "!=")
					BINARY_OP(add, "+")
					BINARY_OP(minus, "-")
					BINARY_OP(times, "*")
					BINARY_OP(divide, "/")
					BINARY_OP(mod, "%")
					BINARY_OP(assign, "=")
					BINARY_OP(add_assign, "+=")
					BINARY_OP(minus_assign, "-=")
					BINARY_OP(times_assign, "*=")
					BINARY_OP(divide_assign, "/=")
					BINARY_OP(mod_assign, "%=")				
					UNARY_OP(prefix_inc, "++");
					UNARY_OP(prefix_dec, "--");
					UNARY_OP(prefix_not, "!");
					UNARY_OP(prefix_plus, "-");
					UNARY_OP(prefix_minus, "+");
					
					case node::t_function : {
						function_node *rd = (function_node*)n;
						accept(rd->usage);
						accept(rd->name);
						write_lparen();
						write_comma_seperated(rd->params);
						write_rparen();
						return visit_abort_subtree;
					} break;
					
					case node::t_function_decl : {
						function_decl_node *rd = (function_decl_node*)n;
						accept(rd->function);
						accept(rd->annotations);
						write_lbrace();
						accept(rd->statements);
						write_rbrace();
						return visit_abort_subtree;
					} break;
					
					case node::t_variable_decl : {
						variable_decl_node *rd = (variable_decl_node *)n;
						accept(rd->type);
						write_comma_seperated(rd->variables);
						write_semi_colon();
						return visit_abort_subtree;
					} break;
					
					case node::t_if_else_statement : {
						if_else_statement_node *rn = (if_else_statement_node*)n;
						write_keyword("if");
						write_lparen();
						accept(rn->condition);
						write_rparen();
						write_lbrace();
						accept(rn->if_statement);
						write_rbrace();
						if(rn->else_statement)
						{
							write_keyword("else");
							write_lbrace();
							accept(rn->else_statement);
							write_rbrace();
						}
						return visit_abort_subtree;
					} break;

					case node::t_typedef : {
						write_keyword("typedef");
						typedef_node *rd = (typedef_node*)n;
						accept(rd->base_type);
						write_comma_seperated(rd->names);
						write_semi_colon();
						return visit_abort_subtree;
					} break;
					
					case node::t_struct : {
						write_keyword("struct");					
						struct_node *rn = (struct_node*)n;
						accept(rn->id);
						write_lbrace();
						accept(rn->decls);
						write_rbrace();
						return visit_abort_subtree;
					} break;

					case node::t_cbuffer : {
						write_keyword("cbuffer");					
						struct_node *rn = (struct_node*)n;
						accept(rn->id);
						write_lbrace();
						accept(rn->decls);
						write_rbrace();
						write_semi_colon();
						return visit_abort_subtree;
					} break;
					
					case node::t_var_struct_decl : {						
						var_struct_decl_node *rn = (var_struct_decl_node*)n;
						accept(rn->s);
						accept(rn->vars);
						write_semi_colon();
						return visit_abort_subtree;
					} break;
					
					case node::t_double_literal :
					case node::t_bool_literal :
					case node::t_string_literal :
					case node::t_int_literal :
					case node::t_float_literal : 
					{
						write_literal(n->string());						
					} break;
					
					case node::t_scalar_type  :	
					{
						write_type_id(n->string());
					} break;
					
					case node::t_variable :
					{
						variable_node *rn = (variable_node*)n;
						accept(rn->var);
						if(rn->semantics)
						{
							write_colon();
							accept(rn->semantics);
						}
						accept(rn->annotations);
						if(rn->initialiser)
						{
							write_operator("=");
							accept(rn->initialiser);
						}
						
						return visit_abort_subtree;
					} break;
					
					case node::t_variable_dim :
					{
						variable_dim_node *rd = (variable_dim_node*)n;
						accept(rd->id);
						expression_list_node::list_type::iterator it = rd->dimensions->begin();
						expression_list_node::list_type::iterator end = rd->dimensions->end();
						for(; it != end; ++it)
						{
							write_lbrack();
							accept(*it);
							write_rbrack();							
						}
						return visit_abort_subtree;
					} break;
							
					case node::t_do_while_statement :
					{
						do_while_statement_node *rn = (do_while_statement_node*)n;
						write_keyword("do");
						write_lbrace();
						accept(rn->statement);
						write_rbrace();
						write_keyword("while");
						write_lparen();
						accept(rn->expression);
						write_rparen();
						write_semi_colon();
						return visit_abort_subtree;
					} break;

					case node::t_while_statement :
					{
						while_statement_node *rn = (while_statement_node*)n;
						write_keyword("while");
						write_lparen();
						accept(rn->expression);
						write_rparen();
						write_lbrace();
						accept(rn->statement);
						write_rbrace();
						write_semi_colon();
						return visit_abort_subtree;
					} break;
					
					case node::t_expression_statement :
					{
						expression_statement_node *rd = (expression_statement_node*)n;
						accept(rd->expression);
						write_semi_colon();
						return visit_abort_subtree;
					} break;
					
					case node::t_ternary_expression :
					{
						ternary_expression_node *rn = (ternary_expression_node*)n;
						write_lparen();
						accept(rn->condition);
						write_rparen();
						write_operator("?");
						accept(rn->if_expression);
						write_operator(":");
						accept(rn->else_expression);
						return visit_abort_subtree;						
					} break;
					
					case node::t_for_statement :
					{
						for_statement_node *rn = (for_statement_node*)n;
						write_keyword("for");
						write_lparen();
						accept(rn->initialiser);
						accept(rn->condition);
						write_semi_colon();
						accept(rn->step);
						write_rparen();
						write_lbrace();
						accept(rn->statement);
						write_rbrace();
						return visit_abort_subtree;
					} break;
					
					case node::t_function_call :
					{
						function_call_node *rn = (function_call_node*)n;
						accept(rn->id);
						write_lparen();
						write_comma_seperated(rn->args);
						write_rparen();						
						return visit_abort_subtree;
					}
					
					case node::t_return_statement :
					{
						return_statement_node *rn = (return_statement_node*)n;
						write_keyword("return");
						accept(rn->expression);
						write_semi_colon();
						return visit_abort_subtree;
					}
										
					case node::t_type_id :
					{
						write_type_id(n->string());
						return visit_abort_subtree;
					} break;

					case node::t_non_type_id :
					{
						write_non_type_id(n->string());
						return visit_abort_subtree;
					} break;
					
					case node::t_matrix_type :
					{
						matrix_type_node *rn = (matrix_type_node*)n;
						write_keyword("matrix");
						write_operator("<");
						accept(rn->type);
						write_operator(",");
						accept(rn->num_rows);
						write_operator(",");
						accept(rn->num_columns);						
						write_operator(">");
						return visit_abort_subtree;
					} break;

					case node::t_vector_type :
					{
						vector_type_node *rn = (vector_type_node*)n;
						write_keyword("vector");
						write_operator("<");
						accept(rn->type);
						write_operator(",");
						accept(rn->size_expr);
						write_operator(">");
						return visit_abort_subtree;
					} break;
					
					case node::t_compile_expression :				
					{
						compile_expression_node *rn = (compile_expression_node*)n;
						write_keyword("compile");
						write_non_type_id(rn->target->string());
						write_non_type_id(rn->function->string());
						write_lparen();
						write_comma_seperated(rn->args);
						write_rparen();					
						return visit_abort_subtree;	
					} break;
					
					case node::t_state :
					{
						state_node *rn = (state_node*)n;
						write_non_type_id(rn->id->string());
						write_operator("=");
						accept(rn->expression);
						write_semi_colon();
						return visit_abort_subtree;
					} break;

					
					// D3D9 version
					case node::t_pass_decl :
					{
						pass_decl_node *rn = (pass_decl_node*)n;
						write_keyword("pass");
						write_non_type_id(rn->id->string());
						accept(rn->annotation);
						write_lbrace();
						accept(rn->states);
						write_rbrace();
						return visit_abort_subtree;
					} break;

					// D3D10 version
					case node::t_pass10_decl:					
					{					
						pass10_decl_node *rn = (pass10_decl_node*)n;
						write_keyword("pass");
						write_non_type_id(rn->id->string());
						accept(rn->annotation);
						write_lbrace();
						write_seperated(rn->states, ";", false, pretty_printer::t_semicolon);
						write_rbrace();
						return visit_abort_subtree;					
					} break;
					
					// D3D9 version
					case node::t_technique_decl :
					{
						technique_decl_node *rn = (technique_decl_node*)n;
						write_keyword("technique");
						write_non_type_id(rn->id->string());
						accept(rn->annotation);
						write_lbrace();
						accept(rn->technique);
						write_rbrace();						
						return visit_abort_subtree;
					} break;

					// D3D10 version
					case node::t_technique10_decl :
					{
						technique_decl_node *rn = (technique_decl_node*)n;
						write_keyword("technique10");
						write_non_type_id(rn->id->string());
						accept(rn->annotation);
						write_lbrace();
						accept(rn->technique);
						write_rbrace();						
						return visit_abort_subtree;
					} break;

					case node::t_cast_expression :
					{
						cast_expression_node *rb = (cast_expression_node*)n;
						write_lparen();
						accept(rb->type);
						write_rparen();
						write_lparen();
						accept(rb->expr);
						write_rparen();
						return visit_abort_subtree;
					} break;
										
					case node::t_constructor_expression :
					{
						constructor_expression_node *rb = (constructor_expression_node*)n;
						write_type_id(rb->type_id->string());
						write_lparen();
						write_comma_seperated(rb->args);
						write_rparen();
						return visit_abort_subtree;
					} break;
					
					case node::t_postfix_member_expression :
					{
						postfix_member_expression_node *rb = (postfix_member_expression_node*)n;
						accept(rb->expression);
						write_operator(".");
						accept(rb->member);
						return visit_abort_subtree;						
					} break;
					
					// D3D9 version
					case node::t_sampler_state_expression :
					{
						sampler_state_expression_node *rn = (sampler_state_expression_node*)n;
						write_keyword("sampler_state");
						write_lbrace();
						accept(rn->states);
						write_rbrace();
						return visit_abort_subtree;						
					} break;

					// D3D10 version
					case node::t_sampler_state10_expression :
					{
						sampler_state10_expression_node *rn = (sampler_state10_expression_node*)n;
						write_keyword("SamplerState");
						write_non_type_id(rn->id->string());
						write_lbrace();
						accept(rn->states);
						write_rbrace();
						return visit_abort_subtree;						
					} break;
					
					case node::t_state_variable_expression :
					{
						state_variable_expression_node *rn = (state_variable_expression_node*)n;
						write_operator("<");
						accept(rn->expr);
						write_operator(">");
						return visit_abort_subtree;
					} break;
					
					case node::t_postfix_array_expression :
					{
						postfix_array_expression_node *rn = (postfix_array_expression_node*)n;
						accept(rn->expr);
						utils::list<expression_node>::iterator it(rn->dimensions.begin()), end(rn->dimensions.end());
						for(; it != end; ++it)
						{
							write_operator("[");
							accept(*it);
							write_operator("]");
						}
						return visit_abort_subtree;
					} break;
					
					case node::t_postfix_member_call_expression : 
					{
						postfix_member_call_expression_node *rn = (postfix_member_call_expression_node*)n;
						accept(rn->expression);
						write_operator(".");
						accept(rn->member);
						write_lparen();
						write_comma_seperated(rn->args);
						write_rparen();
						return visit_abort_subtree;
					} break;
										
					default:
						write_type_id(n->string());					
				}
				return visit_continue;
			}		
			
		private:
			pretty_printer *m_printer;
		};
	}

    //--------------------------------------------------------------------

	// kerning table between token types
	pretty_printer::kerning pretty_printer::ms_kerning_table[pretty_printer::t_count][pretty_printer::t_count] = {
			//					keyword				type_id			non_type_id			literal				operator		punctuation    semicolon		rbrace	         lbrace 
			/* keyword     */	{ { kt_space },		{ kt_space },	{ kt_space },		{ kt_space },		{ kt_space },	{ kt_none },	{ kt_none },    { kt_newline },  { kt_newline } },
			/* type_id     */	{ { kt_space },		{ kt_space },	{ kt_space },		{ kt_space },		{ kt_none },	{ kt_none },	{ kt_none },    { kt_newline },  { kt_newline } },
			/* non_type_id */	{ { kt_space },		{ kt_space },	{ kt_space },		{ kt_none },		{ kt_none },	{ kt_none },	{ kt_none },    { kt_newline },  { kt_newline } },
			/* literal     */	{ { kt_space },		{ kt_space },	{ kt_space },		{ kt_none },		{ kt_none },	{ kt_none },	{ kt_none },    { kt_newline },  { kt_newline } },
			/* operator    */	{ { kt_none },		{ kt_none },	{ kt_none },		{ kt_none },		{ kt_none },	{ kt_none },	{ kt_none },    { kt_newline },  { kt_newline } },
			/* punctuation */	{ { kt_none },		{ kt_none },	{ kt_none },		{ kt_none },		{ kt_none },	{ kt_none },	{ kt_none },    { kt_newline },  { kt_newline } },
			/* semicolon   */	{ { kt_newline },	{ kt_newline },	{ kt_newline },		{ kt_newline },		{ kt_newline },	{ kt_newline },	{ kt_newline }, { kt_newline },  { kt_newline } },
			/* rbrace	   */	{ { kt_newline },	{ kt_newline },	{ kt_newline },		{ kt_newline },		{ kt_newline },	{ kt_newline },	{ kt_none }, { kt_newline },  { kt_newline } },
			/* lbrace	   */	{ { kt_newline },	{ kt_newline },	{ kt_newline },		{ kt_newline },		{ kt_newline },	{ kt_newline },	{ kt_newline }, { kt_newline },  { kt_newline } }
		};

    //--------------------------------------------------------------------

	pretty_printer::pretty_printer()
	{
		set_defaults();
	}

    //--------------------------------------------------------------------

	void pretty_printer::format(const ast::program_node &program, core::printing::text::formatter  &formatter)
	{
		m_last_token = t_count;
		m_formatter = &formatter;
		m_formatter->set_default_foreground_color(m_default_foreground);
		m_formatter->set_default_background_color(m_default_background);
		detail::pretty_printer_visitor visitor(this);
		// ick.
		const_cast<ast::program_node&>(program).accept(visitor, 0);
		m_formatter->newline();
		m_formatter = 0;
	}
	
    //--------------------------------------------------------------------

	void pretty_printer::write_newline()
	{
		m_formatter->newline();
	}
	
    //--------------------------------------------------------------------

	void pretty_printer::inc_depth()
	{
		m_formatter->inc_indent();
	}
	
    //--------------------------------------------------------------------

	void pretty_printer::dec_depth()
	{
		m_formatter->dec_indent();
	}

    //--------------------------------------------------------------------
	
	void pretty_printer::write(const char *str, token t)
	{
		if(core::strlen(str) == 0)
			return;		
		
		if(m_last_token != t_count)
		{
			const kerning &kern = ms_kerning_table[m_last_token][t];
			switch(kern.trail)
			{
				case kt_space	: m_formatter->space(); break;
				case kt_newline : write_newline(); break;
                case kt_none    : break;
			}
		}
		
		const highlight &hl = m_highlights[t];
		core::rgba fore = (hl.use_default_fore) ? m_default_foreground : hl.foreground;
		core::rgba back = (hl.use_default_back) ? m_default_background : hl.background;
		m_formatter->write(str, fore, back);
		m_last_token = t;
	}
	
    //--------------------------------------------------------------------

	void pretty_printer::set_defaults()
	{
		set_option(opt_tab_size, 4);		
		set_option(opt_newline_after_opening_brace, 1);
		set_option(opt_newline_before_opening_brace, 1);
		set_option(opt_newline_after_closing_brace, 1);
		set_option(opt_newline_before_closing_brace, 1);

		set_default_foreground_color(core::colours::yellow);
		set_default_background_color(core::colours::black);
		
		set_highlight(t_keyword, highlight(core::colours::magenta));
		set_highlight(t_type_id, highlight(core::colours::yellow));
		set_highlight(t_non_type_id, highlight(core::colours::cyan));
		set_highlight(t_operator, highlight(core::colours::green));
		set_highlight(t_punctuation, highlight(core::colours::green));
		set_highlight(t_literal, highlight(core::colours::red));
		set_highlight(t_semicolon, highlight(core::colours::yellow));
		set_highlight(t_rbrace, highlight(core::colours::red));
		set_highlight(t_lbrace, highlight(core::colours::red));		
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
