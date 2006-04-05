/**
 * Test program to send a "getfits" command to the test_server, and store the returned image data in a file.
 * test_getfits_command -h &lt;hostname&gt; -p &lt;port number&gt; -f &lt;FITS filename&gt;
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command_server.h"

static void help(void);

/**
 * Main program.
 */
int main(int argc, char* argv[])
{
	FILE *fp = NULL;
	extern char *optarg;
	int c,port,retval,my_errno;     
	char *hostname; 
	char *filename; 
	Command_Server_Handle_T handle;
	char *reply_string;
	void *data_buffer = NULL;
	long bytes_expected;
	size_t data_buffer_length = 0;

	port = -1;
	hostname = "localhost";
	while ((c = getopt(argc, argv, "f:h:p:")) != EOF)
	{
		switch(c)
		{
			case 'f':
				filename = strdup(optarg);
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
	if(port <= 0)
	{
		printf("provide a portnumber!\n");
		help();
		return 2;
	}
	/* setup logging */
	Command_Server_Set_Log_Handler_Function(Command_Server_Log_Handler_Stdout);
	Command_Server_Set_Log_Filter_Function(Command_Server_Log_Filter_Level_Bitwise);
	Command_Server_Set_Log_Filter_Level(COMMAND_SERVER_LOG_BIT_GENERAL);
	/* Establish a TCP connection */
	printf("trying to connect to %s:%d\n", hostname, port);
	retval = Command_Server_Open_Client(hostname, port, &handle);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 4;
	}

	fprintf(stdout,"client: about to send 'getfits' to server.\n");
	retval = Command_Server_Write_Message(handle,"getfits");
	if(retval == FALSE)
	{
		Command_Server_Error();
		Command_Server_Close_Client(&handle);
		return 5;
	}
	fprintf(stdout,"client: sent 'getfits' to server\n");

	fprintf(stdout,"client: about to get text reply...\n");
	retval = Command_Server_Read_Message(handle, &reply_string);
	if(retval == FALSE)
	{
		Command_Server_Error();
		Command_Server_Close_Client(&handle);
		return 6;
	}
	printf("client: reply: %s\n", reply_string);
	/* parse text reply, if it does not start with '0' print message and exit. */
	retval = sscanf(reply_string,"0 %ld",&bytes_expected);
	if(retval != 1)
	{
		fprintf(stderr,"client: reply was: %s.\n",reply_string);
		fprintf(stderr,"client: Server will not return FITS file:exiting.\n");
		free(reply_string);
		Command_Server_Close_Client(&handle);
		return 7;
	}
	free(reply_string);
	/* get binary data */
	data_buffer = NULL;
	data_buffer_length = 0;
	retval = Command_Server_Read_Binary_Message(handle,&data_buffer,&data_buffer_length);
	if(retval == FALSE)
	{
		Command_Server_Error();
		Command_Server_Close_Client(&handle);
		return 6;
	}
	/* and save binary data to filename */
	if(filename != NULL)
	{
		fp = fopen(filename,"wb");
		if(fp == NULL)
		{
			my_errno = errno;
			if(data_buffer != NULL)
				free(data_buffer);
			Command_Server_Close_Client(&handle);
			fprintf(stderr,"client: Failed to open output filename '%s' (%d).\n",filename,my_errno);
			return 8;
		}
		retval = fwrite(data_buffer,sizeof(char),data_buffer_length,fp);
		if(retval != data_buffer_length)
		{
			if(data_buffer != NULL)
				free(data_buffer);
			fclose(fp);
			Command_Server_Close_Client(&handle);
			fprintf(stderr,"client: Failed to write output (%d of %d).\n",retval,data_buffer_length);
			return 9;
		}
		retval = fclose(fp);
		if(retval != 0)
		{
			my_errno = errno;
			if(data_buffer != NULL)
				free(data_buffer);
			Command_Server_Close_Client(&handle);
			fprintf(stderr,"client: Failed to close output filename '%s' (%d).\n",filename,my_errno);
			return 10;
		}
	}
	if(data_buffer != NULL)
		free(data_buffer);
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
	printf("send_command -h <hostname> -p <port number> -f <FITS filename>\n");
}
