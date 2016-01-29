#include "stdafx.h"

#include "dialogsend.h"

/*case WM_COMMAND:
   switch( wParam ) {
      case IDM_ABOUT:
         DialogBox( ghInstance, "AboutDlg", hWnd, (DLGPROC)
                    AboutDlgProc );
      break;
   }
break;

The last parameter is a pointer to a dialog box procedure. It has the following prototype.
*/

BOOL CenterWindow (HWND hwnd)
{
    RECT    rect;
    int     w, h;
    int     wScreen, hScreen, xNew, yNew;
    HDC     hdc;

    GetWindowRect (hwnd, &rect);
    w = rect.right - rect.left;
    h = rect.bottom - rect.top;

    hdc = GetDC (hwnd);
    wScreen = GetDeviceCaps (hdc, HORZRES);
    hScreen = GetDeviceCaps (hdc, VERTRES);
    ReleaseDC (hwnd, hdc);

    xNew = wScreen/2 - w/2;
    yNew = hScreen/2 - h/2;

    return SetWindowPos (hwnd, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

LRESULT WINAPI SendDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 
 static MAILPARMS *Parms ;
 int wmid;

 switch( uMsg ) {
   
  case WM_INITDIALOG:
	  
	  CenterWindow(hDlg);

	  Parms = (MAILPARMS *) lParam;
	   
	   
	  SendDlgItemMessage(hDlg, IDC_EDIT_FROM, EM_SETREADONLY, ( Parms->optNoChange && strlen(Parms->Source) ), 0); 
	  SendDlgItemMessage(hDlg, IDC_EDIT_FROM, EM_LIMITTEXT, MAXADDRESS, NULL);
	  SendDlgItemMessage(hDlg, IDC_EDIT_FROM, WM_SETTEXT, 0, (LPARAM) (Parms->Source)); 

	  SendDlgItemMessage(hDlg, IDC_EDIT_TO, EM_SETREADONLY, ( Parms->optNoChange && strlen(Parms->Destination) ), 0); 
	  SendDlgItemMessage(hDlg, IDC_EDIT_TO, EM_LIMITTEXT, MAXADDRESS, NULL);
 	  SendDlgItemMessage(hDlg, IDC_EDIT_TO, WM_SETTEXT, 0, (LPARAM) (Parms->Destination)); 

	  SendDlgItemMessage(hDlg, IDC_EDIT_SERVER,  EM_SETREADONLY, ( Parms->optNoChange && strlen(Parms->SMTPServer) ), 0); 
	  SendDlgItemMessage(hDlg, IDC_EDIT_SERVER,  EM_LIMITTEXT, MAXADDRESS, NULL);
 	  SendDlgItemMessage(hDlg, IDC_EDIT_SERVER,  WM_SETTEXT, 0, (LPARAM) (Parms->SMTPServer)); 

	  SendDlgItemMessage(hDlg, IDC_EDIT_SUBJECT, EM_SETREADONLY, ( Parms->optNoChange && strlen(Parms->Subject) ), 0); 
	  SendDlgItemMessage(hDlg, IDC_EDIT_SUBJECT, EM_LIMITTEXT, MAXSUBJECT, NULL);
 	  SendDlgItemMessage(hDlg, IDC_EDIT_SUBJECT, WM_SETTEXT, 0, (LPARAM) (Parms->Subject)); 

//	  SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, EM_SETREADONLY, ( Parms->optNoChange && strlen(Parms->arc_buffer) ), 0); 
	  SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, EM_LIMITTEXT, DEFAULT_BUFFERSIZE, NULL);
// 	  SendDlgItemMessage(hDlg, IDC_EDIT_TEXT, WM_SETTEXT, 0, (LPARAM) (Parms->arc_buffer)); 

	  return TRUE;

   case WM_COMMAND:
    wmid = LOWORD(wParam);             
	switch (wmid) { 

		case IDOK:
			if ( 
				!SendDlgItemMessage(hDlg, IDC_EDIT_FROM,   WM_GETTEXTLENGTH, 0, 0) ||
				!SendDlgItemMessage(hDlg, IDC_EDIT_TO,     WM_GETTEXTLENGTH, 0, 0) ||
				!SendDlgItemMessage(hDlg, IDC_EDIT_SERVER, WM_GETTEXTLENGTH, 0, 0) 
			   ) 
			
			{
				MessageBox (hDlg, "From, To and Server must be specified.", "Warning", MB_OK | MB_ICONSTOP ); 
			} else {
			 SendDlgItemMessage(hDlg, IDC_EDIT_FROM,   WM_GETTEXT, MAXADDRESS, (LPARAM) (Parms->Source)); 
			 SendDlgItemMessage(hDlg, IDC_EDIT_TO,     WM_GETTEXT, MAXADDRESS, (LPARAM) (Parms->Destination)); 
			 SendDlgItemMessage(hDlg, IDC_EDIT_SERVER, WM_GETTEXT, MAXADDRESS, (LPARAM) (Parms->SMTPServer)); 
			 SendDlgItemMessage(hDlg, IDC_EDIT_SUBJECT,WM_GETTEXT, MAXSUBJECT, (LPARAM) (Parms->Subject)); 
			 SendDlgItemMessage(hDlg, IDC_EDIT_TEXT,   WM_GETTEXT, DEFAULT_BUFFERSIZE, (LPARAM) (Parms->arc_buffer)); 
             EndDialog( hDlg, 1 );
			}
            return TRUE;

		 case IDCANCEL:
			 if (MessageBox(hDlg, "No message will be sent. Are you sure you want to quit ?", "Warning", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES) {
			  EndDialog( hDlg, 0 );
			 }
			return TRUE;


      }
   break;
 }

 return FALSE;

}

/*

  int GetDialogArgs (char ***pargv)
{
    int ret;
    HANDLE hinst;
    HWND hwnd;
    char szFile[80];

    hinst = GetModuleHandle (NULL);
    hwnd = GetFocus();

    ret = DialogBoxParam (hinst, "CL", NULL, CLDlgProc, (LPARAM)pargv);

    if (-1 == ret) {
        ret = GetLastError();
        printf ("Unable to create dialog: %d\n", ret);
        GetModuleFileName (hinst, szFile, sizeof(szFile));
        printf ("hinst = %d\n", hinst);
        printf ("hwnd = %d\n", hwnd);
        printf ("File = %s\n", szFile);
        return FALSE;

    }
    return ret;
}

  */

