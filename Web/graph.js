var App = App || angular.module('App', []);

function clone(obj) {
	if (null == obj || "object" != typeof obj) return obj;
	var copy = {};
	for (var attr in obj) {
		copy[attr] = clone(obj[attr]);
	}
	return copy;
}

function combine(ar, str, start, end) {
	if (!end) end = ar.length;
	var res = [];
	for (var i = start; i < end; ++i) {
		res.push(ar[i]);
	}
	return res.join(str);
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

	function getNodeClone (statId) {
		var node = null;
		for (var id in nodeInfo) {
			if (nodeInfo[id].statId == statId) {
				node = clone(nodeInfo[id]);
				break;
			}
		}
		return node;
	}

	$scope.addNode = function (name) {
		addNodeObject(nodeInfo[name]);
	};

	function addNodeObject (obj) {
		var newNode = clone(obj);
		var newPos = normScroll(50, 50);
		newNode.x = newPos.x;
		newNode.y = newPos.y;
		$scope.nodes.push(newNode);
	}

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
				evt.y - offset.top, true
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

	function normScroll (x, y, mouse) {
		var scroll = getScrollOffset();
		if (mouse) {
			x += window.scrollX;
			y += window.scrollY;
		}
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

	function addConnection (inputNode, inputPos, outputNode, outputPos) {
		var jElement = $($element);
		var inputEl = jElement.find('.node[node-pos="' + $scope.nodes.indexOf(inputNode) + '"]').first();
		var outputEl = jElement.find('.node[node-pos="' + $scope.nodes.indexOf(outputNode) + '"]').first();

		var inputHandle = inputEl.find('.input[pos="' + inputPos + '"] > .handle');
		var outputHandle = outputEl.find('.output[pos="' + outputPos + '"] > .handle');

		var con = {
			input: {
				el: inputHandle,
				node: inputNode,
				pos: inputPos
			},
			output: {
				el: outputHandle,
				node: outputNode,
				pos: outputPos
			}
		};

		var con = buildConnection(con);
		$scope.connections.push(con);
	}

	$scope.mouseup = function (evt) {
		dragging = null;
		if (selectedOutput && selectedInput
				&& selectedOutput.node != selectedInput.node
				&& isInputOpen(selectedInput.node, selectedInput.pos)) {

			$scope.connections.push(buildConnection({
				output: selectedOutput,
				input: selectedInput
			}));
		}
		selectedOutput = null;
		$scope.clearInput();
		$scope.guideLine = null;
	};

	function isInputOpen(node, pos) {
		for (var i = 0; i < $scope.connections.length; ++i) {
			if (node == $scope.connections[i].input.node
				&& pos == $scope.connections[i].input.pos) return false;
		}
		return true;
	}

	$scope.selectInput = function (evt, node, pos) {
		selectedInput = {node: node, pos: pos, el: evt.target};

		var isOpen = isInputOpen(node, pos);
		if (selectedOutput) {
			node.inputs[pos].style = isOpen? 'add':'';
		} else if (!isOpen) {
			node.inputs[pos].style = 'remove';
		}
	};

	$scope.clearInput = function () {
		if (selectedInput) {
			selectedInput.node.inputs[selectedInput.pos].style = '';
			selectedInput = null;
		}
	};

	$scope.removeInputConnection = function (node, input) {
		for (var i = 0; i < $scope.connections.length; ++i) {
			var line = $scope.connections[i];
			if (line.input.node == node && line.input.pos == input) {
				$scope.connections.splice(i, 1);
				console.log('removed item');
				return;
			}
		}
	};

	$scope.toggleEdit = function (setting) {
		if (!setting.isPublic) {
			setting.publicName = prompt('Setting\'s public name');
		}

		setting.isPublic = !setting.isPublic;
	};

	$scope.selectOutput = function (evt, node, pos) {
		selectedOutput = {node: node, pos: pos, el: evt.target};
	};

	function serialize (addPos) {
		var nodes = {};
		var nodes_dict = {};

		var inputNodes = [];
		function getInputNodePos (node) {
			return inputNodes.indexOf(node);
		}

		var inputsNode = {
			statId: -1,
			settings: []
		};

		var outputNode = {
			statId: 0,
			settings: [],
			inputs: []
		};

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
			var index = inputNodes.indexOf(node);
			if (index > -1) return 'inputs';

			for (var key in nodes_dict) {
				if (node == nodes_dict[key]) return key;
			}
			throw 'WTF! node not found';
		}

		function serializeSetting (setting) {
			return (setting.type || 'str') + '-' + (setting.isPublic? setting.publicName : '') + '-' + setting.val;
		}

		function serializeOutput (con) {
			var node = con.output.node;
			var pos = con.output.pos;

			if (node.statId === -1) {
				pos = getInputNodePos(node);
			}

			return {node: node, output: parseInt(pos)};
		}

		// build inputs node
		for (var i = 0; i < $scope.nodes.length; i++) {
			var node = $scope.nodes[i];
			if (node.statId !== -1) continue;
			
			inputNodes.push(node);
			inputsNode.settings.push(serializeSetting(node.settings[0]));

			if (addPos) {
				if (!inputsNode.pos) {
					inputsNode.pos = [];
				}

				inputsNode.pos.push({x: node.x, y: node.y});
			}
		}

		if (inputsNode.settings.length) {
			nodes['inputs'] = inputsNode;
		}

		// get nodes
		for (var i = 0; i < $scope.nodes.length; ++i) {
			var node = $scope.nodes[i];
			if (!node.statId) continue;

			var sNode = {
				statId: node.statId || -1,
				settings: [],
				inputs: []
			};

			if (node.uid) {
				sNode.uid = node.uid;
			}

			var node_name = 'node_' + i;
			var cons = getConnections(node);

			for (var j = 0; j < cons.length; ++j) {
				var con = cons[j];
				sNode.inputs[con.input.pos] = serializeOutput(con);
			}

			for (var j in node.settings) {
				var setting = node.settings[j];
				sNode.settings[j] = serializeSetting(setting);
			}

			nodes[node_name] = sNode;
			nodes_dict[node_name] = node;

			if (addPos) {
				sNode.x = node.x;
				sNode.y = node.y;
			}
		}

		// build outputs node
		for (var i = 0; i < $scope.nodes.length; ++i) {
			var node = $scope.nodes[i];
			if (node.statId !== 0) continue;
			var con = getConnections(node);
			if (!con.length) continue;
			con = con[0];

			outputNode.settings.push(serializeSetting(node.settings[0]));
			outputNode.inputs.push(serializeOutput(con));

			if (addPos) {
				if (!outputNode.pos) {
					outputNode.pos = [];
				}

				outputNode.pos.push({x: node.x, y: node.y});
			}
		}

		if (!outputNode.inputs) throw 'Outputs are required';
		nodes['outputs'] = outputNode;

		// replace node object in the inputs section with their ids
		for (var key in nodes) {
			var node = nodes[key];
			if (!node.inputs || !node.inputs.length) continue;
			for (var j = 0; j < node.inputs.length; ++j) {
				node.inputs[j].node = getNodeId(node.inputs[j].node);
			}
		}

		return {nodes:nodes, head:'outputs'};
	}

	function load (data) {
		var nodes = data.nodes, head = data.head;

		$scope.nodes.length = 0;
		$scope.connections.length = 0;

		var nodeIds = [];

		for (var id in nodes) {
			var node = nodes[id];

			if (id === 'inputs' || id === 'outputs') {
				for (var i = 0; i < node.settings.length; i++) {
					var setting = node.settings[i];
					
					var newNode = getNodeClone(node.statId);
					newNode.settings[0].val = combine(setting.split('-'), '-', 2);
					newNode.inputs = [];

					if (node.inputs) {
						newNode.inputs.push(node.inputs[i]);
					}

					newNode.x = node.pos[i].x;
					newNode.y = node.pos[i].y;

					$scope.nodes.push(newNode);
					nodeIds.push(id + '-' + i);
				}

				continue;
			}

			var newNode = getNodeClone(node.statId);
			newNode.x = node.x;
			newNode.y = node.y;

			if (node.uid) {
				newNode.uid = node.uid;
			}

			for (var i = 0; i < node.settings.length; i++) {
				var setting = node.settings[i];
				var parts = setting.split('-');
				newNode.settings[i].isPublic = parts[1].length > 0;
				newNode.settings[i].publicName = parts[1];
				newNode.settings[i].val = combine(parts, '-', 2);
			}

			for (var i = 0; i < node.inputs.length; i++) {
				var input = node.inputs[i];
				newNode.inputs[i].node = input.node;
				newNode.inputs[i].output = input.output;
			}

			$scope.nodes.push(newNode);
			nodeIds.push(id);
		}

		function getIndex(input) {
			var id = input.node;
			if (id === 'inputs' || id === 'outputs') {
				id += '-' + input.output;
			}

			return nodeIds.indexOf(id);
		}

		function buildInputConnections(node) {
			if (!node.inputs) return;
			for (var i in node.inputs) {
				var input = node.inputs[i];
				var nodePos = getIndex(input);
				if (nodePos == -1) {
					console.log('Could not find node with input: ', input);
					continue;
				}
				var outputNode = $scope.nodes[nodePos];
				addConnection(node, i, outputNode, input.output);
				buildInputConnections(outputNode);
			}
		}

		// timeout required for dom to updated by angular
		setTimeout(function () {
			if (head === 'outputs') {
				var pos, i = 0;
				while ((pos = nodeIds.indexOf(head + '-' + i)) > -1) {
					buildInputConnections($scope.nodes[pos]);
					++i;
				}
				return;
			}

			buildInputConnections($scope.nodes[nodeIds.indexOf(head)]);
		}, 10);
	}

	$scope.serialize = function () {
		var nodeData = serialize(true);
		var requestData = JSON.stringify(nodeData);
		console.log('node data', nodeData);
		// load(nodeData);
		$http.post('/api/graph/run', requestData).success(function (data) {
			console.log(data);
		});
	};


	$scope.save = function () {
		var uid = prompt('uid');
		$http.put('/api/graph/save/' + uid, JSON.stringify(serialize(true))).success(function (data) {
			alert(data);
		});
	};

	$scope.load = function () {
		var uid = prompt('uid');
		$http.get('/api/graph/get/' + uid).success(function (data) {
			load(data);
		});
	};

	$scope.loadNode = function () {
		var uid = prompt('uid');
		$http.get('/api/graph/get_node/' + uid).success(function (data) {
			addNodeObject(data);
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