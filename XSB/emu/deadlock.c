#include "xsb_config.h"

#ifdef SHARED_COMPL_TABLES
                                                                                
#include "context.h"
#include "basicdefs.h"
#include "basictypes.h"
#include "cell_xsb.h"
#include "choice.h"
#include "cut_xsb.h"
#include "binding.h"
#include "sw_envs.h"
#include "deadlock.h"
#include "debug_xsb.h"
#include "macro_xsb.h"
#include "thread_xsb.h"
#include "trie_internals.h"

counter num_deadlocks = 0;
counter num_suspends = 0;

int would_deadlock( int t1, int t2 )
{
	int t = t1;
                                                                                
        while( valid_tid(t) )
                if( t == t2 )
		{	num_deadlocks++;
                        return TRUE ;
		}
                else 
			t = get_waiting_for_tid(t) ;

	num_suspends++;
        return FALSE ;
}

static VariantSF bottom_leader(th_context *th, VariantSF to_sgf)
{
	CPtr csf = subg_compl_stack_ptr(to_sgf) ;

	while( prev_compl_frame(csf) < COMPLSTACKBOTTOM && !is_leader(csf) )
		csf = prev_compl_frame(csf) ;
	return compl_subgoal_ptr(csf) ;
}

static void ReclaimDSandMarkReset(th_context *th, VariantSF to, int leader)
{
	CPtr csf = openreg ;
	for(;;)
	{	if( !is_completed(compl_subgoal_ptr(csf)))
		/* Handle early completion */
		{	subg_grabbed(compl_subgoal_ptr(csf)) = TRUE ;
			subg_tid(compl_subgoal_ptr(csf)) = leader ;
    			subg_asf_list_ptr(compl_subgoal_ptr(csf)) = NULL;
    			subg_compl_susp_ptr(compl_subgoal_ptr(csf)) = NULL;
		}
        	if( compl_subgoal_ptr(csf) == to ) 
			break;
                csf = prev_compl_frame(csf) ;
        }
}

static void reset_thread( th_context *th, th_context *ctxt, VariantSF sgf,
			  VariantSF *resetsgf )
{
	CPtr tbreg ;
	/* if the subgoal has not yet been computed, the
	   thread should not be reset */
	if( subg_grabbed(sgf) )
	{	subg_tid(sgf) = xsb_thread_id ;
		return ;
	}
	sgf = bottom_leader(ctxt, sgf) ;
        *resetsgf = sgf ;
	ReclaimDSandMarkReset(ctxt, sgf, xsb_thread_id);
	/* trick to use other thread's context */
	th = ctxt ;
        /* reset the stacks by restoring the generator cp of this sg */
	breg = subg_cp_ptr(sgf) ;
	tbreg = breg ;
	openreg = prev_compl_frame(subg_compl_stack_ptr(sgf)) ;
        switch_envs(tbreg);
  	ptcpreg = tcp_ptcp(tbreg);
  	delayreg = tcp_pdreg(tbreg);
        reclaim_stacks(tbreg) ;
	restore_some_wamregs(tbreg, ereg);
  	/* restore_trail_condition_registers */
	ebreg = cp_ebreg(tcp_prevbreg(tbreg));
  	hbreg = cp_hreg(tcp_prevbreg(tbreg));

        pcreg = (byte *)tcp_reset_pcreg(tbreg) ;
	table_restore_registers(tbreg, pcreg[3], reg);

	/* delete the generator cp */
        breg = tcp_prevbreg(breg) ; 
}

void reset_leader( th_context *th )
{
	VariantSF resetsgf ;
	reset_thread( th, th, compl_subgoal_ptr(openreg), &resetsgf );
}
				

void reset_other_threads( th_context *th, th_context *ctxt, VariantSF sgf )
{
	th_context *tmp ;
	VariantSF resetsgf ;

	tmp = ctxt;
	while( tmp != th )
	{
		tmp->tmp_next = find_context(tmp->waiting_for_tid) ;
		tmp->waiting_for_tid = -1 ;
		tmp = tmp->tmp_next ;
	}

	pthread_mutex_unlock( &completing_mut );
	tmp = ctxt;
        while( tmp != th )
	{	reset_thread( th, tmp, sgf, &resetsgf );
		sgf = tmp->waiting_for_subgoal;
                tmp->waiting_for_subgoal = resetsgf ;
                tmp->waiting_for_tid = xsb_thread_id ;
		tmp = tmp->tmp_next;
	}
}

#endif
