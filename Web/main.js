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

	$scope.select = function (node, evt) {
		$scope.dragging = node;
		$scope.dragging.dragOffest = {
			x: evt.x - node.x,
			y: evt.y - node.y
		};
		console.log('mouse down', evt);
	};

	$scope.mousemove = function (evt) {
		if ($scope.dragging) {
			$scope.dragging.x = evt.x - $scope.dragging.dragOffest.x;
			$scope.dragging.y = evt.y - $scope.dragging.dragOffest.y;

			$scope.updateLines();
		}
	};

	function getOffest(el) {

	}

	$scope.updateLines = function (subject) {
		console.log('todo...(1)');
	};

	$scope.buildLine = function (baseLine) {
		console.log(baseLine);
		baseLine.d = [
			'M', baseLine.output.el.offsetLeft + baseLine.output.node.x + 10, ',', baseLine.output.el.offsetTop + baseLine.output.node.y + 10,
			'L', baseLine.input.el.offsetLeft + baseLine.input.node.x + 10, ',', baseLine.input.el.offsetTop + baseLine.input.node.y + 10,
			'z'
		].join('');
		console.log(baseLine.d);
		return baseLine;
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
	}
}