var App = App || angular.module('App', []);

function clone(obj) {
	if (null == obj || "object" != typeof obj) return obj;
	var copy = {};
	for (var attr in obj) {
		copy[attr] = clone(obj[attr]);
	}
	return copy;
}

function GraphController($scope, $element, $http){
	$scope.connections = [];
	$scope.nodes = [];
	$scope.btnGroups = [];
	$scope.guideLine = null;
	
	var nodeInfo = {};

	var selectedOutput, selectedInput, dragging;
	var scrollBoard = $($element[0]).find('.display').first();

	$scope.init = function () {
		$http.get('/api/stats').success(function (stats) {
			nodeInfo = stats;

			$http.get('/api/stats/groups').success(function (groups) {
				$scope.btnGroups = groups;
			});
		});
	};

	$scope.getNodeInfoTitle = function (id) {
		return nodeInfo[id].title;
	};

	$scope.addNode = function (name) {
		var newNode = clone(nodeInfo[name]);
		console.log(name, nodeInfo, nodeInfo[name], newNode);
		var newPos = normScroll(50, 50);
		newNode.x = newPos.x;
		newNode.y = newPos.y;
		$scope.nodes.push(newNode);
	};

	$scope.destroyNode = function (node) {
		if (!confirm('Are you sure you want to delete this node? (' + node.title + ')')) return;
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

	function serialize () {
		var nodes = {};
		var nodes_dict = {};

		function getConnections(node) {
			var cons = [];
			for (var i = 0; i < $scope.connections.length; ++i) {
				var con = $scope.connections[i];
				if (con.input.node === node) {
					cons.push(con);
				}
			}
			return cons;
		}

		function getNodeId (node) {
			for (var key in nodes_dict) {
				if (node == nodes_dict[key]) return key;
			}
			throw 'WTF! node not found';
		}

		function parseSetting (setting) {
			return (setting.type || 'str') + '-' + setting.val;
		}

		function getNodes () {
			for (var i = 0; i < $scope.nodes.length; ++i) {
				var node = $scope.nodes[i];
				if (node.statId === 0) continue;

				var sNode = {
					statId: node.statId || -1,
					settings: [],
					inputs: []
				};

				var node_name = 'node_' + i;
				var cons = getConnections(node);

				for (var j = 0; j < cons.length; ++j) {
					var con = cons[j];
					sNode.inputs[con.input.input] = {
						node: con.output.node,
						output: parseInt(con.output.output)
					}
				}

				for (var j in node.settings) {
					var setting = node.settings[j];
					sNode.settings[j] = parseSetting(setting);
				}

				nodes[node_name] = sNode;
				nodes_dict[node_name] = node;
			}

			for (var key in nodes) {
				var node = nodes[key];
				for (var j = 0; j < node.inputs.length; ++j) {
					node.inputs[j].node = getNodeId(node.inputs[j].node);
				}
			}
		}

		function buildOutput () {
			var outputNode = {
				statId: 0,
				settings: [],
				inputs: []
			};

			for (var i = 0; i < $scope.nodes.length; ++i) {
				var node = $scope.nodes[i];
				if (node.statId !== 0) continue;
				var con = getConnections(node);
				if (!con.length) continue;
				con = con[0];

				outputNode.settings.push(parseSetting(node.settings[0]));
				outputNode.inputs.push({
					node: getNodeId(con.output.node),
					output: parseInt(con.output.output)
				});
			}

			if (!outputNode.inputs) throw 'Outputs are required';
			nodes['output'] = outputNode;
		}

		getNodes();
		buildOutput();

		return {nodes:nodes, head:'output'};
	}

	$scope.serialize = function () {
		var nodeData = serialize();
		var requestData = JSON.stringify(nodeData);
		console.log('request', requestData);

		$http.post('/api/graph/run', requestData).success(function (data) {
			console.log(data);
		});
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