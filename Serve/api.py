from flask import Flask, request

import json
import graph
import stats
from process import Process, PROCESS_END_POINT

app = Flask(__name__)

@app.route('/api/stats/groups', methods=['GET'])
def get_groups():
	return json.dumps(stats.groups)

@app.route('/api/stats', methods=['GET'])
def get_stats():
	return json.dumps(stats.stats)

@app.route('/api/graph/save/<uid>', methods=['PUT', 'POST'])
def save_graph(uid):
	storage = graph.GraphStorage()
	storage.save(uid, request.data, True)
	return 'saved %s' % uid

@app.route('/api/graph/get/<uid>', methods=['GET'])
def get_graph(uid):
	pass

@app.route('/api/graph/run', methods=['POST'])
def run_graph():
	if len(request.data) > 5000:
		return 'TO BIG'

	# get nodes from client
	data = json.loads(request.data)

	# todo: verify graph data...
	
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