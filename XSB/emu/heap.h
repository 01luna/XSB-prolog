/* File:      heap.h
** Author(s): David S. Warren, Jiyang Xu
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
** $Id: heap.h,v 1.5 1999-02-01 23:56:03 kostis Exp $
** 
*/


/*---- The following are macros for setting heap values ----------------*/

#define new_heap_free(sh_reg) bld_free(sh_reg); sh_reg++
/* make a free variable on the top of the heap */

#define new_heap_string(sh_reg, str) bld_string(sh_reg, str); sh_reg++
/* make a new string node on the heap, with a string"str" */

#define new_heap_nil(sh_reg) bld_nil(sh_reg); sh_reg++

#define new_heap_num(sh_reg, val) bld_int(sh_reg, val); sh_reg++
/* make a new num node on the heap, with value val */

#define new_heap_float(sh_reg, val) bld_float(sh_reg, val); sh_reg++
/* make a new float node on the heap, with value val */

#define new_heap_functor(sh_reg, psc) bld_functor(sh_reg++, psc)
/* make a new functor node in heap */

#define new_heap_node(sh_reg, x) bld_copy(sh_reg++, x)
/* make a new heap node with value x (one word type) */

/*----- The following is for the type of garbage collection to be used -*/

#define NO_GC      0
#define SLIDING_GC 1
#define COPYING_GC 2

/*----- The following functions are used in other parts of the system --*/

extern int  gc_heap(int);
extern int  mark_heap(int,int *);
extern void glstack_realloc(int,int);

extern void print_cp(int);
extern void print_tr(int);
extern void print_ls(int);
extern void print_all_stacks(void);
extern void print_regs(int,int);
extern void print_heap(int,int,int);
extern void print_gc_statistics(void);

/*----------------------------------------------------------------------*/
