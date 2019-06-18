#!/usr/bin/perl

use strict;
use CGI;
use CGI::Carp qw ( fatalsToBrowser );
use File::Basename;

$CGI::POST_MAX = 1024 * 5000;
my $safe_filename_characters = "a-zA-Z0-9_.-";
my $upload_dir = "./upload";

my $query = new CGI;
my $filename = $query->param("module");
my $extvalid = '.ko';

if ( !$filename )
{
	print $query->header ( );
	print "There was a problem uploading your photo (try a smaller file).";
	exit;
}
my ($ext1) = $filename =~ /((\.[^.\s]+)+)$/;

if ($ext1 ne '.ko') {
        die("No subiste un .ko wachen");
}

my ( $name, $path, $extension ) = fileparse ( $filename, '..*' );
$filename = $name . $extension;
$filename =~ tr/ /_/;
$filename =~ s/[^$safe_filename_characters]//g;

if ( $filename =~ /^([$safe_filename_characters]+)$/ )
{
	$filename = $1;
}
else
{
	die "Filename contains invalid characters";
}

my $upload_filehandle = $query->upload("module");

open ( UPLOADFILE, ">$upload_dir/$filename" ) or die "$!";
binmode UPLOADFILE;

while ( <$upload_filehandle> )
{
print UPLOADFILE;
}

close UPLOADFILE;

system "sudo insmod /var/www/html/sisop2019/TP3/upload/$filename";
system "cat modulos.html";

