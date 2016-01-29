//=======================================================
//=======================================================
// mail 1.0
//			compileable with vc++ 6.0
//			don't forget to include wsock32.lib 
//=======================================================
//=======================================================

#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <time.h>
#include <io.h>

#include "dialogsend.h"

MAILPARMS Parm;

int  i = 0;
char Buffer[TEXTBUFFERSIZE];  
char Textbuffer[TEXTBUFFERSIZE]; 

//=======================================================
//=======================================================
// showsyntax
//=======================================================
//=======================================================
void showsyntax ( void ) 
{
	printf("%s \n\n",VERSION_INFORMATION);
	printf("by Bjoern Seip for COMLINE AG (c) 1998\n");
	printf("based on smtpsnd by Sergio Steinberg (c) 1998\n\n");
	printf("Usage:   mail -m smtpserver -o source -n destination\n"); 
	printf("              [-p subject] [-f textfile]\n");
	printf("              [options] \n\n");
	printf("Options: -d   print debug information\n");
	printf("         -i   send additional file information\n");
	printf("         -a   send file as an attachment\n");
	printf("         -r   omit MIME data\n");
	printf("         -s   accept body from standard input\n");
	printf("         -q   quiet operation, no error reporting\n");
	printf("         -b   use dialog\n");
	printf("         -x   dont allow preset values to be changed in dialog\n");
	printf("         -h   print this\n\n");
}

//=======================================================
//=======================================================
// check2args
//=======================================================
//=======================================================
int check2args(int argc, int i, char *next, char *message)
{
	if (argc == (i + 1)){
		if (!Parm.optQuiet) printf("%s\n",message);
		return(0);
	}
	else {
	 if ( next[0] == '-' ) {
			if (!Parm.optQuiet) printf("%s\n",message);
			return(0);
	}
	}
	return(1);
}

//=======================================================
//=======================================================
// processcmdline  
//=======================================================
//=======================================================
int processcmdline (int argc, char *argv[])
{
	int result = 1;

	for (i=1; i<argc; i++)
	{
	 
	 if ( (argv[i][0] != '-') || (strlen(argv[i]) < 2 ) ){
	  if (!Parm.optQuiet)	printf("invalid argument %s\n",argv[i]);
		 result = 0;
	 } else

	 switch (argv[i][1])
	 {

	  case 'm':
		 if (check2args(argc, i, argv[i+1], "-m must be followed by a servername")) {
				strcpy(Parm.SMTPServer,argv[i+1]);
				i++;
		 } else result = 0;
		 break;

	  case 'n':
		 if (check2args(argc, i, argv[i+1], "-n must be followed by an address")) {
				strcpy(Parm.Destination,argv[i+1]);
				i++;
		 } else result = 0;
		 break;

	  case 'o':
		 if (check2args(argc, i, argv[i+1], "-o must be followed by an address")) {
		 		strcpy(Parm.Source,argv[i+1]);
				i++;
		 } else result = 0;
		 break;

	  case 'p':
		 if (check2args(argc, i, argv[i+1], "-p must be followed by a text argument")) {
				strcpy(Parm.Subject,argv[i+1]);
				i++;
		 } else result = 0;
		 break;

	  case 'f':
		 if (check2args(argc, i, argv[i+1], "-f must be followed by a filename")) {
				strcpy(Parm.Filename,argv[i+1]);
				i++;
				Parm.optFilename = 1;
		 } else result = 0;
		 break;

	  case 'd': 
		 Parm.optDebug  = 1;
		 break;

	  case 'a': 
		 Parm.optAttach = 1;
		 break;

	  case 'r': 
		 Parm.optNomime = 1;
		 break;

	  case 'i': 
		 Parm.optInfo   = 1;
		 break;

	  case 's': 
		 Parm.optStream = 1;
		 break;

	  case 'q': 
		 Parm.optQuiet  = 1;
		 break;

	  case 'b': 
		 Parm.optDialog = 1;
		 break;

	  case 'x': 
		 Parm.optNoChange = 1;
		 break;

	  case 'h': 
		 Parm.optHelp   = 1;
		 break;

	  default:
	 	 if (!Parm.optQuiet) printf("invalid argument %s\n",argv[i]);
		 result = 0;

	 }
	}

	return (result);
}

//=======================================================
//=======================================================
// getcurrenttime
//=======================================================
//=======================================================
void getcurrenttime(char *text, int size)
{
	time_t ltime;    
	struct tm *today;

	time( &ltime );
	// gmt should be converted to localtime by software at destination 
	// couldnt work out how to get the time difference GMT-localtime from NT
    today = gmtime( &ltime );
    strftime( text, size,"%a, %d %b %Y %H:%M:%S", today );
}

//=======================================================
//=======================================================
// server_open
//=======================================================
//=======================================================
int server_open( char *ServerName, unsigned short port, int socket_type, SOCKET *conn_socket)
{
	unsigned int	addr;
	struct			sockaddr_in server;
	struct			hostent *hp;

	//=======================================================
	// Resolve Name or Address
	//=======================================================
	if (Parm.optDebug) printf("Resolving hostname %s...\n",ServerName);

	if (isalpha(ServerName[0])) {   /* server address is a name */
		hp = gethostbyname(ServerName);
	}
	else  { /* Convert nnn.nnn address to a usable one */
		addr = inet_addr(ServerName);
		hp = gethostbyaddr((char *)&addr,4,AF_INET);
	}

	if (hp == NULL ) {
		if (!Parm.optQuiet) printf("Cannot resolve address [%s]\n",ServerName);
		if (Parm.optDebug)  printf("Error %d\n",WSAGetLastError());
		WSACleanup();
		return(0);
	}

	//=======================================================
	// Establish connection to Port
	//=======================================================
	if (Parm.optDebug) printf("Opening port %d...\n",port);
	memset(&server,0,sizeof(server));
	memcpy(&(server.sin_addr),hp->h_addr,hp->h_length);
	server.sin_family = hp->h_addrtype;
	server.sin_port = htons(port);
	*conn_socket = socket(AF_INET,socket_type,0); 

	if (*conn_socket <0 ) {
		if (!Parm.optQuiet) printf("Error opening socket.\n");
		if (Parm.optDebug)  printf("Error %d\n",WSAGetLastError());
		WSACleanup();
		return(0);
	}

	if (connect(*conn_socket,(struct sockaddr*)&server,sizeof(server)) == SOCKET_ERROR) {
     if (!Parm.optQuiet) 
	  printf("Hostname resolved, but a connection to port %d could not be established.\n",port);
   	 if (Parm.optDebug) 
	  printf("connect() failed: %d\n",WSAGetLastError());
	 WSACleanup(); 
	 return(0);
	}

	return (1);
}

//=======================================================
//=======================================================
// server_send
//=======================================================
//=======================================================
void server_send(SOCKET conn_socket, char *text)
{
	if (Parm.optDebug) printf("send: %s", text);

	if ( send(conn_socket,text,strlen(text),0) == SOCKET_ERROR) {
		printf("send() failed: error %d\n",WSAGetLastError());
		WSACleanup();
		exit(2);
	}
}

//=======================================================
//=======================================================
// server_recv
//=======================================================
//=======================================================
void server_recv(SOCKET conn_socket, char *text, int size) 
{
	int retval;

	retval = recv(conn_socket,text,size,0 );
	if (retval == SOCKET_ERROR) {
     printf("recv() failed: error %d\n",WSAGetLastError());
   	 WSACleanup();
	 exit(2);
	}
	text[retval] = 0 ;   

	if (Parm.optDebug) printf("recv: %s", text);
}

//=======================================================
//=======================================================
// mimesendgeneralheader 
//=======================================================
//=======================================================
void mimesendgeneralheader( SOCKET conn_socket)
{
	if (!Parm.optNomime) {
	 strcpy(Buffer,"MIME-Version: 1.0");
	 strcat(Buffer,"\r\n");
	 server_send(conn_socket, Buffer);
	
	 strcpy(Buffer,"Content-Type: multipart/mixed;");
	 strcat(Buffer,"\r\n");
	 strcat(Buffer,"        boundary=\"----_=_NextPart_000\"");
	 strcat(Buffer,"\r\n\r\n");
	 server_send(conn_socket, Buffer);
	 strcpy(Buffer,"This is a MIME formatted message.\r\n\r\n");
	 server_send(conn_socket, Buffer);
	}
}

//=======================================================
//=======================================================
// mimesendplainheader 
//=======================================================
//=======================================================
void mimesendplainheader(SOCKET conn_socket)
{
	if (!Parm.optNomime) {
	 strcpy(Buffer,"\r\n\r\n------_=_NextPart_000\r\n");
	 server_send(conn_socket, Buffer);
	 //strcpy(Buffer,"Content-Type: text/plain;\r\n    charset=\"US-ASCII\"\r\n");
	 strcpy(Buffer,"Content-Type: text/plain;\r\n    charset=\"ISO-8859-1\"\r\n");
	 //strcpy(Buffer,"Content-Type: text/plain;\r\n");
	 strcat(Buffer,"Content-Transfer-Encoding: quoted-printable\r\n\r\n");
	 server_send(conn_socket, Buffer);
	}
}

//=======================================================
//=======================================================
// mimesendfileheader 
//=======================================================
//=======================================================
void mimesendfileheader(SOCKET conn_socket)
{
	if (!Parm.optNomime) {

	 if (Parm.optAttach) {
	  strcpy(Buffer,"\r\n\r\n------_=_NextPart_000\r\n");
	  server_send(conn_socket, Buffer);
	  strcpy(Buffer,"Content-Type: application/octet-stream;\r\n    name=\"");
	  strcat(Buffer,Parm.Filename);
	  strcat(Buffer,"\"\r\n");
	  server_send(conn_socket, Buffer);
	  strcpy(Buffer,"Content-Transfer-Encoding: quoted-printable\r\n");
	  server_send(conn_socket, Buffer);
	  strcpy(Buffer,"Content-Disposition: attachment; \r\n    filename=\"");
	  strcat(Buffer,Parm.Filename);
	  strcat(Buffer,"\"\r\n");
	  server_send(conn_socket, Buffer);
	  strcpy(Buffer,"Content-Location: ATT-0-004711004711007411-");
	  strcat(Buffer,Parm.Filename);
	  strcat(Buffer,"\r\n");
	  server_send(conn_socket, Buffer);
	 } else {
	  mimesendplainheader(conn_socket);
	 }
	 
	 strcpy(Buffer,"\r\n");
	 server_send(conn_socket, Buffer);
	}
}

//=======================================================
//=======================================================
// main 
//=======================================================
//=======================================================
void main(int argc, char *argv[])
{
	unsigned int	parmFilesize = 0;  // bytes read from file
	FILE			*handle;
	struct			_finddata_t finddata;
	SOCKET			conn_socket;
	int				retval;
	WSADATA			wsaData;

	//=======================================================
	// Initialization
	//=======================================================
	Parm.optFilename = 0;
	Parm.optDebug = 0;
	Parm.optInfo = 0;
	Parm.optStream = 0;
	Parm.optQuiet = 0;
	Parm.optHelp = 0;
	Parm.optAttach = 0;
	Parm.optNomime = 0;
	Parm.optDialog = 0;
	Parm.optNoChange = 0;

	Parm.SMTPServer[0] = 0; 
	Parm.Destination[0] = 0;
	Parm.Source[0] = 0; 
	Parm.Subject[0] = 0;
	Parm.Filename[0] = 0;

	Parm.arc_buffer = NULL;

	//=======================================================
	// process commandline
	//=======================================================
	if ( (!processcmdline (argc, argv)) ){
	 printf("\nInvalid commandline. Use -h for help !\n\n");
	 exit(1);
	}

	if ( (Parm.optHelp) || (argc < 2) ) {
	 showsyntax();
	 exit(0);
	}

	if ( Parm.optQuiet && Parm.optDebug) {
	 //printf("Quiet option overrides debug option.\n");
	 Parm.optDebug = 0;
	}

	if ( Parm.optDialog && Parm.optStream ) {
	 if (!Parm.optQuiet) printf("The dialog option does not permit accepting body from standard input.\n");
	 exit(1);
	}

	if ( Parm.optAttach && Parm.optNomime ) {
	 if (!Parm.optQuiet) printf("File cannot be sent as an attachment without using MIME !\n");
	 exit(1);
	}

	if ( ( (!Parm.SMTPServer[0]) || (!Parm.Destination[0]) || (!Parm.Source[0])) && (!Parm.optDialog) ){
	 if (!Parm.optQuiet) printf("\nServer, source and destination must be specified. Use -h for help !\n\n");
	 exit(1);
	}

	//=======================================================
	// Initialization
	//=======================================================
	Parm.arc_buffer = (char *)malloc(DEFAULT_BUFFERSIZE);
	if (!Parm.arc_buffer) {
		printf("Error allocating memory.\n");
		exit(2);
	}
	Parm.arc_buffer[0] = 0;

	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) {  
		if (!Parm.optQuiet) printf("WSAStartup failed with error %d\n",WSAGetLastError());
		WSACleanup();
		exit(2);
	}

	//=======================================================
	// Read message from standard-input
	//=======================================================
	if (Parm.optStream)
	{
	 if (!Parm.optQuiet) printf("Enter message to %s, terminate with Ctrl-Z and CR:\n",Parm.Destination);
	 parmFilesize = fread(Parm.arc_buffer,1,DEFAULT_BUFFERSIZE,stdin);
	 Parm.arc_buffer[parmFilesize] = 0;
	}

	//=======================================================
	// use dialog
	//=======================================================
	if (Parm.optDialog) {
     HANDLE hinst;
     HWND hwnd;

//	 OemToChar (Parm.arc_buffer,Parm.arc_buffer);
   
     hinst = GetModuleHandle (NULL);
     hwnd = GetFocus();

  	 retval = DialogBoxParam( (HINSTANCE) hinst, MAKEINTRESOURCE(IDD_MAIL_SEND), NULL, (DLGPROC) SendDlgProc, (LONG) &Parm );
	 if (-1 == retval) {
	  if (!Parm.optQuiet) {
		retval = GetLastError();
		printf ("Unable to create dialog, Error %d.\n", retval);
	  }
	  exit(1);
	 } else if ( retval == 0 ) {
		 if (!Parm.optQuiet) printf("User has cancelled dialog.\n");
		 exit(3);
	 }

//	 CharToOem (Parm.arc_buffer,Parm.arc_buffer);
	 parmFilesize = strlen ( Parm.arc_buffer);

	}

	//=======================================================
	// Establish connection to SMTP-Server
	//=======================================================
	if (!server_open(Parm.SMTPServer, DEFAULT_PORT, DEFAULT_PROTO, &conn_socket)) exit(2);

	//=======================================================
	// Begin talking to SMTP-Server
	//=======================================================
	server_recv(conn_socket, Buffer,TEXTBUFFERSIZE); 

	server_send(conn_socket, "HELO mail.exe\n"); 
	server_recv(conn_socket, Buffer,TEXTBUFFERSIZE); 

	//=======================================================
	// Tell it the basic whereabouts
	//=======================================================
	strcpy(Buffer,"MAIL FROM: <"); 
	strcat(Buffer,Parm.Source);
	strcat(Buffer,">\n");
    server_send(conn_socket, Buffer);
	server_recv(conn_socket, Buffer,TEXTBUFFERSIZE); 
	
	strcpy(Buffer,"RCPT TO: <");
	strcat(Buffer,Parm.Destination);
	strcat(Buffer,">\n");
    server_send(conn_socket, Buffer);
	server_recv(conn_socket, Buffer,TEXTBUFFERSIZE); 

	//=======================================================
	// Begin sending content
	//=======================================================
    server_send(conn_socket, "DATA\n");
	server_recv(conn_socket, Buffer,TEXTBUFFERSIZE); 

	//-------------------------------------------------------
	// Header lines
	//-------------------------------------------------------
	if ( Parm.Subject[0]) {
	 strcpy(Buffer,"Subject:");
	 strcat(Buffer,Parm.Subject);
	 strcat(Buffer,"\r\n");
	 server_send(conn_socket, Buffer);
	}
	 
	strcpy(Buffer,"X-Mailer: mail.exe v1.0");
	strcat(Buffer,"\r\n");
	server_send(conn_socket, Buffer);
		
	strcpy(Buffer,"From:"); 
	strcat(Buffer,Parm.Source);
	strcat(Buffer,"\r\n");
    server_send(conn_socket, Buffer);

	strcpy(Buffer,"To:");
	strcat(Buffer,Parm.Destination);
	strcat(Buffer,"\r\n");
    server_send(conn_socket, Buffer);

	strcpy(Buffer,"Date:");
	getcurrenttime(Textbuffer,TEXTBUFFERSIZE);
	strcat(Buffer,Textbuffer);
	strcat(Buffer,"\r\n");
	server_send(conn_socket, Buffer);

	mimesendgeneralheader(conn_socket);

	//-------------------------------------------------------
	// Data from standard-input already in buffer
	//-------------------------------------------------------
	if (parmFilesize)
	{
	  if (parmFilesize) {
		 mimesendplainheader(conn_socket);
		 retval = send(conn_socket,Parm.arc_buffer,parmFilesize,0);
		 if (retval == SOCKET_ERROR) {
			if (!Parm.optQuiet) printf("send() failed: error %d\n",WSAGetLastError());
			WSACleanup(); 
			exit(2);
		 }
	  }
	}
	
	//-------------------------------------------------------
	// Data from text-file
	//-------------------------------------------------------
	if (Parm.optFilename)  
	{ 
	  _findfirst(Parm.Filename,&finddata); 
	  handle=fopen(Parm.Filename,"r");
  
		if (handle != NULL ) 
		{  
	  		parmFilesize=fread(Parm.arc_buffer,1,DEFAULT_BUFFERSIZE,handle); 
      
			if (Parm.optDebug) {
			 printf("%u bytes read from %s, %s\n",parmFilesize,Parm.Filename,ctime (&finddata.time_write )); 
			}

			if (Parm.optInfo) 
			{
			  strcpy(Buffer,"File ");
			  strcat(Buffer,Parm.Filename);
			  strcat(Buffer,", ");
			  strcat(Buffer,itoa(parmFilesize,Parm.Subject,10));
			  strcat(Buffer," bytes, ");
			  strcat(Buffer, ctime (&finddata.time_write));
			  strcat(Buffer,"\r\n");
			  server_send(conn_socket, Buffer); 
			  if (Parm.optDebug) printf("sending info: %s",Buffer);
			} 
			
			if ( (Parm.optAttach) || (!Parm.optStream) ) mimesendfileheader(conn_socket);

		 	if (Parm.optDebug) printf( "sending: %s ",Parm.Filename);

			while(parmFilesize>0)
			{
			 retval = send(conn_socket,Parm.arc_buffer,parmFilesize,0);
			 if (retval == SOCKET_ERROR) {
				if (!Parm.optQuiet) printf("send() failed: error %d\n",WSAGetLastError());
				WSACleanup(); 
				exit(2);
			 }

			 if (Parm.optDebug) {
			  printf("sending %u bytes\n",retval);
			 }

			 if ( strcmp(Parm.Filename,"con:") )
			  parmFilesize=fread(Parm.arc_buffer,1,DEFAULT_BUFFERSIZE,handle); 
			 else
	          parmFilesize = 0;
			}

			fclose(handle);

		}
		else
		{ 
			if (!Parm.optQuiet) printf ("File %s could not be opened.\n",Parm.Filename);
		} 
	}

	//=======================================================
	// Terminate data-section by sending <CR> . <CR>
	//=======================================================
	server_send(conn_socket, "\r\n.\r\n");
	server_recv(conn_socket, Buffer,TEXTBUFFERSIZE); 

	server_send(conn_socket, "quit\r\n");
	server_recv(conn_socket, Buffer,TEXTBUFFERSIZE); 

	//=======================================================
	// Close connection, cleanup and quit
	//=======================================================
	closesocket(conn_socket);

	WSACleanup();

	free(Parm.arc_buffer);

	if (!Parm.optQuiet) printf("Message sent sucessfully.\n");

	exit(0);

}


