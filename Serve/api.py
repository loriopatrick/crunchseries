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
	# todo verify graph
	pass


@app.route('/api/graph/get/<uid>', methods=['GET'])
def get_graph(uid):
	pass

@app.route('/api/graph/run', methods=['POST'])
def run_graph():
	if len(request.data) > 5000:
		return 'TO BIG'

	data = json.loads(request.data)
	output_names = []
	print data['nodes'], data['head']
	for setting in data['nodes'][data['head']]['settings']:
		output_names.append(setting[len('str-'):])
	data['nodes'][data['head']]['settings'] = []
	
	parser = graph.GraphSerializer(data)
	bytes = parser.serialize()

	pro = Process(PROCESS_END_POINT)
	pro.connect()
	results = pro.run_graph(bytes)

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