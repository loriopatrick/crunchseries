from flask import Flask, request

import time
import json
import stats
from graph import GraphSerializer
from process import Process, PROCESS_END_POINT
from pymongo import MongoClient

app = Flask(__name__)

TEMP_USER_ID = 'admin'

@app.errorhandler(500)
def handle_error(error):
	return json.dumps({
		'success': False,
		'error': {
			'message': error.__str__
		}
	}), 500

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
	graph = json.loads(request.data)

	# todo: verify graph

	doc = {
		'uid': uid,
		'creator': TEMP_USER_ID,
		'access': [
			{
				'user': TEMP_USER_ID,
				'access': 0 # 0 = read & write, 1 = read
			}
		],
		'revision': 0,
		'timestamp': time.time(),
		'graph': graph,
		'node_count': len(graph['nodes'])
	}

	latest_graph = graphs.find({
		'uid': uid, 
		'creator': TEMP_USER_ID
	}).sort('revision', -1).limit(1)

	# todo: check if there is any difference from latest revision

	if latest_graph.count():
		doc['revision'] = latest_graph[0]['revision'] + 1
	
	graphs.insert(doc)

	doc.pop('graph')
	doc.pop('_id')

	return json.dumps({
		'success': 'True',
		'result': doc
	})

@app.route('/api/graph/get/<user>/<uid>', methods=['GET'])
def get_graph(user, uid, obj=False, revision=None):
	mongo = MongoClient()
	graphs = mongo.crunchseries.graphs
	if not revision:
		revision = int(request.args.get('revision', '-1'))
	search = {
		'uid': uid, 
		'creator': user, 
		'access.user': TEMP_USER_ID
	}

	if revision > -1:
		search['revision'] = revision

	res = graphs.find(search).sort('revision', -1).limit(1)

	graph = None
	if res.count():
		graph = res[0]['graph']
		graph['revision'] = res[0]['revision']

	if obj:
		return graph

	if not graph:
		raise Exception('There does not exist a graph with uid: %s' % uid)

	return json.dumps(graph)

@app.route('/api/graph/get_node/<user>/<uid>', methods=['GET'])
def get_graph_node(user, uid, obj=False, revision=None):
	graph = get_graph(user, uid, True, revision)

	if graph is None:
		raise Exception('Could not load graph with uid: %s' % uid)

	res = {
		'title': uid + ' : v' + str(graph['revision']),
		'statId': -2,
		'uid': user + '/' + uid + ':' + str(graph['revision']),
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

	if graph['nodes'].get('inputs', None):
		addSettingsToCon(graph['nodes']['inputs'], res['inputs'])

	addSettingsToCon(graph['nodes']['outputs'], res['outputs'])

	for node_name in graph['nodes']:
		node = graph['nodes'][node_name]
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

	if obj:
		return res

	return json.dumps(res)

@app.route('/api/graph/get', methods=['GET'])
def search_graphs():
	uid_reg = request.args.get('uid_reg', None)
	user = request.args.get('user', None)
	order = request.args.get('order', 'time')
	desc = int(request.args.get('desc', '1'))
	skip = int(request.args.get('skip', 0))
	limit = request.args.get('limit', None)

	mongo = MongoClient()
	graphs = mongo.crunchseries.graphs

	search = {
		'access.user': TEMP_USER_ID
	}

	if user:
		search['creator'] = user

	if uid_reg:
		# todo: find way to query
		search['uid'] = uid_reg

	res = graphs.find(search)
	
	if order == 'time':
		if desc == '1': desc = 1
		else: desc = -1

		res = res.sort('timestamp', desc)

	if skip:
		res = res.skip(skip)

	if limit:
		limit = int(limit)
		res = res.limit(limit)

	data_res = {
		'results': []
	}

	for doc in res:
		doc.pop('_id')
		doc.pop('graph')
		data_res['results'].append(doc)

	return json.dumps(data_res)

@app.route('/api/graph/run', methods=['POST', 'GET'])
def run_graph():
	if len(request.data) > 5000:
		return 'TO BIG'

	if request.method == 'GET':
		uid = request.args.get('uid')
		graph = get_graph(uid, True)

		def replace_public_setting(name, value):
			for node_name in graph['nodes']:
				node = graph['nodes'][node_name]
				pos = -1
				for setting in node['settings']:
					pos += 1
					parts = setting.split('-')
					if parts[1] != name:
						continue

					parts[2] = value
					node['settings'][pos] = '-'.join(parts[:3])

		for arg in request.args:
			if arg[:8] == 'setting-':
				replace_public_setting(arg[8:], request.args[arg])
	else:
		# get nodes from client
		graph = json.loads(request.data)

	# todo: verify graph data...

	# todo: go through nodes and replace custom nodes with
	# base nodes

	def expand_node(node, node_name, output):
		if 'uid' not in node:
			return

		# lets load what we need
		parts = node['uid'].split('/')
		parts2 = parts[1].split(':')
		user = parts[0]
		uid = parts2[0]
		rev = int(parts2[1])

		insert_data = get_graph(user, uid, True, revision=rev)
		insert_ref  = get_graph_node(user, uid, True, revision=rev)

		def get_setting_pos(public_name):
			pos = 0
			for setting in insert_ref['settings']:
				if setting['name'] == public_name:
					return pos
				pos += 1

		def fix_inputs(node, inode_name):
			if inode_name == 'inputs':
				return

			for i in node['inputs']:
				i['node'] = node_name + '-' + i['node']

		def update_settings(settings):
			for x in range(0, len(settings)):
				print 'SETTINGS:::', settings[x]
				setting = settings[x]
				parts = setting.split('-')
				public_name = parts[1]
				if not len(public_name):
					return

				# if setting is public, grab setting value from node
				setting_pos = get_setting_pos(public_name)
				settings[x] = node['settings'][setting_pos]

		# itterate through all the nodes we have to add
		for inode_name in insert_data['nodes']:
			inode = insert_data['nodes'][inode_name]

			add_name = node_name + '-' + inode_name
			
			# check at the ends of custom node, we need to do some stitching
			if inode_name == 'inputs':
				# node type is an input, lets copy inputs from acutal node for
				# internal components
				inode['statId'] = 0
				inode['inputs'] = node['inputs']
				inode['settings'] = []

			elif inode_name == 'outputs':
				# so other's inputs match
				add_name = node_name
				inode['settings'] = []

			fix_inputs(inode, inode_name)

			# update settings to external settings
			update_settings(inode['settings'])

			# put out the node
			output[add_name] = inode

			# recurse
			expand_node(inode, add_name, output)

	to_add = {}
	for node_name in graph['nodes']:
		expand_node(graph['nodes'][node_name], node_name, to_add)

	for item_name in to_add:
		print item_name, to_add[item_name]
		graph['nodes'][item_name] = to_add[item_name]
	
	# get output names
	output_names = []
	for setting in graph['nodes'][graph['head']]['settings']:
		output_names.append('-'.join(setting.split('-')[2:]))
	
	# clear the names as process doesn't use them
	graph['nodes'][graph['head']]['settings'] = []
	
	# serialize the graph
	parser = GraphSerializer(graph)
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