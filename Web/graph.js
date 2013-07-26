var App = App || angular.module('App', ['ui.bootstrap']);

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

function GraphController($scope, $element, $http) {
	$scope.connections = [];
	$scope.nodes = [];
	$scope.btnGroups = [];
	$scope.guideLine = null;

	$scope.uid = '';

	$scope.nodeInfo = {};

	var selectedOutput, selectedInput, dragging;
	var scrollBoard = $($element[0]).find('.display').first();

	$scope.init = function () {
		$http.get('/api/stats').success(function (stats) {
			$scope.nodeInfo = stats;

			$http.get('/api/stats/groups').success(function (groups) {
				$scope.btnGroups = groups;
			});
		});
	};

	$scope.rename = function () {
		$scope.uid = prompt('Graph Name: ') || '';
	};

	$scope.getNodeInfoTitle = function (id) {
		return $scope.nodeInfo[id].title;
	};

	function getNodeClone (statId, node, callback) {
		if (node && node.statId == -2) {
			if (!node.uid) return null;

			var parts = node.uid.split('/');
			var parts2 = parts[1].split(':');

			var user = parts[0];
			var uid = parts2[0];
			var rev = parts2[1];

			$http.get('/api/graph/get_node/' + user + '/' + uid + '?revision=' + rev).success(function (data) {
				callback(clone(data));
			});

			return null;
		}

		var node = null;
		for (var id in $scope.nodeInfo) {
			if ($scope.nodeInfo[id].statId == statId) {
				node = clone($scope.nodeInfo[id]);
				break;
			}
		}

		if (callback) callback(node);
		return node;
	}

	$scope.addNode = function (name, x, y) {
		return addNodeObject($scope.nodeInfo[name], x, y);
	};

	function addNodeObject (obj, x, y) {
		var newNode = clone(obj);
		var newPos = normScroll(!x && x !== 0 ? 50 : x,  !y && y !== 0 ? 50 : y);
		newNode.x = newPos.x;
		newNode.y = newPos.y;
		$scope.nodes.push(newNode);
		return newNode;
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
			evt.clientX - node.x,
			evt.clientY - node.y
		);
	};

	$scope.mousemove = function (evt) {
		if (dragging) {
			var mousePos = normScroll(
				evt.clientX - dragging.dragOffest.x,
				evt.clientY - dragging.dragOffest.y
			);
			dragging.x = Math.max(mousePos.x, 0);
			dragging.y = Math.max(mousePos.y, 2);

			updateConnections();
		}

		if (selectedOutput) {
			var pos = getHandlePos(selectedOutput);
			var offset = getGraphOffset();
			var mousePos = normScroll(
				evt.clientX - offset.left,
				evt.clientY - offset.top, true
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

		baseConn.input.node.inputs[baseConn.input.pos].style = 'filled';

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

		var inputHandle = inputEl.find('.inputs [pos="' + inputPos + '"] .handle');
		var outputHandle = outputEl.find('.outputs [pos="' + outputPos + '"] .handle');

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
		setTimeout(updateConnections, 0);
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
		} else if (selectedOutput) {
			var input = $scope.addNode('output', evt.clientX - 10, evt.clientY - 80);
			var node = selectedOutput.node, pos = selectedOutput.pos;
			input.settings[0].val = node.outputs[pos].name;

			setTimeout(function () {
				addConnection(input, 0, node, pos);
			}, 0);
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
			node.inputs[pos].style = isOpen? 'add' : 'filled';
		} else if (!isOpen) {
			node.inputs[pos].style = 'remove';
		}
	};

	$scope.clearInput = function () {
		if (selectedInput) {
			var style = selectedInput.node.inputs[selectedInput.pos].style;

			selectedInput.node.inputs[selectedInput.pos].style = (style === 'remove' || style === 'filled')? 'filled' : '';
			selectedInput = null;
		}
	};

	$scope.removeInputConnection = function (node, input) {
		for (var i = 0; i < $scope.connections.length; ++i) {
			var line = $scope.connections[i];
			if (line.input.node == node && line.input.pos == input) {
				$scope.connections.splice(i, 1);
				node.inputs[input].style = '';
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
			if (!node.statId || node.statId === -1) continue;

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

		var loading_clones = 0;

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

			loading_clones += 1;

			function loadNode (node, id) {
				getNodeClone(node.statId, node, function (newNode) {
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

					loading_clones -= 1;
				});
			}

			loadNode(node, id);
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

		// timeout required for dom to updated by angular & loading custom nodes
		function doCons () {
			console.log('do cons');
			if (head === 'outputs') {
				var pos, i = 0;
				while ((pos = nodeIds.indexOf(head + '-' + i)) > -1) {
					buildInputConnections($scope.nodes[pos]);
					++i;
				}
				return;
			}

			buildInputConnections($scope.nodes[nodeIds.indexOf(head)]);
		}

		function doTimeout () {
			setTimeout(function () {
				if (loading_clones > 0) {
					doTimeout();
					return;
				}
				doCons();
			}, 100);
		}

		doTimeout();
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

	// --------- modals ----------

	$scope.newUid = '';
	$scope.checkNewUid = function () {
		$.blockUI({ message: 'Checking ...' });
		$http.get('/api/graph/get/admin/' + $scope.newUid + '/rev').success(function (data) {
			$.unblockUI();

			if (data == -1) { // new name
				$scope.save(true);
			} else {
				$scope.addAlert('warning', 'Name conflict with ' + $scope.newUid, 1000);
				$scope.saveModalOverride = true;
			}
		});

		$scope.saveModalSpecifyName = false;
	};

	$scope.saveModalSpecifyName = false;
	$scope.save = function (updateUid) {
		if (updateUid) {
			$scope.uid = $scope.newUid;
		}

		if (!$scope.uid.length) {
			$scope.saveModalSpecifyName = true;
			return;
		}

		$scope.saveModalSpecifyName = false;
		$scope.saveModalOverride = false;

		var data = serialize(true);
		var json = JSON.stringify(data);

		// todo encode or restrict charaters in uid

		$.blockUI({ message: 'Saving ...' });
		$http.put('/api/graph/save/' + $scope.uid, json).success(function (data) {
			$.unblockUI();
			var doc = data['result'];
			$scope.addAlert(data['success']? 'success' : 'error', 'Saved graph: "' + doc.uid + '" (Revision: ' + doc.revision + ')', 1500);
		});
	};

	$scope.newGraph = function (discard) {
		if ($scope.nodes.length && !discard) {
			$scope.newModalWarning = true;
			return;
		}

		$scope.uid = '';
		$scope.nodes.length = 0;
		$scope.connections.length = 0;
		$scope.newModalWarning = false;

		$scope.addAlert('success', 'Started new graph.', 500);
	};

	$scope.openModalSearch = '';
	var lastSearch = '';
	$scope.search = function () {
		if (!$scope.openModalSearch.length) return;
		if (lastSearch == $scope.openModalSearch) return;

		lastSearch = $scope.openModalSearch;
		$http.get('/api/graph/get?user=admin&uid_reg=' + $scope.openModalSearch).success(function (data) {
			$scope.openModalSearchRes = data.results;
		});
	};

	$scope.loadConfirm = function (res) {
		$scope.openInfo = res;
		$scope.openModalWarning=true;
	};

	$scope.load = function (uid, rev) {
		if (!uid) {
			$scope.addAlert('error', 'Cannot load: UID not specified');
			return;
		}

		if (!rev && rev !== 0) rev = -1;

		$.blockUI({ message: 'Loading Graph ...' });
		$http.get('/api/graph/get/admin/' + uid + '?revision=' + rev).success(function (data) {
			$.unblockUI();
			load(data);
			$scope.uid = uid;

			$scope.addAlert('success', 'Successfully loaded graph "' + uid + '"' + (rev === -1? '' : ' (Revision: ' + rev + ')'));
			$scope.openModal = false;
		});

		$scope.openModalWarning = false;
	};

	$scope.loadNode = function (uid, rev) {
		if (!uid) {
			$scope.addAlert('error', 'Cannot load: UID not specified');
			return;
		}
		if (!rev) rev = -1;

		$.blockUI({ message: 'Loading Node ...' });
		$http.get('/api/graph/get_node/admin/' + uid + '?revision=' + rev).success(function (data) {
			$.unblockUI();
			addNodeObject(data);

			$scope.addAlert('success', 'Successfully added node "' + uid + '"' + (rev === -1? '' : ' (Revision: ' + rev + ')'));
			$scope.openModal = false;
		});
	};

	$scope.loadSearchInput = '';
	$scope.loadSearchRes = [];
	var lastSearch = '';
	$scope.loadSearch = function () {
		if (!$scope.loadSearchInput.length) {
			$scope.loadSearchRes = [];
			return;
		}
		if ($scope.loadSearchInput == lastSearch) return;
		lastSearch = $scope.loadSearchInput;
		$http.get('/api/graph/get?user=admin&uid_reg=' + $scope.loadSearchInput).success(function (data) {
			$scope.loadSearchRes = data.results;
		});
	};

	// --------- alerts --------

	$scope.alerts = [];
	var alertCount = 0;
	$scope.addAlert = function (type, msg, time) {
		var id = ++alertCount;
		$scope.alerts.push({
			type: type,
			msg: msg,
			id: id
		});

		setTimeout(function () {
			for (var i = 0; i < $scope.alerts.length; i++) {
				var alert = $scope.alerts[i];
				if (alert.id === id) {
					$scope.alerts.splice(i, 1);
					return;
				}
			};
		}, time || 2000);
	};
}