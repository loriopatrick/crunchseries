import time
import datetime
import requests

import database

class Periods:
	EOM='eom', 60
	EOD='eod', 321

class Series:
	def __init__(self, period):
		periods = ['eom', 'eod'] # end of minute, end of day respectively

		if period[0] not in periods:
			raise Exception('Unkown period: %s, options: %s' % (period, periods))

		self.period = period

	def save(self, data, cursor):
		required = ['symbol', 'epoch', 'high', 'low', 'open', 'close', 'volume']
		for item in required:
			if item not in data:
				raise Error('%s required in data along with: %s', (item, required))
		print data
		cursor.execute(''.join([
				'INSERT INTO series_', self.period[0], 
				' (symbol, epoch, high, low, open, close, volume',
				')VALUES(', 
				'%(symbol)s, %(epoch)s, %(high)s, %(low)s, %(open)s, %(close)s, %(volume)s)'
			]), data)

	def first_epoch(self, symbol, cursor, order='DESC'):
		cursor.execute('SELECT epoch FROM series_%s WHERE symbol="%s" ORDER BY epoch %s LIMIT 1' % (self.period[0], symbol, order))

		rows = cursor.fetchall()
		for row in rows:
			return int(row[0])

	def first_zero_epoch(self, symbol, cursor, order='DESC'):
		cursor.execute('SELECT epoch FROM series_%s WHERE symbol="%s" AND volume="0" ORDER BY epoch %s LIMIT 1' % (self.period[0], symbol, order))

		rows = cursor.fetchall()
		for row in rows:
			return int(row[0])


class GoogleFinace():
	def __init__(self, symbol, search_symbol=None):
		self.symbol = symbol

		if search_symbol:
			self.search_symbol = search_symbol
		else:
			self.search_symbol = symbol

	def request(self, fromDate, interval='60'):
		url = ''.join([
			'http://www.google.com/finance/getprices',
			'?q=', self.symbol,
			'&i=', interval,
			'&ts=', str(time.mktime(fromDate.timetuple()))
		])

		self.data_lines = requests.get(url).text.split('\n')

	def extract(self):
		if not hasattr(self, 'data_lines'):
			raise('Rquest has not yet been made')

		header = True
		last_date = 0
		for line in self.data_lines:
			if not len(line):
				continue

			if header:
				if line.startswith('EXCHANGE%3D'):
					self.exchange = line[len('EXCHANGE%3D'):]
					continue
				if line.startswith('INTERVAL='):
					self.interval = int(line[len('INTERVAL='):])
					print 'INTERVAL: %s' % self.interval
					continue
				if line.startswith('a'):
					header = False
				else:
					continue

			parts = line.split(',')
			date = last_date

			if parts[0].startswith('a'):
				last_date = int(parts[0][1:])
				date = last_date
			else:
				date = last_date + self.interval * int(parts[0])

			yield {
				'symbol':self.symbol,
				'epoch':int(date),
				'close':float(parts[1]),
				'high':float(parts[2]),
				'low':float(parts[3]),
				'open':float(parts[4]),
				'volume':int(parts[5])
			}

	def save(self, series, cursor):
		extractor = self.extract()
		first = True
		for item in extractor:

			if first:
				first = False
				if self.interval != series.period[1]:
					raise Exception('intervals don\'t match')

			try:
				series.save(item, cursor)
			except:
				pass


if __name__ == '__main__':
	series = Series(Periods.EOM)

	test = GoogleFinace('GOOG')
	test.request(datetime.datetime.now() - datetime.timedelta(days=12))

	cursor = database.cursor()
	test.save(series, cursor)
	database.commit()
