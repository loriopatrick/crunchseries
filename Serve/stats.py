stats = {
	'input': {
		'title': 'Input',
		'statId': -1,
		'inputs': [],
		'settings': [
			{'name': 'name', 'type': 'str', 'val':''}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'output': {
		'title': 'Output',
		'statId': 0,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'name', 'type': 'str', 'val':''}
		],
		'outputs': []
	},
	'database': {
		'title': 'Database',
		'statId': 1,
		'inputs': [],
		'settings': [
			{'name': 'Database', 'type': 'str'},
			{'name': 'Symbol', 'type': 'str'},
			{'name': 'Begin', 'type': 'float'},
			{'name': 'End', 'type': 'float'}
		],
		'outputs': [
			{'name': 'time'},
			{'name': 'high'},
			{'name': 'low'},
			{'name': 'open'},
			{'name': 'close'},
			{'name': 'volume'}
		]
	},
	'sma': {
		'title': 'Simple Moving Average',
		'statId': 2,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'period', 'type': 'int', 'val': 24}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'ema': {
		'title': 'Exponential Moving Average',
		'statId': 3,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'period', 'type': 'int', 'val': 24}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'stdv': {
		'title': 'Standard Deviation',
		'statId': 4,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'period', 'type': 'int', 'val': 24}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'slope': {
		'title': 'Slope',
		'statId': 5,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [],
		'outputs': [
			{'name': 'out'}
		]
	},
	'percent_difference': {
		'title': '% Difference',
		'statId': 6,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a vs b'}
		]
	},
	'trend_line': {
		'title': 'Trend Line',
		'statId': 7,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'period', 'type': 'int', 'val': 24}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'add_number': {
		'title': 'Add Number',
		'statId': 8,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'number', 'type': 'int', 'val': 0}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'multi_number': {
		'title': 'Multiply Number',
		'statId': 9,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'number', 'type': 'int', 'val': 0}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'power_number': {
		'title': 'Power Number',
		'statId': 10,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'number', 'type': 'int', 'val': 0}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'exp_number': {
		'title': 'Exponentiate Number',
		'statId': 11,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'number', 'type': 'int', 'val': 0}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'sum': {
		'title': 'Sum',
		'statId': 12,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a + b'}
		]
	},
	'difference': {
		'title': 'Difference',
		'statId': 13,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a - b'}
		]
	},
	'product': {
		'title': 'Multiply',
		'statId': 14,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a * b'}
		]
	},
	'quotient': {
		'title': 'Quotient',
		'statId': 15,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a / b'}
		]
	},
	'power': {
		'title': 'Power',
		'statId': 16,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a ^ b'}
		]
	},
	'above_threshold': {
		'title': 'Above Threshold',
		'statId': 17,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'threshold', 'type': 'int', 'val': '100'}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'bellow_threshold': {
		'title': 'Bellow Threshold',
		'statId': 18,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'threshold', 'type': 'int', 'val': '100'}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'between_threshold': {
		'title': 'Between Threshold',
		'statId': 19,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'top', 'type': 'int', 'val': '50'},
			{'name': 'bottom', 'type': 'int', 'val': '100'}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'and_gate': {
		'title': 'AND Gate',
		'statId': 20,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a && b'}
		]
	},
	'or_gate': {
		'title': 'OR Gate',
		'statId': 21,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a || b'}
		]
	},
	'xor_gate': {
		'title': 'XOR Gate',
		'statId': 22,
		'inputs': [
			{'name': 'a'},
			{'name': 'b'}
		],
		'settings': [],
		'outputs': [
			{'name': 'a && !b || !a && b'}
		]
	},
	'offset_series': {
		'title': 'Offset Series',
		'statId': 23,
		'inputs': [
			{'name': 'in'}
		],
		'settings': [
			{'name': 'offset', 'type': 'int', 'val': 0}
		],
		'outputs': [
			{'name': 'out'}
		]
	},
	'long_yield': {
		'title': 'Long Yield',
		'statId': 24,
		'inputs': [
			{'name': 'price'},
			{'name': 'buy'}
		],
		'settings': [],
		'outputs': [
			{'name': 'yield'}
		]
	}
}

groups = [
	{
		'title': 'Statistics',
		'stats': [
			'sma',
			'ema',
			'stdv',
			'slope',
			'long_yield',
			'percent_difference',
			'trend_line'
		]
	},
	{
		'title': 'Logic',
		'stats': [
			'bellow_threshold',
			'above_threshold',
			'between_threshold',
			'and_gate',
			'or_gate',
			'xor_gate',
			'offset_series'
		]
	},
	{
		'title': 'Math',
		'stats': [
			'add_number',
			'multi_number',
			'power_number',
			'exp_number',
			'sum',
			'product',
			'difference',
			'quotient',
			'power'
		]
	},
	{
		'title': 'Nodes',
		'stats': [
			'input',
			'output',
			'database'
		]
	}
]