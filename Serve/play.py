import socket
import struct
import datetime
import time
import json
# struct calcStatRequest {
# 	char symbol[9];
# 	char series[4];
# 	long start_epoch;
# 	long end_epoch;
# 	int number_of_stats;
# };

class Stat(object):
	def __init__(self, name):
		self.name = name

	def serialize(self):
		return struct.pack('10s', self.name)

	def settings(self):
		return None

	def dict(self):
		if not hasattr(self, 'results'):
			return None

		return {
			'name':self.name,
			'settings':self.settings(),
			'results':self.results
		}

	def json(self):
		return json.dumps(self.dict())


class ACD(Stat):
	def __init__(self):
		super(ACD, self).__init__('ACD')

	def serialize(self):
		return struct.pack('10sd', self.name, 0)

class AROON_UP(Stat):
	def __init__(self):
		super(AROON_UP, self).__init__('AROON_UP')
		self.tail_size = 25

	def serialize(self):
		return struct.pack('=10si', self.name, self.tail_size)

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
		data = struct.pack('i9s4sIIi', 1, self.symbol[:8], self.series[:4], self.start, self.end, len(self.stats))
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
				data = struct.unpack('Id', self.recv(16))
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

for x in range(0, 1000):
	test()
	print 'REQUEST: %s completed' % x