#/////////////////////////////////////////////////////////////////////////////
#// HLSL Library : libhlsl : make_msgs.py
#// created Friday, 31 August 2007 11:50:27 PM
#// (C) Martin Slater 2007
#/////////////////////////////////////////////////////////////////////////////

# parses message decription file and generates c++ code

import sys
import re

if len(sys.argv) != 3:
	print("Usage:\n\tmake_msgs.py <description file> <prefix>")
	sys.exit(1)
	
src_file = sys.argv[1]
prefix = sys.argv[2]

file = open(src_file, "r")
desc = file.read()
regex = re.compile(r"""\(([a-zA-Z_]+[a-zA-Z]*)\s*\|\s*([a-z]+)\s*\|\s*([^\|]*)\s*\|\s*(.*)\s*\)""")

results = regex.findall(desc)
#print results

enum_def = ""
msg_defs = ""

def escape(str):
	res = ""
	for c in str:
		if c == '"':
			res += '\\'
		res += c
	return res
	

for res in results:
	id = res[0]
	level = res[1]
	short = escape(res[2])
	long = escape(res[3])
	
	# check level is valid
	if level != 'warning' and level != 'error' and level != 'info' and level != 'internal':
		print("Invalid level : %s" % level)		
		sys.exit(1)
	
	# check for duplicates
	
	# process arguments, %1 , %2 etc
	
	#print "id=%s level=%s%s short=%s long=%s" % (id, prefix, level, short, long)
	
	# add to type enumeration
	enum_def += "%s%s,\n" % (prefix, id)
	
	# add message structure definition
	msg_defs += "{ %s%s, %s%s, \"%s\", \"%s\" },\n"  % (prefix, level, prefix, id, short, long)  
	

src = re.sub(r"(.*)\.(.*)", r"\1.defs", src_file)
hdr = re.sub(r"(.*)\.(.*)", r"\1.types", src_file)
	
f = open(hdr, "w+t")
f.write(enum_def)
f.close()


f = open(src, "w+t")
f.write(msg_defs)
f.close()