/* File:      complete_xsb_i.h
** Author(s): Juliana Freire, Kostis Sagonas, Terry Swift
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
** $Id: complete_xsb_i.h,v 1.18 2002-03-12 17:31:21 lfcastro Exp $
** 
*/

/* special debug includes */
#include "debugs/debug_delay.h"

#define FailIfAnswersFound(func) \
{ \
  CPtr tmp_breg; \
  if ((tmp_breg = func)) { \
    breg = tmp_breg; \
    Fail1; \
    XSB_Next_Instr(); \
  } \
}

#define check_fixpoint(sg, b)    find_fixpoint(sg, b)

/*----------------------------------------------------------------------*/

XSB_Start_Instr(check_complete,_check_complete)
  CPtr    cs_ptr;
  CPtr    orig_breg = breg;
  xsbBool leader = FALSE;
  VariantSF subgoal;

  /* this CP has exhausted program resolution -- backtracking occurs */
  switch_envs(breg);    /* in CHAT: undo_bindings() */
  ptcpreg = tcp_ptcp(breg);
  delayreg = tcp_pdreg(breg);

  subgoal = (VariantSF) tcp_subgoal_ptr(breg);	/* subgoal that is checked */

/*   print_subgoal(stderr, subgoal);  */

  cs_ptr = subg_compl_stack_ptr(subgoal);

  if ((prev_compl_frame(cs_ptr) >= COMPLSTACKBOTTOM || is_leader(cs_ptr))) {
    leader = 1;
  }
  
/* 
 * This code is done regardless of whether breg is a leader.  The
 * thought was that as long as we're here, why not schedule answers
 * for this subgoal.  Its purely a heuristic -- perhaps we should test
 * to see whether its inclusion makes any difference 
 */
  FailIfAnswersFound(sched_answers(subgoal, breg, leader));

  if (leader) {

    /* The following code is only done in profile mode; it keeps track
     * of characteristics of SCCs */

    ProfileLeader;

    /* check if fixpoint has been reached, otherwise schedule any
     * unresolved answers */
    FailIfAnswersFound(check_fixpoint(subgoal,breg));

#ifdef LOCAL_EVAL
    {
      CPtr cc_tbreg = orig_breg;
      
      breg = orig_breg; /* mark topmost SCC as completed */
      
      /* schedule completion suspensions if any */
      cc_tbreg = ProcessSuspensionFrames(cc_tbreg, cs_ptr);
      FailIfAnswersFound((cc_tbreg == orig_breg ? 0 : cc_tbreg));
      
      CompleteSimplifyAndReclaim(cs_ptr);

      remove_unfounded_set(cs_ptr);
      
      /* leader has non-returned answers? */
      if (has_answer_code(subgoal) && (subg_answers(subgoal) > COND_ANSWERS)) {
	reclaim_incomplete_table_structs(subgoal);
	/* schedule return of answers from trie code */
	SetupReturnFromLeader(orig_breg, cs_ptr, subgoal);
	lpcreg = (byte *) subg_ans_root_ptr(subgoal);
	XSB_Next_Instr();
      } else {  /* There are no answers to return */
	reclaim_incomplete_table_structs(subgoal);
	/* there is nothing else to be done for this SCC */
	cc_tbreg = tcp_prevbreg(orig_breg); /* go to prev SCC */
	openreg = prev_compl_frame(cs_ptr); 
	reclaim_stacks(orig_breg);
      } 
      breg = cc_tbreg; /* either orig, prev_cp or compl_susp */
    }
    
#else /* NOT LOCAL:  FOR BATCHED SCHEDULING */

    batched_compute_wfs(cs_ptr, orig_breg, subgoal);

    /* do all possible stack reclamation */
    if (openreg == prev_compl_frame(cs_ptr)) {
      if (breg == orig_breg) {	
	reclaim_stacks(orig_breg); /* lfcastro */
	breg = tcp_prevbreg(breg);
      }
    }

#endif /* ifdef LOCAL_EVAL */

  }
  else {    /* if not leader */
#ifdef LOCAL_EVAL
    makeConsumerFromGenerator(subgoal);
/*     subg_cp_ptr(subgoal) = NULL; */
#endif
    breg = tcp_prevbreg(breg); 
    /* since the trail condition registers are not reset in
     * tabletrust for local, they should be restored here
     */
#ifdef LOCAL_EVAL
    hbreg = tcp_hreg(breg);
    ebreg = tcp_ebreg(breg);
#endif
  }
  Fail1;
XSB_End_Instr()
/* end of check_complete */
