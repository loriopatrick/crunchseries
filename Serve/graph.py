from struct import pack
import json
import os
import process

class GraphSerializer:
	def __init__(self, data):
		self.data = data

	def validate_nodes(self):
		'''
		todo: - check for circle dependencies
				- invalid settings
		'''
		return True

	def get_nodes(self):
		res = []
		for node_name in self.data['nodes']:
			res.append((node_name, self.data['nodes'][node_name]))

		return res

	def prep_node(self, node):
		if not 'inputs' in node[1]:
			node[1]['inputs'] = []

		if not 'settings' in node[1]:
			node[1]['settings'] = []

		return node

	def serialize_setting(self, setting):
		parts = setting.split('-')

		# setting format: type-editable-value
		setting_type = parts[0]
		setting_value = '-'.join(parts[2:])
		print setting_value

		if setting_type == 'int':
			setting = int(setting_value)
			value = pack('i', setting)
			return pack('i', len(value)) + value

		if setting_type == 'float':
			setting = float(setting_value)
			value = pack('d', setting)
			return pack('i', len(value)) + value

		if setting_type == 'str':
			setting = str(setting_value)
			value = pack('%is' % len(setting), setting)
			return pack('i', len(value)) + value

		raise Exception('Unknown type: %s' % setting)

	def get_node_pos(self, name, nodes):
		for x in range(len(nodes)):
			if name == nodes[x][0]:
				return x
		return -1

	def serialize(self):
		'''
		Format:
			int # of nodes
			{foreach node}
				int statId
				int # of settings
				{foreach setting}
					int # setting size
					char[] setting value
				int # of inputs
				{foreach input}
					int stat number in array
					int output
			int head
		'''
		nodes = self.get_nodes()
		data = pack('i', len(nodes))

		for node in nodes:
			self.prep_node(node)

			data += pack('i', node[1]['statId'])
			data += pack('i', len(node[1]['settings']))

			for setting in node[1]['settings']:
				data += self.serialize_setting(setting)

			data += pack('i', len(node[1]['inputs']))
			for dependency in node[1]['inputs']:
				dep_pos = self.get_node_pos(dependency['node'], nodes)
				data += pack('i', dep_pos)
				data += pack('i', dependency['output'])

		data += pack('i', self.get_node_pos(self.data['head'], nodes))

		return data

	def get_bytes(self, data):
		res = []
		for x in data:
			res.append(str(ord(x)))
		return ','.join(res)

def example():
	from json import loads
	request_data = loads(open('example2.json').read())
	parser = GraphSerializer(request_data)
	return parser.serialize()

if __name__ == '__main__':
	print parser.get_bytes(example())
	