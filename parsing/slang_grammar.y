%token_type {parser_token_def}
%extra_argument { slang::parsing::parser::parser_state *state }
%name HlslParse

//TODO : define precedence for all operators
// operator precedence
%left AND.
%left OR.
%nonassoc EQ NE GT LT LE.
%left PLUS MINUS.
%left DIVIDE TIMES MOD.

%include 
{
#include "slang/parsing/parser_inc.h"
using namespace slang::ast;
using namespace slang;
}

%syntax_error {
	state->m_parser->report_syntax_error(state);
}

program ::= decls(B).			{ state->m_parser->parse_complete(state, MAKE_NODE_PTR(program, SAFE_CAST(declaration_list, B))); }
program ::= terminal_terminator.
decls(A) ::= decls(B) decl(C).  { ADD_NODE_TO_LIST(declaration_list, A, B, C); }
decls(A) ::= decl(B).			{ if(GET_NODE(B)) { MAKE_NODE(declaration_list, A, B, GET_NODE(B)); } }
decl  ::= SEMI.			     

decl(A) ::= typeDecl(B).		{ COPY_NODE(A, B); }
decl(A) ::= variableDecl(B).	{ COPY_NODE(A, B); }
decl(A) ::= varstructDecl(B).	{ COPY_NODE(A, B); }
decl(A) ::= functionDecl(B).	{ COPY_NODE(A, B); }
decl(A) ::= techniqueDecl(B).	{ COPY_NODE(A, B); }
decl(A) ::= technique10Decl(B). { COPY_NODE(A, B); }
decl(A) ::= cbufferDecl(B) SEMI.{ COPY_NODE(A, B); }
decl(A) ::= samplerState(B).	{ COPY_NODE(A, B); }

// hack to allow us to track when we are compiling internal code instead of users. we have to do this
// to reduce the last internal definition
decl ::= NULL_TOKEN. 

usages(A) ::= usage(B).					{ MAKE_NODE(usage_list, A, B, SAFE_CAST(usage, B)); }
usages(A) ::= usages(B) usage(C).		{ ADD_NODE_TO_LIST(usage_list, A, B, C); }
usage(A) ::= STATIC(B).					{ MAKE_NODE(usage, A, B, slang::compiler::usage_static); }
usage(A) ::= UNIFORM(B).				{ MAKE_NODE(usage, A, B, slang::compiler::usage_uniform); }
usage(A) ::= EXTERN(B).					{ MAKE_NODE(usage, A, B, slang::compiler::usage_extern); }
usage(A) ::= VOLATILE(B).				{ MAKE_NODE(usage, A, B, slang::compiler::usage_volatile); }
usage(A) ::= INLINE(B).					{ MAKE_NODE(usage, A, B, slang::compiler::usage_inline); }
usage(A) ::= SHARED(B).					{ MAKE_NODE(usage, A, B, slang::compiler::usage_shared); }
usage(A) ::= constUsage(B).				{ COPY_NODE(A,B); }
usage(A) ::= target(B).					{ MAKE_NODE(usage, A, B, slang::compiler::usage_target, GET_NODE(B)); }

constUsages(A) ::= constUsage(B).					{ MAKE_NODE(usage_list, A, B, SAFE_CAST(usage, B)); }
constUsages(A) ::= constUsages(B) constUsage(C).	{ ADD_NODE_TO_LIST(usage_list, A, B, C); }
constUsage(A) ::= CONST(B).							{ MAKE_NODE(usage, A, B, slang::compiler::usage_const_const);  }
constUsage(A) ::= ROW_MAJOR(B).						{ MAKE_NODE(usage, A, B, slang::compiler::usage_const_row_major);  }
constUsage(A) ::= COLUMN_MAJOR(B).					{ MAKE_NODE(usage, A, B, slang::compiler::usage_const_column_major);  }


type0(A) ::= type(B). {	COPY_NODE(A,B); }

usageType(A) ::= type0(B).					{ MAKE_NODE(usage_type, A, B, GET_NODE(B), 0); }
usageType(A) ::= usages(B) type0(C).		{ MAKE_NODE(usage_type, A, B, GET_NODE(C), SAFE_CAST(usage_list, B)); }

usagestructDecl(A) ::= structDecl(B).		 { COPY_NODE(A,B); }
usagestructDecl(A) ::= usages structDecl(B). { COPY_NODE(A,B); }

// catch error case where user has specified a non type
typeDecl0 ::= TYPEDEF nonTypeId(B) typeDefs.
{
	ERROR(mt_typedef_not_a_type, GET_NODE_TOKEN(B), SAFE_CAST(non_type_id, B)->name);
}

typeDecl0(A) ::= TYPEDEF(D) constType(B) typeDefs(C).	
{ 
	MAKE_NODE(typedef, A, D, SAFE_CAST(const_type, B), SAFE_CAST(variable_dim_list, C)); 
	
	// find base type
	const_type_node *type = SAFE_CAST(const_type, B);
	const type_info *base_type = type->type->get_type_info();
	
	// add all names to the symbol table
	variable_dim_list_node *list = SAFE_CAST(variable_dim_list, C);
	variable_dim_list_node::list_type::iterator it = list->begin();
	variable_dim_list_node::list_type::iterator end = list->end();
	for(; it != end; ++it)
	{	
		variable_dim_node *n =  *it;		
		const char *name = n->id->name;
		
		// check for duplicate definitions
		if(SYMBOL_TABLE.identify(name))
		{
			ERROR(mt_duplicate_type_definition, n->get_token(), name);
			// remove this entry from the list and try carry on parsing
		}
		else
		{
//			identifier_info *iinfo =
                SYMBOL_TABLE.add_type(name, base_type, n);
		}
	}
}

typeDecl(A) ::= typeDecl0(B) SEMI. { COPY_NODE(A,B); }

typeDecl ::= TYPEDEF conststructDecl typeDefs SEMI.		
typeDecl(A) ::= structDecl(B) SEMI.						{ MAKE_NODE(var_struct_decl, A, B, SAFE_CAST(struct, B), 0); }
typeDefs(A) ::= variableDim(B).							{ MAKE_NODE(variable_dim_list, A, B, SAFE_CAST(variable_dim, B)); }
typeDefs(A) ::= variableDim(B) COMMA typeDefs(C).		{ ADD_NODE_TO_LIST(variable_dim_list, A, C, B); }

type(A) ::= baseType(B).  { COPY_NODE(A, B); }
type(A) ::= struct(B).	  { COPY_NODE(A, B); }
type(A) ::= typeId(B).    { COPY_NODE(A, B); }
type(A) ::= STRUCT typeId(B). { COPY_NODE(A,B); }  


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TYPES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constType(A) ::= type(B).				   { MAKE_NODE(const_type, A, B, GET_NODE(B), 0); }
constType(A) ::= constUsages(B) type(C).   { MAKE_NODE(const_type, A, B, GET_NODE(C), SAFE_CAST(usage_list, B)); }
constTypeDim(A) ::= constType(B).		   { MAKE_NODE(const_type_dim, A, B, SAFE_CAST(const_type, B)); } 
constTypeDim(A) ::= constTypeDim(B) LBRACK constantExpr(C) RBRACK. 
{ 
	const_type_dim_node *cdt = SAFE_CAST(const_type_dim, B);
	cdt->dimensions->add_node(SAFE_CAST(expression, C));
	COPY_NODE(A,B);
}

baseType(A) ::= VOID(B).		{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_void); }
baseType(A) ::= scalarType(B).  { COPY_NODE(A, B); }
baseType(A) ::= vectorType(B).  { COPY_NODE(A, B); }
baseType(A) ::= matrixType(B).  { COPY_NODE(A, B); }
baseType(A) ::= objectType(B).  { COPY_NODE(A, B); }

scalarType(A) ::= KW_BOOL(B).			{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_bool); }
scalarType(A) ::= KW_INT(B).			{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_int); }
scalarType(A) ::= KW_UINT(B).			{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_uint); }
scalarType(A) ::= KW_HALF(B).			{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_half); }
scalarType(A) ::= KW_FLOAT(B).			{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_float); }
scalarType(A) ::= KW_DOUBLE(B).			{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_double); }

//TODO not strictly hlsl types, should probably error on these
scalarType(A) ::= KW_LONG(B).			{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_int); }
scalarType(A) ::= KW_SHORT(B).			{ MAKE_NODE(scalar_type, A, B, slang::ast::scalar_type_node::bt_int); }

vectorType(A) ::= KW_VECTOR(B).										
{	
	// this is syntatic sugar for vector<float, 4> 
	MAKE_NODE(vector_type, A, B, 0, 0);
}

vectorType(A) ::= KW_VECTOR(D) LT scalarType(B) COMMA addExpr(C) GT.	
{ 
	MAKE_NODE(vector_type, A, D, SAFE_CAST(scalar_type, B), SAFE_CAST(expression, C)); 
}

matrixType(A) ::= KW_MATRIX(B).															 
{ 
	MAKE_NODE(matrix_type, A, B, 0, 0, 0); 
}

matrixType(A) ::= KW_MATRIX(E) LT scalarType(B) COMMA constantExpr(C) COMMA addExpr(D) GT.	 
{ 
	MAKE_NODE(matrix_type, A, E, SAFE_CAST(scalar_type, B), SAFE_CAST(expression, C), SAFE_CAST(expression, D)); 
}
																			 
objectType(A) ::= KW_STRING(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_string);}
objectType(A) ::= TEXTURE(B).			{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_texture);}
objectType(A) ::= TEXTURE1D(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_texture1d); }
objectType(A) ::= TEXTURE2D(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_texture2d); }
objectType(A) ::= TEXTURE3D(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_texture3d); }
objectType(A) ::= TEXTURECUBE(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_texturecube); }
objectType(A) ::= SAMPLER(B).			{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_sampler); }
objectType(A) ::= SAMPLER1D(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_sampler1d); }
objectType(A) ::= SAMPLER2D(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_sampler2d); }
objectType(A) ::= SAMPLER3D(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_sampler3d); }
objectType(A) ::= SAMPLERCUBE(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_samplercube); }
objectType(A) ::= PIXELSHADER(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_pixelshader); }
objectType(A) ::= VERTEXSHADER(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_vertexshader); }
objectType(A) ::= PIXELFRAGMENT(B).	{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_pixelfragment); }
objectType(A) ::= VERTEXFRAGMENT(B).	{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_vertexfragment); }
objectType(A) ::= STATEBLOCK(B).		{ MAKE_NODE(object_type, A, B, slang::ast::object_type_node::ot_stateblock); }
 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STRUCTURES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
// empty anonymous struct : struct { } 
struct(A) ::= STRUCT(Z) structBegin structEnd. { MAKE_NODE(struct, A, Z, 0, 0); }

// anonymous struct : struct { ... }
struct(A) ::= STRUCT(Z) structBegin structDecls(B) structEnd. { MAKE_NODE(struct, A, Z, 0, SAFE_CAST(variable_decl_list, B)); }

// match here so we can push the struct name on the current scope before opening the new one
structDeclBegin(A) ::= STRUCT id(B).  { 	
	COPY_NODE(A, B); 
	
	// add to symbol table so we can differentiate types during lexing
	node *n = GET_NODE(B);
	if(n->isa(node::t_non_type_id))
	{
		SYMBOL_TABLE.add_structure(SAFE_CAST(non_type_id, B)->name, n);
	}
}

// match as soon as we get the opening brace so we can open a new scope
structDeclBeginAux(A) ::= structDeclBegin(B) structBegin.
{
	COPY_NODE(A,B);
	non_type_id_node *id = SAFE_CAST(non_type_id, B); 
	
	// can get a null id here if we errored
	if(id)
		state->m_parser->push_scope(state, id->name); 
}

// empty struct : struct foo {}
structDecl(A) ::= structDeclBeginAux(B) structEnd. { MAKE_NODE(struct, A, B, SAFE_CAST(non_type_id, B), 0); }

// struct : struct foo { ... }
structDecl(A) ::= structDeclBeginAux(B) structDecls(C) structEnd. { MAKE_NODE(struct, A, B, SAFE_CAST(non_type_id, B), SAFE_CAST(variable_decl_list, C)); }

conststructDecl ::= structDecl.
conststructDecl ::= constUsages structDecl.

structBegin ::= LBRACE.
structDecls(A) ::= variableDecl(B).    { MAKE_NODE(variable_decl_list, A, B, SAFE_CAST(variable_decl, B)); 	}
structDecls(A) ::= variableDecl(B) structDecls(C).  { ADD_NODE_TO_LIST(variable_decl_list, A, C, B); }

// force reduction early so scope is maintained correctly
structEnd0  ::= RBRACE. 
{ 
	state->m_parser->pop_scope(state); 	
}
structEnd   ::= structEnd0 NULL_TOKEN.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBUFFERS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// empty anonymous struct : struct { } 
//cbuffer(A) ::= KW_CBUFFER structBegin structEnd. { MAKE_NODE(cbuffer, A, 0, 0); }

// anonymous struct : struct { ... }
//cbuffer(A) ::= KW_CBUFFER structBegin structDecls(B) structEnd. { MAKE_NODE(cbuffer, A, 0, SAFE_CAST(variable_decl_list, B)); }

// match here so we can push the struct name on the scope
cbufferDeclBegin(A) ::= KW_CBUFFER id(B).  
{ 
	COPY_NODE(A, B); 
	//TODO	state->m_parser->push_typedef(state, SAFE_CAST(non_type_id, B)); 
}

// empty struct : struct foo {}
cbufferDecl(A) ::= cbufferDeclBegin(B) structBegin structEnd. { MAKE_NODE(cbuffer, A, B, SAFE_CAST(id, B), 0); }

// struct : struct foo { ... }
cbufferDecl(A) ::= cbufferDeclBegin(B) structBegin structDecls(C) structEnd. { MAKE_NODE(cbuffer, A, B, SAFE_CAST(id, B), SAFE_CAST(variable_decl_list, C)); }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SamplerState : SM 4.0 only
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

samplerState(A) ::= SAMPLER_STATE_D3D10(Z) nonTypeId(B) stateBlock(C). 
{ 
	MAKE_NODE(sampler_state10_expression, A, Z, SAFE_CAST(non_type_id, B), SAFE_CAST(state_list, C)); 
	
	// add to the symbol table
	non_type_id_node *id = SAFE_CAST(non_type_id, B);
	SYMBOL_TABLE.add_sampler_state(id->name, GET_NODE(A));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SEMANTICS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

semantic(A) ::= COLON id(B) LBRACK constantExpr(C) RBRACK.  { MAKE_NODE(id_semantic, A, B, SAFE_CAST(non_type_id, B), SAFE_CAST(expression, C)); }
semantic(A) ::= COLON id(B).								{ MAKE_NODE(id_semantic, A, B, SAFE_CAST(non_type_id, B), 0); }
semantic ::= COLON REGISTER LPAREN register RPAREN.			
semantics(A) ::= semantic(B).								{ MAKE_NODE(semantic_list, A, B, SAFE_CAST(semantic, B)); }
semantics(A) ::= semantics(B) semantic(C).					{ ADD_NODE_TO_LIST(semantic_list, A, B, C); }
semanticsOpt ::= . 
semanticsOpt(A) ::= semantics(B).							{ COPY_NODE(A,B); }

 
register ::= id. 
register ::= target COMMA id. 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ANNOTATIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
annotation ::= annotationBegin annotationEnd.  
annotation ::= annotationBegin annotationDecls annotationEnd. 
annotationOpt ::= . 
annotationOpt ::= annotation. 
annotationBegin ::= LT. 
annotationDecls ::= variableDecl. 
annotationDecls ::= variableDecl annotationDecls. 
annotationEnd ::= GT.

initializer(A) ::= ASSIGN assignmentExpr(B).				{ COPY_NODE(A,B); } 
initializer(A) ::= ASSIGN LBRACE initExprs(B) RBRACE.		{ COPY_NODE(A,B); } 
initializer(A) ::= ASSIGN LBRACE initExprs(B) COMMA RBRACE. { COPY_NODE(A,B); } 

initializerOpt(A) ::= initializer(B).						{ COPY_NODE(A,B); }
initializerOpt(A) ::= .										{ COPY_NODE(A, null_token); }

////////////////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////////////////

variableDecl(A) ::= usageType(B) variables(C) SEMI.			{ MAKE_NODE(variable_decl, A, C, SAFE_CAST(usage_type, B), SAFE_CAST(variable_list, C)); } 
varstructDecl(A) ::= usagestructDecl(B) variables(C) SEMI.	{ MAKE_NODE(var_struct_decl, A, B, SAFE_CAST(struct, B), SAFE_CAST(variable_list, C)); }
variables(A) ::= variable(B).								{ MAKE_NODE(variable_list, A, B, SAFE_CAST(variable, B)); }
variables(A) ::= variables(B) COMMA variable(C).			{ ADD_NODE_TO_LIST(variable_list, A, B, C); } 
variable(A) ::= variableDim(B) semanticsOpt(C) annotationOpt(D) initializerOpt(E). 
{ 
	MAKE_NODE(variable, A, B, SAFE_CAST(variable_dim, B), 
							  SAFE_CAST(semantic_list, C), 
							  SAFE_CAST(annotation_list, D), 
							  SAFE_CAST(expression, E)); 
}

variableDim(A) ::= id(B).
{ 
	// we can get a type_id_node here instead of a non_type_id node, this
	// is illegal but we can give a nicer error message by catching this here
	node *n = GET_NODE(B);
	if(!n->isa(node::t_non_type_id))
	{
		ERROR(mt_invalid_identifier, n->get_token(), n->get_token().str);

		// make up an identifier name and use that so we can keep on going
		// and try to find more errors
		B.ast_node = MAKE_NON_TYPE_ID("__noname__");
	}
	expression_list_node *eln = MAKE_NODE_PTR(expression_list);
	MAKE_NODE(variable_dim, A, B, SAFE_CAST(non_type_id, B), eln); 
}
variableDim(A) ::= variableDim(B) LBRACK RBRACK.				{ ADD_NODE_TO_LIST(variable_dim, A, B, null_token); }
variableDim(A) ::= variableDim(B) LBRACK constantExpr(C) RBRACK.{ ADD_NODE_TO_LIST(variable_dim, A, B, C); } 


////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////

functionDecl(A) ::= functionDef(B) SEMI.							
{ 
	MAKE_NODE(function_decl, A, B, SAFE_CAST(function, B), 0, 0);
	
	// pop parameter scope
	SYMBOL_TABLE.pop_scope();	
// 	function_node *fn = SAFE_CAST(function, B);
// 	SYMBOL_TABLE.add_function(fn->name->name, GET_NODE(A));
}

// reduce this rule before reading the statement block as it may reference this function recursively.
functionDecl0(A) ::= functionDef(B).
{
	COPY_NODE(A,B);
	
	//TODO too late here, need to reduce before param block
	//TODO we may already have an entry in there from a prototype so if we do then update that 
	// to point to this.
// 	function_node *fn = SAFE_CAST(function, B);
// 	SYMBOL_TABLE.add_function(fn->name->name, GET_NODE(A));
}

functionDecl(A) ::= functionDecl0(B) annotationOpt(C) stmtBlock(D).	
{ 
	MAKE_NODE(function_decl, A, B, SAFE_CAST(function, B), SAFE_CAST(annotation_list, C), SAFE_CAST(statement_list, D)); 	
	
	// pop the function parameter scope
	SYMBOL_TABLE.pop_scope();
}

functionDef0(A) ::= usageType(B) id(C).
{
	non_type_id_node *id = SAFE_CAST(non_type_id, C);
	MAKE_NODE(function, A, C, SAFE_CAST(usage_type, B), id);
	SYMBOL_TABLE.push_scope(id->name);
}

functionDef(A) ::= functionDef0(B) paramList(D) semanticsOpt(E). 
{ 
	function_node *fn = SAFE_CAST(function, B);
	fn->params = SAFE_CAST(parameter_list, D);
	fn->semantics = SAFE_CAST(semantic_list, E); 
	COPY_NODE(A,B);
}

paramList ::= LPAREN RPAREN.
//paramList ::= LPAREN VOID RPAREN.
paramList(A) ::= LPAREN parameterDecls(B) RPAREN. { COPY_NODE(A, B); }
//paramListBegin ::= LPAREN.
//paramListEnd ::= RPAREN.

variableOpt ::= .
variableOpt(A) ::= variable(B). { COPY_NODE(A,B); }

parameterDecls(A) ::= parameterDecl(B).	{ MAKE_NODE(parameter_list, A, B, SAFE_CAST(parameter, B)); } 
parameterDecls(A) ::= parameterDecls(B) COMMA parameterDecl(C). { ADD_NODE_TO_LIST(parameter_list, A, B, C); }
parameterDecl(A) ::= paramUsageType(B) variableOpt(C). { MAKE_NODE(parameter, A, B, SAFE_CAST(usage_type, B), SAFE_CAST(variable, C)); } 

paramUsageType(A) ::= type(B).					{ MAKE_NODE(usage_type, A, B, GET_NODE(B), 0); }
paramUsageType(A) ::= paramUsages(B) type(C).	{ MAKE_NODE(usage_type, A, C, GET_NODE(C), SAFE_CAST(usage_list, B)); }

paramUsages(A) ::= paramUsage(B). { MAKE_NODE(usage_list, A, B, SAFE_CAST(usage, B)); }
paramUsages(A) ::= paramUsages(B) paramUsage(C). { ADD_NODE_TO_LIST(usage_list, A, B, C); }
paramUsage(A) ::= IN(B).			{ MAKE_NODE(usage, A, B, usage_param_in); }
paramUsage(A) ::= OUT(B).			{ MAKE_NODE(usage, A, B, usage_param_out); }
paramUsage(A) ::= INOUT(B).		{ MAKE_NODE(usage, A, B, usage_param_inout); }
paramUsage(A) ::= UNIFORM(B).		{ MAKE_NODE(usage, A, B, usage_param_uniform); }
paramUsage(A) ::= constUsage(B).{ COPY_NODE(A,B); }

////////////////////////////////////////////////////////////////////////////////////
// D3D9 techniques
////////////////////////////////////////////////////////////////////////////////////

techniqueDecl(A) ::= TECHNIQUE(Z) idOpt(B) annotationOpt(C) techniqueBody(D). { MAKE_NODE(technique_decl, A, Z, SAFE_CAST(id, B), SAFE_CAST(annotation, C), SAFE_CAST(technique, D)); }
techniqueBody(A) ::= techniqueBegin(Z) techniqueEnd.			{ MAKE_NODE(technique, A, Z, 0); }
techniqueBody(A) ::= techniqueBegin(Z) passDecls(B) techniqueEnd. { MAKE_NODE(technique, A, Z, SAFE_CAST(pass_decl_list, B)); }
techniqueBegin ::= LBRACE.
techniqueEnd ::= RBRACE NULL_TOKEN.

passDecls(A) ::= passDecl(B).									{ MAKE_NODE(pass_decl_list, A, B, SAFE_CAST(pass_decl, B)); }
passDecls(A) ::= passDecls(B) passDecl(C).						{ ADD_NODE_TO_LIST(pass_decl_list, A, B, C); }
passDecl(A) ::= PASS(Z) idOpt(B) annotationOpt(C) stateBlock(D).	{ MAKE_NODE(pass_decl, A, Z, SAFE_CAST(id, B), SAFE_CAST(annotation, C), SAFE_CAST(state_list, D)); }

// empty state block
stateBlock(A) ::= stateBlockBegin(Z) stateBlockEnd.			{ MAKE_NODE(state_list, A, Z); }
stateBlock(A) ::= stateBlockBegin states(B) stateBlockEnd.  { COPY_NODE(A,B); }
stateBlockBegin ::= LBRACE. 
stateBlockEnd0 ::= RBRACE.
stateBlockEnd ::= stateBlockEnd0 NULL_TOKEN.  

states(A) ::= state(B).				{ MAKE_NODE(state_list, A, B, SAFE_CAST(state, B)); }
states(A) ::= states(B) state(C).	{ ADD_NODE_TO_LIST(state_list, A, B, C); }
state(A) ::= id(B) stateIndex(C) stateExprBegin stateExpr(D) stateExprEnd. { MAKE_NODE(state, A, B, SAFE_CAST(id, B), GET_NODE(C), SAFE_CAST(expression, D)); }

stateIndex ::=.
stateIndex ::= LBRACK uint RBRACK.
stateExprBegin ::= ASSIGN.
stateExprEnd ::= SEMI.

////////////////////////////////////////////////////////////////////////////////////
// D3D10 techniques
////////////////////////////////////////////////////////////////////////////////////

technique10Decl(A) ::= TECHNIQUE_D3D10(Z) idOpt(B) annotationOpt(C) technique10Body(D). { MAKE_NODE(technique10_decl, A, Z, SAFE_CAST(id, B), SAFE_CAST(annotation, C), SAFE_CAST(technique10, D)); }
technique10Body(A) ::= techniqueBegin(Z) techniqueEnd.			{ MAKE_NODE(technique10, A, Z, 0); }
technique10Body(A) ::= techniqueBegin(Z) pass10Decls(B) techniqueEnd. { MAKE_NODE(technique10, A, Z, SAFE_CAST(pass10_decl_list, B)); }

pass10Decls(A) ::= pass10Decl(B).									{ MAKE_NODE(pass10_decl_list, A, B, SAFE_CAST(pass10_decl, B)); }
pass10Decls(A) ::= pass10Decls(B) pass10Decl(C).						{ ADD_NODE_TO_LIST(pass10_decl_list, A, B, C); }
pass10Decl0(A) ::= PASS idOpt(B).	
{ 
	COPY_NODE(A,B);
	// push pass specific symbol table onto the scope stack, this contains predefined
	// names visible in a pass scope.
	SYMBOL_TABLE.push_pass_scope(SAFE_CAST(non_type_id, B)->name);
}
pass10Decl(A) ::= pass10Decl0(B) annotationOpt(C) state10Block(D).
{
	MAKE_NODE(pass10_decl, A, B, SAFE_CAST(id, B), SAFE_CAST(annotation, C), SAFE_CAST(state10_list, D)); 
}	

// empty state block
state10Block(A) ::= stateBlockBegin10(Z) stateBlockEnd10.			{ MAKE_NODE(state_list, A, Z); }
state10Block(A) ::= stateBlockBegin10 states10(B) stateBlockEnd10.  { COPY_NODE(A,B); }

// empty state block
stateBlockBegin10 ::= LBRACE. 
stateBlockEnd10_0 ::= RBRACE.
{
	// close the pass symbol table
	SYMBOL_TABLE.pop_scope();
}

stateBlockEnd10 ::= stateBlockEnd10_0 NULL_TOKEN.  

states10(A) ::= state10(B).				
{ 
	// node can be null if there was an error in its definition
	if(GET_NODE(B))
	{
		MAKE_NODE(state10_list, A, B, SAFE_CAST(function_call, B)); 
	}
}

states10(A) ::= states10(B) state10(C).	
{ 
	// node can be null if there was an error in its definition
	if(GET_NODE(C))
	{ 
		ADD_NODE_TO_LIST(state10_list, A, B, C); 
	} 
}

state10(A) ::= nonTypeId(B) LPAREN argumentsOpt(C) RPAREN SEMI. 
{ 
	non_type_id_node *id = SAFE_CAST(non_type_id, B);
	identifier_info *ii = IDENTIFY_THIS_SCOPE(id->name);
	if(!ii)
	{
		ERROR(mt_undeclared_function, id->get_token(), id->name);
	}
	else if(ii && ii->type != it_function)
	{
		ERROR(mt_identifier_not_a_function, id->get_token(), id->name);
	}
	else
	{		
		MAKE_NODE(function_call, A, B, id, SAFE_CAST(expression_list, C)); 
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Statements
////////////////////////////////////////////////////////////////////////////////////

stmtBlock(A) ::= stmtBlockBegin(Z) stmtBlockEnd.			{ MAKE_NODE(statement_list, A, Z); }
stmtBlock(A) ::= stmtBlockBegin stmts(B) stmtBlockEnd.	{ COPY_NODE(A,B); }
stmtBlockBegin ::= LBRACE.								{ state->m_parser->push_scope(state, "$StatementBlock"); }
stmtBlockEnd1 ::= RBRACE.								{ state->m_parser->pop_scope(state); }
stmtBlockEnd ::= stmtBlockEnd1 NULL_TOKEN.
stmts(A) ::= stmt(B).			{ MAKE_NODE(statement_list, A, B, SAFE_CAST(statement, B)); }
stmts(A) ::= stmts(B) stmt(C).	{ ADD_NODE_TO_LIST(statement_list, A, B, C); }

// don't let syntax errors propogate past the statement level.
stmt		  ::= error	SEMI.

simpleStmt(A) ::= SEMI(B).		{ MAKE_NODE(statement_list, A, B, MAKE_NODE_PTR(empty_statement));  }
simpleStmt(A) ::= expr(B) SEMI. { MAKE_NODE(expression_statement, A, B, SAFE_CAST(expression,B)); }
simpleStmt(A) ::= RETURN(B) SEMI.	{ MAKE_NODE(return_statement, A, B, SAFE_CAST(expression,B)); }
simpleStmt(A) ::= RETURN(Z) expr(B) SEMI.{ MAKE_NODE(return_statement, A, Z, SAFE_CAST(expression, B)); }
simpleStmt(A) ::= DO(Z) stmt(B) WHILE LPAREN expr(C) RPAREN SEMI.  { MAKE_NODE(do_while_statement, A, Z, SAFE_CAST(statement_list, B), SAFE_CAST(expression_list, C)); }
simpleStmt(A) ::= stmtBlock(B).		{ COPY_NODE(A,B); }
simpleStmt(A) ::= DISCARD(B) SEMI.		{ MAKE_NODE(discard_statement, A, B); }
simpleStmt(A) ::= typeDecl(B).		{ MAKE_NODE(declaration_statement, A, B, GET_NODE(B)); }
simpleStmt(A) ::= variableDecl(B).  { MAKE_NODE(declaration_statement, A, B, GET_NODE(B)); }
simpleStmt(A) ::= varstructDecl(B). { MAKE_NODE(declaration_statement, A, B, GET_NODE(B)); }

nonIfStmt(A) ::= simpleStmt(B). { COPY_NODE(A,B); }
nonIfStmt(A) ::= WHILE(Z) LPAREN expr(B) RPAREN nonIfStmt(C). { MAKE_NODE(while_statement, A, Z, SAFE_CAST(statement, C), SAFE_CAST(expression, B)); }
nonIfStmt(A) ::= for(Z) LPAREN forInit(B) forCond(C) forStep(D) RPAREN nonIfStmt(E). 
{ 
	MAKE_NODE(for_statement, A, Z, GET_NODE(B), GET_NODE(C), GET_NODE(D), SAFE_CAST(statement, E)); 
	
	// close for loop scope
	SYMBOL_TABLE.pop_scope();
}

stmt(A) ::= simpleStmt(B). { COPY_NODE(A,B); }
stmt(A) ::= WHILE(Z) LPAREN expr(B) RPAREN stmt(C).						{ MAKE_NODE(while_statement, A, Z, SAFE_CAST(statement, C), SAFE_CAST(expression, B)); }
stmt(A) ::= for(Z) LPAREN forInit(B) forCond(C) forStep(D) RPAREN stmt(E). 
{ 
	MAKE_NODE(for_statement, A, Z, GET_NODE(B), GET_NODE(C), GET_NODE(D), SAFE_CAST(statement, E)); 
	
	// close for loop scope
	SYMBOL_TABLE.pop_scope();
}
	
stmt(A) ::= IF(Z) LPAREN expr(B) RPAREN stmt(C).					{ MAKE_NODE(if_else_statement, A, Z, SAFE_CAST(expression_list, B), SAFE_CAST(statement, C), 0); }
stmt(A) ::= IF(Z) LPAREN expr(B) RPAREN nonIfStmt(C) ELSE stmt(D). { MAKE_NODE(if_else_statement, A, Z, SAFE_CAST(expression_list, B), SAFE_CAST(statement, C), SAFE_CAST(statement, D)); }


////////////////////////////////////////////////////////////////////////////////////
// For loops
////////////////////////////////////////////////////////////////////////////////////

for(A) ::= FOR(B).	
{	
	A.token = B.token;
	// open a new scope so all initialisers get handled correctly
	SYMBOL_TABLE.push_scope("for");
}

forInit ::= SEMI.
forInit(A) ::= expr(B) SEMI.	{ COPY_NODE(A,B); }
forInit(A) ::= variableDecl(B). { COPY_NODE(A,B); }
forCond ::= SEMI.
forCond(A) ::= expr(B) SEMI.	{ COPY_NODE(A,B); }
forStep ::= .
forStep(A) ::= expr(B).			{ COPY_NODE(A,B); } 


//TODO : BAND, BNOT, BXOR
dwordExpr(A) ::= dword(B).			{ COPY_NODE(A,B); }
dwordExpr ::= dword BOR dwordExpr.

stateExpr(A) ::= dwordExpr(B).						{ MAKE_NODE(expression_list, A, B, SAFE_CAST(expression, B)); }
stateExpr(A) ::= complexExpr(B).					{ MAKE_NODE(expression_list, A, B, SAFE_CAST(expression, B)); }
stateExpr(A) ::= LBRACE initExprs(B) RBRACE.		{  MAKE_NODE(expression_list, A, B, SAFE_CAST(expression_list, B)); }
stateExpr(A) ::= LBRACE initExprs(B) COMMA RBRACE.	{  MAKE_NODE(expression_list, A, B, SAFE_CAST(expression_list, B)); }
stateExpr(A) ::= LT relationalExpr(B) GT.			{  MAKE_NODE(state_variable_expression, A, B, SAFE_CAST(expression, B)); }

simpleExpr(A) ::= KW_TRUE(B).			{ MAKE_NODE(bool_literal, A, B, true, B.token.str); }
simpleExpr(A) ::= KW_FALSE(B).			{ MAKE_NODE(bool_literal, A, B, false, B.token.str); }
simpleExpr(A) ::= uint(B).			{ COPY_NODE(A, B); }
simpleExpr(A) ::= float(B).			{ COPY_NODE(A, B); }
simpleExpr(A) ::= strings(B).		{ COPY_NODE(A, B); }
simpleExpr(A) ::= nonTypeId(B).		{ COPY_NODE(A, B); }

complexExpr(A) ::= LPAREN expr(B) RPAREN.						{ COPY_NODE(A,B); }

complexExpr(A) ::= typeId(B) LPAREN argumentsOpt(C) RPAREN.		{ MAKE_NODE(constructor_expression, A, B, SAFE_CAST(type_id, B), SAFE_CAST(expression_list, C)); }
complexExpr(A) ::= baseType(B) LPAREN argumentsOpt(C) RPAREN.   { MAKE_NODE(constructor_expression, A, B, SAFE_CAST(type_id, B), SAFE_CAST(expression_list, C)); }

// function call
complexExpr(A) ::= nonTypeId(B) LPAREN argumentsOpt(C) RPAREN.	{ MAKE_NODE(function_call, A, B, SAFE_CAST(non_type_id, B), SAFE_CAST(expression_list, C)); } 
complexExpr(A) ::= objectExpr(B).								{ COPY_NODE(A,B); } 

objectExpr ::= asmDecl. 
objectExpr ::= asmDecl asm.
objectExpr ::= asm. 
objectExpr ::= asmFragment. 

//TODO : Dx9 only?
objectExpr(A) ::= COMPILE(Z) target(B) nonTypeId(C) LPAREN argumentsOpt(D) RPAREN.			 { MAKE_NODE(compile_expression, A, Z, SAFE_CAST(non_type_id, B), SAFE_CAST(non_type_id, C), SAFE_CAST(expression_list, D)); }
objectExpr(A) ::= SAMPLER_STATE(Z) stateBlock(B).											 { MAKE_NODE(sampler_state_expression, A, Z, SAFE_CAST(state_list, B)); }
objectExpr(A) ::= COMPILE_FRAGMENT(Z) target(B) nonTypeId(C) LPAREN argumentsOpt(D) RPAREN. { MAKE_NODE(compile_fragment_expression, A, Z, SAFE_CAST(non_type_id, B), SAFE_CAST(non_type_id, C), SAFE_CAST(expression_list, D)); }
objectExpr(A) ::= STATEBLOCK_STATE(Z) stateBlock(B).										 { MAKE_NODE(stateblock_state_expression, A, Z, SAFE_CAST(state_list, B)); }


primaryExpr(A) ::= simpleExpr(B).	{ COPY_NODE(A,B); }
primaryExpr(A) ::= complexExpr(B).	{ COPY_NODE(A,B); }
postfixExpr(A) ::= primaryExpr(B).	{ COPY_NODE(A,B); }

postfixArray(A) ::= postfixExpr(B) LBRACK expr(C) RBRACK.
{ 
	if(GET_NODE(B)->isa(slang::ast::node::t_postfix_array_expression))
	{
		postfix_array_expression_node *e = SAFE_CAST(postfix_array_expression, B); 
		e->dimensions.push_back(SAFE_CAST(expression, C));
		COPY_NODE(A,B);	
	}
	else
	{
		MAKE_NODE(postfix_array_expression, A, B, SAFE_CAST(expression, B), SAFE_CAST(expression, C)); 
	}
} 


//postfixExpr(A) ::= postfixExpr(B) LBRACK expr(C) RBRACK.  { MAKE_NODE(postfix_array_expression, A, B, SAFE_CAST(expression, B), SAFE_CAST(expression, C)); }

//postfixArray(A) ::= postfixArray(B) LBRACK expr(C) RBRACK. 
//{ 
//	postfix_array_expression_node *e = SAFE_CAST(postfix_array_expression, B); 
//	e->dimensions.push_back(SAFE_CAST(expression, C));
//	COPY_NODE(A,B);
//}

postfixExpr(A) ::= postfixArray(B). { COPY_NODE(A,B); }
postfixExpr(A) ::= postfixExpr(B) DOT id(C). { 	MAKE_NODE(postfix_member_expression, A, B, SAFE_CAST(expression, B), SAFE_CAST(non_type_id, C)); }
postfixExpr(A) ::= postfixExpr(B) INC(O).		 { MAKE_UNARY_EXPR(postfix_inc, A, B, O); }
postfixExpr(A) ::= postfixExpr(B) DEC(O).		 { MAKE_UNARY_EXPR(postfix_dec, A, B, O); }

// D3D10 member function call
postfixExpr(A) ::= postfixExpr(B) DOT nonTypeId(C) LPAREN argumentsOpt(D) RPAREN. { MAKE_NODE(postfix_member_call_expression, A, B, SAFE_CAST(expression, B), SAFE_CAST(non_type_id, C), SAFE_CAST(expression_list, D)); }


unaryExpr(A) ::= postfixExpr(B).		{ COPY_NODE(A,B); }
unaryExpr(A) ::= INC(O) unaryExpr(B).		{ MAKE_UNARY_EXPR(prefix_inc, A, B, O); }
unaryExpr(A) ::= DEC(O) unaryExpr(B).		{ MAKE_UNARY_EXPR(prefix_dec, A, B, O); }
unaryExpr(A) ::= NOT(O) castExpr(B).		{ MAKE_UNARY_EXPR(prefix_not, A, B, O); }
unaryExpr(A) ::= MINUS(O) castExpr(B).		{ MAKE_UNARY_EXPR(prefix_minus, A, B, O); }
unaryExpr(A) ::= PLUS(O) castExpr(B).		{ MAKE_UNARY_EXPR(prefix_plus, A, B, O); } 
castExpr(A) ::= unaryExpr(B).			{ COPY_NODE(A,B); }
castExpr(A) ::= LPAREN constTypeDim(B) RPAREN castExpr(C). { MAKE_NODE(cast_expression, A, B, SAFE_CAST(const_type_dim, B), SAFE_CAST(expression, C)); }

mulExpr(A) ::= castExpr(B).					  { COPY_NODE(A,B); }
mulExpr(A) ::= mulExpr(B) TIMES(O) castExpr(C).  { MAKE_BINARY_EXPR(times, A, B, C, O); }
mulExpr(A) ::= mulExpr(B) DIVIDE(O) castExpr(C). { MAKE_BINARY_EXPR(divide, A, B, C, O); }
mulExpr(A) ::= mulExpr(B) MOD(O) castExpr(C).    { MAKE_BINARY_EXPR(mod, A, B, C, O); }

addExpr(A) ::= mulExpr(B).						{ COPY_NODE(A,B); }
addExpr(A) ::= addExpr(B) PLUS(O) mulExpr(C).		{ MAKE_BINARY_EXPR(add, A, B, C, O); }
addExpr(A) ::= addExpr(B) MINUS(O) mulExpr(C).		{ MAKE_BINARY_EXPR(minus, A, B, C, O); }

relationalExpr(A) ::= addExpr(B).						{ COPY_NODE(A,B); }
relationalExpr(A) ::= relationalExpr(B) LT(O) addExpr(C).	{ MAKE_BINARY_EXPR(comparision_lt, A, B, C, O); }
relationalExpr(A) ::= relationalExpr(B) GT(O) addExpr(C).	{ MAKE_BINARY_EXPR(comparision_gt, A, B, C, O); }

relationalExpr(A) ::= relationalExpr(B) LE(O) addExpr(C). { MAKE_BINARY_EXPR(comparision_le, A, B, C, O); }
relationalExpr(A) ::= relationalExpr(B) GE(O) addExpr(C). { MAKE_BINARY_EXPR(comparision_ge, A, B, C, O); }

equalityExpr(A) ::= relationalExpr(B).						{ COPY_NODE(A,B); }
equalityExpr(A) ::= equalityExpr(B) EQ(O) relationalExpr(C).	{ MAKE_BINARY_EXPR(comparision_eq, A, B, C, O); }
equalityExpr(A) ::= equalityExpr(B) NE(O) relationalExpr(C).	{ MAKE_BINARY_EXPR(comparision_ne, A, B, C, O); }

andExpr(A) ::= equalityExpr(B).						{ COPY_NODE(A,B); }
andExpr(A) ::= andExpr(B) LAND(O) equalityExpr(C).		{ MAKE_BINARY_EXPR(logical_and, A, B, C, O); }
orExpr(A)  ::= andExpr(B).							{ COPY_NODE(A,B); }
orExpr(A)  ::= orExpr(B) LOR(O) andExpr(C).				{ MAKE_BINARY_EXPR(logical_or, A, B, C, O); }
conditionalExpr(A) ::= orExpr(B).					{ COPY_NODE(A,B); }
conditionalExpr(A) ::= orExpr(B) QUESTION assignmentExpr(C) COLON conditionalExpr(D). 
{ 
	MAKE_NODE(ternary_expression, A, B, SAFE_CAST(expression, B), 
									SAFE_CAST(expression,C),
									SAFE_CAST(expression,D)); 
}

assignmentExpr(A) ::= conditionalExpr(B).						{ COPY_NODE(A,B); }
assignmentExpr(A) ::= castExpr(B) ASSIGN(O) assignmentExpr(C).		{ MAKE_BINARY_EXPR(assign, A, B, C, O); }
assignmentExpr(A) ::= castExpr(B) MULASSIGN(O) assignmentExpr(C).	{ MAKE_BINARY_EXPR(times_assign, A, B, C, O); }
assignmentExpr(A) ::= castExpr(B) DIVASSIGN(O) assignmentExpr(C).	{ MAKE_BINARY_EXPR(divide_assign, A, B, C, O); }
assignmentExpr(A) ::= castExpr(B) ADDASSIGN(O) assignmentExpr(C).	{ MAKE_BINARY_EXPR(add_assign, A, B, C, O); }
assignmentExpr(A) ::= castExpr(B) SUBASSIGN(O) assignmentExpr(C).	{ MAKE_BINARY_EXPR(minus_assign, A, B, C, O); }
assignmentExpr(A) ::= castExpr(B) MODASSIGN(O) assignmentExpr(C).	{ MAKE_BINARY_EXPR(mod_assign, A, B, C, O); }

arguments(A) ::= assignmentExpr(B). { MAKE_NODE(expression_list, A, B, SAFE_CAST(expression, B)); }
arguments(A) ::= arguments(B) COMMA assignmentExpr(C).  { ADD_NODE_TO_LIST(expression_list, A, B, C); }
argumentsOpt ::= .
argumentsOpt(A) ::= arguments(B). { COPY_NODE(A,B); }


initExpr(A) ::= assignmentExpr(B).					{ COPY_NODE(A,B); }
initExpr(A) ::= LBRACE initExprs(B) RBRACE.			{ COPY_NODE(A,B); }
initExpr(A) ::= LBRACE initExprs(B) COMMA RBRACE.	{ COPY_NODE(A,B); }
initExprs(A) ::= initExpr(B).						{ MAKE_NODE(expression_list, A, B, SAFE_CAST(expression, B)); }
initExprs(A) ::= initExprs(B) COMMA initExpr(C).	{ ADD_NODE_TO_LIST(expression_list, A, B, C); }

constantExpr(A) ::= assignmentExpr(B).			{ COPY_NODE(A, B); }
expr(A) ::= assignmentExpr(B).					{ MAKE_NODE(expression_list, A, B, SAFE_CAST(expression, B)); }
expr(A) ::= expr(B) COMMA assignmentExpr(C).	{ ADD_NODE_TO_LIST(expression_list, A, B, C); }		

dword(A) ::= uint(B).		{ COPY_NODE(A,B); }
dword(A) ::= MINUS(O) uint(B). { MAKE_UNARY_EXPR(prefix_minus, A, B, O); }
dword(A) ::= float(B).		{ COPY_NODE(A,B); }
dword(A) ::= MINUS(O) float(B).{ MAKE_UNARY_EXPR(prefix_minus, A, B, O); }
dword(A) ::= dwordId(B).	{ COPY_NODE(A,B); }
dword ::= uint dwordId. 
dwordId(A) ::= id(B).	 { MAKE_NODE(id_literal, A, B, SAFE_CAST(id, B)); }
dwordId(A) ::= KW_TRUE(B).  { MAKE_NODE(bool_literal, A, B, true, B.token.str); }
dwordId(A) ::= KW_FALSE(B). { MAKE_NODE(bool_literal, A, B, false, B.token.str); }
dwordId ::= TEXTURE. 

id(A) ::= typeId(B).	 { COPY_NODE(A,B); }
id(A) ::= nonTypeId(B).  { COPY_NODE(A,B); }
idOpt ::= .
idOpt(A) ::= id(B).		 { COPY_NODE(A,B); }
target(A) ::= nonTypeId(B). { COPY_NODE(A,B); }


uint(A) ::= UINT(B).	{ MAKE_NODE(int_literal, A, B, core::atoi(B.str),B.token.str); SAVE_TOKEN(A,B);}
uint(A) ::= INT32(B).	{ MAKE_NODE(int_literal, A, B, core::atoi(B.str), B.token.str); SAVE_TOKEN(A,B);}
uint(A) ::= UINT32(B).	{ MAKE_NODE(int_literal, A, B, core::atoi(B.str), B.token.str); SAVE_TOKEN(A,B);}
uint(A) ::= INT_CONST(B).		{ MAKE_NODE(int_literal, A, B, core::atoi(B.str), B.token.str); SAVE_TOKEN(A,B);}


// these are all valid c but illegal in hlsl so give a nice error
uint(A) ::= INT_CONST_U(B).		{ ERROR(mt_unsupported_integral_suffix, B.token, 0); MAKE_NODE(int_literal, A, B, core::atoi(B.str), B.token.str); SAVE_TOKEN(A, B); }
uint(A) ::= INT_CONST_L(B).		{ ERROR(mt_unsupported_integral_suffix, B.token, 0); MAKE_NODE(int_literal, A, B, core::atoi(B.str), B.token.str); SAVE_TOKEN(A, B); }
uint(A) ::= INT_CONST_UL(B).	{ ERROR(mt_unsupported_integral_suffix, B.token, 0); MAKE_NODE(int_literal, A, B, core::atoi(B.str), B.token.str); SAVE_TOKEN(A, B); }
uint(A) ::= INT_CONST_LL(B).	{ ERROR(mt_unsupported_integral_suffix, B.token, 0); MAKE_NODE(int_literal, A, B, core::atoi(B.str), B.token.str); SAVE_TOKEN(A, B); }
uint(A) ::= INT_CONST_ULL(B).	{ ERROR(mt_unsupported_integral_suffix, B.token, 0); MAKE_NODE(int_literal, A, B, core::atoi(B.str), B.token.str); SAVE_TOKEN(A, B); }
float(A) ::= LONGDOUBLE_CONST(B).	{ ERROR(mt_unsupported_float_suffix, B.token, 0); MAKE_NODE(float_literal, A, B, (float)core::atof(B.str), B.token.str); SAVE_TOKEN(A, B); }
float(A) ::= FLOAT_CONST(B).		{ MAKE_NODE(float_literal, A, B, (float)core::atof(B.str), B.token.str); SAVE_TOKEN(A, B); }

//??
// float ::= FLOAT16.	
// float ::= FLOAT32.
// float ::= FLOAT64.

strings ::= string.
strings ::= strings string.
string  ::= STRING.

typeId(A) ::= TYPE_ID(B). { MAKE_NODE(type_id, A, B, B.str); SAVE_TOKEN(A,B); }
nonTypeId(A) ::= NON_TYPE_ID(B). { MAKE_NODE(non_type_id, A, B, B.str); SAVE_TOKEN(A,B); }

asmDecl ::= DECL LBRACE.
asm ::= ASM LBRACE.
asmFragment ::= ASM_FRAGMENT LBRACE.

// ensure this is last as we use it to add a count to the number of tokens the
// parser has. it must never be actually passed in.
terminal_terminator ::= TOKEN_COUNT. { TYCHO_ASSERT(!"dont't pass in TOKEN_COUNT"); }