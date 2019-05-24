#!/bin/bash
echo "Content-type: text/html"
echo ""
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
