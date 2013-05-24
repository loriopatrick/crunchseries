"""
Turns JSON statGraph data into Process statGraph data
nodes -> steps -> binary
"""

statInfo = {
	'-1':{
		'name':'Output',
		'outputs':-1
	},
	'0':{
		'name':'Copy',
		'inputs':1,
		'outputs':1
	},
	'1':{
		'name':'SQL',
		'inputs':0,
		'settings':{
			'series':{
				'type':'s',
				'size':3
			},
			'symbol':{
				'type':'s',
				'size':0
			},
			'begin':{
				'type':'d',
				'size':8
			},
			'end':{
				'type':'d',
				'size':8
			}
		},
		'outputs':6
	},
	'2':{
		'name':'Simple Moving Average',
		'inputs':1,
		'settings':{
			'period_size':{
				'type':'i',
				'size':4
			}
		},
		'outputs':1
	},
	'3':{
		'name':'Exponential Moving Average',
		'inputs':1,
		'settings':{
			'period_size':{
				'type':'i',
				'size':4
			}
		},
		'outputs':1
	},
	'4':{
		'name':'Standard Deviation',
		'inputs':1,
		'settings':{
			'period_size':{
				'type':'i',
				'size':4
			}
		},
		'outputs':1
	}
}

import struct

class Node:
	def __init__(self, id):
		self.id = id
		self.order = -1

	def set_data(self, statId, inputs, settings, outputs):
		self.data = data
		self.statId = statId
		self.inputs_len = inputs
		self.settings_map = settings
		self.outputs_len = outputs

	def set_settings(self, settings):
		self.settings = []
		for setting_map in self.settings_map:
			got = False
			for setting in settings:
				if setting == setting_map:
					got = True
					setting_dict = {
						'value':settings[setting]
					}
					setting_map_dict = self.settings_map[setting_map]
					s_type, s_size = setting_map_dict['type'], setting_map_dict['size']
					setting_dict['type'] = s_type
					setting_dict['size'] = s_size
					self.settings.append(setting_dict)
					break

			if got is False:
				raise Exception('Settings for stat doesn\'t fit template')

	def set_inputs(self, inputs, nodes):
		self.inputs = []

		if inputs is None:
			return
		
		for inp in inputs:
			self.inputs.append({
				'node':nodes[inp['node']],
				'output':inp['output']
			})
	
	def serialize(self):
		print 'serialize', self.id, self.settings
		data = struct.pack('=ii', self.statId, len(self.settings))
		for setting in self.settings:
			size, value, value_type = setting['size'], setting['value'], setting['type']
			
			if value_type == 's':
				if size == 0:
					size = len(value)
				value = value[:size]

			data += struct.pack('i', size)

			if value_type == 's':
				value_type = 's%s' % size
				value = str(value)

			data += struct.pack('=%s' % value_type, value)

		data += struct.pack('=i', len(self.input_map))
		for inp in self.input_map:
			data += struct.pack('=i', inp)

		data += struct.pack('=i', self.outputs_len)

		return data

	def __str__(self):
		return str(self.id)

def get_nodes(data):
	nodes = {}
	for node_id in data['nodes']:
		node = Node(node_id)
		node_data = data['nodes'][node_id]
		node.inputs_data = node_data.get('inputs', None)
		stat_info = statInfo[str(node_data['statId'])]
		node.set_data(
			statId=node_data['statId'], 
			inputs=stat_info.get('inputs', 0), 
			settings=stat_info.get('settings', []), 
			outputs=stat_info['outputs'])
		node.set_settings(node_data.get('settings', None))
		nodes[node_id] = node

	for node in nodes:
		nodes[node].set_inputs(nodes[node].inputs_data, nodes)

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
	copy = Node('0')
	copy.inputs = [
		{
			'node':node,
			'output':output
		}
	]
	copy.set_data(statId=0, inputs=1, settings=[], outputs=1)
	copy.set_settings(None)

	return copy

def fill_steps(steps):
	for step in range(0, len(steps)):
		step_nodes = steps[step]
		for node in step_nodes:
			for inp in node.inputs:
				if not inp['node'] in steps[step + 1]:
					copy_node = get_copy_node(inp['node'], inp['output'])
					steps[step + 1].append(copy_node)

def build_input_maps(steps, nodes):
	steps_len = len(steps)
	step_outputs = []
	for step in range(0, steps_len):
		step = steps_len - step - 1
		new_step_outputs = []
		for node in steps[step]:

			node.input_map = []
			for inp in node.inputs:
				# map inputs to step_outputs
				got = False
				for i in range(0, len(step_outputs)):
					output = step_outputs[i]
					if output['node'].id == '0':
						output = output['node'].inputs[0]

					if output['node'].id == inp['node'].id and output['output'] == inp['output']:
						node.input_map.append(i)
						got = True
						break

				if not got:
					raise Exception('Could not find value stat: %s inputs' % node.id)
			
			for output in range(0, node.outputs_len):
				# build new step_outputs
				new_step_outputs.append({'node':node, 'output':output})
			

		step_outputs = new_step_outputs

def serialize_steps(steps):
	steps_len = len(steps)
	data = struct.pack('=i', steps_len)

	for step in range(0, steps_len):
		step = steps_len - step - 1
		data += struct.pack('=i', len(steps[step]))
		for node in steps[step]:
			data += node.serialize()

	return data

if __name__ == '__main__':
	import json
	f = open('example.json', 'r')
	data = json.loads(f.read())
	f.close()

	nodes = get_nodes(data)
	order_nodes([nodes[data['end']]])
	steps = build_steps(nodes)
	fill_steps(steps)
	build_input_maps(steps, nodes)
	data = serialize_steps(steps)
	# print data

	toHex = lambda x:'0x%s' % ',0x'.join([hex(ord(c))[2:].zfill(2) for c in x])

	print data.encode('hex')

	# for node in nodes:
	# 	print '%s - %s' % (node, nodes[node].order)