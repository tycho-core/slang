cmake_minimum_required (VERSION 2.8)

# generate compiler message definitions
set(compiler_dir ${CMAKE_CURRENT_SOURCE_DIR}/compiler)
add_custom_command (
	OUTPUT ${compiler_dir}/messages.types ${compiler_dir}/messages.defs
	DEPENDS ${compiler_dir}/make_msgs.py
	MAIN_DEPENDENCY ${compiler_dir}/messages.m
	COMMAND ${PYTHON_EXECUTABLE} ${compiler_dir}/make_msgs.py ${compiler_dir}/messages.m "mt_"
	COMMENT "Generating compiler messages file from ${compiler_dir}/messages.m"
)
	
# default definitions for dx10 hlsl
tycho_stringify_command("${CMAKE_CURRENT_SOURCE_DIR}/parsing/default_dx10.hlsl" "${CMAKE_CURRENT_SOURCE_DIR}/parsing/default_dx10.inl")

# lex.cpp includes flex generated code which emits warnings. Disable these
# only for this file.
ty_clang_file_disable_warnings("${CMAKE_CURRENT_SOURCE_DIR}/parsing/lex.cpp" "unused-label;unused-variable;unused-value")

tycho_add_library(slang "tycore;tyio" "libs")



