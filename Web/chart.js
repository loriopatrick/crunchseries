var App = App || angular.module('App', ['ui.bootstrap']);

function ChartController ($scope, $element, $http) {
	$scope.series = [];

	$scope.nodes = [];
	$scope.output = [];

	$scope.width = 600;
	$scope.height = 300;

	var seriesData = {};

	var chart = $($element).find('.canvas');

	$scope.graphInfo = null;
	$scope.graph = null;
	$scope.xaxis = null;

	$scope.series = [];

	$scope.build = function (xaxisName) {
		if (xaxisName == null) throw 'Xaxis must be defined';
		var url = '/api/graph/run?creator=' + $scope.graphInfo.creator + '&uid=' + $scope.graphInfo.uid;
		if ($scope.graphInfo.revision > -1) {
			url += '&revision=' + $scope.graphInfo.revision;
		}

		$http.get(url).success(function (data) {
			var xaxis = null;
			var series = [];
			for (var i = 0; i < data.results.length; i++) {
				var res = data.results[i];
				if (res.name === xaxisName) {
					xaxis = res.series;
					continue;
				}
				series.push(res);
			};

			var flotData = [];

			for (var i = 0; i < series.length; i++) {
				var s = series[i];
				flotData.push({
					label: s.name,
					data: combine(xaxis, s.series)
				});
			};

			$.plot(chart, flotData);
		});
	};

	function combine (x, y) {
		var len = Math.min(x.length, y.length);
		var res = [];
		for (var i = 0; i < len; i++) {
			res.push([x[i], y[i]]);
		};
		return res;
	}

	// ============ modal =============

	$scope.chartInitModalSearchInput = '';
	$scope.chartInitModalSearchRes = [];
	var lastSearch = '';
	$scope.chartInitModalSearch = function () {
		if (!$scope.chartInitModalSearchInput.length) {
			$scope.chartInitModalSearchRes = [];
			return;
		}
		if ($scope.chartInitModalSearchInput == lastSearch) return;
		lastSearch = $scope.chartInitModalSearchInput;
		$http.get('/api/graph/get?user=admin&uid_reg=' + $scope.chartInitModalSearchInput).success(function (data) {
			$scope.chartInitModalSearchRes.length = 0;
			for (var i = 0; i < data.results.length; ++i) {
				var res = data.results[i];
				if (res.inputs) continue;
				$scope.chartInitModalSearchRes.push(res);
			}
		});
	};

	$scope.selectGraph = function (graphInfo) {
		$scope.graphInfo = graphInfo;
		$http.get('/api/graph/get_node/' + graphInfo.creator + '/' + graphInfo.uid + '?revision=' + graphInfo.revision).success(function (data) {
			$scope.graph = data;
		});
	}

	$scope.setAxis = function (axis) {
		$scope.xaxis = axis;
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