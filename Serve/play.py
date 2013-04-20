import socket
import struct
import datetime
import time
# struct calcStatRequest {
# 	char symbol[9];
# 	char series[4];
# 	long start_epoch;
# 	long end_epoch;
# 	int number_of_stats;
# };

class ACD:
	def serialize(self):
		return struct.pack('10sd', 'ACD', 0)

class CalcStatRequest:
	def __init__(self, series, symbol, start, end):
		self.series = series
		self.symbol = symbol
		self.start = start
		self.end = end
		self.stats = []
		
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	def connect(self):
		self.sock.connect(('127.0.0.1', 4213))

	def request(self):
		data = struct.pack('i9s4sIIi', 1, self.symbol[:8], self.series[:4], self.start, self.end, len(self.stats))
		print 'data len:', len(data)
		self.send(data)

		for stat in self.stats:
			self.send(stat.serialize())

		n_stat, n_quotes = struct.unpack('ii', self.recv(8))

		print n_stat, n_quotes

		for stat_res in range(0, n_stat):
			for q in range(0, n_quotes):
				struct.unpack('Id', self.recv(16))

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
		self.stats.append(stat)

def test():

	req = CalcStatRequest('eom', 'GOOG', 0, 2**32-2)
	req.addStat(ACD())
	req.connect()
	req.request()

	# data = struct.pack('i9s4sIIi', 1, 'GOOG', 'eom', 0, 2**32-2, 1)
	# sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# sock.connect(('127.0.0.1', 4213))

	# sent = 0
	# while sent < len(data):
	# 	r = sock.send(data[sent:])
	# 	if not r:
	# 		print "ERROR"
	# 	sent += r

	# data = struct.pack('10sd', 'ACD', 0)

	# sent = 0
	# while sent < len(data):
	# 	r = sock.send(data[sent:])
	# 	if not r:
	# 		print "ERROR"
	# 	sent += r


	# sock.close()


for x in range(0, 1000):
	test()
	print 'REQUEST: %s completed' % x

# class Calc:

# 	def addStat(self):
# 		pass

# 	def calc():
# 		pass

# 	def serialize():
# 		pass

# 	def write(self, msg):
# 		msg_len = len(msg)
# 		msg_sent = 0
		
# 		while msg_sent < msg_len:
# 			sent = self.socket.send(msg[msg_sent:])
# 			if send == 0:
# 				raise RuntimeError("socket broke")
# 			msg_sent += sent

# 	def recv(self, recv):
# 		chunks = []
# 		msg_recv = 0
		
# 		while (msg_recv < recv):
# 			chunk = self.socket.recv(recv - msg_recv)
# 			if len(chunk) == 0:
# 				raise RuntimeError("socket broke")
# 			chunks.append(chunk)

# 		return ''.join(chunks)

# 	@property
# 	def socket(self):
# 		if not hasattr(self, '_socket') or self._socket is None:
# 			self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# 			self._socket.connect(('127.0.0.1', 6530))
# 		return self._socket

# 	def close():
# 		if not hasattr(self, '_socket') or self._socket is None:
# 			return

# 		self._socket.close()
# 		self._socket = None

# 	def __exit__(self):
# 		self.close()
