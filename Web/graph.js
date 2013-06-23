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
		x: 50,
		y: 50,
		inputs: [
			{name: 'dme'}
		],
		settings: [
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

	$scope.addNode = function (name) {
		$scope.nodes.push(clone(NodesInfo[name]));
	};

	$scope.selectNode = function (node, evt) {
		$scope.dragging = node;
		$scope.dragging.dragOffest = {
			x: evt.x - node.x,
			y: evt.y - node.y
		};
	};

	$scope.mousemove = function (evt) {
		if ($scope.dragging) {
			$scope.dragging.x = evt.x - $scope.dragging.dragOffest.x;
			$scope.dragging.y = evt.y - $scope.dragging.dragOffest.y;

			updateConnections();
		}

		if ($scope.selectedOutput) {
			var pos = getHandlePos($scope.selectedOutput);
			var offset = getGraphOffset();
			$scope.guideLine = ['M', pos.x, ',', pos.y, 'L', evt.x - offset.left, ',', evt.y - offset.top, 'z'].join('');
		}
	};

	function getGraphOffset(obj) {
		var x = 0, y = 0;
		obj = obj || $element[0];
		return $(obj).offset();
	}

	function updateConnections () {
		for (var i = 0; i < $scope.connections.length; ++i) {
			if ($scope.dragging == $scope.connections[i].input.node
				|| $scope.dragging == $scope.connections[i].output.node) {
				buildConnection($scope.connections[i]);
			}
		}
	}

	function getHandlePos (con) {
		var offset = getGraphOffset();
		var handleOffset = getGraphOffset(con.el);

		return {
			x: handleOffset.left - offset.left + 7,
			y: handleOffset.top - offset.top + 7
		}
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
		$scope.dragging = null;
		if ($scope.selectedOutput && $scope.selectedInput
				&& $scope.selectedOutput.node != $scope.selectedInput.node
				&& isInputOpen($scope.selectedInput.node, $scope.selectedInput.input)) {

			$scope.connections.push(buildConnection({
				output: $scope.selectedOutput,
				input: $scope.selectedInput
			}));
		}
		$scope.selectedOutput = null;
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
		$scope.selectedInput = {node: node, input: input, el: evt.target};

		var isOpen = isInputOpen(node, input);
		if ($scope.selectedOutput) {
			node.inputs[input].style = isOpen? 'add':'';
		} else if (!isOpen) {
			node.inputs[input].style = 'remove';
		}
	};

	$scope.clearInput = function () {
		if ($scope.selectedInput) {
			$scope.selectedInput.node.inputs[$scope.selectedInput.input].style = '';
			$scope.selectedInput = null;
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
		$scope.selectedOutput = {node: node, output: output, el: evt.target};
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