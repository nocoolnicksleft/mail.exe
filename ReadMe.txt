
=====================================================================================
Mail 1.0b
=====================================================================================

by Bjoern Seip for COMLINE AG, Hamburg, Germany (bseip@seip.net)

i was inspired by the tool smtpsnd by Sergio Steinberg, Buenos Aires, Argentina 
(sergio_steinberg@hotmail.com) from whose source code i learned how to do SMTP, 
although this Program does not contain any of Sergios code. Thanks, Sergio !

=====================================================================================
Description
=====================================================================================

This command-line utility sends email via the SMTP Protocol on Windows 95/98 
and Windows NT Operating Systems.

It accepts source and destination addresses as well as text file attachments.

It is also possible to send piped data from other programs, to enter messages
at the command prompt or to let users enter messages in a dialog box.

This program is supplied as is; no implicit or explicit warranty for its suitability
for any particular purpose. The source code is not generally available.

Comments and suggestions are welcome, although i cant promise to fulfill any 
particular needs. If you like this tool and make use of it in a production 
environment i ask you to send me a picture-postcard from whereever you live to:

Bjoern Seip
COMLINE AG
Heselstuecken 28
22453 Hamburg
Germany

=====================================================================================
Syntax
=====================================================================================

The syntax is as follows:

Usage:   mail -m smtpserver -o source -n destination
              [-p subject] [-f textfile]
              [options]

Options: -d   print debug information
         -i   send additional file information
         -a   send file as an attachment
         -r   omit MIME data
         -s   accept body from standard input
         -q   quiet operation, no error reporting
         -b   use dialog
         -x   dont allow preset values to be changed
         -h   print this


At least SMTP-Server, source and destination addresses must be specified.

The debugging option -d will echo the entire dialog with the server and display
additional information.

No Information at all will be displayed using quiet mode -q.

By using -s the body of the message will be read from standard input. This is
usually the command shell, but can also be any other program connected to
mail.exe using a pipe (see examples below).

If a textfile is appended, -i can be used to send additional file information 

In default mode the message is generated in MIME format. This can be turned off
by using the -r option.

In default mode a text file is sent as part of the message body. Using the option -a
will send the file as an attachment instead.

Using -d invokes a dialog box to query the user for source, destination, subject, text 
and server. Values specified on the command line serve as defaults. If -x is specified it 
is not possible for the user to change defaults. Using the dialog option does not 
permit accepting the body from standard input. File attachments will be appended to the 
text entered in the dialog.

Only the US-ASCII 7 bit character set is supported.

The following errorlevels will be set in case of failure:
Errorlevel 1	Invalid commandline
Errorlevel 2	Communication failure or internal error
Errorlevel 3	User selected cancel in dialog mode

=====================================================================================
Examples
=====================================================================================

The simplest form is used for testing purposes, just sending an email
without subject oder bodytext:

	mail -m mail.some.org -n bill@microserv.com -o elliot@oracle.mil 

For small messages it is possible to send an email with only a subject line:

	mail -m mail.some.org -n bill@microserv.com -p "Bill rulez" -o elliot@oracle.mil

To enter a bodytext interactively just use -s:

	mail -m mail.some.org -n bill@microserv.com -o elliot@oracle.mil -s 
	Enter message to bill@microserv.com, terminate with Ctrl-Z <CR>:
	Bill rulez
	^Z
	Message sent successfully.

To mail output of other programs use pipes, remember to use -s

	ipconfig /all | mail -m mail.some.org -n bill@microserv.com -o elliot@oracle.mil -s

Textfiles can be attached by -f
(Attention: binary files cannot be sent this way - you may want to use an encoder 
like uuencode, but this tool is not intended to support MIME-style attachments)

	mail -m mail.some.org -n bill@microserv.com -o elliot@oracle.mil -f c:\home\secret.txt

This will open a dialog window

	mail -b

This will open a dialog window with preset values for server and source address,
not letting the user change the preset values.

	mail -b -x -m mail.some.org -o bill@microserv.com




