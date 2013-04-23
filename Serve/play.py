import socket
import struct
import datetime
import time
import json

from stats import *

class CalcStatRequest:
	def __init__(self, series, symbol, start, end):
		self.series = series
		self.symbol = symbol
		self.start = start
		self.end = end
		self.stats = {}
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	def connect(self):
		self.sock.connect(('127.0.0.1', 4213))

	def request(self):
		print str(self.symbol[:8])
		data = struct.pack('i9s4sIIi', 1, str(self.symbol[:8]), str(self.series[:4]), self.start, self.end, len(self.stats))
		self.send(data)

		for stat in self.stats:
			self.send(self.stats[stat].serialize())

		n_stat, n_quotes = struct.unpack('ii', self.recv(8))

		print n_stat, n_quotes

		for stat_res in range(0, n_stat):
			name = self.recv(10)
			stat = self.stats[name.rstrip(' \t\r\n\0')]
			stat.results = []
			for q in range(0, n_quotes):
				data = struct.unpack('Id', self.recv(struct.calcsize('Id')))
				stat.results.append((data[0], round(data[1], 5)))

	def send(self, msg):
		msg_len = len(msg)
		msg_sent = 0
		
		while msg_sent < msg_len:
			sent = self.sock.send(msg[msg_sent:])
			if sent == 0:
				raise RuntimeError("socket broke")
			msg_sent += sent

	def recv(self, recv):
		chunks = []
		msg_recv = 0
		
		while (msg_recv < recv):
			chunk = self.sock.recv(recv - msg_recv)
			if len(chunk) == 0:
				raise RuntimeError("socket broke")
			chunks.append(chunk)
			msg_recv += len(chunk)

		return ''.join(chunks)

	def addStat(self, stat):
		self.stats[stat.name] = stat

	def close(self):
		self.sock.shutdown()
		self.sock.close()

def test():

	req = CalcStatRequest('eom', 'GOOG', 0, 2**32-2)
	a = ACD()
	b = AROON_UP()
	req.addStat(a)
	req.addStat(b)
	req.connect()
	req.request()

	print a.json()
	print b.json()



from flask import Flask, request, Response, render_template, url_for

app = Flask(__name__)
app.debug = True


@app.route('/')
def index():
	return render_template('play.html')

@app.route('/api/calc')
def calc():
	stats = {}
	series = request.args.get('series', 'eom')
	symbol = request.args['symbol']
	begin = int(request.args['begin'])
	end = int(request.args['end'])

	for req in request.args:
		if '_' not in req:
			continue
		val = request.args[req]

		parts = req.split('_')
		if parts[1] == 'stat':
			stats[parts[0]] = stat_key[val]()
		else:
			stats[parts[0]].applySetting(parts[1], val)

	calc = CalcStatRequest(series.lower(), symbol.upper(), begin, end)
	for stat in stats:
		calc.addStat(stats[stat])

	calc.connect()
	calc.request()

	stats_data = []
	for stat in stats:
		stats_data.append(stats[stat].dict())

	return Response(json.dumps(stats_data), mimetype='application/json')

	calc.close()


if __name__ == '__main__':
	app.run(port=8080)

# for x in range(0, 1000):
# 	test()
# 	print 'REQUEST: %s completed' % x