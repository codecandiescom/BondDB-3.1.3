#!/usr/bin/perl
#
use Mail::Sendmail 0.75; # doesn't work with v. 0.74!

$html = <<END_HTML;
END_HTML

%mail = (
	SMTP => 'loki.localnet',
	from => 'andru@treshna.com',
	to => ($ARGV[0] eq '') ? 'andru@treshna.com' : $ARGV[0],
	subject => ($ARGV[1] eq '') ? 'BOND DB Report' : $ARGV[1],
	'content-type' => 'text/html; charset="iso-8859-1"',
);

while (<STDIN>) {
	$html .= $_;
}

$mail{body} = <<END_OF_BODY;
	<html>$html</html>
END_OF_BODY


sendmail(%mail) || print "Error: $Mail::Sendmail::error\n";

