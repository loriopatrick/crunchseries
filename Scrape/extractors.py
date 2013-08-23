import time
import datetime
import requests

import database

class Periods:
	EOM='eom', 60
	EOD='eod', 86400

class Series:
	def __init__(self, symbol, period, search_symbol=None):
		periods = ['eom', 'eod'] # end of minute, end of day respectively

		if period[0] not in periods:
			raise Exception('Unkown period: %s, options: %s' % (period, periods))

		self.period = period
		self._symbol = symbol
		self._search_symbol = search_symbol

	def save(self, cursor, extraction):
		self.clear_symbol(cursor)
		for item in extraction:
			try:
				self.insert(item, cursor)
				print 'inserted: %s' % item
			except Exception as e:
				if e[0] == 1146:
					print 'Table doesn\'t exist'
					break
				if e[0] == 1062:
					continue
				print 'Error: %s' % e
				break
		cursor.connection.commit()

	def insert(self, data, cursor):
		required = ['epoch', 'high', 'low', 'open', 'close', 'volume']
		for item in required:
			if item not in data:
				raise Error('%s required in data along with: %s', (item, required))

		data['symbol'] = self.symbol
		cursor.execute(''.join([
			'INSERT INTO series_', self.period[0], 
			' (symbol, epoch, high, low, open, close, volume',
			')VALUES(', 
			'%(symbol)s, %(epoch)s, %(high)s, %(low)s, %(open)s, %(close)s, %(volume)s)'
		]), data)

	def clear_symbol(self, cursor):
		cursor.execute('DELETE FROM series_%s WHERE symbol=\'%s\'' % (self.period[0], symbol))
		cursor.connection.commit()

	@property
	def symbol(self):
		if self._search_symbol is None:
			return self._symbol
		return self._search_symbol


class GoogleFinace():
	def __init__(self, series, span='100Y'):
		self.series = series
		self.span = span

		url = ''.join([
			'http://www.google.com/finance/getprices',
			'?p=', self.span,
			'&q=', self.series.symbol,
			'&i=', str(self.series.period[1])
		])

		self.data_lines = requests.get(url).text.split('\n')

	def extract(self):
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
					continue
				
				if line.startswith('a'):
					header = False
				else:
					continue

			parts = line.split(',')
			date = last_date

			if parts[0].startswith('T'):
				continue

			if parts[0].startswith('a'):
				last_date = int(parts[0][1:])
				date = last_date
			else:
				date = last_date + self.interval * int(parts[0])

			yield {
				'symbol':self.series.symbol,
				'epoch':int(date),
				'close':float(parts[1]),
				'high':float(parts[2]),
				'low':float(parts[3]),
				'open':float(parts[4]),
				'volume':int(parts[5])
			}


if __name__ == '__main__':
	import sys

	symbols = ['GOOG']
	if len(sys.argv) > 1:
		symbols = sys.argv[1].split(',')

	for symbol in symbols:
		series = Series(symbol, Periods.EOD)
		extractor = GoogleFinace(series)
		series.save(database.cursor(), extractor.extract())

	database.close()