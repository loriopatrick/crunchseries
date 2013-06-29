import socket
import struct

PROCESS_END_POINT = ('127.0.0.1', 5032)

class Process:
	def __init__(self, end_point):
		self.end_point = end_point

	def connect(self):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect(self.end_point)

	def request(self, type, data):
		self.sock.send(struct.pack('ii', type, len(data)))
		self.sock.send(data)
	
	def recv(self, size):
		data = ''
		recieved = 0
		while recieved < size:
			r = self.sock.recv(size)
			recieved += len(r)
			data += r
		return data

	def get_int(self):
		return struct.unpack('i', pro.recv(4))[0]

	def get_double_array(self, size):
		print 'get of size: %s' % size, size * 8
		return struct.unpack('%sd' % size, pro.recv(8 * size))

if __name__ == '__main__':
	pro = Process(PROCESS_END_POINT)
	pro.connect()
	import statGraph
	pro.request(1, statGraph.example())
	num_outputs = pro.get_int()
	outputs = []
	for x in range(num_outputs):
		output_size = pro.get_int()
		outputs.append(pro.get_double_array(output_size))
	print outputs