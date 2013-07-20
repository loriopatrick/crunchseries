var App = App || angular.module('App', []);

function ChartController ($scope, $element, $http) {
	$scope.series = [];

	$scope.nodes = [];
	$scope.output = [];

	$scope.width = 600;
	$scope.height = 300;

	var seriesData = {};

	$scope.chart = function () {
		var chart = $($element).find('.canvas');
		setTimeout(function () {
			$.plot(chart, buildFlotData());
		}, 0);
	};

	function buildFlotData () {
		function combine (x, y) {
			var len = Math.min(x.length, y.length);
			var res = [];
			for (var i = 0; i < len; i++) {
				res.push([x[i], y[i]]);
			};
			return res;
		}

		function getResult (uid, name) {
			var results = seriesData[uid].results;
			for (var i = 0; i < results.length; i++) {
				var res = results[i];
				if (res.name == name) {
					return res.series;
				}
			};
			return null;
		}

		var res = [];

		for (var i = 0; i < $scope.series.length; i++) {
			var series = $scope.series[i];
			res.push({
				label: series.name,
				data: combine(
					getResult(series.x.graph_uid, series.x.output),
					getResult(series.y.graph_uid, series.y.output)
				)
			});
		};

		return res;
	}

	$scope.addSeries = function () {
		$scope.series.push({
			name: prompt('series name'),
			display: '**',
			x: {
				graph_uid: prompt('X axis graph uid'),
				output: prompt('X axis output')
			},
			y: {
				graph_uid: prompt('Y axis graph uid'),
				output: prompt('Y axis output')
			}
		});
	};

	$scope.update = function () {
		var uids = [];
		for (var i = 0; i < $scope.series.length; i++) {
			var series = $scope.series[i];
			if (uids.indexOf(series.x.graph_uid) == -1 && !(series.x.graph_uid in seriesData)) {
				uids.push(series.x.graph_uid);
			}
			if (uids.indexOf(series.y.graph_uid) == -1 && !(series.y.graph_uid in seriesData)) {
				uids.push(series.y.graph_uid);
			}
		};

		seriesData = {};
		var requestCompleted = 0;
		for (var i = 0; i < uids.length; i++) {
			var uid = uids[i];
			$http.get('/api/graph/run?uid=' + uid).success(function (data) {
				seriesData[uid] = data;
				
				if (++requestCompleted == uids.length) {
					$scope.chart();
				}
			});
		};
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