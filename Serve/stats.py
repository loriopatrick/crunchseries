import struct

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

		if hasattr(self, 'beforedict'):
			self.beforedict()

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

	def settings(self):
		return {
			'tail_size':25
		}

	def applySetting(self, name, value):
		settings = ['tail']
		if name not in settings:
			return
		if name == 'tail':
			self.tail_size == int(value)

	def serialize(self):
		return struct.pack('=10si', self.name, self.tail_size)

class SMA(Stat):
	def __init__(self):
		super(SMA, self).__init__('SMA')
		self.tail_size = 25

	def settings(self):
		return {
			'tail_size':25
		}

	def beforedict(self):
		self.results = self.results[self.tail_size:]

	def applySetting(self, name, value):
		settings = ['tail']
		if name not in settings:
			return
		if name == 'tail':
			self.tail_size == int(value)

	def serialize(self):
		return struct.pack('=10si', self.name, self.tail_size)

class STDV(Stat):
	def __init__(self):
		super(STDV, self).__init__('STDV')
		self.tail_size = 25

	def settings(self):
		return {
			'tail_size':25
		}

	def beforedict(self):
		self.results = self.results[self.tail_size:]

	def applySetting(self, name, value):
		settings = ['tail']
		if name not in settings:
			return
		if name == 'tail':
			self.tail_size == int(value)

	def serialize(self):
		return struct.pack('=10si', self.name, self.tail_size)


stat_key = {
	'ACD':ACD,
	'AROON_UP':AROON_UP,
	'SMA':SMA,
	'STDV':STDV
}