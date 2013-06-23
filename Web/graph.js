var App = App || angular.module('App', []);

var NodesInfo = {
	'Standard Deviation': {
		name: 'Standard Deviation',
		x: 50,
		y: 50,
		inputs: [
			{name: 'in'}
		],
		settings: [
			{name: 'period', type: 'int'}
		],
		outputs: [
			{name: 'out'}
		]
	},
	'Simple Moving Average': {
		name: 'Simple Moving Average',
		x: 50,
		y: 50,
		inputs: [
			{name: 'in'}
		],
		settings: [
			{name: 'period', type: 'int'}
		],
		outputs: [
			{name: 'out'}
		]
	},
	'Exponential Moving Average': {
		name: 'Exponential Moving Average',
		x: 50,
		y: 50,
		inputs: [
			{name: 'in'}
		],
		settings: [
			{name: 'period', type: 'int'}
		],
		outputs: [
			{name: 'out'}
		]
	},
	'Slope': {
		name: 'Slope',
		x: 50,
		y: 50,
		inputs: [
			{name: 'in'}
		],
		settings: [
			{name: ''}
		],
		outputs: [
			{name: 'out'}
		]
	},
	'Sum': {
		name: 'Sum',
		x: 50,
		y: 50,
		inputs: [
			{name: 'a'},
			{name: 'b'}
		],
		settings: [
			{name: '+'}
		],
		outputs: [
			{name: 'a+b'}
		]
	},
	'Difference': {
		name: 'Difference',
		x: 50,
		y: 50,
		inputs: [
			{name: 'a'},
			{name: 'b'}
		],
		settings: [
			{name: '-'}
		],
		outputs: [
			{name: 'a-b'}
		]
	},
	'Product': {
		name: 'Product',
		x: 50,
		y: 50,
		inputs: [
			{name: 'a'},
			{name: 'b'}
		],
		settings: [
			{name: '*'}
		],
		outputs: [
			{name: 'a*b'}
		]
	},
	'Quotient': {
		name: 'Quotient',
		x: 50,
		y: 50,
		inputs: [
			{name: 'a'},
			{name: 'b'}
		],
		settings: [
			{name: '/'}
		],
		outputs: [
			{name: 'a/b'}
		]
	},
	'Correlation': {
		name: 'Correlation',
		x: 50,
		y: 50,
		inputs: [
			{name: 'a'},
			{name: 'b'}
		],
		settings: [
			{name: 'r2'}
		],
		outputs: [
			{name: 'out'}
		]
	},
	'% Difference': {
		name: '% Difference',
		x: 50,
		y: 50,
		inputs: [
			{name: 'a'},
			{name: 'b'}
		],
		settings: [
			{name: '%'}
		],
		outputs: [
			{name: 'out'}
		]
	},
	'Database': {
		name: '% Difference',
		statId: 1,
		x: 50,
		y: 50,
		inputs: [],
		settings: [
			{name: 'Database'},
			{name: 'Symbol'},
			{name: 'Begin'},
			{name: 'End'}
		],
		outputs: [
			{name: 'time'},
			{name: 'start'},
			{name: 'close'},
			{name: 'high'},
			{name: 'low'},
			{name: 'volume'}
		]
	},
	'Output': {
		name: 'Output',
		statId: 0,
		x: 50,
		y: 50,
		inputs: [
			{name: 'in'}
		],
		settings: [
			{name: 'name', val:''}
		],
		outputs: []
	}
};

function clone(obj) {
	if (null == obj || "object" != typeof obj) return obj;
	var copy = {};
	for (var attr in obj) {
		copy[attr] = clone(obj[attr]);
	}
	return copy;
}

function GraphController($scope, $element){
	$scope.connections = [];
	$scope.nodes = [];
	$scope.btnGroups = [
		{
			name: 'Statistics',
			buttons: [
				'Standard Deviation',
				'Simple Moving Average',
				'Exponential Moving Average',
				'Slope'
			]
		},
		{
			name: 'Operators',
			buttons: [
				'Sum',
				'Difference',
				'Product',
				'Quotient'
			]
		},
		{
			name: 'Benchmarks',
			buttons: [
				'Correlation',
				'% Difference'
			]
		},
		{
			name: 'Other',
			buttons: [
				'Database',
				'Output'
			]
		}
	];
	$scope.guideLine = null;

	var selectedOutput, selectedInput, dragging;
	var scrollBoard = $($element[0]).find('.display').first();

	$scope.addNode = function (name) {
		var newNode = clone(NodesInfo[name]);
		var newPos = normScroll(newNode.x, newNode.y);
		newNode.x = newPos.x;
		newNode.y = newPos.y;
		$scope.nodes.push(newNode);
	};

	$scope.destroyNode = function (node) {
		if (!confirm('Are you sure you want to delete this node? (' + node.name + ')')) return;
		for (var i = 0; i < $scope.connections.length;) {
			var con = $scope.connections[i];
			if (con.input.node == node || con.output.node == node) {
				$scope.connections.splice(i, 1);
				continue;
			}

			++i;
		}

		var nodePos = $scope.nodes.indexOf(node);
		$scope.nodes.splice(nodePos, 1);
	};

	$scope.selectNode = function (node, evt) {
		dragging = node;
		dragging.dragOffest = normScroll(
			evt.x - node.x,
			evt.y - node.y
		);
	};

	$scope.mousemove = function (evt) {
		if (dragging) {
			var mousePos = normScroll(
				evt.x - dragging.dragOffest.x,
				evt.y - dragging.dragOffest.y
			);
			dragging.x = Math.max(mousePos.x, 0);
			dragging.y = Math.max(mousePos.y, 2);

			updateConnections();
		}

		if (selectedOutput) {
			var pos = getHandlePos(selectedOutput);
			var offset = getGraphOffset();
			var mousePos = normScroll(
				evt.x - offset.left,
				evt.y - offset.top
			);
			$scope.guideLine = ['M', pos.x, ',', pos.y, 'L', mousePos.x, ',', mousePos.y, 'z'].join('');
		}
	};

	function getGraphOffset(obj) {
		var x = 0, y = 0;
		obj = obj || scrollBoard;
		return $(obj).offset();
	}

	function getScrollOffset() {
		return {
			left: scrollBoard[0].scrollLeft,
			top: scrollBoard[0].scrollTop
		};
	}

	function normScroll (x, y) {
		var scroll = getScrollOffset();
		return {
			x: x + scroll.left,
			y: y + scroll.top
		};
	}

	function updateConnections () {
		for (var i = 0; i < $scope.connections.length; ++i) {
			if (dragging == $scope.connections[i].input.node
				|| dragging == $scope.connections[i].output.node) {
				buildConnection($scope.connections[i]);
			}
		}
	}

	function getHandlePos (con) {
		var offset = getGraphOffset();
		var handleOffset = getGraphOffset(con.el);

		return normScroll(
			handleOffset.left - offset.left + 7,
			handleOffset.top - offset.top + 7
		);
	}

	function buildConnection (baseConn) {
		var output = getHandlePos(baseConn.output);
		var input = getHandlePos(baseConn.input);

		var xDist = output.x - input.x;
		var curveDist = Math.abs(xDist / 2);
		if (xDist > -20) curveDist += 100;

		baseConn.d = [
			'M', output.x, ',', output.y,
			'C', output.x + curveDist, ' ', output.y,
			',', input.x - curveDist, ' ', input.y,
			',', input.x, ' ', input.y
		].join('');

		return baseConn;
	}

	$scope.mouseup = function (evt) {
		dragging = null;
		if (selectedOutput && selectedInput
				&& selectedOutput.node != selectedInput.node
				&& isInputOpen(selectedInput.node, selectedInput.input)) {

			$scope.connections.push(buildConnection({
				output: selectedOutput,
				input: selectedInput
			}));
		}
		selectedOutput = null;
		$scope.clearInput();
		$scope.guideLine = null;
	};

	function isInputOpen(node, input) {
		for (var i = 0; i < $scope.connections.length; ++i) {
			if (node == $scope.connections[i].input.node
				&& input == $scope.connections[i].input.input) return false;
		}
		return true;
	}

	$scope.selectInput = function (evt, node, input) {
		selectedInput = {node: node, input: input, el: evt.target};

		var isOpen = isInputOpen(node, input);
		if (selectedOutput) {
			node.inputs[input].style = isOpen? 'add':'';
		} else if (!isOpen) {
			node.inputs[input].style = 'remove';
		}
	};

	$scope.clearInput = function () {
		if (selectedInput) {
			selectedInput.node.inputs[selectedInput.input].style = '';
			selectedInput = null;
		}
	};

	$scope.removeInputConnection = function (node, input) {
		for (var i = 0; i < $scope.connections.length; ++i) {
			var line = $scope.connections[i];
			if (line.input.node == node && line.input.input == input) {
				$scope.connections.splice(i, 1);
				console.log('removed item');
				return;
			}
		}
	};

	$scope.selectOutput = function (evt, node, output) {
		selectedOutput = {node: node, output: output, el: evt.target};
	};

	$scope.serialize = function () {
		function getInputs(node) {
			var inputs = [];
			for (var i = 0; i < $scope.connections.length; ++i) {
				var con = $scope.connections[i];
				if (con.input.node === node) {
					inputs.push(con);
				}
			}

			return inputs;
		}

		var outputs = [], nodes = {};
		for (var i = 0; i < $scope.nodes.length; ++i) {
			var node = $scope.nodes[i];
			if (node.statId === 0) {
				outputs.push(node);
			} else {
				nodes['node-' + i] = node;
			}
		}

		function findNodeId (node) {
			for (var key in nodes) {
				if (node == nodes[key]) return key;
			}
			throw 'WTF! node not found';
		}

		if (!outputs.length) return;
		var outputNode = {
			statId: 0,
			inputs: [],
			settings: []
		};

		for (var i = 0; i < outputs.length; ++i) {
			var con = getInputs(outputs[i])[0];
			outputNode.inputs.push({
				node: findNodeId(con.output.node),
				output: parseInt(con.output.output)
			});
			outputNode.settings.push(outputs[i].settings[0].val);
		}

		var resNodes = {};
		for (var key in nodes) {
			var node = nodes[key];
			var resNode = {
				statId: node.statId || -1,
				inputs: [],
				settings: []
			};

			var cons = getInputs(node);
			for (var i = 0; i < cons.length; ++i) {
				var outputId = findNodeId(cons[i].output.node);
				var output = cons[i].output.output;

				resNode.inputs[cons[i].input.input] = {
					node: outputId,
					output: parseInt(output)
				};
			}

			for (var i in node.settings) {
				resNode.settings.push(node.settings[i].val);
			}

			resNodes[key] = resNode;
		}

		resNodes['output'] = outputNode;

		var res = {
			nodes: resNodes,
			head: 'output'
		};

		console.log(JSON.stringify(res));
	};
}

App.directive('graph', function () {
	return {
		restrict: 'E',
		replace: true,
		templateUrl: 'templates/graph.html',
		controller: GraphController
	}
});