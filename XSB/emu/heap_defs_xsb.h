/* File:      heap_defs_xsb.h
** Author(s): Kostis Sagonas
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1999.
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
** $Id: heap_defs_xsb.h,v 1.2 2001-05-24 17:54:51 lfcastro Exp $
** 
*/


/*--- The following is for the type of garbage collection to be used ---*/

#define NO_GC           0
#define SLIDING_GC      1
#define COPYING_GC      2
#define INDIRECTION_SLIDE_GC   3

/*----------------------------------------------------------------------*/
