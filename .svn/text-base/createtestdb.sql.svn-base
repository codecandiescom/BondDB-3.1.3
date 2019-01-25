\c template1
drop database testbond ;
create database testbond ;
\c testbond

CREATE SEQUENCE "rank_id_seq" start 1 increment 1 maxvalue 2147483647 minvalue 1  cache 1 ;


CREATE TABLE "person" (
	"id" character(32) NOT NULL,
	"firstname" character(32),
	"lastname" character(32),
	"businesstitle" character(32),
	"email" character(64),
	"password" character(32),
	"contactdetails" character(32),
	"contract" character(32) default 'no contract',
	"type" character(1),
	Constraint "person_pkey" Primary Key ("id")
) WITH OIDS;

CREATE SEQUENCE "persontype_id_seq" start 1 increment 1 maxvalue 2147483647 minvalue 1  cache 1 ;

CREATE TABLE "persontype" (
	"id" integer DEFAULT nextval('"persontype_id_seq"'::text) NOT NULL,
	"name" text
) WITH OIDS;

CREATE SEQUENCE "country_id_seq" start 1 increment 1 maxvalue 2147483647 minvalue 1  cache 1 ;

CREATE TABLE "country" (
	"id" integer NOT NULL,
	"name" text NOT NULL,
	"continent" character varying(50),
	Constraint "country_pkey" Primary Key ("id")
) WITH OIDS;

CREATE TABLE "award" (
	"id" integer NOT NULL,
	"description" text NOT NULL,
	Constraint "award_pkey" Primary Key ("id")
);

CREATE TABLE "rank" (
	"id" integer NOT NULL,
	"pay" double precision,
	"description" text NOT NULL,
	"countryid" integer NOT NULL,
	"insuranceid" integer NOT NULL,
	Constraint "rank_pkey" Primary Key ("id")
) WITH OIDS;

CREATE TABLE "weapon" (
	"id" integer NOT NULL,
	"name" text NOT NULL,
	"damage" double precision,
	"soldierid" integer NOT NULL,
	Constraint "weapon_pkey" Primary Key ("id")
) WITH OIDS;

CREATE TABLE "awards" (
	"awardid" integer NOT NULL,
	"soldierid" integer NOT NULL,
	Constraint "awards_pkey" Primary Key ("awardid", "soldierid")
) WITH OIDS;

CREATE TABLE "insurance" (
	"id" integer NOT NULL,
	"value" integer NOT NULL,
	"payments" text NOT NULL,
	"rankid" integer NOT NULL,
	"countryid" integer NOT NULL,
	Constraint "insurance_pkey" Primary Key ("id")
) WITH OIDS;

CREATE TABLE "soldier" (
	"id" serial primary key,
	"name" character(30) NOT NULL,
	"dob" date default date(now()),
	"location" text,
	"kills" integer,
	"active" boolean default 'f' NOT NULL,
	"rankid" integer NOT NULL,
	"countryid" integer NOT NULL,
	"nihil" boolean
) WITH OIDS;

COMMENT ON COLUMN "soldier"."kills" IS '1;rank.pay';

CREATE SEQUENCE "tmptoday_id_seq" start 1 increment 1 maxvalue 2147483647 minvalue 1  cache 1 ;

COPY "person"  FROM stdin;
2                               	andru                           	hill                            	ceo                             	andrew@treshna.com                                              	\N	\N	\N	3
5                               	marika                          	hill                            	                                	marika@treshna.com                                              	                                	                                	                                	2
4                               	hill                            	                                	marika@treshna.com              	                                                                	                                	                                	                                	1
6                               	marika                          	hill                            	                                	marika@treshna.com                                              	                                	                                	                                	2
7                               	marika                          	hill                            	                                	marika@treshna.com                                              	                                	                                	                                	1
\.

COPY "persontype"  FROM stdin;
1	quiet
2	frank
3	to loud
4	way to loud
\.

COPY "country"  FROM stdin;
1	'New Zealand'	'Pacific'
2	'Australia'	'Pacific'
3	'USA'	'North America'
4	'Japan'	'Asia'
\.

COPY "award"  FROM stdin;
1	'Bronze Cross'
2	'Silver Cross'
3	'Gold Cross'
4	'Platinum Ball Gown'
\.

COPY "rank"  FROM stdin;
5	1000	Private	1	2
3	50000	Corporal	1	3
2	10000	General	1	2
4	2	Captian	1	4
1	13	The dog doo stuck to my shoe...	3	1
\.

COPY "weapon"  FROM stdin;
1	'The almighty mofo'	321.3	1
2	'AK47'	123.12	1
3	'MP5'	250	2
4	'M4A1'	100	2
\.

COPY "awards"  FROM stdin;
4	1
4	2
2	2
1	2
\.

COPY "insurance"  FROM stdin;
1	1000	'Monthly'	1	1
2	2500	'Daily'	2	1
3	5000	'Hourly'	3	1
4	999999	'Every second baby!'	4	1
\.

COPY "soldier"  FROM stdin;
2	Liam                          	1981-09-09	'Liam has left the building'	12000	t	3	1	\N
3	Avis                          	1982-02-12	Christchurch	\N	f	2	1	\N
4	Baz                           	1980-02-21	chch	13	t	1	3	t
1	Andru                         	1977-07-04	City	2	t	4	1	\N
\.

CREATE UNIQUE INDEX "persontype_id_key" on "persontype" using btree ( "id" "int4_ops" );

CREATE CONSTRAINT TRIGGER "ref_country" AFTER INSERT OR UPDATE ON "rank"  FROM "country" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ref_country', 'rank', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_country" AFTER DELETE ON "country"  FROM "rank" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_del" ('ref_country', 'rank', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_country" AFTER UPDATE ON "country"  FROM "rank" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_upd" ('ref_country', 'rank', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_award" AFTER INSERT OR UPDATE ON "awards"  FROM "award" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ref_award', 'awards', 'award', 'UNSPECIFIED', 'awardid', 'id');

CREATE CONSTRAINT TRIGGER "ref_award" AFTER DELETE ON "award"  FROM "awards" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_del" ('ref_award', 'awards', 'award', 'UNSPECIFIED', 'awardid', 'id');

CREATE CONSTRAINT TRIGGER "ref_award" AFTER UPDATE ON "award"  FROM "awards" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_upd" ('ref_award', 'awards', 'award', 'UNSPECIFIED', 'awardid', 'id');

CREATE CONSTRAINT TRIGGER "ref_rank" AFTER INSERT OR UPDATE ON "insurance"  FROM "rank" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ref_rank', 'insurance', 'rank', 'UNSPECIFIED', 'rankid', 'id');

CREATE CONSTRAINT TRIGGER "ref_rank" AFTER DELETE ON "rank"  FROM "insurance" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_del" ('ref_rank', 'insurance', 'rank', 'UNSPECIFIED', 'rankid', 'id');

CREATE CONSTRAINT TRIGGER "ref_rank" AFTER UPDATE ON "rank"  FROM "insurance" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_upd" ('ref_rank', 'insurance', 'rank', 'UNSPECIFIED', 'rankid', 'id');

CREATE CONSTRAINT TRIGGER "ref_country" AFTER INSERT OR UPDATE ON "insurance"  FROM "country" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ref_country', 'insurance', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_country" AFTER DELETE ON "country"  FROM "insurance" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_del" ('ref_country', 'insurance', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_country" AFTER UPDATE ON "country"  FROM "insurance" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_upd" ('ref_country', 'insurance', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_insurance" AFTER INSERT OR UPDATE ON "rank"  FROM "insurance" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ref_insurance', 'rank', 'insurance', 'UNSPECIFIED', 'insuranceid', 'id');

CREATE CONSTRAINT TRIGGER "ref_insurance" AFTER DELETE ON "insurance"  FROM "rank" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_del" ('ref_insurance', 'rank', 'insurance', 'UNSPECIFIED', 'insuranceid', 'id');

CREATE CONSTRAINT TRIGGER "ref_insurance" AFTER UPDATE ON "insurance"  FROM "rank" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_upd" ('ref_insurance', 'rank', 'insurance', 'UNSPECIFIED', 'insuranceid', 'id');

CREATE CONSTRAINT TRIGGER "ref_rank" AFTER INSERT OR UPDATE ON "soldier"  FROM "rank" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ref_rank', 'soldier', 'rank', 'UNSPECIFIED', 'rankid', 'id');

CREATE CONSTRAINT TRIGGER "ref_rank" AFTER DELETE ON "rank"  FROM "soldier" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_del" ('ref_rank', 'soldier', 'rank', 'UNSPECIFIED', 'rankid', 'id');

CREATE CONSTRAINT TRIGGER "ref_rank" AFTER UPDATE ON "rank"  FROM "soldier" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_upd" ('ref_rank', 'soldier', 'rank', 'UNSPECIFIED', 'rankid', 'id');

CREATE CONSTRAINT TRIGGER "ref_country" AFTER INSERT OR UPDATE ON "soldier"  FROM "country" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ref_country', 'soldier', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_country" AFTER DELETE ON "country"  FROM "soldier" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_del" ('ref_country', 'soldier', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_country" AFTER UPDATE ON "country"  FROM "soldier" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_upd" ('ref_country', 'soldier', 'country', 'UNSPECIFIED', 'countryid', 'id');

CREATE CONSTRAINT TRIGGER "ref_soldier" AFTER INSERT OR UPDATE ON "weapon"  FROM "soldier" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ref_soldier', 'weapon', 'soldier', 'UNSPECIFIED', 'soldierid', 'id');

CREATE CONSTRAINT TRIGGER "ref_soldier" AFTER DELETE ON "soldier"  FROM "weapon" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_del" ('ref_soldier', 'weapon', 'soldier', 'UNSPECIFIED', 'soldierid', 'id');

CREATE CONSTRAINT TRIGGER "ref_soldier" AFTER UPDATE ON "soldier"  FROM "weapon" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_noaction_upd" ('ref_soldier', 'weapon', 'soldier', 'UNSPECIFIED', 'soldierid', 'id');

SELECT setval ('"rank_id_seq"', 1, 'f');

SELECT setval ('"soldier_id_seq"', 1, 'f');

SELECT setval ('"persontype_id_seq"', 19, 't');

SELECT setval ('"country_id_seq"', 1, 'f');

SELECT setval ('"tmptoday_id_seq"', 2, 't');
