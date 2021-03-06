BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS `Translations` (
	`Captain`	TEXT NOT NULL UNIQUE,
	`Language`	TEXT,
	`LocalText`	TEXT,
	FOREIGN KEY(`Language`) REFERENCES `Languages`(`Code`),
	PRIMARY KEY(`Captain`)
);
CREATE TABLE IF NOT EXISTS `Spells` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL,
	`Description`	TEXT NOT NULL,
	`Tag`	TEXT NOT NULL,
	FOREIGN KEY(`Description`) REFERENCES `Translations`(`Captain`)
);
CREATE TABLE IF NOT EXISTS `SpellPresets` (
	`ID`	INTEGER NOT NULL UNIQUE,
	`SpellID`	INTEGER NOT NULL,
	`InitSpellLevel`	INTEGER NOT NULL,
	PRIMARY KEY(`ID`,`SpellID`),
	FOREIGN KEY(`SpellID`) REFERENCES `Spells`(`ID`)
);
CREATE TABLE IF NOT EXISTS `SpellDetails` (
	`ID`	INTEGER NOT NULL,
	`Level`	INTEGER NOT NULL,
	`PrepareTime`	REAL NOT NULL,
	`PrecastTime`	REAL NOT NULL,
	`PostcastTime`	REAL NOT NULL,
	`Range`	REAL NOT NULL,
	`Distance`	REAL NOT NULL,
	`CastRange`	REAL NOT NULL,
	`CastType`	INTEGER NOT NULL,
	`MissileModel`	INTEGER NOT NULL,
	FOREIGN KEY(`ID`) REFERENCES `Spells`(`ID`),
	PRIMARY KEY(`ID`,`Level`),
	FOREIGN KEY(`CastType`) REFERENCES `CastTypes`(`ID`),
	FOREIGN KEY(`MissileModel`) REFERENCES `Models`(`ID`)
);
CREATE TABLE IF NOT EXISTS `Sounds` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL,
	`Path`	TEXT NOT NULL,
	`Volume`	REAL NOT NULL,
	`Loop`	INTEGER NOT NULL
);
CREATE TABLE IF NOT EXISTS `Services` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL,
	`Protocol`	INTEGER NOT NULL,
	`PreferredPort`	INTEGER NOT NULL,
	`NetworkType`	INTEGER NOT NULL,
	`Arguments`	TEXT NOT NULL,
	FOREIGN KEY(`NetworkType`) REFERENCES `NetworkTypes`(`ID`),
	FOREIGN KEY(`Protocol`) REFERENCES `NetworkProtocols`(`ID`)
);
CREATE TABLE IF NOT EXISTS `ServerTypes` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS `Plugins` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL,
	`Path`	TEXT NOT NULL,
	`StartMode`	INTEGER NOT NULL,
	`Arguments`	TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS `NetworkTypes` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS `NetworkProtocols` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS `Models` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL,
	`Path`	TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS `Languages` (
	`Code`	TEXT NOT NULL UNIQUE,
	`Name`	TEXT NOT NULL,
	`LocalName`	TEXT,
	PRIMARY KEY(`Code`)
);
CREATE TABLE IF NOT EXISTS `Items` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL,
	`Type`	INTEGER NOT NULL,
	`Loop`	INTEGER NOT NULL,
	`Volume`	REAL NOT NULL,
	FOREIGN KEY(`Type`) REFERENCES `ItemTypes`(`ID`)
);
CREATE TABLE IF NOT EXISTS `ItemTypes` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS `GlobalConfigs` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL,
	`Value`	TEXT NOT NULL,
	`Type`	TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS `DomainServers` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Type`	INTEGER NOT NULL,
	`Name`	TEXT NOT NULL,
	`URL`	TEXT NOT NULL,
	`Port`	INTEGER NOT NULL,
	FOREIGN KEY(`Type`) REFERENCES `ServerTypes`(`ID`)
);
CREATE TABLE IF NOT EXISTS `Characters` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Model`	INTEGER NOT NULL,
	`SpellPreset`	INTEGER NOT NULL,
	`Name`	TEXT NOT NULL,
	`Camp`	TEXT NOT NULL,
	`BaseHitPoint`	REAL NOT NULL,
	`BaseManaPoint`	REAL NOT NULL,
	`Level`	INTEGER NOT NULL DEFAULT 1,
	`MasterAttribute`	TEXT NOT NULL,
	FOREIGN KEY(`SpellPreset`) REFERENCES `SpellPresets`(`ID`),
	FOREIGN KEY(`Model`) REFERENCES `Models`(`ID`)
);
CREATE TABLE IF NOT EXISTS `CastTypes` (
	`ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	`Name`	TEXT NOT NULL
);
CREATE UNIQUE INDEX IF NOT EXISTS `Captain` ON `Translations` (
	`Captain`	ASC
);
COMMIT;
