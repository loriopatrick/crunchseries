function Graph($scope, $element){
	$scope.lines = [];
	$scope.nodes = [
		{
			name: 'Standard Deviation',
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
		}
	];

	$scope.ghostLine = null;

	$scope.selectNode = function (node, evt) {
		$scope.dragging = node;
		$scope.dragging.dragOffest = {
			x: evt.x - node.x,
			y: evt.y - node.y
		};
		console.log('mouse down', evt);
	};

	function getConPos (con) {
		return {
			x: con.el.offsetLeft + con.node.x + 7,
			y: con.el.offsetTop + con.node.y + 7
		}
	}

	$scope.mousemove = function (evt) {
		if ($scope.dragging) {
			$scope.dragging.x = evt.x - $scope.dragging.dragOffest.x;
			$scope.dragging.y = evt.y - $scope.dragging.dragOffest.y;

			$scope.updateLines();
		}

		if ($scope.selectedOutput) {
			console.log($scope.selectedOutput);
			var pos = getConPos($scope.selectedOutput);
			$scope.ghostLine = ['M', pos.x, ',', pos.y, 'L', evt.x, ',', evt.y, 'z'].join('');
		}
	};

	$scope.mouseup = function (evt) {
		$scope.dragging = null;
		if ($scope.selectedOutput && $scope.selectedInput) {
			$scope.lines.push($scope.buildLine({
				output: $scope.selectedOutput,
				input: $scope.selectedInput
			}));
		}
		$scope.selectedOutput = null;
		$scope.selectedInput = null;
		$scope.ghostLine = null;
	};

	$scope.updateLines = function () {
		for (var i = 0; i < $scope.lines.length; ++i) {
			if ($scope.dragging == $scope.lines[i].input.node
				|| $scope.dragging == $scope.lines[i].output.node) {
				$scope.buildLine($scope.lines[i]);
			}
		}
	};

	$scope.buildLine = function (baseLine) {
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
	};

	$scope.selectInput = function (evt, node, input) {
		$scope.selectedInput = {node: node, input: input, el: evt.target};
		console.log('select input', node, input);
	};

	$scope.clearInput = function () {
		$scope.selectedInput = null;
		console.log('clear input');
	};

	$scope.selectOutput = function (evt, node, output) {
		$scope.selectedOutput = {node: node, output: output, el: evt.target};
		console.log('select output', node, output);
	};
}