/* File:      setjmp_xsb.h
** Author(s): kifer
** Contact:   xsb-contact@cs.sunysb.edu
**
** Copyright (C) The Research Foundation of SUNY, 1999
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
** $Id: setjmp_xsb.h,v 1.1 1999-11-17 03:54:33 kifer Exp $
** 
*/


/*
  This file tries to do the right job with inclusion of setjmp.h
  Strangely, some compilers require _POSIX_C_SOURCE options in order for
  sigsetjmp to be defined.
  We don't define _POSIX_C_SOURCE globally because some XSB files don't compile
  with it.
*/


#ifndef SETJMP_XSB_INCLUDED

#define SETJMP_XSB_INCLUDED

/* need _POSIX_C_SOURCE for setjmp.h to define the right stuff with gcc */
#ifndef WIN_NT
#define _POSIX_C_SOURCE
#endif
#include <setjmp.h> 

#endif

