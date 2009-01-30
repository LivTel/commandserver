/* Command_Server source file
 * $Header: /home/cjm/cvs/commandserver/c/command_server.c,v 1.8 2009-01-30 15:38:55 cjm Exp $
 */

/**
 * Routines to support a simple one command text over socket command server.
 * @author Chris Mottram,LJMU
 * @revision $Revision: 1.8 $
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
#define _POSIX_C_SOURCE 199506L

/**
 * This hash define is needed to enable the 'pselect' prototype in
 * 'sys/select.h'.
 * The 600 refers to the new 6th edition of X Open Source.
 */
#define _XOPEN_SOURCE 600

/**
 * Define this to enable 'gethostname' prototype in 'unistd.h'.
 */
#define _BSD_SOURCE 1

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h> /* TCP_NODELAY */

#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "log_udp.h"
#include "command_server.h"


/*===========================================================================*/
/*                                                                           */
/* Internal Declarations                                                     */
/*                                                                           */
/*===========================================================================*/

/* internal hash definition */
/**
 * Length of Command_Server_Error_String.
 * @see #Command_Server_Error_String
 */
#define COMMAND_SERVER_ERROR_STRING_LENGTH 	(1024)

/**
 * This define is the length of the server socket timeout in nanoseconds.
 * This is implemented to prevent the race-condition of closing a socket and
 * accepting on that socket.
 */
#define SOCKET_TIMEOUT                           (1000000)
/**
 * Length of the binary message size are prepended to messages sent over the
 * connection.
 */
#define IO_MESSAGE_SIZE_LENGTH	                  (sizeof(long))


/**
 * Enumerated type describing the state of a server connection started in
 * Command_Server_Start_Server.
 * @see #Command_Server_Start_Server
 * @see #Command_Server_Server_Context_Struct
 */
enum COMMAND_SERVER_SERVER_STATE
{
	COMMAND_SERVER_SERVER_STATE_NOT_STARTED = 0,
	COMMAND_SERVER_SERVER_STATE_RUNNING,
	COMMAND_SERVER_SERVER_STATE_TERMINATING,
	COMMAND_SERVER_SERVER_STATE_TERMINATED
};

/* internal typedefs */
struct Command_Server_Handle_Struct
{
	int  Socket_fd;
	struct sockaddr_in Address;
};

/**
 * Typedef of the connection callback function declaration.
 * This is passed as a parameter when starting a server, and is called
 * internally in each connection thread.
 */
typedef void (*Command_Server_Server_Connection_Callback_T)(Command_Server_Handle_T connection_handle);

/**
 * Structure containing the context information for this server.
 * An instance of this structure is allocated each time a server is started.
 * The following data is stored:
 * <dl>
 * <dt>Listener_Handle</dt><dd>
 * The Command_Server_Handle_T handle used for the listener socket.
 * </dd>
 * <dt>State</dt><dd>
 * The state of the server, whether it is terminating or not
 * (COMMAND_SERVER_SERVER_STATE).
 * </dd>
 * <dt>Connection_Callback</dt><dd>
 * The user suppplied routine to call for each connection.
 * </dd>
 * </dl>
 * @see #COMMAND_SERVER_SERVER_STATE
 * @see #Server_Connection_Callback_T
 */
struct Command_Server_Server_Context_Struct
{
	Command_Server_Handle_T Listener_Handle;
	enum COMMAND_SERVER_SERVER_STATE State;
	Command_Server_Server_Connection_Callback_T Connection_Callback;
};


/**
 * Structure containing the context information for a  server connection.
 * An instance of this structure is allocated each time a connection is made to
 * the server. It is passed as the user pointer when starting the thread, and
 * is freed in Server_Connection_Thread (after saving the server context for
 * later).   The following data is stored:
 * <dl>
 * <dt>Server</dt><dd>
 * A pointer to the server context.
 * </dd>
 * <dt>Connection_Handle</dt><dd>
 * The connection handle Command_Server_Handle_T for this connection.
 * </dd>
 * </dl>
 * @see #Command_Server_Server_Context_Struct
 * @see #Command_Server_Server_Connection_Thread
 */
struct Command_Server_Server_Connection_Context_Struct
{
	struct Command_Server_Server_Context_Struct *Server;
	Command_Server_Handle_T Connection_Handle;
};

/**
 * Typedef for connection context.
 * @see #Command_Server_Server_Connection_Context_Struct
 */
typedef struct Command_Server_Server_Connection_Context_Struct Command_Server_Server_Connection_Context_T;

/**
 * Structure declaration for holding global data to the command server.
 * <dl>
 * <dt>Command_Server_Log_Handler</dt> <dd>Function pointer to the routine that will log messages passed to it.</dd>
 * <dt>Command_Server_Log_Filter</dt> <dd>Function pointer to the routine that will filter log messages passed to it.
 * 		The funtion will return TRUE if the message should be logged, and FALSE if it shouldn't.</dd>
 * <dt>Command_Server_Log_Filter_Level</dt> <dd>A globally maintained log filter level. 
 * 		This is set using Command_Server_Set_Log_Filter_Level.
 * 		Command_Server_Log_Filter_Level_Absolute and Command_Server_Log_Filter_Level_Bitwise test it against
 * 		message levels to determine whether to log messages.</dd>
 * </dl>
 * @see #Command_Server_Log
 * @see #Command_Server_Set_Log_Filter_Level
 * @see #Command_Server_Log_Filter_Level_Absolute
 * @see #Command_Server_Log_Filter_Level_Bitwise
 */
struct Command_Server_Data_Struct
{
	void (*Command_Server_Log_Handler)(char *sub_system,char *source_filename,
					   char *function,int level,char *category,char *string);
	int (*Command_Server_Log_Filter)(char *sub_system,char *source_filename,
					 char *function,int level,char *category,char *string);
	int Command_Server_Log_Filter_Level;
};

/* internal functions */
static void *Command_Server_Server_Connection_Thread(void *user_arg);
static int Write_Buffer(Command_Server_Handle_T handle,void *buffer,size_t buffer_length);
static int Read_Binary_Buffer(Command_Server_Handle_T handle,void *data_buffer,size_t data_buffer_length);


/*===========================================================================*/
/* External Declarations                                                     */
/*===========================================================================*/

/**
 * Internal error variable.
 * @see #Command_Server_Error_String
 */
int Command_Server_Error_Number = 0;

/**
 * Internal error string.
 * @see #Command_Server_Error_Number
 */
char Command_Server_Error_String[COMMAND_SERVER_ERROR_STRING_LENGTH];

/**
 * The instance of Command_Server_Data_Struct that contains local data for this module.
 * This is statically initialised to the following:
 * <dl>
 * <dt>Command_Server_Log_Handler</dt> <dd>NULL</dd>
 * <dt>Command_Server_Log_Filter</dt> <dd>NULL</dd>
 * <dt>Command_Server_Log_Filter_Level</dt> <dd>0</dd>
 * </dl>
 * @see #Command_Server_Data_Struct
 */
static struct Command_Server_Data_Struct Command_Server_Data = 
{
	NULL,NULL,0
};

/* -----------------------------------------------------------------
 * Internal functions
 * ----------------------------------------------------------------- */
static void  Get_Current_Time(char *time_string,int string_length);
static void *Command_Server_Server_Connection_Thread (void *user_arg);

/**
 * Revision Control System identifier.
 */
static const char rcsid[] = "$Id: command_server.c,v 1.8 2009-01-30 15:38:55 cjm Exp $";


/*===========================================================================*/
/* External Functions                                                        */
/*===========================================================================*/
/**
 * Routine to open a client connection.
 * @param hostname The FQDN of the host to connect to.
 * @param port The port number to connect to.
 * @param handle The handle used to distinguish communications
 * @return The routine returns TRUE on success, FALSE on failure.
 */
int Command_Server_Open_Client(char *hostname,int port,Command_Server_Handle_T *handle)
{
	char host_ip[256];
	struct hostent *host;
	int retval,i,flag;

	if(hostname == NULL)
	{
		Command_Server_Error_Number = 11;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Open_Client: hostname was NULL.");
		return(FALSE);
	}
	if(handle == NULL)
	{
		Command_Server_Error_Number = 12;
		sprintf(Command_Server_Error_String,"Command_Server_Open_Client: handle was NULL.");
		return(FALSE);
	}
	if((*handle = (Command_Server_Handle_T)(malloc(sizeof(struct Command_Server_Handle_Struct)))) == NULL)
	{
		Command_Server_Error_Number = 10;
		sprintf(Command_Server_Error_String,
			"Command_Server_Open_Client: failed allocating Command_Server_Handle_Struct");
		return(FALSE);
	}
	(*handle)->Socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if((*handle)->Socket_fd == -1)
	{
		i = errno;
#if COMMAND_SERVER_DEBUG > 0
		Command_Server_Log("command server","command_server.c","Command_Server_Open_Client",
				   LOG_VERBOSITY_INTERMEDIATE,NULL,"failed creating socket.");
#endif

		Command_Server_Error_Number = 13;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Open_Client: socket error(%s:%d,%s).",
			 hostname,port,strerror(i));
		return(FALSE);
	}
	/* disable nagle's algorithm */
	/* diddly
	flag = 1;
	retval = setsockopt((*handle)->Socket_fd,IPPROTO_TCP,TCP_NODELAY,(char *) &flag,sizeof(int));
	if(retval == -1)
	{
		i = errno;
#if COMMAND_SERVER_DEBUG > 0
		Command_Server_Log("command server","command_server.c","Command_Server_Open_Client",
				   LOG_VERBOSITY_INTERMEDIATE,NULL,"failed setting socket options.");
#endif
		Command_Server_Error_Number = 40;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Open_Client: setsockopt error(%d,%s).",i,strerror(i));
		return(FALSE);
	}
	*/
	/* get IP address from hostname */
	host = gethostbyname(hostname);
	strcpy(host_ip,inet_ntoa(*(struct in_addr *)(host->h_addr_list[0])));
	memset(&((*handle)->Address),0,sizeof(struct sockaddr_in));
	(*handle)->Address.sin_family = AF_INET;
	(*handle)->Address.sin_addr.s_addr = inet_addr(host_ip);
	(*handle)->Address.sin_port = htons(port);
#if COMMAND_SERVER_DEBUG > 0
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Open_Client",
				  LOG_VERBOSITY_INTERMEDIATE,NULL,
				  "trying to connect to %s [%s:%d]  ...  ",
				  hostname,host_ip,port);
#endif

	if(connect((*handle)->Socket_fd,(struct sockaddr *)&((*handle)->Address),
		   sizeof((*handle)->Address)) == -1)
	{
		i = errno;
#if COMMAND_SERVER_DEBUG > 0
		Command_Server_Log("command server","command_server.c","Command_Server_Open_Client",
				   LOG_VERBOSITY_INTERMEDIATE,NULL,"connect FAILED");
#endif

		Command_Server_Error_Number = 16;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Open_Client: connect error(%s [%s:%d],%s).",
			 hostname,host_ip,port,strerror(i));
		return(FALSE);
	}
#if COMMAND_SERVER_DEBUG > 0
	Command_Server_Log("command server","command_server.c","Command_Server_Open_Client",
			   LOG_VERBOSITY_INTERMEDIATE,NULL,"connect OK");
#endif
	return(TRUE);
}

/**
 * Close a client connection. Also destroys the tcpattr created in Open_Client.
 * @param handle The communications handle to close
 * @return The routine returns TRUE on success, FALSE on failure.
 * @see #Command_Server_Open_Client
 */
int Command_Server_Close_Client(Command_Server_Handle_T *handle)
{
	int i;

	if(*handle == NULL)
	{
		Command_Server_Error_Number = 14;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Close_Client: handle was NULL.");
		return(FALSE);
	}
	if(close((*handle)->Socket_fd))
	{
		i = errno;
		Command_Server_Error_Number = 15;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Close_Client: close error(%s).",strerror(i));
		return(FALSE);
	}
	free(*handle);
	*handle = NULL;
	return(TRUE);
}

/**
 * Routine to start a server listening for connections. The server starts a new
 * thread for each connection it receives.
 * <b>Note</b> The server is Multi-threaded.
 * The routine is passed the Command_Server_Handle_T of the connection. The routine
 * is called in a newly created POSIX thread.
 * <br>
 * @param port The address of an integer holding the port number. 
 * @param connection_callback The routine to be called each connection
 * @param server_context The server_context distinguishing differing servers
 * @return The routine returns TRUE on success, FALSE on failure.
 * @see #Command_Server_Close_Server
 * @see #Server_Context_T
 * @see #Server_Connection_Context_T
 * @see #Server_Connection_Callback
 */
int Command_Server_Start_Server(unsigned short *port,void (*connection_callback)
				(Command_Server_Handle_T connection_handle),
				Command_Server_Server_Context_T *server_context)
{
#if COMMAND_SERVER_DEBUG > 0
	static int n_pthreads = 0;
#endif
	int adlen,sel,perr,i;
	struct hostent *host;
	char hostname[256],host_ip[256];
	Command_Server_Server_Connection_Context_T *connection_context = NULL;
	pthread_t new_thread;
	pthread_attr_t attr;
	struct timespec select_timeout = {0,SOCKET_TIMEOUT};
	fd_set read_fds,write_fds,except_fds;

	/* check arguments */
	if(port == NULL)
	{
		Command_Server_Error_Number = 17;
		sprintf(Command_Server_Error_String,"Command_Server_Start_Server: port was NULL.");
		return(FALSE);
	}
	if(connection_callback == NULL)
	{
		Command_Server_Error_Number = 19;
		sprintf(Command_Server_Error_String,
			"Command_Server_Start_Server: connection_callback was NULL.");
		return(FALSE);
	}
	if(server_context == NULL)
	{
		Command_Server_Error_Number = 35;
		sprintf(Command_Server_Error_String,
			"Command_Server_Start_Server: server_context was NULL.");
		return(FALSE);
	}
	/* setup server context */
	(*server_context) = (Command_Server_Server_Context_T)
		(malloc(sizeof(struct Command_Server_Server_Context_Struct)));
	if((*server_context) == NULL)
	{
		Command_Server_Error_Number = 30;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Start_Server: failed to allocate server context.");
		return(FALSE);
	}
	(*server_context)->Connection_Callback = connection_callback;
	(*server_context)->State = COMMAND_SERVER_SERVER_STATE_NOT_STARTED;
	/* initialise */
#if COMMAND_SERVER_DEBUG > 0
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Start_Server",
				  LOG_VERBOSITY_TERSE,NULL,
				  "trying to listen on port %hu",(*port));
#endif
	(*server_context)->Listener_Handle = (Command_Server_Handle_T)
		(malloc(sizeof(struct Command_Server_Handle_Struct)));
	if((*server_context)->Listener_Handle == NULL)
	{
		Command_Server_Error_Number = 20;
		sprintf(Command_Server_Error_String,
			"Command_Server_Start_Server: failed to allocate server handle context.");
		free(*server_context);
		*server_context = NULL;
		return(FALSE);
	}
	(*server_context)->Listener_Handle->Socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if((*server_context)->Listener_Handle->Socket_fd == -1)
	{
		i = errno;
		Command_Server_Error_Number = 31;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Start_Server: failed to create server socket: %s\n",
			 strerror(i));
		free((*server_context)->Listener_Handle);
		free(*server_context);
		*server_context = NULL;
		return(FALSE);
	}
	/* get server hostname */
	if(gethostname(hostname,256) != 0)
	{
		Command_Server_Error_Number = 21;
		sprintf(Command_Server_Error_String,
			"Command_Server_Start_Server: failed to get hostname");
		free((*server_context)->Listener_Handle);
		free(*server_context);
		*server_context = NULL;
		return(FALSE);
	}
	/* get host details (IP) */
	if((host = gethostbyname(hostname)) == NULL)
	{
		Command_Server_Error_Number = 22;
		sprintf(Command_Server_Error_String,
			"Command_Server_Start_Server: failed to get host data for %s",hostname);
		free((*server_context)->Listener_Handle);
		free(*server_context);
		*server_context = NULL;
		return(FALSE);
	}
	strcpy(host_ip,inet_ntoa(*(struct in_addr *)(host->h_addr_list[0])));
	(*server_context)->Listener_Handle->Address.sin_family = AF_INET;
	(*server_context)->Listener_Handle->Address.sin_addr.s_addr = inet_addr(host_ip);
	(*server_context)->Listener_Handle->Address.sin_port = htons(*port);
	/* bind socket to server name */
	if(bind((*server_context)->Listener_Handle->Socket_fd,
		  (struct sockaddr *)&((*server_context)->Listener_Handle->Address),
		  sizeof((*server_context)->Listener_Handle->Address)) != 0)
	{
		i = errno;
		Command_Server_Error_Number = 27;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Start_Server: failed to bind server socket: %s\n",
			 strerror(i));
		free((*server_context)->Listener_Handle);
		free(*server_context);
		*server_context = NULL;
		return(FALSE);
	}
	/* listen for connections */
	if(listen((*server_context)->Listener_Handle->Socket_fd,10) == -1)
	{
		i = errno;
		Command_Server_Error_Number = 29;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Start_Server: listen failed(%hu,%s).",
			 *port,strerror(i));
		Command_Server_Error();
		free((*server_context)->Listener_Handle);
		free(*server_context);
		*server_context = NULL;
		return(FALSE);
	}
#if COMMAND_SERVER_DEBUG > 0
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Start_Server",
				   LOG_VERBOSITY_TERSE,NULL,"listening on port %hu",*port);
#endif
	/* accept connections */
	(*server_context)->State = COMMAND_SERVER_SERVER_STATE_RUNNING;
	while((*server_context)->State == COMMAND_SERVER_SERVER_STATE_RUNNING)
	{
		/* clear the listening socket sets */
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);
		FD_ZERO(&except_fds);
		/* set the sockets to listen for reading and errors */
		FD_SET((*server_context)->Listener_Handle->Socket_fd,&read_fds); 
		FD_SET((*server_context)->Listener_Handle->Socket_fd,&except_fds); 
		sel = pselect(((*server_context)->Listener_Handle->Socket_fd + 1),
			       &read_fds,&write_fds,&except_fds,&select_timeout,NULL);
		/*
		 * If sel = -1 an error occurred.
		 * If sel == 0 this error was because a timeout occurred.
		 */
		if(sel == -1)
		{
			i = errno;
			Command_Server_Error_Number = 37;
			sprintf(Command_Server_Error_String,
				  "Command_Server_Start_Server: failed to accept connection: %s",
				  strerror(i));
			Command_Server_Error();
			continue;
		}
		else if(sel == 0)
			continue;
		/*
		 * If this is true the server socket was closed from another thread.
		 */
		if((*server_context)->State == COMMAND_SERVER_SERVER_STATE_TERMINATING)
			continue;
		/* create connection context */
		connection_context = ((Command_Server_Server_Connection_Context_T *)
			  malloc(sizeof(struct Command_Server_Server_Connection_Context_Struct)));
		if(connection_context == NULL)
		{
			Command_Server_Error_Number = 38;
			sprintf(Command_Server_Error_String,
				  "Command_Server_Start_Server: failed to allocate connection context.");
			Command_Server_Error();
			continue;
		}
		connection_context->Server = (*server_context);
		/* malloc Conection_Handle */
		connection_context->Connection_Handle =(Command_Server_Handle_T)
			malloc(sizeof(struct Command_Server_Handle_Struct));
		if(connection_context->Connection_Handle == NULL)
		{
			Command_Server_Error_Number = 39;
			sprintf(Command_Server_Error_String,
				  "Command_Server_Start_Server: failed to allocate connection "
				  "context handle.");
			Command_Server_Error();
			continue;
		}
		/* accept connection */
		connection_context->Connection_Handle->Socket_fd =
			accept((*server_context)->Listener_Handle->Socket_fd,
				(struct sockaddr *)&(connection_context->Connection_Handle->Address),
				(socklen_t *)&adlen);
#if COMMAND_SERVER_DEBUG > 0
		Command_Server_Log_Format("command server","command_server.c","Command_Server_Start_Server",
					  LOG_VERBOSITY_INTERMEDIATE,NULL,"connection accepted: "
					  "about to create pthread %d",n_pthreads++);
#endif
		/* create the thread with detatched attrributes */
		/* these next two should really be error checked */
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		if((perr = pthread_create(&new_thread,&attr,&Command_Server_Server_Connection_Thread,
					     (void *)connection_context)) != 0)
		{
			Command_Server_Error_Number = 28;
			sprintf(Command_Server_Error_String,
				 "Command_Server_Start_Server: creating thread failed: %s",
				 strerror(perr));
			Command_Server_Error();
			continue;
		}
#if COMMAND_SERVER_DEBUG > 0
		Command_Server_Log("command server","command_server.c","Command_Server_Start_Server",
				   LOG_VERBOSITY_VERBOSE,NULL,"started thread.");
#endif
	} /* end while */
	/* free resources and shutdown */
	(*server_context)->State = COMMAND_SERVER_SERVER_STATE_TERMINATED;
	if((*server_context)->Listener_Handle)
		free((*server_context)->Listener_Handle);
	if((*server_context))
		free(*server_context);
	*server_context = NULL;
	return(TRUE);
}

/**
 * Close a server connection. 
 * @param server_context The address of the server context of the server we want to close. This is the same
 * 	address that was passed into Command_Server_Start_Server.
 * @return The routine returns TRUE on success, FALSE on failure.
 * @see #Command_Server_Start_Server
 * @see #Command_Server_Server_Context_T
 */
int Command_Server_Close_Server(Command_Server_Server_Context_T *server_context)
{
	/* check arguments */
	if(server_context == NULL)
	{
		Command_Server_Error_Number = 32;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Close_Server: server context address was NULL.");
		return(FALSE);
	}
	if(*server_context == NULL)
	{
		Command_Server_Error_Number = 36;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Close_Server: server context was NULL.");
		return(FALSE);
	}
	if((*server_context)->State != COMMAND_SERVER_SERVER_STATE_RUNNING)
	{
		Command_Server_Error_Number = 18;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Close_Server: Illegal Server State(%d). ",
			 (*server_context)->State);
		return(FALSE);
	}
	/* set quit before closing handle, so server does not throw error */
	(*server_context)->State = COMMAND_SERVER_SERVER_STATE_TERMINATING;
	/* close server listener handle */
	if(close((*server_context)->Listener_Handle->Socket_fd) == -1)
	{
		Command_Server_Error_Number = 26;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Close_Server: close error.");
		return(FALSE);
	}
	return(TRUE);
}


/**
 * Routine to write the text message to an Command_Server_IO stream represented by
 * handle. A newline is sent after the string, if it does not altready contain one.
 * Command_Server_Read_Message will read a mesage sent with this routine.
 * @param handle The IO handle used to make communications
 * @param message A NULL terminated character string, that should not be NULL.
 * @return the success state of this function call
 * @see #Command_Server_Read_Message
 * @see #COMMAND_SERVER_MESSAGE_SIZE_LENGTH
 * @see #Write_Buffer
 */
int Command_Server_Write_Message(Command_Server_Handle_T handle,char *message)
{
	char *ch_ptr = NULL;

	if(handle == NULL)
	{
		Command_Server_Error_Number = 4;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Write_Message:handle was NULL.");
		return(FALSE);
	}
	if(message == NULL)
	{
		Command_Server_Error_Number = 1;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Write_Message:message was NULL.");
		return(FALSE);
	}
	/* send message block */
#if COMMAND_SERVER_DEBUG > 5
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Write_Message",
				   LOG_VERBOSITY_VERBOSE,NULL,
				  "about to send '%.80s'... of length %ld bytes .",message,strlen(message));
#endif
	if(!Write_Buffer(handle,message,strlen(message)))
		return FALSE;
	/* check newline, if not already sent send one */
	ch_ptr = strchr(message,'\n');
	if(ch_ptr == NULL)
	{
#if COMMAND_SERVER_DEBUG > 9
		Command_Server_Log_Format("command server","command_server.c","Command_Server_Write_Message",
					  LOG_VERBOSITY_VERY_VERBOSE,NULL,
					  "about to write newline (seperately) to handle.");
#endif
		if(!Write_Buffer(handle,"\n",strlen("\n")))
			return FALSE;
	}
#if COMMAND_SERVER_DEBUG > 5
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Write_Message",
				   LOG_VERBOSITY_VERY_VERBOSE,NULL,"sent message.");
#endif
	/* free allocated memory */
	return(TRUE);
}

/**
 * Routine to read a text message from a socket stream represented by
 * handle.
 * @param handle The connection to communicate with
 * @param message The string to send
 * <b>NOTE:</b>The received message memory should be freed with: <code>free(message);</code>
 * @see #COMMAND_SERVER_MESSAGE_SIZE_LENGTH
 * @see #Command_Server_Write_Message
 * @see #Command_Server_Write_Binary_Message
 */
int Command_Server_Read_Message(Command_Server_Handle_T handle,char **message)
{
	size_t bytes_read,total_bytes_read;
	char *ch_ptr = NULL;
	char buff[256];
	int done,read_errno,i;

	/* check arguments */
	if(handle == NULL)
	{
		Command_Server_Error_Number = 5;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Read_Message: handle was NULL.");
		return(FALSE);
	}
	if(message == NULL)
	{
		Command_Server_Error_Number = 6;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Read_Message: message was NULL.");
		return(FALSE);
	}

	/* initialse message */
	*message = NULL;
	/* read actual message */
	total_bytes_read = 0;
	done = FALSE;
	while(done == FALSE)
	{
#if COMMAND_SERVER_DEBUG > 5
		Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Message",
					  LOG_VERBOSITY_VERY_VERBOSE,NULL,
					  "Reading from Socket Fd %d.",handle->Socket_fd);
#endif
		bytes_read = read(handle->Socket_fd,buff,256);
		if(bytes_read == -1)
		{
			read_errno = errno;
			Command_Server_Error_Number = 42;
			sprintf(Command_Server_Error_String,"Command_Server_Read_Message: read error(%d,%d,%s).",
				 bytes_read,total_bytes_read,strerror(read_errno));
			return(FALSE);
		}
#if COMMAND_SERVER_DEBUG > 7
		Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Message",
					  LOG_VERBOSITY_VERY_VERBOSE,NULL,"Read %d bytes.",bytes_read);
#endif
		/* allocate message buffer */
		if((*message) == NULL)
		{
#if COMMAND_SERVER_DEBUG > 7
			Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Message",
						  LOG_VERBOSITY_VERY_VERBOSE,NULL,
						  "Allocating start of buffer length %d\n",
						  (total_bytes_read + bytes_read + 1));
#endif
			*message = malloc((total_bytes_read + bytes_read + 1) * sizeof(char));		       
		}
		else
		{
#if COMMAND_SERVER_DEBUG > 7
			Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Message",
						  LOG_VERBOSITY_VERY_VERBOSE,NULL,
						  "Reallocating buffer length %d",
						  (total_bytes_read + bytes_read + 1));
#endif
			*message = realloc(*message,(total_bytes_read + bytes_read + 1) * sizeof(char));
		}
		if(*message == NULL)
		{
			i = errno;
			Command_Server_Error_Number = 9;
			sprintf(Command_Server_Error_String,
				"Command_Server_Read_Message: memory allocation error: %s",strerror(i));
			return(FALSE);
		}
#if COMMAND_SERVER_DEBUG > 8
		Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Message",
					  LOG_VERBOSITY_VERY_VERBOSE,NULL,
					  "strncpy(%p+%d,buff,%d).\n",(*message),total_bytes_read,bytes_read);
#endif
		strncpy((*message)+total_bytes_read,buff,bytes_read);
		(*message)[total_bytes_read+bytes_read] = '\0';
#if COMMAND_SERVER_DEBUG > 9
		Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Message",
					  LOG_VERBOSITY_VERY_VERBOSE,NULL,
					  "Command_Server_Read_Message: Message now %.80s...",(*message));
#endif
		/* check if EOF */
		if(bytes_read == 0)
		{
#if COMMAND_SERVER_DEBUG > 7
			Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Message",
						  LLOG_VERBOSITY_VERY_VERBOSE,NULL,
						  "Detected EOF (bytes_read == 0).");
#endif
			done = TRUE;
		}
		/* check if new-line has been read */
		if((*message) != NULL)
		{
			/* diddly
			** What happens is there is a new-line at the last character of the first read
			** we do, even though the reply is multi-line and the next read would produce more characters.
			** NB If we don't have a new-line test here, the server Read hangs forever as
			** EOF is not set.
			*/
			if((*message)[strlen((*message))-1] == '\n')
			{
#if COMMAND_SERVER_DEBUG > 7
				Command_Server_Log_Format("command server","command_server.c",
							  "Command_Server_Read_Message",
							  LOG_VERBOSITY_VERY_VERBOSE,NULL,
							  " Detected input newline at charcter %d of %d.",
							  strlen((*message))-1,total_bytes_read+bytes_read);
#endif
				/* remove ONLY last newline, others may be intentional */
				(*message)[strlen((*message))-1] = '\0';
				/* remove all CRs */
				for(i=0;i<strlen((*message));i++)
				{
					if((*message)[i] == '\r')
						(*message)[i] = '\0';
				}
				done = TRUE;
			}/* end if newline found */
			else
			{
#if COMMAND_SERVER_DEBUG > 9
				Command_Server_Log_Format("command server","command_server.c",
							  "Command_Server_Read_Message",
							  LOG_VERBOSITY_VERY_VERBOSE,NULL,
							  "Last character is message currently %c at index %ld.",
							  (*message)[strlen((*message))-1],strlen((*message))-1);
#endif
			}
		}/* end if message allocated */
		/* increment total bytes read */
		total_bytes_read += bytes_read;
	}/* end while */
	/* do something with message */
	(*message)[total_bytes_read] = '\0';
	/* Note, this next debug line is dangerous with binary data */
#if COMMAND_SERVER_DEBUG > 5
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Message",
				  LOG_VERBOSITY_VERBOSE,NULL,"received '%.80s'...",*message);
#endif
	return(TRUE);
}

/**
 * Routine to write some binary data of the specified length over the open handle.
 * @param handle The handle to write the data to.
 * @param data_buffer Pointer to the data.
 * @param data_buffer_length The number of bytes of data.
 * @return The routine returns TRUE on success, and FALSE on failure.
 * @see #Command_Server_Read_Binary_Message
 * @see #IO_MESSAGE_SIZE_LENGTH
 */
int Command_Server_Write_Binary_Message(Command_Server_Handle_T handle,void *data_buffer,
					       size_t data_buffer_length)
{
	void *message_block = NULL;
	size_t bytes_written,total_bytes_written,message_block_len,message_length;
	int i;

	/* check arguments */
	if(handle == NULL)
	{
		Command_Server_Error_Number = 2;
		sprintf(Command_Server_Error_String,"Command_Server_Write_Binary_Message: handle was NULL.");
		return(FALSE);
	}
	if(data_buffer == NULL)
	{
		Command_Server_Error_Number = 7;
		sprintf(Command_Server_Error_String,"Command_Server_Write_Binary_Message: data buffer was NULL.");
		return(FALSE);
	}
	message_block_len = data_buffer_length + IO_MESSAGE_SIZE_LENGTH;
	/* allocate message block */
	message_block = malloc(message_block_len * sizeof(char));
	if(message_block == NULL)
	{
		i = errno;
		Command_Server_Error_Number = 8;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Write_Binary_Message: memory allocation error: %s",strerror(i));
		return(FALSE);
	}
	/* setup message block */
	message_length = htonl(data_buffer_length);
	memcpy(message_block,&message_length,IO_MESSAGE_SIZE_LENGTH);
	memcpy((void *)((char *)message_block + IO_MESSAGE_SIZE_LENGTH),data_buffer,data_buffer_length);
	/* send message block */
#if COMMAND_SERVER_DEBUG > 3
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Write_Binary_Message",
				  LOG_VERBOSITY_VERY_VERBOSE,NULL,"about to send buffer of %d bytes.",
				  (data_buffer_length + IO_MESSAGE_SIZE_LENGTH) *sizeof(char));
#endif
	if(!Write_Buffer(handle,message_block,message_block_len))
		return FALSE;
#if COMMAND_SERVER_DEBUG > 3
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Write_Binary_Message",
				  LOG_VERBOSITY_VERY_VERBOSE,NULL,"sent buffer of length %d.",bytes_written);
#endif
	/* free allocated memory */
	free(message_block);
	return(TRUE);

}

/**
 * Routine to read some binary data over the open handle. The remote end of the handle should have 
 * sent the message using Command_Server_Write_Binary_Message.
 * @param handle The handle to read the data from.
 * @param data_buffer Address of a void pointer. On return from the routine, this will point to the read binary data
 *                    if the routine returns TRUE.
 * @param data_buffer_length Address of a size_t. On return from the routine, this will store the number of bytes of 
 *                    data in the data_buffer.
 * @return The routine returns TRUE on success, and FALSE on failure.
 * @see #Command_Server_Write_Binary_Message
 * @see #COMMAND_SERVER_ONE_MILLISECOND_NS
 * @see #Read_Binary_Buffer
 */
int Command_Server_Read_Binary_Message(Command_Server_Handle_T handle,void **data_buffer,
					      size_t *data_buffer_length)
{
	size_t bytes_read,total_bytes_read,message_length;
	char message_length_buffer[IO_MESSAGE_SIZE_LENGTH];
	int i;

	/* check arguments */
	if(handle == NULL)
	{
		Command_Server_Error_Number = 24;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Read_Binary_Message:handle was NULL.");
		return(FALSE);
	}
	if(data_buffer == NULL)
	{
		Command_Server_Error_Number = 25;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Read_Binary_Message:data_buffer was NULL.");
		return(FALSE);
	}
	if(data_buffer_length == NULL)
	{
		Command_Server_Error_Number = 41;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Read_Binary_Message:data_buffer_length was NULL.");
		return(FALSE);
	}
	/* initialse data_buffer */
	(*data_buffer) = NULL;
	(*data_buffer_length) = 0;
	/* read byes containing length of rest of message */
	/* should really be in a while loop, until total_types_read == IO_MESSAGE_SIZE_LENGTH */
	if(!Read_Binary_Buffer(handle,message_length_buffer,IO_MESSAGE_SIZE_LENGTH))
		return FALSE;
	/* convert to integer */
	memcpy(&message_length,message_length_buffer,IO_MESSAGE_SIZE_LENGTH);
	message_length = ntohl(message_length);
	if(message_length < 1)
	{
		Command_Server_Error_Number = 44;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Read_Binary_Message: message length error(%d bytes).",
			 message_length);
		return(FALSE);
	}
	(*data_buffer_length) = message_length;
#if COMMAND_SERVER_DEBUG > 3
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Binary_Message",
				  LOG_VERBOSITY_VERY_VERBOSE,NULL,"message length is '%d'.",message_length);
#endif
	/* allocate data_buffer buffer */
	*data_buffer = malloc((*data_buffer_length) * sizeof(char));
	if(*data_buffer == NULL)
	{
		i = errno;
		Command_Server_Error_Number = 45;
		sprintf(Command_Server_Error_String,
			 "Command_Server_Read_Binary_Message: memory allocation error(%d): %s",
			 (*data_buffer_length),strerror(i));
		return(FALSE);
	}
	/* read actual message */
	if(!Read_Binary_Buffer(handle,(*data_buffer),(*data_buffer_length)))
		return FALSE;
#if COMMAND_SERVER_DEBUG > 3
	Command_Server_Log_Format("command server","command_server.c","Command_Server_Read_Binary_Message",
				  LOG_VERBOSITY_VERY_VERBOSE,NULL,"received %d bytes of data.",message_length);
#endif
	return(TRUE);

}

/**
 * Routine to print out the error to stderr.
 * @see #Get_Current_Time
 * @see #Command_Server_Error_Number
 * @see #Command_Server_Error_String
 */
void Command_Server_Error(void)
{
	char time_string[32];

	Get_Current_Time(time_string,32);
	if(Command_Server_Error_Number == 0)
	{
		sprintf(Command_Server_Error_String,
			 "Command_Server_Error:Internal Error:Error code was zero.\n");
	}
	fprintf(stderr,"%s Command Server Error (%d) : %s\n",
		 time_string,Command_Server_Error_Number,Command_Server_Error_String);
}

/**
 * Error routine. This prints out the current values of Command_Server_Error_Number
 * and Command_Server_Error_String  in a tidy way to the passed in string.
 * If the error number was zero, a logic error is printed.
 * The error number is reset to zero in this routine.
 * @param error_string A character string to put the error string into. This buffer needs to be <b>big</b>, 
 * 	at least 1024 bytes.
 * @see #Command_Server_Error_Number
 * @see #Command_Server_Error_String
 * @see #Get_Current_Time
 */
void Command_Server_Error_To_String(char *error_string)
{
	char time_string[32];

	Get_Current_Time(time_string,32);
	/*
	 * if the error number is zero an error message has not been set up
	 * This is in itself an error as we should not be calling this routine
	 * without there being an error to display
	 */
	if(Command_Server_Error_Number == 0)
	{
		sprintf(Command_Server_Error_String,"Logic Error: No Error defined");
	}
	/* print error to the string */
	sprintf(error_string,"%s Command Server Error (%d) : %s\n",time_string,Command_Server_Error_Number,
		Command_Server_Error_String);
	/* reset error number */
	Command_Server_Error_Number = 0;
}

/**
 * Routine checking whether the command server currently has an 'active' error.
 * @return The routine returns TRUE if there is an 'active' error, otherwise FALSE.
 * @see #Command_Server_Error_Number
 */
int Command_Server_Is_Error(void)
{
	return (Command_Server_Error_Number != 0);
}

/**
 * Routine to log a message to a defined logging mechanism. This routine has an arbitary number of arguments,
 * and uses vsprintf to format them i.e. like fprintf. A temporary buffer is used to hold the created string,
 * therefore the total length of the generated string should not be longer than COMMAND_SERVER_ERROR_STRING_LENGTH.
 * Command_Server_Log is then called to handle the log message.
 * @param sub_system The sub system. Can be NULL.
 * @param source_file The source filename. Can be NULL.
 * @param function The function calling the log. Can be NULL.
 * @param level At what level is the log message (TERSE/high level or VERBOSE/low level), 
 *         a valid member of LOG_VERBOSITY.
 * @param category What sort of information is the message. Designed to be used as a filter. Can be NULL.
 * 	logging or not.
 * @param format A string, with formatting statements the same as fprintf would use to determine the type
 * 	of the following arguments.
 * @see #Command_Server_Log
 * @see #COMMAND_SERVER_ERROR_STRING_LENGTH
 */
void Command_Server_Log_Format(char *sub_system,char *source_filename,char *function,int level,
			       char *category,char *format,...)
{
	char buff[COMMAND_SERVER_ERROR_STRING_LENGTH];
	va_list ap;

/* format the arguments */
	va_start(ap,format);
	vsprintf(buff,format,ap);
	va_end(ap);
/* call the log routine to log the results */
	Command_Server_Log(sub_system,source_filename,function,level,category,buff);
}

/**
 * Routine to log a message to a defined logging mechanism. If the string or 
 * Command_Server_Data.Command_Server_Log_Handler are NULL the routine does not log the message. 
 * If the Command_Server_Data.Command_Server_Log_Filter function pointer is non-NULL, the
 * message is passed to it to determoine whether to log the message.
 * @param sub_system The sub system. Can be NULL.
 * @param source_file The source filename. Can be NULL.
 * @param function The function calling the log. Can be NULL.
 * @param level At what level is the log message (TERSE/high level or VERBOSE/low level), 
 *         a valid member of LOG_VERBOSITY.
 * @param category What sort of information is the message. Designed to be used as a filter. Can be NULL.
 * @param string The message to log.
 * @see #Command_Server_Data
 */
void Command_Server_Log(char *sub_system,char *source_filename,char *function,int level,
			char *category,char *string)
{
/* If the string is NULL, don't log. */
	if(string == NULL)
		return;
/* If there is no log handler, return */
	if(Command_Server_Data.Command_Server_Log_Handler == NULL)
		return;
/* If there's a log filter, check it returns TRUE for this message */
	if(Command_Server_Data.Command_Server_Log_Filter != NULL)
	{
		if(Command_Server_Data.Command_Server_Log_Filter(sub_system,source_filename,function,level,
								 category,string) == FALSE)
			return;
	}
/* We can log the message */
	(*Command_Server_Data.Command_Server_Log_Handler)(sub_system,source_filename,function,level,category,string);
}

/**
 * Routine to set the Command_Server_Data.Command_Server_Log_Handler used by Command_Server_Log.
 * @param log_fn A function pointer to a suitable handler.
 * @see #Command_Server_Data
 * @see #Command_Server_Log
 */
void Command_Server_Set_Log_Handler_Function(void (*log_fn)(char *sub_system,char *source_filename,
						 char *function,int level,char *category,char *string))
{
	Command_Server_Data.Command_Server_Log_Handler = log_fn;
}

/**
 * Routine to set the Command_Server_Data.Command_Server_Log_Filter used by Command_Server_Log.
 * @param log_fn A function pointer to a suitable filter function.
 * @see #Command_Server_Data
 * @see #Command_Server_Log
 */
void Command_Server_Set_Log_Filter_Function(int (*filter_fn)(char *sub_system,char *source_filename,
							     char *function,int level,char *category,char *string))
{
	Command_Server_Data.Command_Server_Log_Filter = filter_fn;
}

/**
 * A log handler to be used for the Command_Server_Data.Command_Server_Log_Handler function.
 * Just prints the message to stdout, terminated by a newline.
 * @param sub_system The sub system. Can be NULL.
 * @param source_file The source filename. Can be NULL.
 * @param function The function calling the log. Can be NULL.
 * @param level At what level is the log message (TERSE/high level or VERBOSE/low level), 
 *         a valid member of LOG_VERBOSITY.
 * @param category What sort of information is the message. Designed to be used as a filter. Can be NULL.
 * @param string The log message to be logged. 
 */
void Command_Server_Log_Handler_Stdout(char *sub_system,char *source_filename,
				       char *function,int level,char *category,char *string)
{
	if(string == NULL)
		return;
	fprintf(stdout,"%s:%s\n",function,string);
}

/**
 * Routine to set the Command_Server_Data.Command_Server_Log_Filter_Level.
 * @see #Command_Server_Data
 */
void Command_Server_Set_Log_Filter_Level(int level)
{
	Command_Server_Data.Command_Server_Log_Filter_Level = level;
}

/**
 * A log message filter routine, to be used for the Command_Server_Data.Command_Server_Log_Filter function pointer.
 * @param sub_system The sub system. Can be NULL.
 * @param source_file The source filename. Can be NULL.
 * @param function The function calling the log. Can be NULL.
 * @param level At what level is the log message (TERSE/high level or VERBOSE/low level), 
 *         a valid member of LOG_VERBOSITY.
 * @param category What sort of information is the message. Designed to be used as a filter. Can be NULL.
 * @param string The log message to be logged, not used in this filter. 
 * @return The routine returns TRUE if the level is less than or equal to the Command_Server_Data.Command_Server_Log_Filter_Level,
 * 	otherwise it returns FALSE.
 * @see #Command_Server_Data
 */
int Command_Server_Log_Filter_Level_Absolute(char *sub_system,char *source_filename,char *function,
					     int level,char *category,char *string)
{
	return (level <= Command_Server_Data.Command_Server_Log_Filter_Level);
}

/**
 * A log message filter routine, to be used for the Command_Server_Data.Command_Server_Log_Filter function pointer.
 * @param sub_system The sub system. Can be NULL.
 * @param source_file The source filename. Can be NULL.
 * @param function The function calling the log. Can be NULL.
 * @param level At what level is the log message (TERSE/high level or VERBOSE/low level), 
 *         a valid member of LOG_VERBOSITY.
 * @param category What sort of information is the message. Designed to be used as a filter. Can be NULL.
 * @param string The log message to be logged, not used in this filter. 
 * @return The routine returns TRUE if the level has bits set that are also set in the 
 * 	Command_Server_Data.Command_Server_Log_Filter_Level, otherwise it returns FALSE.
 * @see #Command_Server_Data
 */
int Command_Server_Log_Filter_Level_Bitwise(char *sub_system,char *source_filename,char *function,
					    int level,char *category,char *string)
{
	return ((level & Command_Server_Data.Command_Server_Log_Filter_Level) > 0);
}

/*===========================================================================*/
/*                                                                           */
/*	 internal function definitions                                       */
/*                                                                           */
/*===========================================================================*/

/**
 * Connection thread routine. This routine is the top level function called by
 * each new thread generated in Command_Server_Start_Server. i.e. This routine is
 * called in a different thread for each server started.
 * @param user_arg The thread specific data for this thread. In this case, this is an instance of
 * 	Server_Connection_Context_T allocatd in Command_Server_Start_Server, that must be freed in this routine.
 * @see #Server_Context_T
 * @see #Server_Connection_Context_T
 */
static void *Command_Server_Server_Connection_Thread(void *user_arg)
{
	Command_Server_Server_Context_T server_context = NULL;
	Command_Server_Server_Connection_Context_T *connection_context = NULL;
	Command_Server_Server_Connection_Callback_T connection_callback;

	connection_context = (Command_Server_Server_Connection_Context_T *)user_arg;
	server_context = connection_context->Server;
	if(server_context == NULL)
	{
		Command_Server_Error_Number = 33;
		sprintf(Command_Server_Error_String,
			"Command_Server_Server_Connection_Thread:server context was NULL.");
		Command_Server_Error();
		close(connection_context->Connection_Handle->Socket_fd);
		free(connection_context);
		return NULL;
	}
	/* Call the connection callback. */
	connection_callback = server_context->Connection_Callback;
	if(connection_callback == NULL)
	{
		Command_Server_Error_Number = 34;
		sprintf
			(Command_Server_Error_String,
			  "Command_Server_Server_Connection_Thread:connection callback was NULL.");
		Command_Server_Error();
		close(connection_context->Connection_Handle->Socket_fd);
		free(connection_context);
		return NULL;
	}
#if COMMAND_SERVER_DEBUG > 3
	Command_Server_Log("command server","command_server.c","Command_Server_Server_Connection_Thread",
			   LOG_VERBOSITY_VERBOSE,NULL,"connection callback about to be called");
#endif
	connection_callback(connection_context->Connection_Handle);
	/*
	 * server context may be freed by here,
	 * if Command_Server_Close_Server was called.
	 */
#if COMMAND_SERVER_DEBUG > 3
	Command_Server_Log("command server","command_server.c","Command_Server_Server_Connection_Thread",
			   LOG_VERBOSITY_VERBOSE,NULL,"connection callback finished.");
#endif
	close(connection_context->Connection_Handle->Socket_fd);
	/* free the connection context allocated in Command_Server_Start_Server */
	free(connection_context->Connection_Handle);
	free(connection_context);
	return NULL;
}

/**
 * Write the contents of the buffer to a socket specified by handle.
 * Calls <b>write</b> in a loop until all the bytes are written, or an error occurs.
 * @param handle The handle containing the socket to write to.
 * @param buffer A pointer to an area of memory containing something to write.
 * @param buffer_length The length of data in the buffer to be written, in bytes.
 * @return The routine returns TRUE on success, and FALSE on failure.
 * @see #Command_Server_Handle_T
 */
static int Write_Buffer(Command_Server_Handle_T handle,void *buffer,size_t buffer_length)
{
	size_t total_bytes_written,bytes_written;
	int write_errno,retval;

#if COMMAND_SERVER_DEBUG > 9
	Command_Server_Log_Format("command server","command_server.c","Write_Buffer",
				  LOG_VERBOSITY_VERY_VERBOSE,NULL,"About to write %ld bytes.",buffer_length);
#endif
	total_bytes_written = 0;
	while(total_bytes_written < buffer_length)
	{
#if COMMAND_SERVER_DEBUG > 9
		Command_Server_Log_Format("command server","command_server.c","Write_Buffer",
					  LOG_VERBOSITY_VERY_VERBOSE,NULL,
					  "Writing %ld bytes starting at offset %ld.",
					  buffer_length-total_bytes_written,total_bytes_written);
#endif
		bytes_written = write(handle->Socket_fd,buffer+total_bytes_written,
				      buffer_length-total_bytes_written);
		if(bytes_written == -1)
		{
			write_errno = errno;
			Command_Server_Error_Number = 3;
			sprintf(Command_Server_Error_String,
				"Write_Buffer: write error(%ld + %ld/%d : %s).",
				total_bytes_written,bytes_written,buffer_length,strerror(write_errno));
			return(FALSE);
		}
#if COMMAND_SERVER_DEBUG > 9
		Command_Server_Log_Format("command server","command_server.c","Write_Buffer",
					  LOG_VERBOSITY_VERY_VERBOSE,NULL,"Wrote %ld bytes.",bytes_written);
#endif
		total_bytes_written += bytes_written;
	}
	return TRUE;
}

/**
 * Read (potentially) binary data into the specified (previously allocated) buffer.
 * @param handle The socket handle.
 * @param data_buffer An already allocated or fixed buffer of at least data_buffer_length length bytes.
 *        On successful return from the routine this will contain data_buffer_length bytes of data 
 *        read from the socket.
 * @param data_buffer_length The length of the data_buffer, and also the number of bytes to read from the socket
 *        into the buffer.
 * @return The routine returns TRUE on success and FALSE on failure.
 * @see #Command_Server_Handle_T
 */
static int Read_Binary_Buffer(Command_Server_Handle_T handle,void *data_buffer,size_t data_buffer_length)
{
	size_t total_bytes_read,bytes_read;
	int read_errno;

	total_bytes_read = 0;
	while(total_bytes_read < data_buffer_length)
	{
		bytes_read = read(handle->Socket_fd,(void *)(data_buffer+total_bytes_read),
				  data_buffer_length-total_bytes_read);
		if(bytes_read == -1)
		{
			read_errno = errno;
			Command_Server_Error_Number = 43;
			sprintf(Command_Server_Error_String,"Read_Binary_Buffer:read error(%d vs %d bytes: %s).",
				total_bytes_read,data_buffer_length,strerror(read_errno));
			return(FALSE);
		}
		/* check if EOF */
		if(bytes_read == 0)
		{
			Command_Server_Error_Number = 49;
			sprintf(Command_Server_Error_String,"Read_Binary_Buffer: "
				"Detected EOF (bytes_read == 0) after %ld of %ld bytes read.",
				total_bytes_read,data_buffer_length);
			return(FALSE);
		}
#if COMMAND_SERVER_DEBUG > 9
		Command_Server_Log_Format("command server","command_server.c","Read_Binary_Buffer",
					  LOG_VERBOSITY_VERY_VERBOSE,NULL,
					  "Last read %ld bytes (starting at %ld byte).",bytes_read,total_bytes_read);
#endif
		total_bytes_read += bytes_read;
	}/* end while */
	return TRUE;
}

/**
 * Internal routine to get the current time in a string. The string is returned in the format
 * '01/01/2000 13:59:59', or the string "Unknown time" if the routine failed.
 * @param time_string The string to fill with the current time.
 * @param string_length The length of the buffer passed in. It is recommended the length is at least 20 characters.
 */
static void Get_Current_Time(char *time_string,int string_length)
{
	time_t current_time;
	struct tm *utc_time = NULL;

	if(time(&current_time) > -1)
	{
		utc_time = gmtime(&current_time);
		strftime(time_string,string_length,"%d/%m/%Y %H:%M:%S",utc_time);
	}
	else
		strncpy(time_string,"Unknown time",string_length);
}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2006/10/19 10:15:09  cjm
 * Changed COMMAND_SERVER_DEBUG to be integer based to give more granular control of logging.
 *
 * Revision 1.6  2006/06/02 13:42:46  cjm
 * Temporary fix for reading long multi-line strings.
 * Will not work if a partial read ends on a newline!
 *
 * Revision 1.5  2006/06/02 13:17:23  cjm
 * Fixed log buffer overflow when logging long text message writes.
 *
 * Revision 1.4  2006/05/08 18:31:51  cjm
 * Attempts to fix  a bug that didn't exist (dodgy alias).
 * Added a load of commented out uber-debugging, however.
 *
 * Revision 1.3  2006/04/12 14:29:56  cjm
 * Rewritten.
 * Now uses newline for end of text determination - now compatible with telnet.
 * Binary transfers use new subroutines - figured out a new strategy for
 * FITS image transfer.
 *
 * Revision 1.2  2006/04/03 13:57:19  cjm
 * Added Command_Server_Is_Error.
 *
 * Revision 1.1  2006/03/16 11:07:39  cjm
 * Initial revision
 *
 */
