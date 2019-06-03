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

if ( !$filename )
{
print $query->header ( );
print "There was a problem uploading your photo (try a smaller file).";
exit;
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

system "sudo insmod /var/www/html/sisop2019/TP3/upload/pix_mod.ko";
system "cat modulos.html";

#print $query->header ( );
#print <<END_HTML;
#<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "DTD/xhtml1-strict.dtd">

#<html lang="en">

#<head>

#  <meta charset="utf-8">
#  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
#  <meta name="description" content="">
#  <meta name="author" content="">

#  <title>kleinerTP2</title>

#  <!-- Bootstrap core CSS -->
#  <link href="vendor/bootstrap/css/bootstrap.min.css" rel="stylesheet">

#<meta charset="utf-8">
#  <script src="https://code.jquery.com/jquery-3.2.1.slim.min.js"></script>
#  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.11.0/umd/popper.min.js"></script>
#  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js"></script>
#  <meta name="viewport" content="width=device-width, initial-scale=1">
#  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css">
#  <link rel="stylesheet" href="//maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap-theme.min.css">
#  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
#  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js"></script>
#  <link href="//fonts.googleapis.com/css?family=Lobster&subset=latin,latin-ext" rel="stylesheet" type="text/css">
#  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css">
#  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.1/css/bootstrap-select.css" />
#  <script src="https://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js"></script>
#  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.bundle.min.js"></script>
#  <script src="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.1/js/bootstrap-select.min.js"></script>

#  <!-- Latest compiled and minified CSS -->
#  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.2/css/bootstrap-select.min.css">

#  <!-- Latest compiled and minified JavaScript -->
#  <script src="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.2/js/bootstrap-select.min.js"></script>



#  <!-- (Optional) Mis script (javascript) utilizado en index -->
#  <script src="//code.jquery.com/jquery-3.3.1.min.js"></script>
#  <script type="text/javascript" src="//cdnjs.cloudflare.com/ajax/libs/socket.io/2.2.0/socket.io.js"></script>
#  <script src="static/js/index.js"></script>


#  <link href="//maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css" rel="stylesheet" id="bootstrap-css">
#<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">

#</head>

#<body>

#  <!-- Navigation -->
#<div id="navbar-frame">  
#<nav class="navbar navbar-expand-lg navbar-dark bg-dark static-top">
#  </nav>
#</div>
#  <!-- Page Content -->
#  <div class="container">
#    <div class="row">
#      <div class="col-lg-12 text-center">
#        <h1 class="mt-5">Informacion sobre el sistema embebido</h1>
#        <p class="lead">En esta seccion se muestra informacion del procesador, la memoria, el uptime, la fecha y hora actual</p>
#      </div>
#    </div>
#  </div>

#  <!-- Bootstrap core JavaScript -->
#  <script src="vendor/jquery/jquery.slim.min.js"></script>
#  <script src="vendor/bootstrap/js/bootstrap.bundle.min.js"></script>

#</body>


#<div class="container">
#<div class="card border-info" style="max-width: 100rem;">
#  <div class="card-header">

#<form action="./upload.cgi" method="post" enctype="multipart/form-data">
#    <p>Modulo a cargar:<input type="file" name="module" /></p>
#    <button class="btn btn-primary" name="Submit" type="submit">Button</button>
#</form>



#</div>
#  <div class="card-body text-info">
#    <div class="embed-responsive embed-responsive-16by9">
#    <iframe scrolling="yes" class="embed-responsive-item" src="./modulos.cgi" width="700" height="200"></iframe>
#    </div>
#  </div>
#</div>
#</div>

#</html>


#<!--adding my own JS script file-->
#<script src="js/script.js"></script>

#END_HTML

