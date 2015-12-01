#!/usr/bin/perl

use CGI;
use File::Spec;

$path_curf = File::Spec->rel2abs(__FILE__);
($vol, $dirs, $script) = File::Spec->splitpath($path_curf);
    
$cgi = CGI->new;
$device_id = $cgi->param('device_id');
$upload_filename = $cgi->param('upload_file');
$upload_filename =~ s/\\/\//g;
$upload_filename =~ s/:/\//g;

#@filename_array = split(/\//, $upload_filename); 
#$filename = $filename_array[$#filename_array];
$filename = $upload_filename;

$file = $dirs;
$path = "upload/";
$file .= $path;
mkdir $file;
$file .= $device_id;
$file .= "/";
mkdir $file;
$path .= $device_id;
$path .= "/";

$path .= $filename;
$file .= $filename;
$json_send = "{ret:realret,filepath:\"realfile\"}";

$fh = $cgi->upload('upload_file');
if (defined $fh) {
	$ioh = $fh->handle;
	open (OUTFILE, '>', $file);
	while ($bytesread = $ioh->read($buffer, 1024)) {
		print OUTFILE $buffer;
	}
	close OUTFILE;
	
	$json_send =~ s/realfile/$path/g;
	$json_send =~ s/realret/1/g;
	
	print $cgi->header,
	#      $cgi->start_html('uploaded'),
	#      $cgi->h1('The file '),
	      $json_send,
	#      $cgi->h1(' has been uploaded'),
	#      $cgi->end_html;
} else {

	$json_send =~ s/realfile/\"\"/g;
	$json_send =~ s/realret/0/g;
	
	print $cgi->header,
	#      $cgi->start_html('uploaded'),
	#      $cgi->h1('The file '),
	      $json_send,
	#      $cgi->h1(' upload failed!'),
	#      $cgi->end_html;
}
