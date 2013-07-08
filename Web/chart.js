var App = App || angular.module('App', []);

function ChartController ($scope, $element) {
	$scope.series = [
		{
			name: 'time vs close',
			display: 'none',
			x: {
				graph: null,
				output: -1
			},
			y: {
				graph: null,
				output: -1
			}
		}
	];

	$scope.nodes = [];
	$scope.output = [];

	$scope.width = 600;
	$scope.height = 300;

	$scope.chart = function () {
		var chart = $($element).find('.canvas');
		setTimeout(function () {
			$.plot(chart, [{data: [[0, 1], [10, 10]]}]);
		}, 0);
	};
}

App.directive('chart', function () {
	return {
		restrict: 'E',
		replace: true,
		templateUrl: 'templates/chart.html',
		controller: ChartController
	}
});