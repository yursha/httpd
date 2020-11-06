/*
 * httpd.c - a minimal HTTP server
 * usage: httpd <port>
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

#define MSGLEN_MAX 256
char msgbuf[MSGLEN_MAX] = "";

#define FAIL(msg) \
  snprintf(msgbuf, MSGLEN_MAX, "%s %s:%d %s() ERROR %s", \
           argv[0], __FILE__, __LINE__, __func__, #msg); \
  perror(msgbuf); \
  exit(1);

#define LOG1(msg, arg1) \
  snprintf(msgbuf, MSGLEN_MAX, "%s %s:%d %s() INFO ", \
           argv[0], __FILE__, __LINE__, __func__); \
  printf("%s", msgbuf); \
  printf(msg, arg1);

/*
 * cerror - returns an error message to the client
 */
void cerror(FILE *stream, char *cause, char *error_code, 
    char *shortmsg, char *longmsg) {
  fprintf(stream, "HTTP/1.1 %s %s\n", error_code, shortmsg);
  fprintf(stream, "Content-type: text/html\n");
  fprintf(stream, "\n");
  fprintf(stream, "<html><title>Error</title>");
  fprintf(stream, "<body bgcolor=""ffffff"">\n");
  fprintf(stream, "%s: %s\n", error_code, shortmsg);
  fprintf(stream, "<p>%s: %s\n", longmsg, cause);
  fprintf(stream, "<hr><em>The CodeWatch Web server</em>\n");
}

// Doesn't own `stream` or `connfd`. Doesn't close either of them.
// Returns 0 on success, -1 on failure
int serve_file(char* uri, FILE* stream, int connfd, const char* dir) {
  char filename[BUFFER_SIZE];
  char filetype[BUFFER_SIZE];
  fprintf(stdout, "Serving static content\n");
  strcpy(filename, dir);
  strcat(filename, uri);

  /* make sure the file exists */
  struct stat sbuf;
  if (stat(filename, &sbuf) < 0) {
    cerror(stream, filename, "404", "Not found", 
        "Web server couldn't find this file");
    return -1;
  }

  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpg");
  else 
    strcpy(filetype, "text/plain");

  /* print response header */
  fprintf(stream, "HTTP/1.1 200 OK\n");
  fprintf(stream, "Server: CodeWatch Web Server\n");
  fprintf(stream, "Content-length: %d\n", (int)sbuf.st_size);
  fprintf(stream, "Content-type: %s\n", filetype);
  fprintf(stream, "\r\n"); 
  fflush(stream);

  int static_file = open(
      /*pathname=*/filename,
      /*flags=*/ O_RDONLY);

  /* Use mmap to return arbitrary-sized response body */
  // Create a new mapping in the virtual address space of this process.
  char *mapped_area = mmap(
      /*starting_address=*/NULL, // let the kernel choose (at a nearby page boundary)
      /*mapping_length=*/sbuf.st_size, // requested file size
      /*memory_protection=*/PROT_READ, // readonly mapping
      /*flags=*/ MAP_PRIVATE,          // doesn't mean anything in the context of PROT_READ
      /*file_to_map=*/static_file,
      /*file_offset=*/0); // map from the beginning of the file
  size_t bytes_written = fwrite(
      /*from=*/mapped_area,
      /*bytes_in_chunk=*/1,
      /*num_chunks=*/sbuf.st_size,
      /*to=*/stream);
  if (bytes_written != sbuf.st_size) {
    fprintf(stderr, "Error: partial static file written\n");
    fflush(stderr);
    return -1;
  }
  munmap(mapped_area, sbuf.st_size);
  return 0;
}

// single-threaded loop
void start_request_processing_loop(int sockfd, const char* dir) {
  while (1) {
    struct sockaddr_in client_address;
    socklen_t length_ptr = sizeof client_address;
    int connfd = accept(sockfd, (struct sockaddr*) &client_address, &length_ptr);
    if (connfd == -1) {
      fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__);
      exit(1);
    }

    struct hostent* hostp = gethostbyaddr(&client_address.sin_addr.s_addr, 
        sizeof client_address.sin_addr.s_addr, AF_INET);
    if (hostp == NULL) {
      fprintf(stderr, "error getting client host information: %d\n", errno);
      exit(1);
    }

    char* hostaddrp = inet_ntoa(client_address.sin_addr);
    if (hostaddrp == NULL) {
      fprintf(stderr, "error on inet_ntoa: %d\n", errno);
      exit(1);
    }

    /* open the child socket descriptor as a stream */
    FILE *stream;
    if ((stream = fdopen(connfd, "r+")) == NULL) {
      fprintf(stderr, "error on fdopen: %d\n", errno);
      exit(1);
    }

    /* get the HTTP request line */
    char buffer[BUFFER_SIZE];
    char method[BUFFER_SIZE];
    char uri[BUFFER_SIZE];
    char version[BUFFER_SIZE];
    fgets(buffer, BUFFER_SIZE, stream);
    fprintf(stdout, "%s", buffer);
    sscanf(buffer, "%s %s %s\n", method, uri, version);

    /* We only support the GET method */
    if (strcasecmp(method, "GET")) {
      cerror(stream, method, "501", "Not Implemented", 
          "CodeWatch HTTP server does not implement this method");
      fclose(stream);
      close(connfd);
      continue;
    }

    /* read (and ignore) the HTTP headers */
    fgets(buffer, BUFFER_SIZE, stream);
    fprintf(stdout, "%s", buffer);
    while(strcmp(buffer, "\r\n")) {
      fgets(buffer, BUFFER_SIZE, stream);
      fprintf(stdout, "%s", buffer);
    }

    if (!strcmp("/", uri)) {
      strcpy(uri, "/index.html");
    }
    serve_file(uri, stream, connfd, dir);
    fclose(stream);
    close(connfd);
  }
}

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <port> <directory>\n", argv[0]);
    exit(1);
  }
  int port = atoi(argv[1]);
  char* dir = argv[2];

  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = SIG_IGN;
  sigemptyset(&action.sa_mask);
  if (sigaction(SIGPIPE, &action, NULL) == -1) {
    fprintf(stderr, "Error: Cannot disable SIGPIPE");
    exit(1);
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    fprintf(stderr, "Error: Cannot create a TCP/IP socket: %d\n", errno);
    exit(1);
  }

  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == -1) {
    fprintf(stderr, "Error: Cannot configure the TCP/IP socket for fast restarts: %d\n", errno);
    exit(1);
  }

  struct sockaddr_in server_address; 
  bzero(&server_address, sizeof server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons((unsigned short)port);
  if (bind(sockfd, (struct sockaddr*) &server_address, sizeof server_address) == -1) {
    FAIL("Cannot bind to a port");
  }

  if (listen(sockfd, /*backlog=*/5) == -1) {
    fprintf(stderr, "Error: Cannot start listening on a socket: %d\n", errno);
    exit(1);
  }

  LOG1("Listening on port %d\n", port);

  start_request_processing_loop(sockfd, dir);

  return 0;
}
