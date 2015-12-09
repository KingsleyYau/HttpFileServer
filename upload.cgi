#!/usr/bin/perl
#
# Copyright (C) 2015 KK
# upload file cgi
#
# Created on: 2015/12/2
# Author: Max.Chiu
# Email: Kingsleyyau@gmail.com
#

use CGI;
use File::Spec;
use File::Path;

$path_curf = File::Spec->rel2abs(__FILE__);
($vol, $dirs, $script) = File::Spec->splitpath($path_curf);

# 返回json
$json_send = "{\"ret\":realret,\"data\":{\"filepath\":\"realfile\"}}";

$cgi = CGI->new;

# 获取设备Id
$device_id = $cgi->param('device_id');
if( length $device_id == 0 ) {
	$json_send =~ s/realfile//g;
	$json_send =~ s/realret/0/g;
	
	print $cgi->header, 
	$json_send;	

 	die;
}

# 获取上传文件名和目录
# 上传目录
$upload_dir = $cgi->param('upload_dir');

# 上传文件
$upload_filename = $cgi->param('upload_file');
$upload_filename =~ s/\\/\//g;
$upload_filename =~ s/://g;

# 只获取文件名
@filename_array = split(/\//, $upload_filename); 
$filename = $filename_array[$#filename_array];

# 获取全部路径
#$filename = $upload_filename;

# 上传文件相对路径
$path = "/upload/";
$path .= $device_id;
$path .= "/";
$path .= $upload_dir;
$path .= "/";
$path .= $filename;
$path =~ s/\/\//\//g;

# 上传文件绝对路径
$file = $dirs;
$file .= $path;

# 创建上传目录
$dirindex = rindex($file, "\/");
if( $dirindex != -1 ) {
	$dirpath = substr($file, 0, $dirindex);
	mkpath($dirpath);
}

# 获取上传文件
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
	      $json_send;
	#      $cgi->h1(' has been uploaded'),
	#      $cgi->end_html;
} else {

	$json_send =~ s/realfile//g;
	$json_send =~ s/realret/0/g;
	
	print $cgi->header,
	#      $cgi->start_html('uploaded'),
	#      $cgi->h1('The file '),
	      $json_send;
	#      $cgi->h1(' upload failed!'),
	#      $cgi->end_html;
}
