/* File:      xmacro.h
** Author(s): Swift, Sagonas, Rao, Freire
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
** $Id: xmacro.h,v 1.1.1.1 1998-11-05 16:55:26 sbprolog Exp $
** 
*/


/*----------------------------------------------------------------------*/
/*  Table Information Structure.					*/
/*----------------------------------------------------------------------*/

typedef struct tab_info {
  CPtr next_tip;	/* pointer to next table info frame */
  CPtr call_trie_root;	/* pointer to the root of the call trie */
  Psc  psc_ptr;		/* pointer to the PSC record of the subgoal */
} *tab_inf_ptr;

#define ti_next_tip(TipPtr)		((tab_inf_ptr)TipPtr)->next_tip
#define ti_call_trie_root(TipPtr)	((tab_inf_ptr)TipPtr)->call_trie_root
#define ti_psc_ptr(TipPtr)		((tab_inf_ptr)TipPtr)->psc_ptr

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

typedef struct ascc_edge *EPtr;
typedef struct completion_stack_frame *ComplStackFrame;

/*----------------------------------------------------------------------*/
/*  Approximate Strongly Connected Component Edge Structure.		*/
/*----------------------------------------------------------------------*/

struct ascc_edge {
  ComplStackFrame ascc_node_ptr;
  EPtr next;
};

#define ASCC_EDGE_SIZE		(sizeof(struct ascc_edge)/sizeof(CPtr))

#define edge_to_node(e)		((EPtr)(e))->ascc_node_ptr
#define next_edge(e)		((EPtr)(e))->next

/*----------------------------------------------------------------------*/
/*  Completion Stack Structure (ASCC node structure).			*/
/*									*/
/*  NOTE: Please make sure that fields "DG_edges" and "DGT_edges" are	*/
/*	  the last fields of the structure, and each time you modify	*/
/*	  this structure you also update the definition of the		*/
/*	  "compl_stk_frame_field" array defined in file debug.c		*/
/*----------------------------------------------------------------------*/

#define DELAYED		-1

/*----------------------------------------------------------------------*/

typedef struct subgoal_frame *SGFrame;

/*----------------------------------------------------------------------*/

struct completion_stack_frame {
  SGFrame subgoal_ptr;
  int     level_num;
  ALPtr   del_ret_list;  /* to reclaim deleted returns */
  int     visited;
  EPtr    DG_edges;
  EPtr    DGT_edges;
} ;

#define COMPLFRAMESIZE	(sizeof(struct completion_stack_frame)/sizeof(CPtr))

#define compl_subgoal_ptr(b)	((ComplStackFrame)(b))->subgoal_ptr
#define compl_level(b)		((ComplStackFrame)(b))->level_num
#define compl_del_ret_list(b)	((ComplStackFrame)(b))->del_ret_list
#define compl_visited(b)	((ComplStackFrame)(b))->visited
#define compl_DG_edges(b)	((ComplStackFrame)(b))->DG_edges
#define compl_DGT_edges(b)	((ComplStackFrame)(b))->DGT_edges

#define next_compl_frame(b)	((CPtr)(b))-COMPLFRAMESIZE
#define prev_compl_frame(b)	((CPtr)(b))+COMPLFRAMESIZE

/*
 *  The overflow test MUST be placed after the initialization of the
 *  ComplStackFrame in the current implementation.  This is so that the
 *  corresponding subgoal which points to this frame can be found and its
 *  link can be updated if an expansion is required.  This was the simplest
 *  solution to not leaving any dangling pointers to the old area.
 */
#define	push_completion_frame(subgoal)	\
  level_num++; \
  openreg -= COMPLFRAMESIZE; \
  compl_subgoal_ptr(openreg) = subgoal; \
  compl_level(openreg) = level_num; \
  compl_del_ret_list(openreg) = (ALPtr)NULL; \
  compl_visited(openreg) = FALSE; \
  compl_DG_edges(openreg) = compl_DGT_edges(openreg) = (EPtr) NULL; \
  check_completion_stack_overflow

#define compact_completion_frame(cp_frame,cs_frame,subgoal)	\
  compl_subgoal_ptr(cp_frame) = subgoal;			\
  compl_level(cp_frame) = compl_level(cs_frame);		\
  compl_visited(cp_frame) = FALSE;				\
  compl_DG_edges(cp_frame) = compl_DGT_edges(cp_frame) = (EPtr) NULL; \
  cp_frame = next_compl_frame(cp_frame)

/*----------------------------------------------------------------------*/
/*  Subgoal (Call) Structure.						*/
/*----------------------------------------------------------------------*/

#include "slgdelay.h"

/*----------------------------------------------------------------------*/

     /* should not change the order - unless tables.P is updated accordingly
      * also, if adding fields, add to the end! 
      */
struct subgoal_frame {
  SGFrame next_subgoal;
  NODEptr ans_root_ptr;	/* Root of the return trie */
  CPtr asf_list_ptr;	/* Pointer to list of (CP) active subgoal frames */
  tab_inf_ptr tip_ptr;	/* Used only in remove_open_tries */
  CPtr compl_stack_ptr;	/* Pointer to subgoal's completion stack frame */
  CPtr compl_suspens_ptr; /* CP Stack ptr */
  ALPtr ans_list_ptr;	/* Pointer to the list of returns in the ret trie */
  SGFrame prev_subgoal;
  NODEptr leaf_ptr;	/* Used only in remove_open_tries */
  CPtr cp_ptr;          /* Pointer to the Generator CP */
  ALPtr ans_list_tail;  /* pointer to the tail of the answer list */
  CPtr compl_flag;      /* jf: indicate whether subg is completed */
  IDE  nide;		/* pointer to a negative IDE */
} ;

#define CALLSTRUCTSIZE	(sizeof(struct subgoal_frame)/sizeof(CPtr))

#define subg_next_subgoal(b)	((SGFrame)(b))->next_subgoal
#define subg_prev_subgoal(b)	((SGFrame)(b))->prev_subgoal
#define subg_ans_root_ptr(b)	((SGFrame)(b))->ans_root_ptr
#define subg_asf_list_ptr(b)	((SGFrame)(b))->asf_list_ptr
#define subg_tip_ptr(b)		((SGFrame)(b))->tip_ptr
#define subg_leaf_ptr(b)	((SGFrame)(b))->leaf_ptr
/* use this for mark as completed == 0 */
#define subg_compl_stack_ptr(b)	((SGFrame)(b))->compl_stack_ptr
#define subg_compl_susp_ptr(b)	((SGFrame)(b))->compl_suspens_ptr
#define subg_ans_list_ptr(b)	((SGFrame)(b))->ans_list_ptr
#define subg_cp_ptr(b)		((SGFrame)(b))->cp_ptr
#define subg_ans_list_tail(b)	((SGFrame)(b))->ans_list_tail
/* jf: 072295 */
#define subg_compl_flag(b)	((SGFrame)(b))->compl_flag
#define subg_nide(b)		((SGFrame)(b))->nide
extern SGFrame subg_structure_list;
extern ALPtr empty_return();
#define subg_answers(subg) aln_next_aln(subg_ans_list_ptr(subg))

/*
 *  Creates a new subgoal frame, setting the first arg to point to it, and
 *  inserts it into the global subgoal list.  The TIP and leaf ptr fields are
 *  given useful values, while the completion stack frame pointer is set to
 *  the next available (frame) location on the stack, but the space is not yet
 *  allocated to one.
 */

#define create_subgoal_frame(storeptr,LeafPtr){\
   SGFrame NewFrame;\
   if ((NewFrame = (SGFrame)malloc(sizeof(struct subgoal_frame))) == NULL){\
     xsb_exit("Out of Memory\n");\
   }\
   else{\
	storeptr = (Cell) NewFrame;\
	if (subg_structure_list != NULL)\
	  subg_prev_subgoal(subg_structure_list) = NewFrame;\
	subg_ans_root_ptr(NewFrame)= NULL;\
	subg_asf_list_ptr(NewFrame) = (CPtr) 0; \
	subg_tip_ptr(NewFrame) = UglyHackForTip;\
	subg_leaf_ptr(NewFrame) =  LeafPtr; \
	subg_compl_stack_ptr(NewFrame)= (CPtr)(openreg - COMPLFRAMESIZE); \
	subg_compl_susp_ptr(NewFrame)= NULL;\
	/****subg_ans_list_ptr(NewFrame) = NULL; *******/ \
	subg_ans_list_ptr(NewFrame) = (ALPtr) empty_return(); \
	subg_cp_ptr(NewFrame) = NULL;\
	subg_next_subgoal(NewFrame) = subg_structure_list;\
	subg_structure_list = NewFrame;\
	subg_prev_subgoal(NewFrame) = NULL;\
        subg_ans_list_tail(NewFrame) = NULL; /* REV_ANSWER_LIST */\
	subg_compl_flag(NewFrame) = (CPtr) 0; /* jf: 072295 */\
	subg_nide(NewFrame) = NULL;\
  }\
}

#define free_subgoal_frame(x){\
 if(subg_prev_subgoal(x) == NULL) \
    subg_structure_list = subg_next_subgoal(x);\
 else\
    subg_next_subgoal(subg_prev_subgoal(x)) = subg_next_subgoal(x);\
 if(subg_next_subgoal(x) != NULL)\
    subg_prev_subgoal(subg_next_subgoal(x)) = subg_prev_subgoal(x);\
 free(x);\
                            }

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#define set_min(a,b,c)	if (b < c) a = b; else a = c

#define tab_level(SUBG_PTR)	\
	compl_level((subg_compl_stack_ptr(SUBG_PTR)))
#define next_tab_level(SUBG_PTR) \
	compl_level(prev_compl_frame(subg_compl_stack_ptr(SUBG_PTR)))

#define is_leader(SUBG_PTR)	\
	(next_tab_level(SUBG_PTR) < tab_level(SUBG_PTR))

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#define NO_ANSWERS	NULL
#define UNCOND_ANSWERS	(ALPtr)1
#define COND_ANSWERS	(ALPtr)2

/*----------------------------------------------------------------------*/
/* The following 2 macros are to be used for incomplete subgoals.	*/
/*----------------------------------------------------------------------*/

#define has_answers(SUBG_PTR)	\
	(subg_answers(SUBG_PTR) != NO_ANSWERS)
#define has_no_answers(SUBG_PTR)	\
	(subg_answers(SUBG_PTR) == NO_ANSWERS)

/*----------------------------------------------------------------------*/
/* The following 5 macros should be used only for completed subgoals.	*/
/*----------------------------------------------------------------------*/

#define has_answer_code(SUBG_PTR)	\
	(subg_ans_root_ptr(SUBG_PTR) != NULL)
#define subgoal_fails(SUBG_PTR)		\
	(subg_ans_root_ptr(SUBG_PTR) == NULL)
#define subgoal_unconditionally_succeeds(SUBG_PTR)	\
	(is_unconditional_answer(subg_ans_root_ptr(SUBG_PTR)))

#define mark_subgoal_failed(SUBG_PTR)	\
	(subg_ans_root_ptr(SUBG_PTR) = NULL)

#define neg_simplif_possible(SUBG_PTR)	\
	((subgoal_fails(SUBG_PTR)) && (subg_nide(SUBG_PTR) != NULL))

/*----------------------------------------------------------------------*/

#define is_completed(SUBG_PTR)	\
        ((Integer) subg_compl_flag(SUBG_PTR) < 0)

#define mark_as_completed(SUBG_PTR) {\
        subg_compl_flag(SUBG_PTR) = (CPtr) -1;  \
        reclaim_del_ret_list((SGFrame)SUBG_PTR); } 

#define mark_delayed(csf1, csf2, susp) { \
	compl_visited(csf1) = DELAYED; \
	compl_visited(csf2) = DELAYED; \
	csf_neg_loop(susp) = TRUE; }

/*----------------------------------------------------------------------*/

#define reclaim_subg_space(SUBG_PTR) {\
	  reclaim_ans_list_nodes(SUBG_PTR); \
	}

/*----------------------------------------------------------------------*/

#define adjust_level(CS_FRAME) \
    xtemp2 = (CPtr) compl_level(CS_FRAME);	\
    if ((Integer) xtemp2 < compl_level(openreg)) {	\
      for (xtemp1 = CS_FRAME;			\
	   compl_level(xtemp1) >= (Integer) xtemp2 && xtemp1 >= openreg; \
	   xtemp1 = next_compl_frame(xtemp1)) {	\
	     compl_level(xtemp1) = (Integer) xtemp2;\
      }						\
    }

/*----------------------------------------------------------------------*/

#define reset_freeze_registers \
    bfreg = (CPtr)(tcpstack.high) - CP_SIZE; \
    trfreg = (CPtr *)(tcpstack.low); \
    hfreg = (CPtr)(glstack.low); \
    efreg = (CPtr)(glstack.high) - 1; \
    level_num = xwammode = 0; \
    root_address = ptcpreg = NULL

    /* JF: added parameter 0912 */
#define adjust_freeze_registers(tcp) \
    if (bfreg < tcp_bfreg(tcp)) { bfreg = tcp_bfreg(tcp); }	 \
    if (trfreg > tcp_trfreg(tcp)) { trfreg = tcp_trfreg(tcp); }\
    if (hfreg > tcp_hfreg(tcp)) { hfreg = tcp_hfreg(tcp); }	 \
    if (efreg < tcp_efreg(tcp)) { efreg = tcp_efreg(tcp); }


/* JF: added factoring out repeated code - reclaims all stack
 * to the state they were when the subgoal corresponding to tcp
 * was created
*/
#define reclaim_stacks(tcp) \
  if (tcp == root_address) { \
    reset_freeze_registers; \
    /* fprintf(stderr,"reset registers.... \n"); */ \
  } \
  else { \
    adjust_freeze_registers(tcp); \
    /* fprintf(stderr,"adjust registers.... \n"); */ \
  } \


#define pdlpush(cell)	*(pdlreg) = cell;  pdlreg--

#define pdlpop		*(++pdlreg)

#define pdlempty	(pdlreg == (CPtr)(pdl.high) - 1)

#define resetpdl \
   if (pdlreg < (CPtr) pdl.low) \
     xsb_exit("pdlreg grew too much"); \
   else (pdlreg = (CPtr)(pdl.high) - 1)


/* second arg is just a temporary variable */
#define table_undo_bindings(old_trreg, traddr) \
  while (trreg > old_trreg) {\
     traddr = *(trreg-2);\
     untrail(traddr);\
     trreg = (CPtr *) *trreg;\
  }

#define remove_open_tables_loop(Endpoint) remove_open_tries(Endpoint)

#define remove_open_tables() remove_open_tries(COMPLSTACKBOTTOM)

