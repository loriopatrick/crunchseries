import MySQLdb, atexit

_con = None

def con():
	global _con
	if not _con:
		_con = MySQLdb.connect('localhost', 'root', 'root', 'crunchseries')
	return _con

def cursor():
	return con().cursor()

def commit():
	if _con:
		_con.commit()

def close():
	global _con
	if _con:
		_con.close()
	_con = None

atexit.register(close)