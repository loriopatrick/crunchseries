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
	pass

if __name__ == '__main__':
	import json
	f = open('example.json', 'r')
	data = json.loads(f.read())
	f.close()

	nodes = get_nodes(data)
	order_nodes([nodes[data['end']]])

	for node in nodes:
		print '%s - %s' % (node, nodes[node].order)