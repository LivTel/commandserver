#ifndef COMMAND_SERVER_H
#define COMMAND_SERVER_H
/* hash defines */
#ifndef TRUE
/**
 * Truth value.
 */
#define TRUE    (1)
#endif

#ifndef FALSE
/**
 * False value.
 */
#define FALSE    (0)
#endif

/**
 * The number of nanoseconds in one millisecond. A struct timespec has fields in nanoseconds.
 */
#define COMMAND_SERVER_ONE_MILLISECOND_NS	(1000000)

/* macros */
#ifndef max
/**
 * Return maximum of two inputs.
 */
#define max(A,B)                         ((A) > (B) ? (A) : (B))
#endif

#ifndef min
/**
 * Return minimum of two inputs.
 */
#define min(A,B)                         ((A) < (B) ? (A) : (B))
#endif

/* typedefs */
/**
 * Typedef for server context pointer. The structure is not externally defined,
 * it's contents are opaque to client applications.
 * @see #Server_Context_Struct
 */
typedef struct Command_Server_Server_Context_Struct * Command_Server_Server_Context_T;
typedef struct Command_Server_Handle_Struct * Command_Server_Handle_T;

/* external functions */
extern int Command_Server_Start_Server ( unsigned short *port,void (*connection_callback)
					    (Command_Server_Handle_T connection_handle),
					    Command_Server_Server_Context_T *server_context);
extern int Command_Server_Open_Client(char *hostname,int port,Command_Server_Handle_T *handle);
extern int Command_Server_Write_Message(Command_Server_Handle_T handle,char *message);
extern int Command_Server_Read_Message(Command_Server_Handle_T handle,char **message);
extern int Command_Server_Write_Binary_Message(Command_Server_Handle_T handle,void *data_buffer,
					       size_t data_buffer_length );
extern int Command_Server_Read_Binary_Message(Command_Server_Handle_T handle,void **data_buffer,
					      size_t *data_buffer_length);
extern int Command_Server_Close_Client(Command_Server_Handle_T *handle);
extern int Command_Server_Close_Server(Command_Server_Server_Context_T *server_context);
extern void Command_Server_Error(void);
extern void Command_Server_Error_To_String(char *error_string);
extern int Command_Server_Is_Error(void);
/* logging routines */
extern void Command_Server_Log_Format(char *sub_system,char *source_filename,char *function,int level,
			       char *category,char *format,...);
extern void Command_Server_Log(char *sub_system,char *source_filename,char *function,int level,
			       char *category,char *string);
extern void Command_Server_Set_Log_Handler_Function(void (*log_fn)(char *sub_system,char *source_filename,
							char *function,int level,char *category,char *message));
extern void Command_Server_Set_Log_Filter_Function(int (*filter_fn)(char *sub_system,char *source_filename,
							char *function,int level,char *category,char *message));
extern void Command_Server_Log_Handler_Stdout(char *sub_system,char *source_filename,
					      char *function,int level,char *category,char *message);
extern void Command_Server_Set_Log_Filter_Level(int level);
extern int Command_Server_Log_Filter_Level_Absolute(char *sub_system,char *source_filename,
						    char *function,int level,char *category,char *message);
extern int Command_Server_Log_Filter_Level_Bitwise(char *sub_system,char *source_filename,
						   char *function,int level,char *category,char *message);
/*
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2006/04/12 14:28:02  cjm
 * Added min/max macros.
 *
 * Revision 1.3  2006/04/10 16:41:43  cjm
 * Added COMMAND_SERVER_ONE_MILLISECOND_NS.
 *
 * Revision 1.2  2006/04/03 13:57:13  cjm
 * Added Command_Server_Is_Error.
 *
 * Revision 1.1  2006/03/16 11:11:32  cjm
 * Initial revision
 *
 */
#endif
