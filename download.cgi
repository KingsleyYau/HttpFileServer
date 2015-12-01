#!/usr/bin/perl

use CGI;
use File::Spec;

$path_curf = File::Spec->rel2abs(__FILE__);
($vol, $dirs, $script) = File::Spec->splitpath($path_curf);

$cgi = CGI->new;
$file = $cgi->param('file');

$filepath = $dirs;
$filepath .= $file;

print "Content-Type:application/x-download\n";
print "Content-Disposition: attachment; filename=$file\n\n";

open FILE, "<$filepath" or die "can't open : $!";
binmode FILE;
local $/ = \10240;
while (<FILE>){
	print $_;
}
close FILE;