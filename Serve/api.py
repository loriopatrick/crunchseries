from flask import Flask, request

import json
import graph
import stats
from process import Process, PROCESS_END_POINT
from pymongo import MongoClient

app = Flask(__name__)

@app.route('/api/stats/groups', methods=['GET'])
def get_groups():
	return json.dumps(stats.groups)

@app.route('/api/stats', methods=['GET'])
def get_stats():
	return json.dumps(stats.stats)

@app.route('/api/graph/save/<uid>', methods=['PUT', 'POST'])
def save_graph(uid):
	mongo = MongoClient()
	graphs = mongo.crunchseries.graphs
	data = json.loads(request.data)
	# todo: verify data

	data['uid'] = uid
	graphs.save(data)
	return 'saved %s' % uid

@app.route('/api/graph/get/<uid>', methods=['GET'])
def get_graph(uid):
	mongo = MongoClient()
	graphs = mongo.crunchseries.graphs
	data = graphs.find_one({'uid':uid})
	data.pop('_id')

	return json.dumps(data)

@app.route('/api/graph/get_node/<uid>', methods=['GET'])
def get_graph_node(uid):
	mongo = MongoClient()
	graphs = mongo.crunchseries.graphs
	data = graphs.find_one({'uid':uid})
	data.pop('_id')

	res = {
		'title': uid,
		'statId': -132,
		'inputs': [],
		'outputs': [],
		'settings': []
	}

	def addSettingsToCon(obj, target):
		for setting in obj['settings']:
			parts = setting.split('-')
			target.append({
				'name': '-'.join(parts[2:])
			})

	if data['nodes'].get('inputs', None):
		addSettingsToCon(data['nodes']['inputs'], res['inputs'])

	addSettingsToCon(data['nodes']['outputs'], res['outputs'])

	for node_name in data['nodes']:
		node = data['nodes'][node_name]
		for setting in node['settings']:
			print setting
			parts = setting.split('-')
			
			public_name = parts[1]
			if not len(public_name):
				continue

			res['settings'].append({
				'name': public_name,
				'type': parts[0],
				'val': '-'.join(parts[2:])
			})

	return json.dumps(res)


@app.route('/api/graph/run', methods=['POST'])
def run_graph():
	if len(request.data) > 5000:
		return 'TO BIG'

	# get nodes from client
	data = json.loads(request.data)

	# todo: verify graph data...

	# todo: go through nodes and replace custom nodes with
	# base nodes
	
	# get output names
	output_names = []
	for setting in data['nodes'][data['head']]['settings']:
		output_names.append('-'.join(setting.split('-')[2:]))
	
	# clear the names as process doesn't use them
	data['nodes'][data['head']]['settings'] = []
	
	# serialize the graph
	parser = graph.GraphSerializer(data)
	bytes = parser.serialize()

	# get the results
	pro = Process(PROCESS_END_POINT)
	pro.connect()
	results = pro.run_graph(bytes)

	# return the results with the output names
	response = {
		'results': []
	}
	for x in range(len(results)):
		series = results[x]
		response['results'].append({
			'name': output_names[x],
			'series': series
		})

	print response

	return json.dumps(response)

if __name__ == '__main__':
	app.debug = True
	app.run(port=8080)