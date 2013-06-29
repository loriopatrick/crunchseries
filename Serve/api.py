from flask import Flask, request

import json
import statGraph
from process import Process, PROCESS_END_POINT

app = Flask(__name__)

@app.route('/api/runGraph', methods=['POST'])
def run_graph():
	if len(request.data) > 5000:
		return 'TO BIG'

	data = json.loads(request.data)
	output_names = []
	print data['nodes'], data['head']
	for setting in data['nodes'][data['head']]['settings']:
		output_names.append(setting[len('str-'):])
	data['nodes'][data['head']]['settings'] = []
	
	parser = statGraph.StatGraphSerializer(data)
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

	return json.dumps(response)

if __name__ == '__main__':
	app.debug = True
	app.run(port=8080)