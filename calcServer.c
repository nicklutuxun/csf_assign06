#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"

#define LINEBUF_SIZE 1024

int chat_with_client(struct Calc *calc, int client_fd);

/**
 * This is the connection information for one thread.
 * Every thread will have their own ConnInfo
 * 
 * @param clientfd The file descriptor of client 
 * @param calc The pointer to the shared Calc struct
 */
struct ConnInfo {
	int clientfd;
	struct Calc *calc;
};


/**
 * The function executed when pthread_create is called
 * 
 * @param arg ConnInfo passed in
 * @return void* 
 */
void *worker(void *arg) {
	struct ConnInfo *info = arg;
	pthread_detach(pthread_self());		// Let the client threads be detached so that the server does not wait for it to complete
	chat_with_client(info->calc, info->clientfd);		// interact with the server
	close(info->clientfd);		// close the client thread
	free(info);

	return NULL;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		exit(0); 	// incorrect number of command line argument
	}

	struct Calc *calc = calc_create();		// create calc and initialize pthread mutex
	const char *port = argv[1];

	int server_fd = open_listenfd((char*) port);		// Open and return a listening socket on port for the server
	if (server_fd < 0) { return 0; } // fatal error

	int keep_going = 1;
	while (keep_going) {
		int client_fd = Accept(server_fd, NULL, NULL);		// establish client connection with the server
		if (client_fd < 0) { 
			printf("Fatal: Error accepting client connection");
			return 0;
		}		// fatal: Error accepting client connection
		
		struct ConnInfo *info = malloc(sizeof(struct ConnInfo));		// reserve memory for ConnInfo
		info->clientfd = client_fd;
		info->calc = calc;

		pthread_t thr_id;

		// create thread and execute worker function
		if (pthread_create(&thr_id, NULL, worker, info) != 0)
		{
			printf("Fatal: pthread_create failed");
			return 0;		 // fatal: pthread_create failed
		}
	}
	close(server_fd);		// close server file descriptor

	calc_destroy(calc);		// delete calc and destory pthread mutex
	return 0;
}

/**
 * Read lines of input, evaluate them as calculator expressions,
 * and (if evaluation was successful) print the result of each
 * expression.  Quit when "quit" command is received.
 * 
 * @param calc The shared Calc struct
 * @param client_fd client file descriptor
 * @return int 
 */
int chat_with_client(struct Calc *calc, int client_fd) {
	rio_t in;
	char linebuf[LINEBUF_SIZE];

	rio_readinitb(&in, client_fd);

	int done = 0;
	while (!done) {
		ssize_t n = rio_readlineb(&in, linebuf, LINEBUF_SIZE);
		if (n <= 0) {
			/* error or end of input */
			done = 1;
		} else if (strcmp(linebuf, "quit\n") == 0 || strcmp(linebuf, "quit\r\n") == 0) {
			/* quit command */
			done = 1;
		} else if (strcmp(linebuf, "shutdown\n") == 0 || strcmp(linebuf, "shutdown\r\n") == 0) {
			done = 1;
			return 0;
		}
		else {
			/* process input line */
			int result;
			if (calc_eval(calc, linebuf, &result) == 0) {
				/* expression couldn't be evaluated */
				rio_writen(client_fd, "Error\n", 6);
			} else {
				/* output result */
				int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
				if (len < LINEBUF_SIZE) {
					rio_writen(client_fd, linebuf, len);
				}
			}
		}
	}
	return 1;
}

