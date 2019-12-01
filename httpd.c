/*
 * httpd.c - a minimal HTTP server 
 * usage: httpd <port>
 */

/*
 * https://tools.ietf.org/html/rfc793 - TCP
 * https://tools.ietf.org/html/rfc8200 - IPv6 Specification
 */

/***   INCLUDES   ***/

// inet_ntoa, htonl
#include <arpa/inet.h>

// errno
#include <errno.h>

// open
#include <fcntl.h>

// gethostbyaddr
#include <netdb.h>

// FILE, fclose, fgets, fdopen, fflush, fprintf, fwrite, perror, sprintf, sscanf, stderr, stdout
#include <stdio.h>

// atoi, exit, setenv
#include <stdlib.h>

// strcat, strcmp, strcpy, strlen, strstr
#include <str.h>

// bzero, index, strcasecmp
#include <strings.h>

// mmap, munmap
#include <sys/mman.h>

// accept, bind, listen, socket
#include <sys/socket.h>

// stat
#include <sys/stat.h>

// wait
#include <sys/wait.h>

// close, dup2, execve, fork, write
#include <unistd.h>

/***   CODE   ***/

#define BUFFER_SIZE 1024

extern char **environ; /* the environment */

// Allow fast server restarts.
// https://stackoverflow.com/questions/3229860/what-is-the-meaning-of-so-reuseaddr-setsockopt-option-linux
void allow_fast_restarts(int sockfd) {
  int optval = 1;
  if (setsockopt(
      /*sockfd=*/sockfd,
      /*level=*/SOL_SOCKET,
      /*optname=*/SO_REUSEADDR, 
	    /*optval=*/(const void *)&optval,
			/*optlen=*/sizeof(int)) == -1) {
		int e = errno;
		fprintf(stderr, "error setting socket options: %d\n", e);
		exit(1);
	}
}

// returns socket file descriptor
int create_socket() {
	// create an endpoint for communication
	int sockfd = socket(
			/*domain=*/AF_INET,  // IPv4 Internet protocols ip(7)
			/*type=*/SOCK_STREAM, // sequenced, reliable, two-way byte streams
      /*protocol=*/0); // only TCP protocol supports AF_INET + SOCK_STREAM
  // TODO: Many C functions return 0 in case of success and -1 in case of
  // failure. Check with x86-64 assembler which is faster and document and prefer than.
  // What is faster:
  //  if x == 0
  //  if x == -1
  //  if x < 0
	if (sockfd == -1) {
		int e = errno;
		fprintf(stderr, "error opening a listening socket: %d\n", e);
		exit(1);
	}
  return sockfd;
}

void start_listening(int sockfd) {
  // https://stackoverflow.com/questions/10002868/what-value-of-backlog-should-i-use
  if (listen(sockfd, /*backlog=*/5) == -1) {
		int e = errno;
		fprintf(stderr, "error listening on a socket: %d\n", e);
		exit(1);
  }

}

void bind_port_to_socket(int sockfd, int port) {
  struct sockaddr_in server_address; 
	bzero((char *) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons((unsigned short)port);
  if (bind(
      /*sockfd=*/sockfd,
      /*addr=*/(struct sockaddr *) &server_address, 
	    /*addrlen=*/sizeof(server_address)) == -1) {
		int e = errno;
		fprintf(stderr, "error on binding: %d\n", e);
		exit(1);
  }
}

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
int serve_static(char* uri, FILE* stream, int connfd) {
  char filename[BUFFER_SIZE];
  char filetype[BUFFER_SIZE];
  fprintf(stdout, "Serving static content\n");
  strcpy(filename, ".");
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

// Returns 0 on success, -1 on failure
int serve_api(char* uri) {
  fprintf(stdout, "Serving API request\n");
  return 0;
}

// single-threaded loop
void start_request_processing_loop(int sockfd) {
	int connfd; // connected socket file descriptor
  struct sockaddr_in client_address;
  char *hostaddrp; /* dotted decimal host addr string */
  FILE *stream;
  char buffer[BUFFER_SIZE];     /* message buffer */
  char method[BUFFER_SIZE];  /* request method */
  char uri[BUFFER_SIZE];     /* request uri */
  char version[BUFFER_SIZE]; /* request method */
  char cgiargs[BUFFER_SIZE]; /* cgi argument list */
  char *p;               /* temporary pointer */
  int is_static;         /* static request? */
  int pid;               /* process id from fork */
  int wait_status;       /* status from wait */
  socklen_t client_length = sizeof(client_address);
  while (1) {
    // Wait for connection request. A blocking call.
    connfd = accept(sockfd, (struct sockaddr *) &client_address, &client_length);
    if (connfd == -1) {
			int e = errno;
			fprintf(stderr, "error accepting a request: %d\n", e);
			exit(1);
    }

    // Determine who sent the message
    struct hostent *hostp; /* client host info */
    // TODO: gethostbyaddr is obsolete. Replace.
    // TODO: How does gethostbyaddr signal errors?
    hostp = gethostbyaddr(
        /*addr=*/(const char *)&client_address.sin_addr.s_addr, 
			  /*len=*/sizeof(client_address.sin_addr.s_addr),
        /*type=*/ AF_INET);

    if (hostp == NULL) {
			int e = errno;
			fprintf(stderr, "error getting client host information: %d\n", e);
			exit(1);
    }

    hostaddrp = inet_ntoa(client_address.sin_addr);
    if (hostaddrp == NULL) {
			int e = errno;
			fprintf(stderr, "error on inet_ntoa: %d\n", e);
			exit(1);
    }

    /* open the child socket descriptor as a stream */
    if ((stream = fdopen(connfd, "r+")) == NULL) {
			int e = errno;
			fprintf(stderr, "error on fdopen: %d\n", e);
			exit(1);
    }

    /* get the HTTP request line */
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
      if (serve_static("/static/index.html", stream, connfd) == -1) {
        fclose(stream);
        close(connfd);
        continue;
      }
    } else if (strstr(uri, "static")) { //check that it starts with "static", not contains
      if (serve_static(uri, stream, connfd) == -1) {
        fclose(stream);
        close(connfd);
        continue;
      }
    } else {
      if (serve_api(uri) == -1) {
        fclose(stream);
        close(connfd);
        continue;
      }
    }

    /* clean up */
    fclose(stream);
    close(connfd);
  }
}

// Disables SIGPIPE caused by clients closing connections earlier
void init_signals() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGPIPE, &sa, NULL) == -1) {
    fprintf(stderr, "error: failed to disable SIGPIPE");
    exit(1);
  }
}

int main(int argc, char **argv) {
	int port; // port number to listen on
	int sockfd; // listening socket file descriptor

	// check command line args
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}

  init_signals();

  sockfd = create_socket();
  allow_fast_restarts(sockfd);

	port = atoi(argv[1]);
  bind_port_to_socket(sockfd, port);
	fprintf(stdout, "Listening on port: %d\n", port);
  start_listening(sockfd);
  start_request_processing_loop(sockfd);

  return 0;
}
