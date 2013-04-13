create table series_eom(
	id INT PRIMARY KEY AUTO_INCREMENT,
	symbol char(8),
	epoch int(11),
	high double,
	low double,
	open double,
	close double,
	volume int,
	UNIQUE KEY `symbol` (`symbol`, `epoch`));

create table series_eod(
	id INT PRIMARY KEY AUTO_INCREMENT,
	symbol char(8),
	epoch int(11),
	high double,
	low double,
	open double,
	close double,
	volume bigint,
	UNIQUE KEY `symbol` (`symbol`, `epoch`));