import sys
import re

print sys.argv

sys.argv = [sys.argv[0], "e:\\code\\libraries\\libhlsl\\libhlsl\\hlsl_intrinsics.i", "e:\\code\\libraries\\libhlsl\\libhlsl\\hlsl_intrinsics.out"]

if len(sys.argv) != 3:
	print "Usage:\n\tmake_intrinsics.py <input file> <output file>"
	sys.exit(1)

input_file = sys.argv[1]
output_file = sys.argv[2]

# regular expressions for matching intrinsic definitions
re_name = "([a-zA-Z][a-zA-Z0-9]*)"
re_1_param = re.compile("\s*%s\(\s*%s\s*\)\s*" % (re_name, re_name))
re_2_param = re.compile("\s*%s\(\s*%s\s*,\s*%s\s*\)\s*" % (re_name, re_name, re_name))
re_3_param = re.compile("\s*%s\(\s*%s\s*,\s*%s\s*,\s*%s\s*\)\s*" % (re_name, re_name, re_name, re_name))
re_4_param = re.compile("\s*%s\(\s*%s\s*,\s*%s\s*,\s*%s\s*,\s*%s\s*\)\s*" % (re_name, re_name, re_name, re_name, re_name))
re_comment = re.compile("//(.*)")

re_func = re.compile("\s*%s\(([^)]*)\)" % (re_name))


class paramater:
	name = ""
	out_var = 0
	
	def __init__(self, n, o):
		self.name = n
		self.out_var = o

	def __str__(self):
		if self.out_var:
			return "out " + self.name
		return self.name
		
class intrinsic:	
	name = ""
	params = []
	result = ""

	def __str__(self):
		return "%s(%s)" % (self.name, self.params)
	
	def	__init__(self, n, p):
		self.name = n
		self.params = p
	
re_complex_param = re.compile("\s*out\s+%s" % (re_name))
re_simple_param = re.compile("\s*%s" % (re_name))

def make_param(param):
	m = re.match(re_complex_param, param)
	if m:
		return paramater(m.group(1), 1)
	m = re.match(re_simple_param, param)
	if m:
		return paramater(m.group(1), 0)
	return null
		

# load intrinsics definition file
f = open(input_file)

intrinsics = []
line_count = 0
trace = 0

def parse_file():
	for line in f:
		global line_count
		line_count += 1
		if re.match(re_comment, line):
			continue
		line  = line.lstrip().rstrip()
		if len(line) == 0:
			continue
		# match a function definition
		fname = ""
		fparams = []
		m = re.match(re_func, line)
		if m:
			fname = m.group(1)
			if m.group(2):
				params = m.group(2).split(',')
			# split paramaters
			for p in params:
				param = make_param(p)
				if param:
					fparams.append(param)
			intrinsics.append(intrinsic(fname, fparams))			
		else:
			print "%s(%s) : Error : Invalid syntax\n" % (input_file, line_count)

	if trace:				
		for i in intrinsics:
			print ("%s %s(" % (i.result, i.name)),
			first = 1
			for p in i.params:
				if not first:
					print ",",
				first = 0
				print p,
			print ")"

parse_file()

# write out function prototype definitions for each type applicable to each intrinsic
builtin_types = ["float", "half", "int", "double", "uint"]
vector_types  = []
matrix_types  = []

for b in builtin_types:
	for i in [1,2,3,4]:
		vector_types.append("%s%s" % (b, i))

for b in builtin_types:
	for i in [1,2,3,4]:
		for j in [1,2,3,4]:
			matrix_types.append("%s%sx%s" % (b, i, j))

all_types = builtin_types + vector_types + matrix_types
num_fps = 0
for i in intrinsics:
	for t in all_types:
		fp = "%s %s(" % (t, i.name)
		first = 1
		for p in i.params:
			if not first:
				fp += ", "
			first = 0
			if p.out_var:
				fp += "out "
			fp += t
			fp += " "
			fp += p.name
		fp += ");"
		print fp
		num_fps += 1

print "----------------------------------------------------------------------"
print "Generated %s prototypes" % num_fps
print "----------------------------------------------------------------------"
sys.exit(1)
