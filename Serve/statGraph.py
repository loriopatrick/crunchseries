"""
Turns JSON statGraph data into Process statGraph data
nodes -> steps
"""

class Node:
	def __init__(self, id, data):
		self.id = id
		self.data = data
	
	def serialize(self):
		pass

class StatGraph:
	def __init__(self, data):
		pass