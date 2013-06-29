from flask import Flask, request

import json
import statGraph

app = Flask(__name__)

@app.route('/api/runGraph', methods=['POST'])
def run_graph():
	if len(request.data) > 5000:
		return 'TO BIG'
	
	data = json.loads(request.data)
	parser = statGraph.StatGraphSerializer(data)
	bytes = parser.serialize()
	return parser.get_bytes(bytes)

if __name__ == '__main__':
	app.debug = True
	app.run(port=8080)