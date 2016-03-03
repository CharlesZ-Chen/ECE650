import re
import sys

#global variables
re_command = re.compile(r'^\s*([acgr])(.*)$')
re_ac = re.compile(r'^\s*\"(.+)\"\s*((\s*\(\s*-?(\s*\d+)+\s*\,\s*-?(\s*\d+)+\s*\)){1,})\s*$')
re_r = re.compile(r'^\s*\"(.+)\"\s*$')
re_g = re.compile(r'^\s*$')
re_coordinate = re.compile(r'\((-?\d+)\,(-?\d+)\)')

intersection_dictionary = {}

#classes
class Edge(object):
	__slot__ = ('start_point', 'end_point', 'intersection_set')
	start_point = None
	end_point = None
	intersection_set = None

	def __init__(self, start_point, end_point):
		if not self.type_check_4_point(start_point) or not self.type_check_4_point(end_point):
			return None
		self.start_point = start_point
		self.end_point = end_point
		self.intersection_set = set([])

	def sort_intersections(self):
		if len(self.intersection_set) <= 1:
			return tuple(self.intersection_set)
		DIRECTION = 1
		COMPARE_MARK = 0
		if self.start_point[0] == self.end_point[0]:
			COMPARE_MARK = 1
			if self.start_point[1] > self.end_point[1]:
				DIRECTION = -1
		elif self.start_point[0] > self.end_point[0]:
			DIRECTION = -1

		intersec_list = list(self.intersection_set)
		return sorted(intersec_list, key = lambda coordinate: coordinate[COMPARE_MARK]*DIRECTION)

	def type_check_4_point(self,point):
		for item in point:
			if not isinstance(item,(int, float)):
				return False
		return True

	def insert_intersection(self, intersec_point):
		if not self.type_check_4_point(intersec_point):
			return False
		if  intersec_point in self.intersection_set:
			return True
		self.intersection_set.add(intersec_point)

	def remove_intersection(self, intersec_point):
		if intersec_point in self.intersection_set:
			self.intersection_set.pop(intersec_point)

	#check intersections that already not existed any more
	#return self.intersection_set
	def clean_zombie_intersection(self):
		clean_target_set = set([])
		for intersection in self.intersection_set:
			if not (intersection in intersection_dictionary) or intersection_dictionary[intersection] <= 1:
				clean_target_set.add(intersection)
		self.intersection_set = self.intersection_set - clean_target_set
		return self.intersection_set
	
class Street(object):
	__slot__ = ('street_name', 'edge_list', 'intersection_set')
	street_name = None
	edge_list = None

	def __init__(self, street_name, coordinates_tuple):
		self.street_name = ""
		self.edge_list = []
		self.intersection_set = set([])

		if not isinstance(street_name, (str)) or len(street_name) <= 0:
			return None
		self.street_name = street_name
		for i in range(len(coordinates_tuple) - 1):
			if coordinates_tuple[i] == coordinates_tuple[i + 1]:
				continue
			edge = Edge(coordinates_tuple[i], coordinates_tuple[i + 1])
			self.edge_list.append(edge)

class Graph(object):
	__slot__ = ('streets_dict', 'graphEdge_dict', 'graphPoint_dict', 'V')
	V = 0
	streets_dict = {}
	graphEdge_dict = {}
	graphPoint_dict = {}

	def add_street(self, street_name, coordinates_tuple):
		if street_name in self.streets_dict:
			sys.stderr.write ("Error: \'a\' specified for a street that already exists.\n")
			return False
		new_street = Street(street_name, coordinates_tuple)

		for checked_edge in new_street.edge_list:
			check_edge_set = set(new_street.edge_list)
			check_edge_set.remove(checked_edge)
			if len(check_edge_set) > 0:
				for edge in check_edge_set:
					if self.is_overlap(checked_edge, edge):
						return False

		if len(self.streets_dict) == 0:
			self.streets_dict[street_name] = new_street
			return self.streets_dict[street_name]

		for checked_edge in new_street.edge_list:
			for street in self.streets_dict.itervalues():
				for edge in street.edge_list:
					if self.is_overlap(checked_edge, edge):
						return False

		tmp_intersection_set = set([])
		
		for street in self.streets_dict.itervalues():

			tmp_intersection_set = tmp_intersection_set | self.check_intersection(new_street, street)

		for intersection in tmp_intersection_set:
			if not intersection in intersection_dictionary:
				intersection_dictionary[intersection] = 2
			else:
				intersection_dictionary[intersection] += 1
		# print intersection_dictionary
		self.streets_dict[street_name] = new_street
		return self.streets_dict[street_name] 

	def check_intersection(self, new_street, street):
	
		new_st_edge_list = new_street.edge_list
		st_edge_list = street.edge_list	

		intersection_set = set([])
		for new_st_edge in new_st_edge_list:
			for st_edge in st_edge_list:
				intersec_result = self.find_intersection(new_st_edge, st_edge)
				if intersec_result == False:
					continue
				else:
					intersection_set.add(intersec_result)
					new_st_edge.insert_intersection(intersec_result)
					st_edge.insert_intersection(intersec_result)
		return intersection_set
		
	def find_normal_line(self, start_coordinate, end_coordinate):
		nx = end_coordinate[1] - start_coordinate[1]
		ny = start_coordinate[0] - end_coordinate[0]
		return (nx,ny)

	#return absulote distance of a given point to a given normal line
	def dist_of_point_2_normal(self, point, normal_line):
		return normal_line[0] * point[0] + normal_line[1] * point[1]

	#check whether there is an intersection
	#@dist_start & @dist_end are distances of end points of line_b to line_a's normal line
	#end point located at other line is deemed as an intersection
	#return True if intersecte else return False
	def is_intersecte(self, dist_start, dist_end):
		if dist_start == dist_end or (dist_start * dist_end) > 0:
			return False
		return True

	def is_overlap(self, edge1, edge2):
		e1_start_point, e1_end_point = edge1.start_point, edge1.end_point
		e2_start_point, e2_end_point = edge2.start_point, edge2.end_point

		e1_nl_coord = self.find_normal_line(e1_start_point, e1_end_point)
		e2_nl_coord = self.find_normal_line(e2_start_point, e2_end_point)

		denominator = e1_nl_coord[0] * e2_nl_coord[1] - e1_nl_coord[1] * e2_nl_coord[0]

		if not denominator == 0:
			return False
		dist_e1_e1n = self.dist_of_point_2_normal(e1_start_point, e1_nl_coord)
		re_dist_e2_e1n = self.dist_of_point_2_normal(e2_start_point, e1_nl_coord) - dist_e1_e1n

		if not re_dist_e2_e1n == 0:
			return False

		big_edge_sorted = sorted([edge1.start_point, edge1.end_point])
		small_edge_sorted = sorted([edge2.start_point, edge2.end_point])

		if big_edge_sorted[1] < small_edge_sorted[1]:
			tmp = big_edge_sorted
			big_edge_sorted = small_edge_sorted
			small_edge_sorted = tmp

		if small_edge_sorted[1] > big_edge_sorted[0]:
			sys.stderr.write("Error: Overlapped edge detected.\n")
			return True

	def find_intersection(self, new_st_edge, st_edge):
		new_st_start_point, new_st_end_point = new_st_edge.start_point, new_st_edge.end_point
		st_start_point, st_end_point = st_edge.start_point, st_edge.end_point
		new_st_normal_coordinate = self.find_normal_line(new_st_start_point, new_st_end_point)
		st_normal_coordinate = self.find_normal_line(st_start_point, st_end_point)

		denominator = new_st_normal_coordinate[0] * st_normal_coordinate[1]\
								 - new_st_normal_coordinate[1] * st_normal_coordinate[0]

		dist_st_2_st_normal = self.dist_of_point_2_normal(st_start_point, st_normal_coordinate)

		re_dist_newSt_start_2_st_normal = self.dist_of_point_2_normal(new_st_start_point, st_normal_coordinate)\
												- dist_st_2_st_normal
		re_dist_newSt_end_2_st_normal = self.dist_of_point_2_normal(new_st_end_point, st_normal_coordinate)\
												- dist_st_2_st_normal

		if denominator == 0:
			if not re_dist_newSt_start_2_st_normal == 0:
				return False
			nst_eg_sorted = sorted([new_st_edge.start_point, new_st_edge.end_point])
			st_eg_sorted = sorted([st_edge.start_point, st_edge.end_point])
			if nst_eg_sorted[1] < st_eg_sorted[1]:
				if nst_eg_sorted[1] == st_eg_sorted[0]:
					return nst_eg_sorted[1]
				else:
					return False
			else:
				if st_eg_sorted[1] == nst_eg_sorted[0]:
					return st_eg_sorted[1]
				else:
					return False

		if not self.is_intersecte(re_dist_newSt_start_2_st_normal, re_dist_newSt_end_2_st_normal):
			return False

		dist_newSt_2_newSt_normal = self.dist_of_point_2_normal(new_st_start_point, new_st_normal_coordinate)

		re_dist_st_start_2_newSt_normal = self.dist_of_point_2_normal(st_start_point, new_st_normal_coordinate)\
												- dist_newSt_2_newSt_normal
		re_dist_st_end_2_newSt_normal = self.dist_of_point_2_normal(st_end_point, new_st_normal_coordinate)\
												- dist_newSt_2_newSt_normal

		if not self.is_intersecte(re_dist_st_start_2_newSt_normal, re_dist_st_end_2_newSt_normal):
			return False

		fraction = float(re_dist_newSt_start_2_st_normal) / float(denominator)
   		dx = fraction * new_st_normal_coordinate[1]
   		dy = - fraction * new_st_normal_coordinate[0]
   		intersect_x = new_st_start_point[0] + dx
   		intersect_y = new_st_start_point[1] + dy
   		return (intersect_x, intersect_y)

   	def print_graph(self):
   		self.graphPoint_dict = {}
   		self.graphEdge_dict = {}
   		self.V = 0
   		self.generate_graph()

   		# graphPointList = sorted(list(self.graphPoint_dict.iteritems()), key = lambda item:item[1])
   		sys.stdout.write("V %d\n" % len(self.graphPoint_dict))
   		# for item in graphPointList:
   		# 	sys.stdout.write("%d: (%g, %g)\n" % (item[1], round(item[0][0], 2), round(item[0][1], 2)))
   		# sys.stdout.write("}\n")
   		i = 0;
   		sys.stdout.write("E {")
   		for edge in self.graphEdge_dict.itervalues():
   			sys.stdout.write("<%s,%s>" %(str(edge[0]), str(edge[1])))
   			if i < len(self.graphEdge_dict) - 1:
   				sys.stdout.write(",")
   			i += 1
  		sys.stdout.write("}\n")
  		sys.stdout.flush()

  	def generate_graph(self):
   		for street in self.streets_dict.itervalues():
   			for edge in street.edge_list:
   				if len(edge.intersection_set) > 0:
   					if len(edge.clean_zombie_intersection()) == 0:
   						continue
   					sorted_intersec_points = edge.sort_intersections()
   					self.add_to_graph(edge.start_point, sorted_intersec_points[0])
   					for i in range(len(sorted_intersec_points) - 1):
   						self.add_to_graph(sorted_intersec_points[i], sorted_intersec_points[i+1])
   					self.add_to_graph(sorted_intersec_points[-1], edge.end_point)

  	def add_to_graph(self, point_a, point_b):
   		if point_a[0] == point_b[0] and point_a[1] == point_b[1]:
   			return False
   		COMPARE_MARK = 0
   		if point_a[0] == point_b[0]:
   			COMPARE_MARK = 1

   		start_point, end_point = sorted([point_a, point_b], key = lambda cmp_value : cmp_value[COMPARE_MARK])
   		if not start_point in self.graphPoint_dict:
   			# self.V += 1
   			self.graphPoint_dict[start_point] = self.V
   			self.V += 1
   		if not end_point in self.graphPoint_dict:
   			# self.V += 1
   			self.graphPoint_dict[end_point] = self.V
   			self.V += 1
   		self.graphEdge_dict[(start_point, end_point)] = (self.graphPoint_dict[start_point], self.graphPoint_dict[end_point])
 
   	def remove_street(self, street_name):
   		if not street_name in self.streets_dict:
   			sys.stderr.write("Error: 'r' specified for a street that does not exist.\n")
   			return False

   		street = self.streets_dict[street_name]

   		intersection_set_4_this_street = set([])

   		for edge in street.edge_list:
   			edge.clean_zombie_intersection()
   			if len(edge.intersection_set) > 0:
   				for intersection in edge.intersection_set:
   					intersection_set_4_this_street.add(intersection)
   		for intersection in intersection_set_4_this_street:
   			intersection_dictionary[intersection] -= 1
   			if intersection_dictionary[intersection] <= 1:
   				intersection_dictionary.pop(intersection)
   		self.streets_dict.pop(street_name)

   		return True

   	def change_street(self, street_name, coordinates_tuple):
   		if not street_name in self.streets_dict:
   			sys.stderr.write("Error: 'c' specified for a street that does not exist.\n")
   			return False

   		self.remove_street(street_name)
   		self.add_street(street_name, coordinates_tuple)

#functions
def str2int4coordinates(coordinate_tuple):
	return (int(coordinate_tuple[0]),int(coordinate_tuple[1]))

def filterSpace(x):
	if x >= '0' and x <= '9' or x == '-' or x == '(' or x == ')' or x == ',':
		return True
	return False

#input: input Str
#return: False if input has any incorrect or unexpected format
#else return a dict :{command, street_name, coordiantes_tuple}
def parse_input(inputStr):
	parsed_params_dict = {}
	parse_result = re.match(re_command, inputStr)
	if parse_result == None:
		sys.stderr.write("Error: Incorrect input.\n")
		return False
	
	parsed_params_dict['command'] = parse_result.group(1)
	remainStr = parse_result.group(2)
	if parsed_params_dict['command'] == 'a' or parsed_params_dict['command'] == 'c':

		parse_result = re.match(re_ac , remainStr)

		if parse_result == None:
			sys.stderr.write("Error: Incorrect input.\n")
			return False

		parsed_params_dict['street_name'] = parse_result.group(1)
		coordinates_str_without_space = filter(filterSpace, parse_result.group(2))
		coordinates_str_list = re.findall(re_coordinate, coordinates_str_without_space)
		if len(coordinates_str_list) < 2:
			sys.stderr.write("Error: Incomplete coordinates in %s \"%s\", no end point.\n" % (parsed_params_dict['command'], parsed_params_dict['street_name']))
			return False

		parsed_params_dict['coordinates_tuple'] = tuple(map(str2int4coordinates, coordinates_str_list))

	elif parsed_params_dict['command'] == 'r':

		parse_result = re.match(re_r , remainStr)
		if parse_result == None:
			sys.stderr.write("Error: Incorrect Input.\n")
			return False
		parsed_params_dict['street_name'] = parse_result.group(1)

	elif parsed_params_dict['command'] == 'g':
		parse_result = re.match(re_g, remainStr)
		if parse_result == None:
			sys.stderr.write("Error: Incorrect Input.\n")
			return False
	else:
		sys.stderr.write("Error: Incorrect input.\n")
		return False
	return parsed_params_dict

def main():
	#Initialization
	#write intialize code here
	command = None
	streetName = None
	coordinates = None
	graph = Graph()

	#input module
	rawString = sys.stdin.readline()
	while rawString:

		#parsing input
		parseResult = parse_input(rawString)
		if parseResult == False:
			rawString = sys.stdin.readline()
			continue

		command = parseResult['command']

		#flow control of command
		if command == 'g':
			graph.print_graph()

		elif command == 'r':
			graph.remove_street(parseResult['street_name'])

		elif command == 'a':
			graph.add_street(parseResult['street_name'], parseResult['coordinates_tuple'])

		elif command == 'c':
			graph.change_street(parseResult['street_name'], parseResult['coordinates_tuple'])

		rawString = sys.stdin.readline()
			
if __name__ == '__main__':
	main()