#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
  char *readInput = strchr(getenv("QUERY_STRING"), '=') + 1;

  char *inputParse = strdup(readInput);




system("aws s3 --no-sign-request ls --recursive noaa-goes16/ABI-L2-CMIPF/2019/007/ | grep M3C13 > ./aws");




  //printf("%s \n",inputParse);
  printf("<!DOCTYPE html>");

  printf("<html lang='en'>");

  printf("<head>");

  printf("<meta charset='utf-8'>");
  printf("<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>");
  printf("<meta name='description' content=''>");
  printf("<meta name='author' content=''>");

  printf("<title>kleinerTP2</title>");

  printf("<!-- Bootstrap core CSS -->");
  printf("<link href='vendor/bootstrap/css/bootstrap.min.css' rel='stylesheet'>");

  printf("<meta charset='utf-8'>");
  printf("<script src='https://code.jquery.com/jquery-3.2.1.slim.min.js'></script>");
  printf("<script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.11.0/umd/popper.min.js'></script>");
  printf("<script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js'></script>");
  printf("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  printf("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css'>");
  printf("<link rel='stylesheet' href='//maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap-theme.min.css'>");
  printf("<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>");
  printf("<script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js'></script>");
  printf("<link href='//fonts.googleapis.com/css?family=Lobster&subset=latin,latin-ext' rel='stylesheet' type='text/css'>");
  printf("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css'>");
  printf("<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.1/css/bootstrap-select.css' />");
  printf("<script src='https://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js'></script>");
  printf("<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.bundle.min.js'></script>");
  printf("<script src='https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.1/js/bootstrap-select.min.js'></script>");

  printf("<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.2/css/bootstrap-select.min.css'>");

  printf("<script src='https://cdnjs.cloudflare.com/ajax/libs/bootstrap-select/1.13.2/js/bootstrap-select.min.js'></script>");
  printf("<script src='//code.jquery.com/jquery-3.3.1.min.js'></script>");
  printf("<script type='text/javascript' src='//cdnjs.cloudflare.com/ajax/libs/socket.io/2.2.0/socket.io.js'></script>");
  printf("<script src='static/js/index.js'></script>");

  printf("<link href='//maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css' rel='stylesheet' id='bootstrap-css'>");
  printf("<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>");
  printf("</head>");

  printf("<body><div id='navbar-frame'><nav class='navbar navbar-expand-lg navbar-dark bg-dark static-top'></nav></div>");
  printf("%s \n", inputParse);
  

  printf("<script src='vendor/jquery/jquery.slim.min.js'></script><script src='vendor/bootstrap/js/bootstrap.bundle.min.js'></script></body>");

  printf("<form method='GET' action='./pip.cgi'><div class='container'><div class='row'><div class='col'><div class='card'><div class='card-header'><button class='btn btn-primary' type='submit' name='boton_agregar_dispositivo' id='boton_agregar_dispositivo'>Aiuda</button></div><div class='card-body'>");

  printf("<div class='form-group row'><div class='col'><input class='form-control' name='ip' id='ip' type='text' placeholder='año'></div><div class='col'><input class='form-control' name='puerto' id='puerto' type='text' placeholder='dia'></div></div>");
  printf("</div></div></div></div></div></form><div class='embed-responsive embed-responsive-16by9'></div>");

FILE *binary;
binary = fopen("./aws", "r");
char send_buffer[1500] = {0};
while (!feof(binary))
	{
    fread(send_buffer, 1, sizeof(send_buffer) - 1, binary);
    printf("<pre>");
    printf("%s",binary);
    printf("</pre>");
}
printf("</html><script src='js/script.js'></script>");


  return 0;
}
