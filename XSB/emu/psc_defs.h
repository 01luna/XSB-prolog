/* File:      psc_defs.h
** Author(s): Jiyang Xu, Terry Swift, Kostis Sagonas
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
** $Id: psc_defs.h,v 1.6 2005-08-08 21:25:02 dwarren Exp $
** 
*/

#ifndef _PSC_DEFS_
#define _PSC_DEFS_

#define T_VISIBLE	0
#define T_HIDDEN	1
#define T_UNLOADED	2
#define T_ENV		3
#define T_TABLED	8
#define T_SHARED	32 // 0x20  use decimal for Prolog include
#define T_PRIVATE	16 // 0x10  use decimal for Prolog include
#define T_SPY		192 // 0xc0 use decimal for Prolog include

/* === type definition: (entry_type) ============================== */

#define T_ORDI  0	/* constant-type: no ep definition, may be set later */
#define T_DYNA	1	/* constant-type: dynamic, code in buffer */
#define T_PRED  2	/* constant-type: ep points to compiled code */

#define T_MODU  4	/* ep field is used to hold psc-list */
#define T_FILE  5	/* ep field could be file descriptor (not now) */

#define T_UDEF 12	/* unloaded T_PRED */
#define T_FORN 13	/* predicate in foreign language */

/* === loader definitions (module information) ==================== */

#define T_EXPORTED 0
#define T_LOCAL    1
#define T_IMPORTED 2
#define T_GLOBAL   4


/* Global module */

#define USERMOD_PSC    1

#endif /* _PSC_DEFS_ */
