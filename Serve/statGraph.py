"""
Turns JSON statGraph data into Process statGraph data
nodes -> steps
"""

statInfo = {
	'-1':{
		'name':'Output'
	},
	'0':{
		'name':'Copy',
		'inputs':{
			'data':0
		},
		'outputs':1
	},
	'1':{
		'name':'SQL',
		'inputs':0,
		'settings':[]
	}
}

class Node:
	def __init__(self, id, data):
		self.id = id
		self.data = data
		self.order = -1
	
	def serialize(self):
		pass

	def get_inputs(self, nodes):
		self.inputs = []

		if 'inputs' not in self.data:
			return
		
		for inp in self.data['inputs']:
			self.inputs.append({
				'node':nodes[inp['node']],
				'output':inp['output']
			})

def get_nodes(data):
	nodes = {}
	for node in data['nodes']:
		nodes[node] = Node(node, data['nodes'][node])

	for node in nodes:
		nodes[node].get_inputs(nodes)

	return nodes

def order_nodes(to_define, order=0):
	for node in to_define:
		if order > node.order:
			node.order = order

		inputs = node.inputs
		next_to_define = []
		for inp in inputs:
			next_to_define.append(inp['node'])
		order_nodes(next_to_define, order + 1)

def build_steps(nodes):
	high_order = 0
	steps = {}

	for node_id in nodes:
		node = nodes[node_id]
		if node.order == -1: continue
		if str(node.order) not in steps:
			steps[str(node.order)] = []
		steps[str(node.order)].append(node)
		if node.order > high_order:
			high_order = node.order

	res = []
	for x in range(0, high_order + 1):
		res.append(steps[str(x)])	

	return res

def get_copy_node(node, output):
	copy = Node(0, None)
	copy.inputs = [
		{
			'node':node,
			'output':output
		}
	]

	return copy

def fill_steps(steps):
	for step in range(0, len(steps)):
		step_nodes = steps[step]
		for node in step_nodes:
			for inp in node.inputs:
				if not inp['node'] in steps[step + 1]:
					copy_node = get_copy_node(inp['node'], inp['output'])
					steps[step + 1].append(copy_node)


def connect_steps(steps):
	pass

def serialize_steps(steps):
	pass

if __name__ == '__main__':
	import json
	f = open('example.json', 'r')
	data = json.loads(f.read())
	f.close()

	nodes = get_nodes(data)
	order_nodes([nodes[data['end']]])
	steps = build_steps(nodes)
	fill_steps(steps)
	connect_steps(steps)

	# print steps

	# for node in nodes:
	# 	print '%s - %s' % (node, nodes[node].order)