#!/bin/bash
echo "Content-type: text/html"
echo ""


echo "<head>"
echo "<meta charset='utf-8'>"
echo "<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>"
echo "<meta name='description' content=''>"
echo "<meta name='author' content=''>"
echo "<title>kleinerTP2</title>"
echo "<!-- Bootstrap core CSS -->"
echo "<link href='vendor/bootstrap/css/bootstrap.min.css' rel='stylesheet'>"
echo "<meta charset='utf-8'>"
echo "<script src='https://code.jquery.com/jquery-3.2.1.slim.min.js'></script>"
echo "<script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.11.0/umd/popper.min.js'></script>"
echo "<script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js'></script>"
echo "<meta name='viewport' content='width=device-width, initial-scale=1'>"
echo "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css'>"
echo "<link rel='stylesheet' href='//maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap-theme.min.css'>"
echo "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>" 
echo "<script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js'></script>"
echo "<link href='//fonts.googleapis.com/css?family=Lobster&subset=latin,latin-ext' rel='stylesheet' type='text/css'>"
echo "<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css'>"
echo "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.1/css/bootstrap-select.css' />"
echo "<script src='https://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js'></script>"
echo "<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.bundle.min.js'></script>"
echo "<script src='https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.1/js/bootstrap-select.min.js'></script>"
echo "<!-- Latest compiled and minified CSS -->"
echo "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.2/css/bootstrap-select.min.css'>"
echo "<!-- Latest compiled and minified JavaScript -->"
echo "<script src='https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.2/js/bootstrap-select.min.js'></script>"
echo "<!-- (Optional) Mis script (javascript) utilizado en index -->"
echo "<script src='//code.jquery.com/jquery-3.3.1.min.js'></script>"
echo "<script type='text/javascript' src='//cdnjs.cloudflare.com/ajax/libs/socket.io/2.2.0/socket.io.js'></script>"
echo "<script src='static/js/index.js'></script>"
echo "<link href='//maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css' rel='stylesheet' id='bootstrap-css'>"
echo "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>"
echo "</head>"



echo "<html><head><title>Bash as CGI"
echo "</title></head><body>"

echo ""

echo "<h1>Info de memoria. Se obtiene con 'free -m'</h1>"
echo "<pre> $(free -m) </pre>"

echo "<h1>Info de disco. Se obtiene con 'df -h':</h1>"
echo "<pre> $(df -h) </pre>"

echo "<h1>Fecha, uptime y usuarios activos. Se obtiene con 'w'</h1>"
echo "<pre> $(w) </pre>"

echo "</body></html>"



