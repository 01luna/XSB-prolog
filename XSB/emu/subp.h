/* File:      subp.h
** Author(s): Kostis Sagonas
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
** $Id: subp.h,v 1.1.1.1 1998-11-05 16:55:23 sbprolog Exp $
** 
*/


extern void intercept(Psc);
extern void init_interrupt(void);
extern void print_statistics(int);
extern void print_qatom(FILE *, char *);
extern void print_op(FILE *, char *, int);
extern void remove_open_tables_reset_freezes(void);

extern bool unify(Cell, Cell);
extern bool int_unify(Cell, Cell);
extern bool atom_unify(Cell, Cell);
extern bool compare(/* Cell, Cell */);
extern bool key_compare(/* Cell, Cell */);

extern byte *exception_handler(char *);

extern Psc synint_proc(Psc, int, byte *);

