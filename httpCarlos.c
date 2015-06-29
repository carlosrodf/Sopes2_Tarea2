#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
 
using namespace std;

#define DAEMON_NAME "httpCarlos"

char response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Sopes2 - Tarea2</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}"
"h2 {text-align: center; color: white;}</style></head>"
"<body><h1>Sistemas Operativos 2</h1><h2>201212947 - Carlos Arturo Rodriguez Furlan</h2></body></html>\r\n";
 
int main()
{
  //DAEMON PART*************************************
  pid_t pid, sid;

  //Fork the Parent Process
  pid = fork();

  if (pid < 0) { exit(EXIT_FAILURE); }

  //We got a good pid, Close the Parent Process
  if (pid > 0) { exit(EXIT_SUCCESS); }

  //Change File Mask
  umask(0);

  //Create a new Signature Id for our child
  sid = setsid();
  if (sid < 0) { exit(EXIT_FAILURE); }

  //Change Directory
  //If we cant find the directory we exit with failure.
  if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

  //Close Standard File Descriptors
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  //DAEMON PART*************************************

  openlog ("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

  int one = 1, client_fd;
  struct sockaddr_in svr_addr, cli_addr;
  socklen_t sin_len = sizeof(cli_addr);
 
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    syslog(LOG_INFO, "Can't open socket.");
 
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
 
  int port = 1212;
  svr_addr.sin_family = AF_INET;
  svr_addr.sin_addr.s_addr = INADDR_ANY;
  svr_addr.sin_port = htons(port);
 
  if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
    close(sock);
    syslog(LOG_INFO, "Can't bind.");
  }
 
  listen(sock, 5);
  while (1) {
    client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
    syslog(LOG_INFO, "Got connection!");
 
    if (client_fd == -1) {
      syslog(LOG_INFO, "Can't accept.");
      continue;
    }
 
    write(client_fd, response, sizeof(response) - 1); /*-1:'\0'*/
    close(client_fd);
  }
  closelog ();
  return(EXIT_SUCCESS);
}