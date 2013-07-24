var App = App || angular.module('App', ['ui.bootstrap']);

function ChartController ($scope, $element, $http) {
	var placeholder = null;
	var plot = null;

	$scope.init = function () {
		placeholder = $($element).find('.canvas');
		$scope.buildSeries('admin', 'calc', -1, 'time');
	};

	$scope.buildSeries = function (creator, uid, revision, xaxisName) {
		if (xaxisName == null) throw 'Xaxis must be defined';
		if (!revision) revision = -1;

		var url = '/api/graph/run?creator=' + creator + '&uid=' + uid;
		if (revision > -1) {
			url += '&revision=' + revision;
		}

		$http.get(url).success(function (data) {
			var xaxis = null;
			var series = [];

			// seperate xaxis and series
			for (var i = 0; i < data.results.length; i++) {
				var res = data.results[i];
				if (res.name === xaxisName) {
					xaxis = res.series;
					continue;
				}
				series.push(res);
			};

			var maxX = xaxis[xaxis.length - 1] * 1000, minX = xaxis[0] * 1000, 
				maxY = null, minY = null;

			function combine (x, y) {
				var len = Math.min(x.length, y.length);
				var res = [];
				for (var i = 0; i < len; i++) {
					if (maxY == null || y[i] > maxY) maxY = y[i];
					if (minY == null || y[i] < minY) minY = y[i];
					res.push([x[i] * 1000, y[i]]);
				};
				return res;
			}

			// build flot data by combining series with xaxis
			var flotData = [];
			for (var i = 0; i < series.length; i++) {
				var s = series[i];
				flotData.push({
					label: s.name,
					data: combine(xaxis, s.series)
				});
			};

			var range = maxY - minY;

			runFlot(flotData, {
				xaxis: {
					panRange: [minX, maxX]
				},
				yaxis: {
					panRange: [minY - range / 10, maxY + range / 10]
				}
			});
		});
	};

	function runFlot (data, opts) {
		var defaultOpts = {
			xaxis: {
				mode: 'time'
			},
			yaxis: {
			},
			grid: {
				borderWidth: 1,
				borderColor: 'rgb(243, 243, 243)',
				tickColor: 'rgb(243, 243, 243)'
			},
			series: {
				shadowSize: 1,
				lines: { lineWidth: 1 }
			},
			crosshair: {
				mode: 'xy',
				color: 'grey',
			},
			zoom: {
				interactive: true,
				amount: 1.1
			},
			pan: {
				interactive: true,
				frameRate: 60,
				cursor: 'hidden'
			}
		};
		// if (plot) {
		// 	$.each(plot.getAxes(), function(_, axis) {

		// 	});
		// }
		plot = $.plot(placeholder, data, $.extend(true, defaultOpts, opts || {}));
	}

	$scope.toggleAxis = function (axis, on) {
		if (!plot) return;
		var axes = plot.getAxes();
		if ((!on && on !== false && axes[axis].zoomRange === false) || on) {
			axes[axis].zoomRange = axes[axis].zoomRangeBackup || null;
		} else {
			axes[axis].zoomRangeBackup = axes[axis].zoomRange;
			axes[axis].zoomRange = false;
		}
		axes[axis].zoomRange = !axes[axis].zoomRange;
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