
#include <windows.h>
#include <commdlg.h>

#ifndef DIALOGSEND
#define DIALOGSEND

#define VERSION_INFORMATION "mail v1.0b"

#define DEFAULT_PORT  25			// SMTP
#define DEFAULT_PROTO SOCK_STREAM	// TCP
#define DEFAULT_SENDER "mail.exe"
#define DEFAULT_BUFFERSIZE 32000
#define TEXTBUFFERSIZE 128
#define MAXADDRESS 60
#define MAXSUBJECT 150

#include "resource.h"

LRESULT WINAPI SendDlgProc( HWND, UINT , WPARAM, LPARAM);

struct MAILPARMS {

int	 optFilename,
	 optDebug,
	 optInfo,
	 optStream,
	 optQuiet,
	 optHelp,
	 optAttach,
	 optNomime,
	 optDialog,
	 optNoChange;

char SMTPServer[MAXADDRESS]; 
char Destination[MAXADDRESS]; 
char Source[MAXADDRESS]; 
char Subject[TEXTBUFFERSIZE];
char Filename[MAXSUBJECT];

char *arc_buffer;

};


#endif


