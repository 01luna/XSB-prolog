/* File:      slginsts_xsb_i.h
** Author(s): Swift, Rao, Sagonas, Freire, Cui, Johnson
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
** $Id:$
** 
*/


/* special debug includes */
#include "debugs/debug_delay.h"


#define ARITY	op1	/* register Cell */
#define Yn	op2	/* register Cell */
#define LABEL	op3	/* CPtr */

/*-------------------------------------------------------------------------*/

/*
 *  Organization of Tabling Choice Points:
 *
 *             +-------------+
 *             |             |   LOW MEMORY
 *             |    Trail    |
 *             |             |
 *             |      |      |
 *             |      V      |
 *             |             |
 *             |             |
 *             |      ^      |
 *             |      |      |
 *             |             |
 *             |  CP Stack   |
 *             |             |
 *             |             |
 *             |=============|
 *             | Rest of CPF |--- Different for Generator and Consumer
 *             |-------------|_
 *             |   INT: m    | \
 *             |   Term-m    |  |
 *             |      .      |  |- Answer Template
 *             |      .      |  |
 *             |      .      |  |
 *             |   Term-1    |_/
 *             |=============|
 *             |      .      |
 *             |      .      |
 *             |      .      |    HIGH MEMORY
 *             +-------------+
 *
 *
 *  Under CHAT, Answer Templates for producer calls are stored in the Heap:
 *
 *             +-------------+
 *             |      .      |   LOW MEMORY
 *             |      .      |
 *             |      .      |
 *             |-------------|_
 *             |   Term-m    | \
 *             |      .      |  |
 *             |      .      |  |- Answer Template
 *             |      .      |  |
 *             |   Term-1    |  |
 *             |   INT: m    |_/
 *             |-------------|
 *             |             |
 *             |    Heap     |
 *             |             |
 *             |      |      |
 *             |      V      |
 *             |             |
 *             |             |
 *             |      ^      |
 *             |      |      |
 *             |             |
 *             |    Local    |
 *             |             |    HIGH MEMORY
 *             +-------------+
 */

/*-------------------------------------------------------------------------*/

/*
 *  Instruction format:
 *    1st word: opcode X X pred_arity
 *    2nd word: pred_first_clause_label
 *    3rd word: preds_TableInfo_record
 */

case tabletry:
case tabletrysingle: {
  /*
   *  Retrieve instruction arguments and test the system stacks for
   *  overflow.  The local PCreg, "lpcreg", is incremented to point to
   *  the instruction to be executed should this one fail.
   */
  byte this_instr = *(lpcreg - 1);
  byte *continuation;
  TabledCallInfo callInfo;
  CallLookupResults lookupResults;
  SGFrame producer_sf, consumer_sf;
  CPtr answer_template;
  int template_size, attv_num, tmp;

  xwammode = 1;
  CallInfo_Arguments(callInfo) = reg + 1;
  ppad;   CallInfo_CallArity(callInfo) = (Cell)(*lpcreg++);  pad64;
  LABEL = (CPtr)(*(byte **) lpcreg);  ADVANCE_PC;
  CallInfo_TableInfo(callInfo) = (* (TIFptr *) lpcreg);  ADVANCE_PC;

  check_tcpstack_overflow;
  CallInfo_VarVectorLoc(callInfo) = top_of_cpstack;

  if ( this_instr == tabletry ) {
    /* lpcreg was left pointing to the next clause, e.g. tableretry */
    continuation = lpcreg;
    check_glstack_overflow(MAX_ARITY,lpcreg,OVERFLOW_MARGIN) ;
  }
  else
    continuation = (pb) &check_complete_inst;

  /*
   *  Perform a call-check/insert operation on the current call.  The
   *  subterms of this call which form the answer template are computed
   *  and pushed on top of the CP stack, along with its size (encoded as
   *  a Prolog INT) .  A pointer to this size, followed by the reverse
   *  template vector (as depicted above), is returned in
   *  CallLUR_VarVector(lookupResults).
   *  NOTE: ONLY if we find a new producer and ONLY if we are computing
   *    under CHAT, the answer template is pushed on the Heap rather than
   *    the CPS.  In that case, (heap - 1) points to the A.T. and
   *    CallLUR_VarVector(lookupResults) has the same value as
   *    CallInfo_VarVectorLoc(callInfo).
   */

  table_call_search(&callInfo,&lookupResults);

  producer_sf = CallLUR_Subsumer(lookupResults);
  answer_template = CallLUR_VarVector(lookupResults);

  if ( IsNULL(producer_sf) ) {

    /* New Producer
       ------------ */
    CPtr producer_cpf;
    NewProducerSF(producer_sf, CallLUR_Leaf(lookupResults),
		  CallInfo_TableInfo(callInfo));
    producer_cpf = answer_template;
    save_find_locx(ereg);
    save_registers(producer_cpf, CallInfo_CallArity(callInfo), rreg);
    SaveProducerCPF(producer_cpf, continuation, producer_sf,
		    CallInfo_CallArity(callInfo));
    push_completion_frame(producer_sf);
    ptcpreg = (CPtr)producer_sf;
    subg_cp_ptr(producer_sf) = breg = producer_cpf;
    delayreg = NULL;
    if (root_address == 0)
      root_address = breg;
    hbreg = hreg;
    lpcreg = (byte *) LABEL;	/* branch to program clause */
    goto contcase;
  }

  else if ( is_completed(producer_sf) ) {

    /* Unify Call with Answer Trie
       --------------------------- */
    if (has_answer_code(producer_sf)) {
      int i;
#ifdef DEBUG_DELAY
      fprintf(stddbg, "++Returning answers from COMPLETED table: ");
      print_subgoal(stddbg, producer_sf);
      fprintf(stddbg, "\n");
#endif
      tmp = int_val(cell(answer_template));
      get_var_and_attv_nums(template_size, attv_num, tmp);
      num_vars_in_var_regs = -1;

      /* Initialize var_regs[] as the attvs in the call. */
      if (attv_num > 0) {
	CPtr cptr;
	for (cptr = answer_template + template_size;
	     cptr > answer_template; cptr--) {
	  if (isattv(cell(cptr)))
	    var_regs[++num_vars_in_var_regs] = (CPtr) cell(cptr);
	}
	/* now num_vars_in_var_regs should be CallNumAttv - 1 */
      }

      reg_arrayptr = reg_array-1;
      for (i = 1; i <= template_size; i++) {
	pushreg(cell(answer_template+i));
      }
      delay_it = 1;
      lpcreg = (byte *)subg_ans_root_ptr(producer_sf);
      goto contcase;
    }
    else {
      Fail1;
      goto contcase;
    }
  }

  else if ( CallLUR_VariantFound(lookupResults) )

    /* Previously Seen Subsumed Call
       ----------------------------- */
    consumer_sf = CallTrieLeaf_GetSF(CallLUR_Leaf(lookupResults));

  else

    /* New Subsumed Call
       ----------------- */
    NewConsumerSF( consumer_sf, CallLUR_Leaf(lookupResults),
		   CallInfo_TableInfo(callInfo), producer_sf );

  /*
   * The call, represented by "consumer_sf", will consume from an
   * incomplete producer, represented by "producer_sf".
   */
  {
    CPtr consumer_cpf;
    ALNptr answer_set;
    BTNptr first_answer;

    /* Create Consumer Choice Point
       ---------------------------- */
    adjust_level(subg_compl_stack_ptr(producer_sf));
    save_find_locx(ereg);

    consumer_cpf = answer_template;
#if (!defined(CHAT))
    efreg = ebreg;
    if (trreg > trfreg) trfreg = trreg;
    if (hfreg < hreg) hfreg = hreg;
    SaveConsumerCPF( consumer_cpf, consumer_sf,
		     subg_asf_list_ptr(producer_sf) );
    subg_asf_list_ptr(producer_sf) = breg = bfreg = consumer_cpf;
#else
    SaveConsumerCPF( consumer_cpf, consumer_sf );
    breg = consumer_cpf;   /* save_a_consumer_copy() needs this update */
    compl_cons_copy_list(subg_compl_stack_ptr(producer_sf)) =
      nlcp_chat_area(consumer_cpf) =
        (CPtr) save_a_consumer_copy(producer_sf, CHAT_CONS_AREA);
#endif

    /* Consume First Answer or Suspend
       ------------------------------- */
    answer_set = subg_answers(consumer_sf);
    if ( IsNULL(answer_set) &&
	 ConsumerCacheNeedsUpdating(consumer_sf,producer_sf) )
      answer_set =
	table_retrieve_answers(producer_sf,consumer_sf,answer_template);

    if ( IsNonNULL(answer_set) ) {
      int tmp;
#ifdef CHAT      /* for the time being let's update consumed answers eagerly */
      nlcp_trie_return((CPtr)(&chat_get_cons_start((chat_init_pheader)nlcp_chat_area(consumer_cpf)))) =
#endif
      nlcp_trie_return(consumer_cpf) = answer_set; 
      hbreg = hreg;
      first_answer = ALN_Answer(answer_set);

      tmp = int_val(cell(answer_template));
      get_var_and_attv_nums(template_size, attv_num, tmp);
      answer_template += template_size;

      table_consume_answer(first_answer,template_size,attv_num,answer_template,
			   CallInfo_TableInfo(callInfo));

      if (is_conditional_answer(first_answer)) {
#ifdef DEBUG_DELAY
	fprintf(stddbg,
		"! POSITIVELY DELAYING in lay active (delayreg = %p)\n",
		delayreg);
	fprintf(stddbg, "\n>>>> delay_positively in lay_down_active\n");
	fprintf(stddbg, ">>>> subgoal = ");
	print_subgoal(stddbg, producer_sf);
	fprintf(stddbg, "\n");
#endif
	{
	  /*
	   * Similar to delay_positively() in retry_active, we also
	   * need to put the substitution factor of the answer,
	   * var_addr[], into a term ret/n and pass it to
	   * delay_positively().
	   */
	  if (num_heap_term_vars == 0) {
	    delay_positively(producer_sf, first_answer,
			     makestring((char *) ret_psc[0]));
	  }
	  else {
#ifndef IGNORE_DELAYVAR
	    int i;
	    CPtr temp_hreg = hreg;
	    temp_hreg = hreg;
	    new_heap_functor(hreg, get_ret_psc(num_heap_term_vars));
	    for (i = 0; i < num_heap_term_vars; i++)
	      cell(hreg++) = (Cell) var_addr[i];
	    delay_positively(producer_sf, first_answer, makecs(temp_hreg));
#else
	    delay_positively(producer_sf, first_answer,
			     makestring((char *) ret_psc[0]));
#endif /* IGNORE_DELAYVAR */
	  }
	}
      }
      lpcreg = cpreg;
    }
    else {
      breg = nlcp_prevbreg(consumer_cpf);
#ifdef CHAT
      hreg = cp_hreg(breg);
      ereg = cp_ereg(breg);
#endif
      Fail1;
    }
  }
  goto contcase;
}

/*-------------------------------------------------------------------------*/

/*
 *  Instruction format:
 *    1st word: opcode X X X
 *
 *  Description:
 *    Returns to a consumer an answer if one is available, otherwise it
 *    suspends.  Answer consumption is effected by unifying the consumer's
 *    answer template with an answer.  This instruction is encountered only
 *    by backtracking into a consumer choice point frame, either as a
 *    result of WAM- style backtracking or having been resumed via a
 *    check-complete instruction.  The CPF field "nlcp-trie-return" points
 *    to the last answer consumed.  If none have yet been consumed, then it
 *    points to the dummy answer.
 */

case answer_return: {
  SGFrame consumer_sf, producer_sf;
  ALNptr answer_set;
  BTNptr answer_leaf;
  CPtr answer_template;
  int template_size, attv_num;

  /* Locate relevant answers
     ----------------------- */
  answer_set = ALN_Next(nlcp_trie_return(breg)); /* step to next answer */
  consumer_sf = (SGFrame)nlcp_subgoal_ptr(breg);
  producer_sf = subg_producer(consumer_sf);
  answer_template = breg + NLCPSIZE;
  if ( IsNULL(answer_set) &&
       ConsumerCacheNeedsUpdating(consumer_sf,producer_sf) ) {
    switch_envs(breg);
    answer_set =
      table_retrieve_answers(producer_sf,consumer_sf,answer_template);
  }

  if ( IsNonNULL(answer_set) ) {
    int tmp;
    
    /* Restore Consumer's state
       ------------------------ */
    switch_envs(breg);
    ptcpreg = nlcp_ptcp(breg);
    delayreg = nlcp_pdreg(breg);
    restore_some_wamregs(breg, ereg);

    /* Consume the next answer
       ----------------------- */
    nlcp_trie_return(breg) = answer_set;   /* update answer continuation */
    answer_leaf = ALN_Answer(answer_set);
    tmp = int_val(cell(answer_template));
    get_var_and_attv_nums(template_size, attv_num, tmp);
    answer_template += template_size;

    table_consume_answer(answer_leaf,template_size,attv_num,answer_template,
			 subg_tif_ptr(consumer_sf));

    if (is_conditional_answer(answer_leaf)) {
      /*
       * After load_solution_trie(), the substitution factor of the
       * answer is left in array var_addr[], and its arity is in
       * num_heap_term_vars.  We have to put it into a term ret/n (on 
       * the heap) and pass it to delay_positively().
       */
      if (num_heap_term_vars == 0) {
	delay_positively(consumer_sf, answer_leaf,
			 makestring((char *) ret_psc[0]));
      }
      else {
#ifndef IGNORE_DELAYVAR
	int i;
	CPtr temp_hreg = hreg;
	new_heap_functor(hreg, get_ret_psc(num_heap_term_vars));
	for (i = 0; i < num_heap_term_vars; i++) {
	  cell(hreg++) = (Cell) var_addr[i];
	}
	delay_positively(consumer_sf, answer_leaf, makecs(temp_hreg));
#else
	delay_positively(consumer_sf, answer_leaf,
			 makestring((char *) ret_psc[0]));
#endif /* IGNORE_DELAYVAR */
      }
    }
    lpcreg = cpreg;
  }

  else {

    /* Suspend this Consumer
       --------------------- */
#ifdef CHAT	/* update consumed answers in CHAT-area before failing out */
    nlcp_trie_return((CPtr)(&chat_get_cons_start((chat_init_pheader)nlcp_chat_area(breg)))) =
      nlcp_trie_return(breg);	/* last answer consumed */
#endif
    breg = nlcp_prevbreg(breg); /* in semi-naive this execs next active */
#ifdef CHAT
    hreg = cp_hreg(breg);
    restore_trail_condition_registers(breg);
#else
    restore_trail_condition_registers(breg);
    if (hbreg >= hfreg) hreg = hbreg; else hreg = hfreg;
#endif
    Fail1;
  }
  goto contcase;
}

/*-------------------------------------------------------------------------*/

/*
 *  Instruction format:
 *    1st word: opcode X pred_arity perm_var_index
 *
 *  Description:
 *    Last instruction in each clause of a tabled predicate.  Is executed
 *    by a producing call to record a computed answer.  Two notations are
 *    made in the answer set: (1) the variable substitutions are saved in
 *    the answer trie, and (2) a new entry is made at the end of the answer
 *    list which points to the answer trie entry of (1).  All the
 *    information necessary to perform this Answer Check/Insert operation
 *    is saved in the producer's choice point frame.  This structure can be
 *    reached through the subgoal frame, which is noted in the first
 *    environment variable.  Upon derivation of the first answer, all
 *    negation suspensions of the subgoal are abolished.
 */

case new_answer_dealloc: {

  CPtr producer_cpf, producer_csf, answer_template;
  int template_size, attv_num, tmp;
  SGFrame producer_sf;
  bool isNewAnswer = FALSE;
  BTNptr answer_leaf;

  pad;
  ARITY = (Cell) (*lpcreg++);
  Yn = (Cell) (*lpcreg++);
  pad64;
  producer_sf = (SGFrame)cell(ereg-Yn);
  producer_cpf = subg_cp_ptr(producer_sf);

#ifdef DEBUG_DELAYVAR
  xsb_dbgmsg(">>>> New answer for %s subgoal: ",
	     (is_completed(producer_sf) ? "completed" : "incomplete"));
  print_subgoal(stddbg, producer_sf);
  xsb_dbgmsg("\n");
  xsb_dbgmsg(">>>>              has delayreg = %p", delayreg);
#endif

  producer_csf = subg_compl_stack_ptr(producer_sf);

  /* if the subgoal has been early completed and its space reclaimed
   * from the stacks, access to its relevant information (e.g. to its
   * substitution factor) in the stacks is not safe, so better not
   * try to add this answer; it is a redundant one anyway...
   */
  if ((subgoal_space_has_been_reclaimed(producer_sf,producer_csf)) ||
      (IsNonNULL(delayreg) && answer_is_junk(delayreg))) {
    Fail1;
    goto contcase;
  }

#ifdef CHAT
  /* answer template is now in the heap for generators */
  tmp = int_val(cell(compl_hreg(producer_csf)));
  get_var_and_attv_nums(template_size, attv_num, tmp);
  answer_template = compl_hreg(producer_csf)-1;
#else
  answer_template = producer_cpf + TCP_SIZE + (Cell) ARITY;
  tmp = int_val(*answer_template);
  get_var_and_attv_nums(template_size, attv_num, tmp);
  answer_template += template_size;
#endif

#ifdef DEBUG_DELAYVAR
    xsb_dbgmsg(">>>> ARITY = %d; Yn = %d", (int)ARITY, (int)Yn);
#endif

#ifdef DEBUG_DELAY
  fprintf(stddbg, "\t--> This answer for ");
  print_subgoal(stddbg, producer_sf);
  if (delayreg != NULL) {
    fprintf(stddbg, " has delay list = ");
    print_delay_list(stddbg, delayreg);
  }
  else
    fprintf(stddbg, " has no delay list");
#endif

#ifdef DEBUG_DELAYVAR
  fprintf(stddbg, "\n>>>> (before variant_answer_search) template_size = %d\n",
	  (int)template_size);
  {
    int i;
    for (i = 0; i < template_size; i++) {
      fprintf(stddbg, ">>>> answer_template[%d] = ", i);
      printterm((Cell)(answer_template - i), 1, 25);
      fprintf(stddbg, "\n");
    }
  }
#endif

  answer_leaf = table_answer_search( producer_sf, template_size, attv_num,
				     answer_template, &isNewAnswer );

  if ( isNewAnswer ) {   /* go ahead -- look for more answers */
#ifdef CHAT
    delayreg = compl_pdreg(producer_csf); /* restore delayreg of parent */
#else
    delayreg = tcp_pdreg(producer_cpf);      /* restore delayreg of parent */
#endif
    if (is_conditional_answer(answer_leaf)) {	/* positive delay */
#ifndef LOCAL_EVAL
#ifdef DEBUG_DELAYVAR
      fprintf(stddbg, "\n>>>> delay_positively in new_answer_dealloc\n");
#endif
      /*
       * The new answer for this call is a conditional one, so add it
       * into the delay list for the parent predicate.  Notice that
       * delayreg has already been restored to the delayreg of parent.
       *
       * This is the new version of delay_positively().  Here,
       * ans_var_pos_reg is passed from variant_answer_search().  It is a
       * pointer to the heap where the substitution factor of the
       * answer was saved as a term ret/n (in variant_answer_search()).
       */
#ifndef IGNORE_DELAYVAR
      if (isinteger(cell(ans_var_pos_reg))) {
	delay_positively(producer_sf, answer_leaf,
			 makestring((char *) ret_psc[0]));
      }
      else 
	delay_positively(producer_sf, answer_leaf, makecs(ans_var_pos_reg));
#else
	delay_positively(producer_sf, answer_leaf,
			 makestring((char *) ret_psc[0]));
#endif /* IGNORE_DELAYVAR */
#endif /* LOCAL_EVAL */
    }
    else {
      if (template_size == 0) {
	/*
	 *  The table is for a ground call which we just proved true.
	 *  (We entered an ESCAPE Node, above, to note this fact in the
	 *  table.)  As we only need to do this once, we perform "early
	 *  completion" by ignoring the other clauses of the predicate
	 *  and setting the failure continuation (next_clause) field of
	 *  the CPF to a check_complete instr.
	 */
	perform_early_completion(producer_sf, producer_cpf);
#if (defined(LOCAL_EVAL) && !defined(CHAT))
	breg = producer_cpf;
#endif
      }
    }
#ifdef LOCAL_EVAL
    Fail1;	/* and do not return answer to the generator */
#else
#ifdef CHAT
    ptcpreg = compl_ptcp(producer_csf);
#else
    ptcpreg = tcp_ptcp(producer_cpf);
#endif
    cpreg = *((byte **)ereg-1);
    ereg = *(CPtr *)ereg;
    lpcreg = cpreg; 
#endif
  }
  else     /* repeat answer -- ignore */
    Fail1;
  goto contcase;
}

/*-------------------------------------------------------------------------*/

/*
 *  Instruction format:
 *    1st word: opcode X X pred_arity
 *    2nd word: pred_next_clause_label
 *
 *  Description:
 *    Store the predicate's arity in "op1", update the failure continuation
 *    to the instruction following this one, and set the program counter to
 *    the predicate's next code subblock to be executed, as pointed to by
 *    the second argument to this instruction.  Finally, restore the state
 *    at the point of choice and continue execution using the predicate's
 *    next code subblock.
 */

case tableretry:
  ppad; op1byte;
  pad64;
  tcp_pcreg(breg) = lpcreg+sizeof(Cell);
  lpcreg = *(pb *)lpcreg;
  restore_type = 0;
  goto table_restore_sub;

/*-------------------------------------------------------------------------*/

/*
 *  Instruction format:
 *    1st word: opcode X X pred_arity
 *
 *  Description:
 *    Store the predicate's arity in "op1", update the failure continuation
 *    to a check_complete instruction, and set the program counter to the
 *    predicate's last code subblock to be executed, as pointed to by the
 *    second argument to this instruction.  Finally, restore the state at
 *    the point of choice and continue execution with this last code
 *    subblock.
 */

case tabletrust:
    ppad; op1byte;
    pad64;
    tcp_pcreg(breg) = (byte *) &check_complete_inst;
    lpcreg = *(pb *)lpcreg;
#if (defined(LOCAL_EVAL) || defined(CHAT))
    /* trail cond. registers should not be restored here for Local */
    restore_type = 0;
#else
    restore_type = 1;
#endif
    goto table_restore_sub;

/*-------------------------------------------------------------------------*/

#include "complete_xsb_i.h"

/*-------------------------------------------------------------------------*/

case resume_compl_suspension:
#ifdef DEBUG_DELAYVAR
      fprintf(stddbg, ">>>> resume_compl_suspension is called\n");
#endif
#ifdef CHAT
  {
    chat_init_pheader chat_area;

    switch_envs(breg);
    ptcpreg = csf_ptcp(breg);
    delayreg = csf_pdreg(breg);
    neg_delay = csf_neg_loop(breg);
    restore_some_wamregs(breg, ereg); /* this also restores cpreg */
    chat_area = (chat_init_pheader)csf_chat_area(breg);
    chat_restore_compl_susp_trail(chat_area); /* the chat area is freed here */
    if ((chat_area = (chat_init_pheader)csf_prevcsf(breg)) != NULL) {
      chat_update_compl_susp(chat_area);
    } else {
      breg = csf_prev(breg);  /* forget this CP; simulates Fail1 */
    }
    lpcreg = cpreg;
    goto contcase;
  }
#else
  {
    CPtr csf = cs_compsuspptr(breg);
    /* Switches the environment to a frame of a subgoal that was	*/
    /* suspended on completion, and sets the continuation pointer.	*/
    check_glstack_overflow(MAX_ARITY,lpcreg,OVERFLOW_MARGIN);
    freeze_and_switch_envs(csf, COMPL_SUSP_CP_SIZE);
    ptcpreg = csf_ptcp(csf);
    neg_delay = csf_neg_loop(csf);
    delayreg = csf_pdreg(csf);
    cpreg = csf_cpreg(csf); 
    ereg = csf_ereg(csf);
    ebreg = csf_ebreg(csf);
    hbreg = csf_hreg(csf);
    save_find_locx(ereg);
    hbreg = hreg;
    if (csf_prevcsf(csf) != NULL) {
      cs_compsuspptr(breg) = csf_prevcsf(csf);
    } else {
      breg = cs_prevbreg(breg);
    }
    lpcreg = cpreg;
    goto contcase;
  }
#endif

/*----------------------------------------------------------------------*/
