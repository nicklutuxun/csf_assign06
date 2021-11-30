#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"

#define LINEBUF_SIZE 1024

int chat_with_client(struct Calc *calc, int client_fd);

int main(int argc, char **argv) {
	if (argc != 2) {
		exit(0); 	// incorrect number of command line argument
	}

	struct Calc *calc = calc_create();

	const char *port = argv[1];

	int server_fd = open_listenfd((char*) port);
	if (server_fd < 0) { return 0; } // fatal error

	int keep_going = 1;
	while (keep_going) {
		int client_fd = Accept(server_fd, NULL, NULL);
		if (client_fd > 0) {
			keep_going = chat_with_client(calc, client_fd);
			close(client_fd);
		}
	}
	close(server_fd);

	calc_destroy(calc);
	return 0;
}

int chat_with_client(struct Calc *calc, int client_fd) {
	rio_t in;
	char linebuf[LINEBUF_SIZE];

	rio_readinitb(&in, client_fd);

	/*
	 * Read lines of input, evaluate them as calculator expressions,
	 * and (if evaluation was successful) print the result of each
	 * expression.  Quit when "quit" command is received.
	 */
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
