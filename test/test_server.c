/**
 * Test Server Program.
 * This program listens on a port and establishes TCP link with the
 * connecting process. The command line is as follows:
 * <pre>
 * test_server -p &lt;port number&gt; -h
 * </pre>
 * The command line arguments are as follows:
 * <ul>
 * <li><b>-h</b> Print help and quit.
 * <li><b>-p</b> Specify the port to listen on.
 * </li>
 * The following commands can be sent to the server using send_command:
 * <ul>
 * <li><b>abort</b> Abort a wait operation.
 * <li><b>shutdown</b> Stop the server.
 * <li><b>wait &lt;n&gt;</b> Wait for n seconds.
 * </ul>
 * @see send_command.html
 */

/**
 * This hash define is needed before including source files give us POSIX.4/IEEE1003.1b-1993 prototypes
 * for time.
 */
#define _POSIX_SOURCE 1

/**
 * This hash define is needed before including source files give us POSIX.4/IEEE1003.1b-1993 prototypes
 * for time.
 */
#define _POSIX_C_SOURCE 199309L

#include <errno.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fitsio.h"

#include "command_server.h"

/* internal functions */
static void Test_Server_Connection_Callback(Command_Server_Handle_T connection_handle);
static void Send_Reply(Command_Server_Handle_T connection_handle,char *reply_message);
static int Send_Fits_Reply(Command_Server_Handle_T connection_handle);
static int Save_Buffer(char *filename,void *buffer,size_t buffer_length);
static void Help(void);


/* internal variables */

/**
 * Abort flag to abort a wait operation.
 */
static int Abort = FALSE;

/**
 * The server context to use for this server.
 */
static Command_Server_Server_Context_T Server_Context = NULL;

/**
 * Main program.
 * @see #Help
 */
int main(int argc, char* argv[])
{
	unsigned short port;
	int c;
	extern char *optarg;
	int retval;

	port = 0;
	while((c = getopt(argc, argv, "hmp:")) != EOF)
	{
		switch(c)
		{
			case 'h':
				Help();
				exit(0);
				break;
			case 'p':
				port = atoi(optarg);
				if(port <= 0)
				{
					fprintf(stderr, "port number too low (%d).\n", port);
					return 1;
				}
				break;
			default:
				fprintf(stderr, "unknown flag -%c\n", (char)c);
				Help();
				return 2;
		}
	}
	if(port == 0)
	{
		Help();
		return 3;
	}
	/* setup logging */
	Command_Server_Set_Log_Handler_Function(Command_Server_Log_Handler_Stdout);
	Command_Server_Set_Log_Filter_Function(Command_Server_Log_Filter_Level_Bitwise);
	Command_Server_Set_Log_Filter_Level(COMMAND_SERVER_LOG_BIT_GENERAL);
	/* start server */
	fprintf(stdout, "Starting multi-threaded server on port %hu.\n",port);
	retval = Command_Server_Start_Server(&port,Test_Server_Connection_Callback,&Server_Context);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 4;
	}
	fprintf(stdout, "Server finished\n");
	return 0;
}/* main */

/**
 * Function invoked in each thread.
 * @param connection_handle Globus_io connection handle for this thread.
 * @see #Abort
 * @see #Send_Reply
 */
void Test_Server_Connection_Callback(Command_Server_Handle_T connection_handle)
{
	char *client_message = NULL;
	int retval;
	int seconds,i;

	/* get message from client */
	retval = Command_Server_Read_Message(connection_handle, &client_message);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return;
	}
	printf("test_server: received '%s'\n", client_message);
	/* do something with message */
	if(strcmp(client_message, "abort") == 0)
	{
		printf("server: abort detected: setting abort flag.\n");
		/* should use mutex etc */
		Abort = TRUE;
		Send_Reply(connection_handle, "ok");
	}
	else if(strcmp(client_message, "getfits") == 0)
	{
		printf("server: getfits detected.\n");
		Send_Fits_Reply(connection_handle);
	}
	else if(strcmp(client_message, "help") == 0)
	{
		printf("server: help detected.\n");
		Send_Reply(connection_handle, "help:\n"
			   "\tabort\n"
			   "\tgetfits\n"
			   "\thelp\n"
			   "\tshutdown\n"
			   "\twait <n>\n");
	}
	else if(strcmp(client_message, "shutdown") == 0)
	{
		printf("server: shutdown detected:about to stop.\n");
		Send_Reply(connection_handle, "ok");
		retval = Command_Server_Close_Server(&Server_Context);
		if(retval == FALSE)
			Command_Server_Error();
	}
	else if(strncmp(client_message, "wait", strlen("wait")) == 0)
	{
		printf("server: wait detected.\n");
		seconds = atoi(client_message + strlen("wait"));
		printf("server: waiting for %d seconds:type abort to abort this "
			"thread.\n", seconds);
		Abort = FALSE;
		i = 0;
		while((i < seconds)&&(Abort == FALSE))
		{
			sleep(1);
			i++;
		}
		if(Abort)
		{
			printf("server: wait aborted.\n");
			Send_Reply(connection_handle, "aborted");
		}
		else
		{
			printf("server: wait completed.\n");
			Send_Reply(connection_handle, "ok");
		}
	}
	else
	{
		printf("test_server: message unknown: '%s'\n", client_message);
		Send_Reply(connection_handle, "failed message unknown");
	}
	/* free message */
	free(client_message);
}

/**
 * Send a message back to the client.
 * @param connection_handle Globus_io connection handle for this thread.
 * @param reply_message The message to send.
 */
static void Send_Reply(Command_Server_Handle_T connection_handle,char *reply_message)
{
	int retval;

	/* send something back to the client */
	printf("server: about to send '%s'\n", reply_message);
	retval = Command_Server_Write_Message(connection_handle, reply_message);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return;
	}
	printf("server: sent '%s'\n", reply_message);
}

/**
 * Send a binary FITS message back to the client.
 * @param connection_handle Globus_io connection handle for this thread.
 * @return The routine returns TRUE on success and FALSE on failure.
 */
static int Send_Fits_Reply(Command_Server_Handle_T connection_handle)
{
	fitsfile *fits_fp = NULL;
	void *buffer = NULL;
	int retval,cfitsio_status,i,j,index;
	size_t buffer_length = 0;
	long axes[ 2 ];
	unsigned short row_data[1024];
	char reply_buff[16];

	buffer_length = 288000;
	buffer = (void*)malloc(buffer_length);
	if(buffer == NULL)
	{
		Send_Reply(connection_handle,"Send_Fits_Reply:failed to allocate buffer.");
		fprintf(stderr,"Send_Fits_Reply:failed to allocate buffer (%d).\n",buffer_length);
		return FALSE;
	}
	retval = 0;
	cfitsio_status = 0;
	/* NB needs at least v2.037 of the cfitsio library */
#ifdef GETFITS_DEBUG
	retval = fits_create_file(&fits_fp,"test_server_getfits.fits",&cfitsio_status);
#else
	retval = fits_create_memfile(&fits_fp,&buffer,&buffer_length,288000,realloc,&cfitsio_status);
#endif
	if(retval)
	{
		Send_Reply(connection_handle,"Send_Fits_Reply : Creating FITS memfile failed.");
		fprintf(stderr,"Send_Fits_Reply : Creating FITS memfile failed.\n");
		fits_report_error(stderr,cfitsio_status);
		return FALSE;
	}
	/* create image block */
	axes[0] = 1024;
	axes[1] = 1024;
	retval = fits_create_img(fits_fp,USHORT_IMG,2,axes,&cfitsio_status);
	if(retval)
	{
		Send_Reply(connection_handle,"Send_Fits_Reply : Creating FITS image failed.");
		fprintf(stderr,"Send_Fits_Reply : Creating FITS image failed.\n");
		fits_report_error(stderr,cfitsio_status);
		if(buffer != NULL)
			free(buffer);
		return FALSE;
	}
	/* "read out" the fake data into the fits file in memory */
	index = 0;
	for(i=0;i<1024;i++)
	{
		for(j=0;j<1024;j++)
		{
			row_data[j] = (unsigned short)index;
			index = (index+1)%65535;
		}
		fprintf(stdout,"server: fits_write_img(%d=%d,%d,row_data[]=%hu,%hu...%hu)\n",i,(i*1024)+1,1024,
			row_data[0],
			row_data[1],row_data[1023]);
		retval = fits_write_img(fits_fp,TUSHORT,(i*1024)+1,1024,row_data,&cfitsio_status);
		if(retval)
		{
			Send_Reply(connection_handle,"Send_Fits_Reply : Writing FITS image row failed.");
			fprintf(stderr,"Send_Fits_Reply : Writing FITS image row failed.\n");
			fits_report_error(stderr,cfitsio_status);
			if(buffer != NULL)
				free(buffer);
			return FALSE;
		}
	}
	/* ensure data we have written is in the actual data buffer, not CFITSIO's internal buffers */
	/* This is an alternative to closing the CFITSIO FITS file
	retval = fits_flush_file(fits_fp,&cfitsio_status);
	if(retval)
	{
		Send_Reply(connection_handle,"Send_Fits_Reply : Flushing FITS file failed.");
		fprintf(stderr,"Send_Fits_Reply : Flushing FITS file failed.\n");
		fits_report_error(stderr,cfitsio_status);
		if(buffer != NULL)
			free(buffer);
		return FALSE;
	}
	*/
	/* ensure data we have written is in the actual data buffer, not CFITSIO's internal buffers */
	/* closing the file ensures this. */ 
	retval = fits_close_file(fits_fp,&cfitsio_status);
	if(retval)
	{
		Send_Reply(connection_handle,"Send_Fits_Reply : Closing FITS file failed.");
		fprintf(stderr,"Send_Fits_Reply : Closing FITS file failed.\n");
		fits_report_error(stderr,cfitsio_status);
		if(buffer != NULL)
			free(buffer);
		return FALSE;
	}
#ifdef GETFITS_DEBUG
	Send_Reply(connection_handle,"Send_Fits_Reply:test_server compiled with GETFITS_DEBUG, test file written by server to test_server_getfits.fits.");
#else
	/* send a text message saying binary FITS image to follow. */
	sprintf(reply_buff,"0 %ld",buffer_length);
	Send_Reply(connection_handle,reply_buff);
	/* send image back to the client */
	fprintf(stdout,"server: about to send binary message of length '%d'\n",buffer_length);
	retval = Command_Server_Write_Binary_Message(connection_handle,buffer,buffer_length);
	if(retval == FALSE)
	{
		Command_Server_Error();
		if(buffer != NULL)
			free(buffer);
		return FALSE;
	}
#endif
#ifdef GETFITS_DEBUG2
	Save_Buffer("test_server_getfits.fits",buffer,buffer_length);
#endif
	if(buffer != NULL)
		free(buffer);
	fprintf(stdout,"server: sent binary message of length '%d'\n",buffer_length);
	return TRUE;
}

/**
 * Save the buffer to the specified filename.
 */
static int Save_Buffer(char *filename,void *buffer,size_t buffer_length)
{
	FILE *fp = NULL;
	int retval,my_errno;

	fp = fopen(filename,"wb");
	if(fp == NULL)
	{
		my_errno = errno;
		fprintf(stderr,"Save_Buffer: Failed to open output filename '%s' (%d).\n",filename,my_errno);
		return FALSE;
	}
	retval = fwrite(buffer,sizeof(char),buffer_length,fp);
	if(retval != buffer_length)
	{
		fclose(fp);
		fprintf(stderr,"Save_Buffer: Failed to write output (%d of %d).\n",retval,buffer_length);
		return FALSE;
	}
	retval = fclose(fp);
	if(retval != 0)
	{
		my_errno = errno;
		fprintf(stderr,"Save_Buffer: Failed to close output filename '%s' (%d).\n",filename,my_errno);
		return FALSE;
	}
	return TRUE;
}

/**
 * Help routine.
 */
static void Help(void)
{
	fprintf(stdout, "test_server help:\n");
	fprintf(stdout, "test_server [-h][-m] -p <port number>\n");
	fprintf(stdout, "-h prints this help message.\n");
	fprintf(stdout, "-m runs a mono-threaded server (the default is "
		 "multi-threaded).\n");
	fprintf(stdout, "-p specifies the port to listen for connections on.\n");  
}
/*
** $log$
*/
