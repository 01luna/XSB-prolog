/*  -*-c-*-  Make sure this file comes up in the C mode of emacs */ 
/* File:      sp_unify.i
** Author(s): Kostis Sagonas
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1986, 1993-1998
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
** $Id: sp_unify.i,v 1.3 1999-06-23 19:50:50 kostis Exp $
** 
*/


/* to be used when cell op1 is known to contain an int */
static inline bool int_unify(Cell op1, Cell op2)
{	/* No checking for interrupts is needed!       */
  deref(op2);
  if (isref(op2)) {
    /* op2 is FREE:                       num ... free */
    bind_copy0((CPtr)(op2), op1);
    return TRUE;
  }
  return (op1 == op2);
}


/* to be used when cell op1 is known to contain an atom */
static inline bool atom_unify(Cell op1, Cell op2)
{	/* No checking for interrupts is needed!	*/
  deref(op2);
  if (isref(op2)) {
    /* op2 is FREE                      string ... free */
    bind_copy0((CPtr)(op2), op1);
    return TRUE;
  }
  return (op1 == op2);
}
