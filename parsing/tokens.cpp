//////////////////////////////////////////////////////////////////////////////
// Tycho Game Library
// Copyright (C) 2008 Martin Slater
// Created : Monday, 5 October 2009 10:30:07 PM
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "tokens.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS
//////////////////////////////////////////////////////////////////////////////
namespace tycho
{
namespace slang
{
namespace parsing
{
    //--------------------------------------------------------------------


	const char* to_string(tokens tok)
	{
		static bool init = true;
		static const char *strings[TOTAL_TOKEN_COUNT];
		if(init)
		{
			init = false;		

	#define TOKEN(_name) TYCHO_ASSERT(strings[_name] == 0); strings[_name] = #_name;
			TOKEN(MINUS)
			TOKEN(PLUS)
			TOKEN(END_OF_FILE)
			TOKEN(WSPC)
			TOKEN(RETURN_TOKEN)
			TOKEN(COMMENT)
			TOKEN(PREPROC)
			TOKEN(RBRACK)
			TOKEN(LBRACK)
			TOKEN(LPAREN)
			TOKEN(RPAREN)
			TOKEN(LBRACE)
			TOKEN(RBRACE)
			TOKEN(DOT)
			TOKEN(PTR)
			TOKEN(INC)
			TOKEN(DEC)
			TOKEN(BAND)
			TOKEN(TIMES)
			TOKEN(BNOT)
			TOKEN(NOT)
			TOKEN(DIV)
			TOKEN(MOD)
			TOKEN(LSHIFT)
			TOKEN(RSHIFT)
			TOKEN(LT)
			TOKEN(GT)
			TOKEN(LE)
			TOKEN(GE)
			TOKEN(EQ)
			TOKEN(NE)
			TOKEN(BXOR)
			TOKEN(BOR)
			TOKEN(LAND)
			TOKEN(LOR)
			TOKEN(QUESTION)
			TOKEN(COLON)
			TOKEN(SEMI)
			TOKEN(ELLIPSIS)
			TOKEN(ASSIGN)
			TOKEN(MULASSIGN)
			TOKEN(DIVASSIGN)
			TOKEN(MODASSIGN)
			TOKEN(ADDASSIGN)
			TOKEN(SUBASSIGN)
			TOKEN(ANDASSIGN)
			TOKEN(XORASSIGN)
			TOKEN(ORASSIGN)
			TOKEN(SHLASSIGN)
			TOKEN(SHRASSIGN)
			TOKEN(COMMA)
			TOKEN(CHAR_CONST)
			TOKEN(WIDE_CHAR_CONST)
			TOKEN(INT_CONST)
			TOKEN(INT_CONST_U)
			TOKEN(INT_CONST_L)
			TOKEN(INT_CONST_UL)
			TOKEN(INT_CONST_LL)
			TOKEN(INT_CONST_ULL)
			TOKEN(FLOAT_CONST)
			TOKEN(LONGDOUBLE_CONST)
			TOKEN(STR_CONST)
			TOKEN(WIDE_STR_CONST)
			TOKEN(AUTO)
			TOKEN(BREAK)
			TOKEN(CASE)
			TOKEN(CLANGCHAR)
			TOKEN(CONST)
			TOKEN(CONTINUE)
			TOKEN(DEFAULT)
			TOKEN(DO)
			TOKEN(ELSE)
			TOKEN(ENUM)
			TOKEN(EXTERN)
			TOKEN(KW_FLOAT)
			TOKEN(FOR)
			TOKEN(GOTO)
			TOKEN(IF)
			TOKEN(INLINE)
			TOKEN(KW_INT)
			TOKEN(KW_LONG)
			TOKEN(REGISTER)
			TOKEN(RESTRICT)
			TOKEN(RETURN)
			TOKEN(CLANGSHORT)
			TOKEN(SIGNED)
			TOKEN(SIZEOF)
			TOKEN(STATIC)
			TOKEN(STRUCT)
			TOKEN(SWITCH)
			TOKEN(TYPEDEF)
			TOKEN(UNION)
			TOKEN(UNSIGNED)
			TOKEN(VOID)
			TOKEN(VOLATILE)
			TOKEN(WHILE)
			TOKEN(_BOOL)
			TOKEN(_COMPLEX)
			TOKEN(_IMAGINARY)
			TOKEN(IDSYM)
			TOKEN(ERROR_ACTION)
			TOKEN(ROW_MAJOR)
			TOKEN(COLUMN_MAJOR)
		}
	#undef TOKEN

		const int count = sizeof(strings) / sizeof(char*);
		if((int)tok >= count || !strings[tok])
			return "BAD TOKEN!";

		return strings[(int)tok];
	}

    //--------------------------------------------------------------------


} // end namespace
} // end namespace
} // end namespace
