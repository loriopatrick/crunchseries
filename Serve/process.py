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
			r = self.sock.recv(size - recieved)
			recieved += len(r)
			data += r
		return data

	def get_int(self):
		return struct.unpack('i', self.recv(4))[0]

	def get_double_array(self, size):
		return struct.unpack('%sd' % size, self.recv(8 * size))

	def run_graph(self, data):
		self.request(1, data)
		num_outputs = self.get_int()
		outputs = []
		for x in range(num_outputs):
			output_size = self.get_int()
			outputs.append(self.get_double_array(output_size))
		return outputs

	def close(self):
		self.sock.close()

if __name__ == '__main__':
	pro = Process(PROCESS_END_POINT)
	pro.connect()
	import statGraph
	print pro.run_graph(statGraph.example())