/* File:      cell_xsb.h
** Author(s): David S. Warren, Jiyang Xu, Terrance Swift
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1993-19989
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
** $Id: cell_xsb.h,v 1.20 2005-02-04 22:26:56 dwarren Exp $
** 
*/

#ifndef __CELL_XSB_H__
#define __CELL_XSB_H__

#ifndef CONFIG_INCLUDED
#error "File xsb_config.h must be included before this file"
#endif

/*======================================================================*/
/* CELL: an element in the local stack or global stack (heap).		*/
/*									*/
/* Corresponds to the basic word type of the physical machine.		*/
/* This module also defines the basic INTEGER and REAL types to use	*/
/* by the Prolog interpreter						*/
/*									*/
/* Interface routines							*/
/*	They are put in the different files indicated below, according  */
/*	to the tagging scheme you used.					*/
/* The functions (macros, indeed) include:				*/
/*	cell_tag(cell):		give any cell, return its tag		*/
/*	isnonvar(dcell):	check if the cell is a ref or a var.	*/
/*				(an index in VARASINDEX)		*/
/*				when used with derefed cell, check if	*/
/*				the cell is instanciated 		*/
/*	int_val(dcell):		assume derefed integer cell, return its */
/*				value in machine format			*/
/*	clref_val(dcell):	assume derefed cs or list cell, return  */
/*				a CPtr to the address it points to.	*/
/*	cs_val(dcell):		assume derefed cs cell, return Pair	*/
/*	bld_int(addr, val):	build a new integer cell		*/
/*	bld_float(addr, val):	build a new float cell			*/
/*	bld_ref(addr, val):	build a new reference cell		*/
/*	bld_cs(addr, str):	build a new cs cell			*/
/*	bld_string(addr, str):	build a new string cell			*/
/*	bld_list(addr, list):	build a new list cell			*/
/*	bld_functor(addr, psc):	build a functor cell for a new structure*/
/*	bld_free(addr):		build a new free variable 		*/
/*	bld_copy(dest, source): build a copy of the given cell.		*/
/*			if the source is a free var, the copy is indeed */
/*			a ref cell. Need special checking when free var */
/*			is not a pointer to itself but a special tag.	*/
/*	bld_copy0(dest, src):   the same as bld_copy except assume      */
/*                    non-var, or where semantics is to resume/set.	*/
/*                    (in set CP and resume CP)				*/
/*                    For variable as selfpointer, no differnce.    	*/
/*======================================================================*/

/* ==== types of cells =================================================*/

#include "celltags_xsb.h"

/*======================================================================*/
/* CELL: an element in the local stack or global stack (heap).		*/
/*======================================================================*/

#include "cell_def_xsb.h"

typedef Cell *CPtr;

#ifdef BITS64
typedef long Integer ;
typedef unsigned long UInteger ;
#else
typedef int Integer ;
typedef unsigned int UInteger ;
#endif
 
#ifdef BITS64
typedef double Float ;
#else
typedef float Float ;
#endif

#define cell(cptr) *(cptr)
#define follow(cell) (*(CPtr)(cell))

extern Float asfloat(Cell);
extern Cell  makefloat(Float);
extern Float getfloatval(Cell);

#define isref(cell)  (!((word)(cell)&0x3))
#define isnonvar(cell) ((word)(cell)&0x3)		/* dcell -> xsbBool */

#define cell_tag(cell) ((word)(cell)&0x7)

/*======================================================================*/
/*======================================================================*/

#if (defined(HP700) || defined(IBM) || defined(MIPS_BASED) || defined(SOLARIS_x86))
#define GENERAL_TAGGING
#endif

#if defined(GENERAL_TAGGING)

/* General Tagging for systems that return addresses in higher half
   (2-4GB) of virtual memory.  This builds a table for the high-order
   nibble of addresses and maps them to 0-7, to free up the 1st bit to
   allow it to be stolen. */

extern unsigned long enc[], dec[];

#define enc_int(val)  (((Integer)(val) << 3))
#define dec_int(val)  ((Integer)(val) >> 3)

#define enc_addr(addr) ((Cell)((enc[((unsigned long)addr)>>28] | ((unsigned long)(addr) & 0x0ffffffc)) << 1))
#define dec_addr(dcell) ((Cell)(dec[(unsigned long)(dcell)>>29] | (((unsigned long)(dcell) >> 1) & 0x0ffffffc)))

#elif BITS64
/* 64 bit, take bits 0, 61-63 */
/* Encoded integers/addresses */
#define enc_int(val) ((Integer)(val) << 3)
#define dec_int(dcell) ((Integer)(dcell) >> 3)
/* Fewer bit representation of pointers */
#define enc_addr(addr) ((Cell)(addr))
#define dec_addr(dcell) (((Cell)(dcell)) & 0xfffffffffffffff8)

#else
/* take bits 0-1, 31 */
/* Steals bit 31 (high order).  If system allocates memory (malloc) in
   the upper 2 GB of virtual memory, then use GENERAL_TAGGING. */

#define enc_int(val)  (((Integer)(val) << 3))
#define dec_int(val)  ((Integer)(val) >> 3)

#define enc_addr(addr) ((Cell)(addr) << 1)
#define dec_addr(dcell) (((Cell)(dcell) >> 1) & 0x7ffffffc)

#endif

/*======================================================================*/

/* integer manipulation */
#define int_val(dcell) (Integer)dec_int(dcell)
#define makeint(val) (Cell)((enc_int(val)) | XSB_INT)

/* string manipulation */
#define string_val(dcell) (char *)dec_addr(dcell)
#define makestring(str) (Cell)(enc_addr(str) | XSB_STRING)

/* special-case strings [] */
#define	makenil		makestring(nil_sym)

/* pointer manipulation */
#define cs_val(dcell) (Pair)dec_addr(dcell)
#define makecs(str) (Cell)(enc_addr(str) | XSB_STRUCT)
#define clref_val(dcell) (CPtr)dec_addr(dcell)
#define makelist(list) (Cell)(enc_addr(list) | XSB_LIST)
#define makeattv(attv) (Cell)(enc_addr(attv) | XSB_ATTV)
#define trievar_val(dcell) (Integer)dec_int(dcell)
#define maketrievar(val) (Cell)(enc_int(val) | XSB_TrieVar)

/**#define get_str_psc(dcell) ((cs_val(dcell))->psc_ptr)**/
#define get_str_psc(dcell) (*((Psc *)dec_addr(dcell)))

#define addr_val(dcell) string_val(dcell)
#define makeaddr(val) makestring(val)

/* #define addr_val(dcell) int_val(dcell) */
/* #define makeaddr(val) makeint(val) */

/* common representations */
#define vptr(dcell) (CPtr)(dcell)
#define float_val(dcell) getfloatval(dcell)
#define ref_val(dcell) (CPtr)(dcell)

#define bld_nil(addr) cell(addr) = makenil
#define bld_string(addr, str) cell(addr) = makestring(str)
#define bld_int_tagged(addr, val) cell(addr) = val
#define bld_int(addr, val) cell(addr) = makeint(val)
#define bld_float_tagged(addr, val) cell(addr) = val
#define bld_float(addr, val) cell(addr) = makefloat((float)val)
#define bld_ref(addr, val) cell(addr) = (Cell)(val)
#define bld_cs(addr, str) cell(addr) = makecs(str)
#define bld_list(addr, list) cell(addr) = makelist(list)
#define bld_attv(addr, attv) cell(addr) = makeattv(attv)
#define bld_functor(addr, psc) cell(addr) = (word)psc
#define bld_copy0(addr, val) cell(addr) = val
#define bld_copy(addr, val) cell(addr) = val
/* tls -- this bld_copy wont work for VARASINDEX */
#define bld_free(addr) cell(addr) = (Cell)(addr) /* CPtr => XSB_FREE cell */

#define isinteger(dcell) (cell_tag(dcell)==XSB_INT)	/* dcell -> xsbBool */
#define isfloat(dcell) (cell_tag(dcell)==XSB_FLOAT)	/* dcell -> xsbBool */
#define isconstr(dcell) (cell_tag(dcell)==XSB_STRUCT)	/* dcell -> xsbBool */
#define islist(dcell) (cell_tag(dcell)==XSB_LIST)	/* dcell -> xsbBool */
#define isattv(dcell) (cell_tag(dcell)==XSB_ATTV)	/* dcell -> xsbBool */

#define isstring(dcell) (cell_tag(dcell)==XSB_STRING)
#define numequal(num1, num2) num1 == num2

#define isnumber(dcell)	((isinteger(dcell)) || (isfloat(dcell)))
#define isconstant(dcell)  ( isstring(dcell) || isnumber(dcell) )
#define isatom(dcell)	((isstring(dcell)) || \
			 (isconstr(dcell) && get_arity(get_str_psc(dcell))==0))
#define isatomic(dcell)	((isstring(dcell)) || (isnumber(dcell)) || \
			 (isconstr(dcell) && get_arity(get_str_psc(dcell))==0))

#define isnil(dcell) (isstring(dcell) && (char *)string_val(dcell) == nil_sym)


/*======================================================================*/
/* Miscellaneous							*/
/*======================================================================*/

#define MAX_ARITY	255

#define arity_integer(dcell) \
			(isinteger(dcell) && int_val(dcell) >= 0 \
					  && int_val(dcell) <= MAX_ARITY)

#define CELL_DEFS_INCLUDED

#define isboxedinteger(dcell) (isconstr(dcell) && (get_str_psc(dcell)==box_psc) && (int_val(cell(clref_val(dcell)+1)) == 1))

#define boxedint_val(dcell) \
       ((Integer)((((unsigned long)int_val(cell(clref_val(dcell)+2))<<24)   \
                  | int_val(cell(clref_val(dcell)+3))))) 

#define oint_val(dcell)      \
    (isinteger(dcell)        \
     ?int_val(dcell)         \
     :(isboxedinteger(dcell) \
       ?boxedint_val(dcell)  \
       :0x80000000))

#define int_overflow(value)                 \
   (int_val(makeint(value)) != (value))

#define bld_boxedint(addr, value)				\
     {Cell binttemp = makecs(hreg);				\
      new_heap_functor(hreg,box_psc);				\
      bld_int(hreg,1); hreg++;					\
      bld_int(hreg,(((unsigned)(value)) >> 24)); hreg++;	\
      bld_int(hreg,((value) & 0xffffff)); hreg++;		\
      cell(addr) = binttemp;}

#define bld_oint(addr, value)					\
    if (int_overflow(((Integer)value))) {					\
      bld_boxedint(addr, ((Integer)value));				\
    } else {bld_int(addr,((Integer)value));}

#endif /* __CELL_XSB_H__ */
