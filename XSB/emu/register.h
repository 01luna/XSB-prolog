/* File:      register.h
** Author(s): Warren, Swift, Xu, Sagonas
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
** $Id: register.h,v 1.1.1.1 1998-11-05 16:55:20 sbprolog Exp $
** 
*/


/* Argument Registers
   ------------------ */
#define MAX_REGS    257
extern Cell reg[MAX_REGS];

/*---- special registers -----------------------------------------------*/

extern CPtr ereg;	/* last activation record       */
extern CPtr breg;	/* last choice point            */
extern CPtr hreg;	/* top of heap                  */
extern CPtr *trreg;	/* top of trail stack           */
extern CPtr hbreg;	/* heap back track point        */
extern CPtr sreg;	/* current build or unify field */
extern byte *cpreg;	/* return point register        */
extern byte *pcreg;	/* program counter              */

/*---- registers added for the SLG-WAM ---------------------------------*/

extern CPtr efreg;
extern CPtr bfreg;
extern CPtr hfreg;
extern CPtr *trfreg;
extern CPtr pdlreg;
extern CPtr openreg;

extern CPtr ptcpreg;	/* parent tabled CP (subgoal)	*/
extern CPtr delayreg;

/*---- other stuff added for the SLG-WAM -------------------------------*/

extern int  xwammode;
extern int  level_num;
extern CPtr root_address;

/*---- for splitting stack mode ----------------------------------------*/

extern CPtr ebreg;	/* breg into environment stack */


