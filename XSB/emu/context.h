/* File:      emudef.h
** Author(s): Warren, Swift, Xu, Sagonas
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
** $Id: context.h,v 1.3 2005-02-13 15:28:55 vidrevich Exp $
** 
*/

#ifndef __CONTEXT_H__

#define __CONTEXT_H__

#ifdef MULTI_THREAD

#include <sys/types.h>
#include <stdio.h>

#include "debugs/debug_attv.h"

#include "basictypes.h"
#include "cell_xsb.h"
#include "memory_xsb.h"
#include "varstring_xsb.h"
#include "dynamic_stack.h"
#include "psc_xsb.h"
#include "tries.h"
#include "choice.h"
#include "macro_xsb.h"
#include "token_xsb.h"

#define MAX_REGS 257

#define MAX_FLAGS 64

struct th_context
{
/* System & user Flags */

int _call_intercept ;

/* The SLG-WAM data regions
   ------------------------ */

System_Stack	_pdl,           /* PDL                   */
		_glstack,	/* Global + Local Stacks */
		_tcpstack,	/* Trail + CP Stack      */
		_complstack;	/* Completion Stack  */

/* Argument Registers
   ------------------ */
Cell _reg[MAX_REGS];


/* Special Registers
   ----------------- */
CPtr _ereg;		/* last activation record       */
CPtr _breg;		/* last choice point            */
CPtr _hreg;		/* top of heap                  */
CPtr *_trreg;		/* top of trail stack           */
CPtr _hbreg;		/* heap back track point        */
CPtr _sreg;		/* current build or unify field */
byte *_cpreg;		/* return point register        */
byte *_pcreg;		/* program counter              */
CPtr _ebreg;		/* breg into environment stack	*/

#if (!defined(CHAT))
CPtr _efreg;
CPtr _bfreg;
CPtr _hfreg;
CPtr *_trfreg;
#endif
CPtr _pdlreg;
CPtr _openreg;
xsbBool _neg_delay;
int _xwammode;
int _level_num;
CPtr _root_address;

CPtr _ptcpreg;
CPtr _delayreg;
CPtr _interrupt_reg;
Cell _interrupt_counter;

/* Compiled trie stuff - some of this may be able to be changed to
  local variables */

Cell *_reg_array;
CPtr _reg_arrayptr;
int  _reg_array_size;

#define MAX_TRIE_REGS 500
CPtr _var_regs[MAX_TRIE_REGS];
int  _num_vars_in_var_regs ;

int  _num_heap_term_vars;
CPtr *_var_addr;
int  _var_addr_arraysz;

Cell _VarEnumerator[NUM_TRIEVARS];
Cell _TrieVarBindings[NUM_TRIEVARS];

CPtr _VarEnumerator_trail[NUM_TRIEVARS];
CPtr *_VarEnumerator_trail_top;

int _addr_stack_pointer;
CPtr *_Addr_Stack;
int _addr_stack_size;
                                                                                
int  _term_stackptr;
Cell *_term_stack;
long _term_stacksize;

int _global_num_vars;


BTNptr  _NodePtr, 
	_Last_Nod_Sav;

int     _delay_it;

/* Flag used in the locking of called tries */

int	trie_locked;

int	_AnsVarCtr;
CPtr	_ans_var_pos_reg;

struct Structure_Manager *_smBTN;
struct Structure_Manager *_smBTHT;

DynamicStack  _tstTermStack;
DynamicStack  _tstTermStackLog;
DynamicStack  _tstSymbolStack;
DynamicStack  _tstTrail;

} ;

typedef struct th_context th_context ;

#define call_intercept		(th->_call_intercept)

#define pdl			(th->_pdl)
#define glstack			(th->_glstack)
#define tcpstack		(th->_tcpstack)
#define complstack		(th->_complstack)

#define reg			(th->_reg)

#define ereg			(th->_ereg)
#define breg			(th->_breg)
#define hreg			(th->_hreg)
#define trreg			(th->_trreg)
#define hbreg			(th->_hbreg)
#define sreg			(th->_sreg)
#define cpreg			(th->_cpreg)
#define pcreg			(th->_pcreg)
#define ebreg			(th->_ebreg)

#define efreg			(th->_efreg)
#define bfreg			(th->_bfreg)
#define hfreg			(th->_hfreg)
#define trfreg			(th->_trfreg)

#define pdlreg			(th->_pdlreg)
#define openreg			(th->_openreg)
#define neg_delay		(th->_neg_delay)
#define xwammode		(th->_xwammode)
#define level_num		(th->_level_num)
#define root_address		(th->_root_address)

#define ptcpreg			(th->_ptcpreg)
#define delayreg		(th->_delayreg)
#define interrupt_reg		(th->_interrupt_reg)
#define interrupt_counter	(th->_interrupt_counter)


#define reg_array		(th->_reg_array)
#define reg_arrayptr		(th->_reg_arrayptr)
#define reg_array_size		(th->_reg_array_size)

#define var_regs		(th->_var_regs)
#define num_vars_in_var_regs	(th->_num_vars_in_var_regs)

#define num_heap_term_vars	(th->_num_heap_term_vars)
#define var_addr		(th->_var_addr)
#define var_addr_arraysz	(th->_var_addr_arraysz)

#define VarEnumerator		(th->_VarEnumerator)
#define TrieVarBindings		(th->_TrieVarBindings)

#define VarEnumerator_trail	(th->_VarEnumerator_trail)
#define VarEnumerator_trail_top	(th->_VarEnumerator_trail_top)

#define addr_stack_pointer	(th->_addr_stack_pointer)
#define Addr_Stack		(th->_Addr_Stack)
#define addr_stack_size		(th->_addr_stack_size)
                                                                                
#define term_stackptr		(th->_term_stackptr)
#define term_stack		(th->_term_stack)
#define term_stacksize		(th->_term_stacksize)

#define global_num_vars		(th->_global_num_vars)

#define NodePtr			(th->_NodePtr)
#define Last_Nod_Sav		(th->_Last_Nod_Sav)

#define delay_it		(th->_delay_it)

#define AnsVarCtr		(th->_AnsVarCtr)
#define ans_var_pos_reg		(th->_ans_var_pos_reg)

#define smBTN			(th->_smBTN)
#define smBTHT			(th->_smBTHT)

#define  tstTermStack		(th->_tstTermStack)
#define  tstTermStackLog	(th->_tstTermStackLog)
#define  tstSymbolStack		(th->_tstSymbolStack)
#define  tstTrail		(th->_tstTrail)

#define CTXT			th
#define CTXTc			th ,

#define CTXTdecl		th_context *th
#define CTXTdeclc		th_context *th ,

#else

#define CTXT
#define CTXTc

#define CTXTdecl
#define CTXTdeclc

#endif /* MULTI_THREAD */

#endif /* __CONTEXT_H__ */
