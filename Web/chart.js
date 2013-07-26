var App = App || angular.module('App', ['ui.bootstrap']);

function ChartController ($scope, $element, $http) {
	var canvas = $element.find('div.canvas').first();

	$scope.foo = 'test';

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
				data: combine(results.time.series, results.close.series),
				name: 'test',
				dataGrouping : {
					units : [
						['week', [1]],
						['month', [1, 2, 3, 4, 6]]
					]
				}
			});

			series.push({
				data: combine(results.time.series, results.sma.series),
				name: 'test 2',
				dataGrouping : {
					units : [
						['week', [1]],
						['month', [1, 2, 3, 4, 6]]
					]
				}
			});

			series.push({
				data: combine(results.time.series, results['high band'].series),
				name: 'test 2',
				dataGrouping : {
					units : [
						['week', [1]],
						['month', [1, 2, 3, 4, 6]]
					]
				}
			});

			series.push({
				data: combine(results.time.series, results['low band'].series),
				name: 'test 2',
				dataGrouping : {
					units : [
						['week', [1]],
						['month', [1, 2, 3, 4, 6]]
					]
				}
			});

			$scope.chartTest(series);
		});
	};

	$scope.chartTest = function (series) {
		$(canvas).highcharts('StockChart', {
			rangeSelector : {
				selected : 1
			},

			title : {
				text : 'Test'
			},

			series: series
		});
	};
}