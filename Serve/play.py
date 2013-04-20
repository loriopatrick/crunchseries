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

def test():

	data = struct.pack('i9s4sIIi', 1, 'GOOG', 'eom', 0, 2**32-2, 1)
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect(('127.0.0.1', 4213))

	sent = 0
	while sent < len(data):
		r = sock.send(data[sent:])
		if not r:
			print "ERROR"
		sent += r

	data = struct.pack('10sd', 'ACD', 0)

	sent = 0
	while sent < len(data):
		r = sock.send(data[sent:])
		if not r:
			print "ERROR"
		sent += r


	sock.close()


for x in range(0, 5000):
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
