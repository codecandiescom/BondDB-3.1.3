CREATE TABLE person (
	id integer NOT NULL PRIMARY KEY AUTO_INCREMENT,
	firstname character(32),
	lastname character(32),
	businesstitle character(32),
	email character(64),
	password character(32),
	contactdetails character(32),
	contract character(32) default 'no contract',
	type character(1)
);

CREATE TABLE persontype (
	id integer AUTO_INCREMENT NOT NULL PRIMARY KEY,
	name text
);

CREATE TABLE country (
	id integer NOT NULL,
	name text NOT NULL,
	continent character varying(50),
	Constraint country_pkey Primary Key (id)
);

CREATE TABLE award (
	id integer NOT NULL,
	description text NOT NULL,
	Constraint award_pkey Primary Key (id)
);

CREATE TABLE rank (
	id integer NOT NULL,
	pay double precision,
	description text NOT NULL,
	countryid integer NOT NULL,
	insuranceid integer NOT NULL,
	Constraint rank_pkey Primary Key (id)
);

CREATE TABLE weapon (
	id integer NOT NULL,
	name text NOT NULL,
	damage double precision,
	soldierid integer NOT NULL,
	Constraint weapon_pkey Primary Key (id)
);

CREATE TABLE awards (
	awardid integer NOT NULL,
	soldierid integer NOT NULL,
	Constraint awards_pkey Primary Key (awardid, soldierid)
);

CREATE TABLE insurance (
	id integer NOT NULL,
	value integer NOT NULL,
	payments text NOT NULL,
	rankid integer NOT NULL,
	countryid integer NOT NULL,
	Constraint insurance_pkey Primary Key (id)
);

CREATE TABLE soldier (
	id serial primary key,
	name character(30) NOT NULL,
	dob date default '2005-01-05',
	location text,
	kills integer,
	active boolean default false NOT NULL,
	rankid integer NOT NULL,
	countryid integer NOT NULL,
	nihil boolean
);


DELETE FROM person;
INSERT INTO person VALUES
(2,'andru','hill','ceo','andrew@treshna.com',\N,\N,\N,3),
(5,'marika','hill','','marika@treshna.com',\N,\N,\N,2),
(4,'hill','','','marika@treshna.com',\N,\N,\N,1),
(6,'marika','hill','','marika@treshna.com',\N,\N,\N,2),
(7,'marika','hill','','marika@treshna.com',\N,\N,\N,1);


DELETE FROM persontype;
INSERT INTO persontype VALUES
(1,'quiet'),
(2,'frank'),
(3,'to loud'),
(4,'way to loud');

DELETE FROM country;
INSERT INTO country  VALUES
(1,'New Zealand','Pacific'),
(2,'Australia','Pacific'),
(3,'USA','North America'),
(4,'Japan','Asia');

DELETE FROM award;
INSERT INTO award  VALUES
(1,'Bronze Cross'),
(2,'Silver Cross'),
(3,'Gold Cross'),
(4,'Platinum Ball Gown');

DELETE FROM rank;
INSERT INTO rank VALUES
(5,1000,'Private',1,2),
(3,50000,'Corporal',1,3),
(2,10000,'General',1,2),
(4,2,'Captian',1,4),
(1,13,'The dog doo stuck to my shoe...',3,1);


DELETE FROM weapon;
INSERT INTO weapon VALUES
(1,'The almighty mofo','321.3',1),
(2,'AK47',123.12,1),
(3,'MP5',250,2),
(4,'M4A1',100,2);

DELETE FROM awards; 
INSERT INTO awards VALUES
(4,1),
(4,2),
(2,2),
(1,2);

