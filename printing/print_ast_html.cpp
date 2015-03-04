//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:38 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "print_ast_html.h"
#include "slang/compiler/symbol_table.h"
#include "slang/compiler/program.h"
#include "slang/ast/visitor.h"
#include "slang/ast/ast.h"
#include "slang/types/structure_type.h"
#include "core/string.h"

#include <string>
#include <sstream>
#include <map>


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
	using namespace compiler;
	using namespace utils;
	using namespace parsing;
	using namespace types;

namespace printing
{

    //--------------------------------------------------------------------

	namespace detail
	{		
	
		/// very simple HTML writer helper
		class html_doc_writer
		{
		public:
			std::string str() const { return m_doc.str(); }
			
			void doc_header(const char *stylesheet)
			{
				m_doc << "<html>\n";
				m_doc << "<head>\n";
				m_doc << "<script src=\"hlsl_debug_ast.js\" type=\"text/javascript\"></script>";
				if(stylesheet)
				{
					m_doc << "<link rel=\"stylesheet\" type=\"text/css\" href=\"";
					m_doc << stylesheet;
					m_doc << "\"/>";
				}
				m_doc << "</head>\n";
				m_doc << "<body>\n";
			}
			
			void doc_footer()
			{
				m_doc << "</body>\n";
				m_doc << "</html>\n";
			}
			void heading(const char *id, const char *text)
			{
				m_doc << "<h id=\"" << id << "\">" << text << "</h>";
			}

			void pre()
			{
				m_doc << "<pre>\n";
			}
			void epre()
			{
				m_doc << "</pre>\n";			
			}
			
			void div(const char *id)
			{
				m_doc << "<div id=\"" << id << "\">\n";
			}
			void ediv()
			{
				m_doc << "</div>\n";			
			}
			void div(const char *id, const char *str)
			{
				div(id);
				text(str);
				ediv();
			}
			void anchor(int id)
			{
				m_doc << "<a name=\"" << id << "\">\n";
			}			

			void anchor(const char *id)
			{
				m_doc << "<a name=\"" << id << "\">\n";
			}			
			
			void eanchor()
			{
				m_doc << "</a>\n";
			}
			
			void anchor(int id, const char *str)
			{
				anchor(id);
				text(str);
				eanchor();
			}	

			void anchor(const char *id, const char *str)
			{
				anchor(id);
				text(str);
				eanchor();
			}	

			void anchor(const char *id, int i)
			{
				anchor(id);
				text(i);
				eanchor();
			}	
			
			void local_link(int id)
			{
				m_doc << "<a href=\"#" << id << "\">\n";
			}

			void local_link(const char *id)
			{
				m_doc << "<a href=\"#" << id << "\">\n";
			}
			
			void elocal_link()
			{
				m_doc << "</a>\n";
			}
			
			void local_link(int id, const char *str)
			{
				local_link(id);
				text(str);
				elocal_link();
			}

			void local_link(const char* id, const char *str)
			{
				local_link(id);
				text(str);
				elocal_link();
			}

			void local_link(const char* id, int i)
			{
				local_link(id);
				text(i);
				elocal_link();
			}
			
			void table(const char *id)
			{
				m_doc << "<table id=\"" << id << "\">\n";
			}
			void etable()
			{
				m_doc << "</table>\n";
			}			
			void table_row(const char *id)
			{
				m_doc << "<tr id=\"" << id << "\">\n";
			}

			void etable_row()
			{
				m_doc << "</tr>\n";
			}			
						
			void table_data(const char *id)
			{
				m_doc << "<td id=\"" << id << "\">\n";
			}
			
			void etable_data()
			{
				m_doc << "</td>\n";
			}

			void table_data(const char *id, const char *str)
			{
				table_data(id);
				text(str);
				etable_data();
			}

			void table_data(const char *id, int i)
			{
				table_data(id);
				text(i);
				etable_data();
			}
			
			void table_data_link_local(const char *id, int anchor_id, const char *str)
			{
				table_data(id);
				local_link(anchor_id, str);
				etable_data();
			}

			void table_data_link_local(const char *id, const char* anchor_id, const char *str)
			{
				table_data(id);
				local_link(anchor_id, str);
				etable_data();
			}

			void table_data_link_local(const char *id, const char* anchor_id, int i)
			{
				table_data(id);
				local_link(anchor_id, i);
				etable_data();
			}

			void table_data_anchor_local(const char *id, const char* anchor_id, const char *str)
			{
				table_data(id);
				anchor(anchor_id, str);
				etable_data();
			}

			void table_data_anchor_local(const char *id, const char* anchor_id, int i)
			{
				table_data(id);
				anchor(anchor_id, i);
				etable_data();
			}

			void table_data_anchor_local(const char *id, int anchor_id, const char *str)
			{
				table_data(id);
				anchor(anchor_id, str);
				etable_data();
			}
			
			void text(const char *str)
			{
				while(str && *str)
				{
					if(*str == '<')
						m_doc << "&lt;";
					else if(*str == '>')
						m_doc << "&gt;";
					else if(*str == ' ')
						m_doc << "&nbsp;";
					else
						m_doc << *str;				
					++str;
				}

			}
			
			void text(int i)
			{
				m_doc << i;
			}
			
			void raw_text(const char *str)
			{
				m_doc << str;
			}
			
		private:
			std::stringstream m_doc;
		};
		
		/// helper class to encapsulate printing the ast to a HTML document
		class ast_html_printer : public ast::visitor_preorder
		{
		public:
			ast_html_printer(compiler::program *program, const char *filename) :
				base(visitor::visit_internal),
                m_filename(filename),
                m_program(program),
                m_depth(0)
			{
				m_doc.doc_header("hlsl_debug.css");
				m_doc.div("menu");
				m_doc.raw_text("<a id=\"menu-item\" href=\"#\" onclick=\"toggle_show_internal()\">Hide Internal Nodes</a>");
				m_doc.ediv();
				m_doc.div("content");
				output_program_messages();
				output_symbol_table();
				output_structures();
				output_ast();
				output_source_code();
				m_doc.ediv();			
				m_doc.doc_footer();	
				if(!m_filename)
				{
					puts(m_doc.str().c_str());
				}
				else
				{
					FILE *f = fopen(m_filename, "w+t");
					if(f)
					{	
						fprintf(f, "%s", m_doc.str().c_str());
						fclose(f);
					}
				}
			}
			
		private:
			const char* getline(const char *ptr, char *linebuf, int linebuf_len, int tab_len)
			{
				if(!ptr)
					return 0;
				int filled = 0;
				while(*ptr && filled < (linebuf_len-1))
				{
					if(*ptr == '\t')
					{
						for(int i = 0; i < tab_len; ++i)
							linebuf[filled++] = ' ';
					}
					else if(*ptr != '\n')
					{
						linebuf[filled++] = *ptr;
					}
					else
					{
						linebuf[filled++] = 0;
						return ptr+1;
					}
					++ptr;
				}
				linebuf[filled] = 0;				
				return (*ptr == 0) ? 0 : ptr;
			}
		
			void make_anchor_safe_string(char *src)
			{
				while(*src)
				{
					char ch = *src;
					if((ch >= 'A' && ch <= 'Z') || 
					   (ch >= 'a' && ch <= 'z') ||
					   (ch >= '0' && ch <= '9'))
					{
						// no - op
				    }
				    else
				    {
						*src = '_';
				    }
					++src;
				}
			}
			
			void make_file_line_link(const char *filename, int line, char *buf)
			{
				if(filename)
				{
					core::strcpy(buf, filename);
					char lbuf[128] = { 0 };
					core::snprintf(lbuf, 128, "%d", line);
					core::strcat(buf, lbuf);
					make_anchor_safe_string(buf);				
				}
			}
			
			/// write compile warnings and errors			
			void output_program_messages()
			{
				m_doc.div("heading1", "Errors and Warnings");
				const message_list &msgs = m_program->get_messages();
				message_list::const_iterator it = msgs.begin();
				message_list::const_iterator end = msgs.end();
				m_doc.table("pm_table");
				
				// column headings
				m_doc.table_row("pm_row");
				m_doc.table_data("pm_heading", "File");
				m_doc.table_data("pm_heading", "Line");				
				m_doc.table_data("pm_heading", "Column");
				m_doc.table_data("pm_heading", "Severity");				
				m_doc.table_data("pm_heading", "Description");
				m_doc.etable_row();
				
				for(; it != end; ++it)
				{
					const message *msg = *it;
					m_doc.table_row("pm_row");

					static char buffer[1024];
					m_program->format_description(buffer, msg->info->short_desc, msg->extra[0], msg->extra[1], msg->extra[2]);

					const int linebuf_len = 2048;
					char anchorbuf[linebuf_len];
					make_file_line_link(msg->pos.file, msg->pos.line, anchorbuf);
					
					m_doc.table_data_link_local("pm_default", anchorbuf, msg->pos.file);
					m_doc.table_data_link_local("pm_default", anchorbuf, msg->pos.line);
					m_doc.table_data("pm_default", msg->pos.col);
					m_doc.table_data("pm_default", get_message_level_string(msg->info->level));
					m_doc.table_data("pm_default", buffer);						
						
					m_doc.etable_row();					
				}
				m_doc.etable();
			}
			
			/// write the source code out, including line anchors and row numbers
			void output_source_code() 
			{
				m_doc.div("heading1");
				m_doc.text("Source Code");
				m_doc.ediv();
				const program::sources &files = m_program->get_source_files();
				program::sources::const_iterator it = files.begin();
				program::sources::const_iterator end = files.end();
				for(; it != end; ++it)
				{
					const program::source_text *file =  *it;
					m_doc.div("heading2", file->filename);					
					m_doc.table("sc_table");
					
					// iterate over all lines in the file and output them in their own
					// table row with line number
					const char * ptr = file->text;
					const int linebuf_len = 2048;
					char linebuf[linebuf_len];
					char anchorbuf[linebuf_len];
					int line = 1;
					while((ptr = getline(ptr, linebuf, linebuf_len, 8)))
					{						
						make_file_line_link(file->filename, line, anchorbuf);
						m_doc.table_row("sc_row");
						m_doc.table_data_anchor_local("sc_line_number", (const char*)anchorbuf, line);
						m_doc.table_data("sc_code");
						m_doc.text(linebuf);
						m_doc.etable_data();
						m_doc.etable_row();
						++line;
					}
					
					m_doc.etable();							
				}
			}
			
			/// write out the ast, adding links to all symbols and child nodes
			void output_ast() 
			{
				m_doc.div("heading1", "Abstract Syntax Tree");
				m_doc.table("ast_table");
				
				// column headings
				m_doc.table_data("ast_heading", "Depth");
				m_doc.table_data("ast_heading", "Name");				
				m_doc.table_data("ast_heading", "Source String");
				m_doc.table_data("ast_heading", "Annotations");				
				m_doc.table_data("ast_heading", "Type");
				m_doc.table_data("ast_heading", "Token String");
				m_doc.table_data("ast_heading", "File");
				m_doc.table_data("ast_heading", "Line");
				m_doc.table_data("ast_heading", "Col");
				
				if(m_program->get_ast())
					m_program->get_ast()->accept(*this, 0);			
				m_doc.etable();				
			}
			
			/// write out the symbol table
			void output_symbol_table()
			{
				m_doc.div("heading1", "Symbol Table");
				// output each symbol table as multiple indented rows of a table
				m_doc.table("st_table");
				const compiler::scope *root = m_program->get_symbol_table()->get_root_scope();				
				output_symbol_table_aux(root, 0);				
				m_doc.etable();				
			}
			
			void output_table_indent(int depth)
			{
				for(int i = 0; i < depth; ++i)
				{
					m_doc.table_data("indent_row");	
					m_doc.text("****");
					m_doc.etable_data();				
				}								
			}
			
			void output_symbol_table_aux(const compiler::scope *scope, int depth)
			{
				// suppress empty scopes
				if(scope->children == 0 && scope->types == 0 && scope->identifiers == 0)
					return;
					
				m_doc.table_row("st_row");
				output_table_indent(depth);
				m_doc.table_data("st_scope_name\" colspan=\"7");
				m_doc.text(scope->name ? scope->name : "<Anonymous>");
				m_doc.etable_data();				
				
				// pad out to number of columns in table
				for(int i = 0; i < 6; ++i)
				{
					m_doc.table_data("st_scope_name");
					m_doc.etable_data();
				}
				m_doc.etable_row();

				// headings
				m_doc.table_row("st_default");
				output_table_indent(depth);
				m_doc.table_data("st_heading", "Name");				
				m_doc.table_data("st_heading", "Id Type");
				m_doc.table_data("st_heading", "Type Name");				
				m_doc.table_data("st_heading", "Mangled Name");
				m_doc.table_data("st_heading", "File");
				m_doc.table_data("st_heading", "Line");
				m_doc.table_data("st_heading", "Col");
				m_doc.etable_row();

				
				// write types in this scope
				types::type_info *type = scope->types;
				while(type)
				{
					m_doc.table_row("st_row");				
					output_table_indent(depth);
					m_doc.table_data("st_type_name");
					m_doc.text(type->get_name());
					m_doc.etable_data();

					m_doc.table_data("st_default");
					m_doc.text(type->get_byte_size());
					m_doc.etable_data();

					m_doc.table_data("st_default");
					m_doc.text(type->get_alignment());
					m_doc.etable_data();
					
					m_doc.etable_row();
					
					type = type->intrusive_list_get_next();
				}				

				identifier_info *id = scope->identifiers;
				while(id)
				{
					output_symbol_identifier(id, depth);
					// siblings
					identifier_info *sid = id->sibling;
					while(sid)
					{
						output_symbol_identifier(sid, depth);
						sid = sid->sibling;				
					}
					
					id = id->next;
				}				
				// output nested scopes
				compiler::scope *child = scope->children;
				while(child)
				{
					output_symbol_table_aux(child, depth+1);
					child = child->next;
				}
			}
			
			void output_symbol_identifier(identifier_info *id, int depth)
			{
				if(!id->ast_node || id->ast_node->has_annotation(ast::node::annotation_internal))
					m_doc.table_row("internal");
				else
					m_doc.table_row("st_row");

				// empty table data up to depth
				output_table_indent(depth);
			
				// create link to source code 
				char linkbuf[2048]= {0};
				parsing::token_def token;
				token.pos.file = 0;
				token.pos.line = 0;
				token.pos.col  = 0;
				if(id->ast_node)
					token = id->ast_node->get_token();
				make_file_line_link(token.pos.file, token.pos.line, linkbuf);
				
				// emit anchor so we can link to it from ast
				m_doc.table_data_anchor_local("st_id_name", (std::ptrdiff_t)(id), id->name);
				m_doc.table_data("st_default", identifier_type_strings[id->type]);
				m_doc.table_data("st_default", id->type_id ? id->type_id->get_name() : "N/A");
				m_doc.table_data("st_default", id->type_id ? id->type_id->get_mangled_name() : "N/A");
				m_doc.table_data_link_local("st_default", linkbuf, id->ast_node ? id->ast_node->get_token().pos.file : "");
				m_doc.table_data("st_default", id->ast_node ? id->ast_node->get_token().pos.line : -1);
				m_doc.table_data("st_default", id->ast_node ? id->ast_node->get_token().pos.col : -1);
				
				m_doc.etable_row();			
			}
			
			void output_structure_cb(const identifier_info *ii)
			{
				if(ii && ii->type == it_structure)
				{
					const structure_type *t = safe_cast<structure_type>(ii->type_id);
					if(t)
					{
						m_doc.div("heading2", ii->name);
						m_doc.table("s_table");						
						// headings
						m_doc.table_row("s_default");
						m_doc.table_data("s_heading", "Name");				
						m_doc.table_data("s_heading", "Type");
						m_doc.table_data("s_heading", "Size / Bytes");
						m_doc.etable_row();
						
						const structure_type::member_list &members = t->get_members();
						structure_type::member_list::const_iterator it = members.begin();
						structure_type::member_list::const_iterator end = members.end();
						for(; it != end; ++it)
						{
							const structure_type::member *m = *it;
							m_doc.table_row("s_member");
							m_doc.table_data("s_default", m->m_name);
							m_doc.table_data("s_default", m->m_type->get_name());
							m_doc.table_data("s_default", m->m_type->get_byte_size());
							m_doc.etable_row();
						}
						
						m_doc.etable();					
					}
				}
			}
			
			void output_structures_aux(const scope *s)
			{			
				const identifier_info *ii = s->identifiers;
				while(ii)
				{
					output_structure_cb(ii);
					const identifier_info *sib = ii->sibling;
					while(sib)
					{
						output_structure_cb(sib);
						sib = sib->sibling;
					}																			
					ii = ii->next;
				}
				// output nested scopes
				compiler::scope *child = s->children;
				while(child)
				{
					output_structures_aux(child);
					child = child->next;
				}
				
			}
			
			void output_structures()
			{
				m_doc.div("heading1", "Structures");				
				const scope *s = m_program->get_symbol_table()->get_root_scope();
				output_structures_aux(s);
			}
			
		private:
			typedef std::map<identifier_info*, std::string> symbol_map;
			const char*		m_filename;
			html_doc_writer	m_doc;
			compiler::program*	m_program;
			symbol_map		m_symbol_map; ///< map of symbols to their anchor names
			int				m_depth;		///< depth used when traversing ast
			
		private:
			//////////////////////////////////////////////////////////////////////////
			// visitor interface
			//////////////////////////////////////////////////////////////////////////
			typedef ast::visitor_preorder base;

			virtual result pre_visit(ast::node *n, ast::node *parent)
			{
				++m_depth;
				return base::pre_visit(n, parent);
			}
			virtual result post_visit(ast::node *n, ast::node *parent)
			{
				result r = base::post_visit(n, parent);
				--m_depth;
				return r;
				
			}
			virtual result visit(ast::node *n, ast::node *)
			{			
				if(n->has_annotation(ast::node::annotation_internal))
					m_doc.table_row("internal");				
				else	
					m_doc.table_row("ast_row");				
						
				m_doc.table_data("ast_default");
				m_doc.text(m_depth);
				m_doc.etable_data();
				if(!n)
				{				
					m_doc.text("<null>");
				}
				else
				{
					const parsing::token_def &token = n->get_token();

					char linkbuf[2048]= {0};
					
					// create link to identifier
					
					
					m_doc.table_data("ast_default", typeid(*n).name());
					m_doc.table_data("ast_default", n->string());
					m_doc.table_data("ast_default", n->has_annotation(ast::node::annotation_internal) ? "<internal>" : "");
					m_doc.table_data("ast_default", n->get_type_info() ? n->get_type_info()->get_name() : "");
					
					if(n->get_id_info())					
						m_doc.table_data_link_local("ast_default", (std::ptrdiff_t)n->get_id_info(), token.str ? token.str : "");
					else
						m_doc.table_data("ast_default", token.str ? token.str : "");										

					// create link to source code 
					make_file_line_link(token.pos.file, token.pos.line, linkbuf);
					m_doc.table_data_link_local("ast_default", linkbuf, token.pos.file ? token.pos.file : "");
					m_doc.table_data_link_local("ast_default", linkbuf, token.pos.line);
					m_doc.table_data_link_local("ast_default", linkbuf, token.pos.col);
					
					// link to parent node
// 						(void*)parent,
				}				
				m_doc.etable_row();
				
				return visit_continue;
			}
		};
	
	} // end namespace

    //--------------------------------------------------------------------

	TYCHO_SLANG_ABI void print_ast_html(compiler::program *program, const char *filename)
	{
		detail::ast_html_printer(program, filename);
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
