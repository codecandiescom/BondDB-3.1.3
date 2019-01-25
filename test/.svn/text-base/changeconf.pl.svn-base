#!/usr/bin/perl
#

while (<STDIN>) {
	s/(dbname=)([^ \t\n]*)([ \t\n])/\1$ARGV[0]\3/;
	s/(host=)([^ \t\n]*)([ \t\n])/\1$ARGV[1]\3/;
	s/(user=)([^ \t\n]*)([ \t\n])/\1$ARGV[2]\3/;
	print;
}
