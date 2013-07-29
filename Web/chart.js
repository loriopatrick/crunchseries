var App = App || angular.module('App', ['ui.bootstrap']);

function ChartController ($scope, $element, $http) {
	var canvas = $element.find('div.canvas').first();

	$scope.flotSettings = {
		xaxis: {
			mode: 'time',
			panRange: true,
			zoomRange: true,
		},
		yaxis: {
			panRange: false,
			zoomRange: false,
			panScale: true
		},
		xaxes: [{}],
		yaxes: [{}, {}],
		grid: {
			borderWidth: 1,
			borderColor: 'rgb(230, 230, 230)',
			color: 'rgb(200, 200, 200)'
		},
		zoom: {
			interactive: true,
			trigger: "dblclick",
			amount: 1.1
		},
		pan: {
			interactive: true,
			cursor: "move",
			frameRate: 60
		},
		series: {
			shadowSize: 0,
			lines: {
				show: true,
				lineWidth: 2
			}
		},
		crosshair: {
			mode: 'xy',
			color: 'rgba(73, 175, 205, 0.89)'
		}
	};

	$scope.runGraph = function (creator, uid, revision, callback) {
		var url = '/api/graph/run?creator=' + creator + '&uid=' + uid;
		if (revision > -1) {
			url += '&revision=' + revision;
		}

		$http.get(url).success(callback);
	};

	function combine (x, y) {
		var res = [];
		var len = x.length;

		// console.log(arguments);
		// return [];

		for (var i = 0; i < arguments.length; i++) {
			var arg = arguments[i];
			if (arg.length < len) {
				len = arg.length;
			}
		};

		for (var i = 0; i < len; i++) {
			var item = [];
			for (var j = 0; j < arguments.length; j++) {
				var arg = arguments[j];
				item.push(arg[i] * (j == 0? 1000 : 1));
			};

			res.push(item);
		};

		return res;
	}

	$scope.init = function () {
		$scope.runGraph('admin', 'time', -1, function (data) {
			var results = data.results;

			var series = [];
			series.push({
				data: combine(results.time.series, results.close.series)
			});

			series.push({
				data: combine(results.time.series, results.sma.series)
			});

			series.push({
				data: combine(results.time.series, results['high band'].series),
				yaxis: 2
			});

			series.push({
				data: combine(results.time.series, results['low band'].series),
				yaxis: 2
			});

			$scope.chartTest(series);
		});
	};

	$scope.chartTest = function (series) {
		$.plot(canvas, series, $scope.flotSettings);
	};



	// ============= modal stuff ===============
	$scope.addXAxis = function () {
	};
}