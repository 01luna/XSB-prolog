/* File:      main_xsb.c
** Author(s): Warren
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1993-1998
** 
** XSB is free software; you can redistribute it and/or modify it under the
** terms of the GNU Library General Public License as published by the Free
** Software Foundation; either version 2 of the License, or (at your option)
** any later version.
** 
** XSB is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
** more details.
** 
** You should have received a copy of the GNU Library General Public License
** along with XSB; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** $Id: main_xsb.c,v 1.1 1999-10-25 05:58:45 kifer Exp $
** 
*/


#include "configs/config.h"
#include "basictypes.h"


#ifdef WIN_NT
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
/* special.h must be included after sys/stat.h */
#include "configs/special.h"

#include "emuloop.h"

#ifdef HAVE_SOCKET
#ifdef WIN_NT
#include <windows.h>
#include <winsock.h>
#endif
#endif

#include "orient_xsb.h"

int main(int argc, char *argv[])
{ 
#ifdef HAVE_SOCKET
#ifdef WIN_NT
  INT err;
  WSADATA wsaData;
#ifdef SILENT_NT
  FILE *stream_err, *stream_out;
  stream_err = freopen("errorlog", "w", stderr);
  stream_out = freopen("outlog", "w", stdout);
#endif
  err = WSAStartup(0x0101, &wsaData);
  if (err == SOCKET_ERROR) {
    fprintf (stdout, "WSAStartup Failed\n");
    return FALSE;
  }
#endif
#endif

  xsb(0, argc, argv);  /* init xsb */

  xsb(1, 0, 0);        /* normal execution */
  xsb(2, 0, 0);        /* when halts, exit */
  return 0;
}

