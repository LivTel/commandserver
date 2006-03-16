/**
 * Test program to send a command using Command_Server.
 */

/**
 * This hash define is needed before including source files give us
 * POSIX.4/IEEE1003.1b-1993 prototypes for time.
 */
#define _POSIX_SOURCE 1

/**
 * This hash define is needed before including source files give us
 * POSIX.4/IEEE1003.1b-1993 prototypes for time.
 */
#define _POSIX_C_SOURCE 199309L

/**
 * This enables the 'strdup' prototype in 'string.h', which is not enabled in
 * POSIX.
 */
#define _BSD_SOURCE    1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command_server.h"

static void help(void);

int main(int argc, char* argv[])
{
	int c;
	extern char *optarg;
	int port;     
	char *hostname; 
	char *command_string = NULL;
	Command_Server_Handle_T handle;
	char *reply_string;
	int retval;

	port = -1;
	hostname = "localhost";
	while ((c = getopt(argc, argv, "h:p:c:")) != EOF)
	{
		switch(c)
		{
			case 'c':
				command_string = strdup(optarg);
				break;
			case 'h':
				hostname = strdup(optarg);
				break;
			case 'p':
				if (c == 'p')
					port = atoi(optarg);
				break;
			default:
				printf("unknown flag -%c\n", (char)c);
				help();
				return 1;
		}
	}

	if (port <= 0)
	{
		printf("provide a portnumber!\n");
		help();
		return 2;
	}

	/* Establish a TCP connection */
	printf("trying to connect to %s:%d\n", hostname, port);
	retval = Command_Server_Open_Client(hostname, port, &handle);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 4;
	}

	printf("client: about to send '%s' to server\n", command_string);
	retval = Command_Server_Write_Message(handle, command_string);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 5;
	}
	printf("client: sent '%s' to server\n", command_string);

	printf("client: about to get reply...\n");
	retval = Command_Server_Read_Message(handle, &reply_string);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 6;
	}
	printf("client: reply: %s\n", reply_string);
	free(reply_string);

	/* close and quit*/
	retval = Command_Server_Close_Client(&handle);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 7;
	}

	return 0;
}
/* main */

static void help(void)
{
	printf("send_command help:\n");
	printf("send_command -h <hostname> -p <port number> -c \"<command string>\"\n");
}
