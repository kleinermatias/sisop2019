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

FILE *pipe;
    char buffer[100];

  system("cat ./cabecera");

printf(" <hr />");
printf("<div class='container'><div class='row'>");
printf("<div class='col-sm-3 text-center'>");
printf("<i class='fas fa-memory' style='font-size:55px;color:green'></i>");

printf("<h3>");
printf("Memoria en uso: ");
pipe = popen("free | grep Mem | awk '{print $3/$2 * 100.0}'", "r");
    if (pipe == NULL)
        return -1;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        fprintf(stdout, "%s", buffer);
    pclose(pipe);
printf("%%");
printf("</h3>");

printf("</div>");
printf("<div class='col-sm-3 text-center'>");
printf("<i class='fas fa-microchip' style='font-size:55px;color:green'></i>");

printf("<h3>");
printf("Uso total de los cores: ");
pipe = popen("grep 'cpu ' /proc/stat | awk '{print ($2+$4)*100/($2+$4+$5)}'", "r");
    if (pipe == NULL)
        return -1;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        fprintf(stdout, "%s", buffer);
    pclose(pipe);
printf("</h3>");

printf("</div>");
printf("<div class='col-sm-3 text-center'>");
printf("<i class='fas fa-hourglass-half' style='font-size:55px;color:green'></i>");
printf("<h3>");
printf("Uptime: ");
pipe = popen("w | grep up | awk '{print $3}'", "r");
    if (pipe == NULL)
        return -1;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        fprintf(stdout, "%s", buffer);
    pclose(pipe);
printf("</h3>");
printf("</div>");
printf("<div class='col-sm-3 text-center'>");
printf("<i class='fas fa-calendar-alt' style='font-size:55px;color:green'></i>");
printf("<h3>");
printf("Fecha: ");
pipe = popen("date +%F", "r");
    if (pipe == NULL)
        return -1;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        fprintf(stdout, "%s", buffer);
    pclose(pipe);

printf("Hora: ");
pipe = popen("date | awk '{print $4}'", "r");
    if (pipe == NULL)
        return -1;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        fprintf(stdout, "%s", buffer);
    pclose(pipe);

printf("</h3>");
printf("</div>");
printf("</div>");
printf("</div>");


printf(" <hr />");

  //system("free -m");
//system("df -h");
  //system("w");
  printf("</body>");
  return 0;
}
