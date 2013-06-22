function Graph($scope, $element){
	$scope.lines = [];
	$scope.nodes = [
		{
			name: 'Standard Deviation',
			x: 0,
			y: 0,
			inputs: [
				{name: 'in'}
			],
			settings: [
				{name: 'p-size', type: 'int'}
			],
			outputs: [
				{name: 'out'}
			]
		},
		{
			name: 'SMA',
			x: 10,
			y: 10,
			inputs: [
				{name: 'in'}
			],
			settings: [
				{name: 'p-size', type: 'int'}
			],
			outputs: [
				{name: 'out'}
			]
		},
		{
			name: 'Difference',
			x: 10,
			y: 10,
			inputs: [
				{name: 'a'},
				{name: 'b'}
			],
			settings: [
				{name: 'foo'}
			],
			outputs: [
				{name: 'a-b'}
			]
		}
	];

	$scope.ghostLine = null;

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

			updateLines();
		}

		if ($scope.selectedOutput) {
			var pos = getConPos($scope.selectedOutput);
			$scope.ghostLine = ['M', pos.x, ',', pos.y, 'L', evt.x, ',', evt.y, 'z'].join('');
		}
	};

	function updateLines () {
		for (var i = 0; i < $scope.lines.length; ++i) {
			if ($scope.dragging == $scope.lines[i].input.node
				|| $scope.dragging == $scope.lines[i].output.node) {
				buildLine($scope.lines[i]);
			}
		}
	}

	function getConPos (con) {
		return {
			x: con.el.offsetLeft + con.node.x + 7,
			y: con.el.offsetTop + con.node.y + 7
		}
	}

	function buildLine (baseLine) {
		var output = getConPos(baseLine.output);
		var input = getConPos(baseLine.input);

		var xDist = output.x - input.x;
		var curveDist = Math.abs(xDist / 2);
		if (xDist > -20) curveDist += 100;

		baseLine.d = [
			'M', output.x, ',', output.y,
			'C', output.x + curveDist, ' ', output.y,
			',', input.x - curveDist, ' ', input.y,
			',', input.x, ' ', input.y
		].join('');

		return baseLine;
	}

	$scope.mouseup = function (evt) {
		$scope.dragging = null;
		if ($scope.selectedOutput && $scope.selectedInput
				&& $scope.selectedOutput.node != $scope.selectedInput.node
				&& isInputOpen($scope.selectedInput.node, $scope.selectedInput.input)) {

			$scope.lines.push(buildLine({
				output: $scope.selectedOutput,
				input: $scope.selectedInput
			}));
		}
		$scope.selectedOutput = null;
		$scope.clearInput();
		$scope.ghostLine = null;
	};

	function isInputOpen(node, input) {
		for (var i = 0; i < $scope.lines.length; ++i) {
			if (node == $scope.lines[i].input.node
				&& input == $scope.lines[i].input.input) return false;
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

	$scope.removeInputLine = function (node, input) {
		for (var i = 0; i < $scope.lines.length; ++i) {
			var line = $scope.lines[i];
			if (line.input.node == node && line.input.input == input) {
				$scope.lines.splice(i, 1);
				console.log('removed item');
				return;
			}
		}
	};

	$scope.selectOutput = function (evt, node, output) {
		$scope.selectedOutput = {node: node, output: output, el: evt.target};
	};
}