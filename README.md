![Screenshot](https://github.com/loriopatrick/crunchseries/raw/master/Screen%20Shot%202013-06-23%20at%2012.09.22%20AM.png)

TODO:
	- add series node the converts to outputs with special names (ex. #series-x-series_name)
		- build a meta language for outputs
		- examples:
			- ##cs:<query string request>
			- ##cs:series=series1&series-axis=x
	- build system that accepts the custom output names and renders the data to a chart

NOTE:
	- remember to commit individual changes, not groups!!!

FEATURES:
	- double click custom node in edit inners, ability to go between layers
	- colapse nodes into custom node
	- have serveral setting have the same public name and they all take the same value
	- get inputs for settings to fit the value type (ex. date selector)


Graph document database fields
==============================

- uid : the file name
- creator : the id of the creator (uid has to be unique for creator)
- access : an array of user ids who have access
- revision : the revision number (starts at 0)
- timestamp : the time at which the file was saved
