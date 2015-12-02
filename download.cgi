#!/usr/bin/perl

use CGI;
use File::Spec;

$path_curf = File::Spec->rel2abs(__FILE__);
($vol, $dirs, $script) = File::Spec->splitpath($path_curf);

$cgi = CGI->new;
$file = $cgi->param('file');

$filepath = $dirs;
$filepath .= $file;

# 获取下载文件名
$filename = $file;
$dirindex = rindex($filepath, "\/");
if( $dirindex != -1 && (length($filepath) > $dirindex + 1) ) {
	$filename = substr($filepath, $dirindex + 1, length($filepath) - ($dirindex + 1));
}

print "Content-Type:application/x-download\n";
print "Content-Disposition: attachment; filename=$filename\n\n";

open FILE, "<$filepath" or die "can't open : $!";
binmode FILE;
local $/ = \10240;
while (<FILE>){
	print $_;
}
close FILE;