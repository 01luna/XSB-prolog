/* File:      deref.h
** Author(s): Jiyang Xu, Terrance Swift, Kostis Sagonas
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1986, 1993-1998
** Copyright (C) ECRC, Germany, 1990
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
** $Id: deref.h,v 1.1.1.1 1998-11-05 16:55:14 sbprolog Exp $
** 
*/


/* deref expects argument of type Cell */

#define deref(op) while (isref(op)) { \
		    if (op == follow(op)) \
  			break; \
		    op = follow(op); }

#define cptr_deref(op) while (isref(op)) { \
			 if (op == (CPtr) cell(op)) \
  			     break; \
			 op = (CPtr) cell(op); }

#define printderef(op) while (isref(op) && op > 0) { \
			 if (op==follow(op)) \
  			    break; \
			 op=follow(op); }


/* dereference, if cell_tag(op) is FREE or REF1 */
#define free_deref(op) \
  while ( (cell_tag(op) == FREE) || (cell_tag(op) == REF1) ) { \
    if (op == follow(op)) \
      break; \
    op = follow(op); \
  }


