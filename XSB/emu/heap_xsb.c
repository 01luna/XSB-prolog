/* File:      heap_xsb.c
** Author(s): Bart Demoen, Kostis Sagonas
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1998
** Copyright (C) K.U. Leuven, 1998-1999
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
** $Id: heap_xsb.c,v 1.32 2002-02-26 18:22:07 lfcastro Exp $
** 
*/


/*************************************************************************
 * This module provides:

	reallocation of the heap/environment area
	-----------------------------------------
	Function glstack_realloc(new_size,arity)
		originally written by E. Johnson in file
		memory_xsb.c, but completely redone by Bart Demoen

	heap garbage collection
	-----------------------
	Function gc_heap(arity) - currently only supported for CHAT
	   To understand the usefulness logic, see paper:
		B. Demoen and K. Sagonas.
		Memory Management for Prolog with Tabling.
		in Proceedings of the 1998 ACM SIGPLAN International
		Symposium on Memory Management, Vancouver, B.C., Canada,
		Oct. 1998. ACM Press. p. 97-106

	   To understand the implementation and for additional information see:
		B. Demoen and K. Sagonas.
		Heap Garbage Collection in XSB: Practice and Experience.
		CW report 272, September 1998; finished February 1999.


	Function slide_heap() implements a sliding collector a la Morris
	   It was mostly written by Bart Demoen
	       for a Prolog specific one see paper:
	       K. Appleby, M. Carlsson, S. Haridi, and D. Sahlin.
	       Garbage Collection for Prolog Based on WAM.
	       Communications of the ACM, 31(6):719--741, June 1988.


	Function copy_heap() implements a copying collector a la Cheney
	   It was mostly written by Kostis Sagonas
               for a Prolog specific one see paper:
	       J. Bevemyr and T. Lindgren.
	       A Simple and Efficient Copying Garbage Collector for Prolog.
	       In M. Hermenegildo and J. Penjam, editors,
	       Proceedings of the Sixth International Symposium on
	       Programming Language Implementation and Logic Programming,
	       number 844 in LNCS, pages 88--101, Madrid, Spain, Sept.  1994.
	       Springer-Verlag.


	printing routines for some areas
		print_heap
		print_ls
		print_cp
		print_regs
		print_tr
		print_all_stacks: does all of the above
	some - maybe all - of these were somewhere in the system already
		but weren't entirely what we needed


Todo:
        adapt the garbage collectors to SLG-WAM
	provide a decent user interface to the garbage collector
	integrate with compiler

****************************************************************************/

/* xsb_config.h must be the first #include. Pls don't move it. */
#include "xsb_config.h"
#include "xsb_debug.h"

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

/* Take care of the time.h business */
/* #include "xsb_time.h" */
/* But I need time.h, not sys/time.h here! -lfcastro */
#include <time.h>

#include "auxlry.h"
#include "cell_xsb.h"
#include "memory_xsb.h"
#include "inst_xsb.h"

/* For Reallocation Routines
   ------------------------- */
#include <stdio.h>         /* for printf and friends */

#include "register.h"      /* breg, trreg */
#include "psc_xsb.h"       /* needed by "tries.h" and "macro_xsb.h" */
#include "tries.h"         /* needed by "choice.h" */
#include "choice.h"        /* choice point structures and macros */
#include "error_xsb.h"     /* xsb_exit() and friends */
#include "macro_xsb.h"     /* Completion Stack and Subgoal Frame def's */
#include "realloc.h"       /* Heap - ls reallocation macros */
#include "chat.h"          /* CHAT related declarations */
#include "flags_xsb.h"     /* for checking whether functionality is enabled */
#include "heap_xsb.h"
#include "io_builtins_xsb.h"
#include "subp.h"          /* for attv_interrupts[][] */
#include "binding.h"       /* for PRE_IMAGE_TRAIL */

/*=========================================================================*/

/* this might belong somewhere else (or should be accessible to init.c),
   but in the meantime, this will do */
#ifdef GC
static float mark_threshold = 0.9F;
#endif

#ifdef DEBUG
#define GC_PROFILE
#endif

#ifdef GC_PROFILE

static char count_chains=0, examine_data=0, verbose_gc=0;
unsigned long chains[64];
unsigned long tag_examined[9];
unsigned long deep_mark;
unsigned long current_mark;
unsigned long old_gens;
unsigned long current_gen;
CPtr start_hbreg;
unsigned long functor;
unsigned long chain_from_ls;
unsigned long active_cps, frozen_cps;
void print_cpf_pred(CPtr cpf);

#endif /* GC_PROFILE */

/* #define REALLOC_DEBUG */

/*=========================================================================*/

/* to choose between copying or sliding collector:
   its value is determined based on the the value of flags[GARBAGE_COLLECT] */
static xsbBool slide;

/* max value of active delay register fields in CHAT areas.  it should
   not be bigger than the margin (2nd arg) of gc_heap instruction minus
   stuff (like aregs) which are put on heap in the beginning of a sliding
   collection. and it should be used only for a sliding collector. */
#ifdef CHAT
#define MAX_DREG_MARKS 1000
#endif


#ifdef GC
/* measuring fragmentation without collection - it also sets slide = 0 */
static const int fragmentation_only = 0;
#endif
		      
/* choose to do early reset or not */
/* #define EARLY_RESET 1 */


/* expresses how often early reset of a trailed heap cell occurs */
static int heap_early_reset;

/* expresses how often early reset of a trailed local stack cell occurs */
static int ls_early_reset;


/* ways to count gc and control the output during a gc */
static int printnum = 0 ;
static int num_gc = 0 ;

#ifdef DEBUG
static int print_at = 0 ; /* at the print_at-th gc, the stacks are printed */
static int print_after = 0 ; /* if non zero, print all after this numgc */
static int print_anyway = 0 ;

#define print_on_gc \
        ((print_at == num_gc) \
	 || ((print_after > 0) && (print_after <= num_gc)) \
	 || print_anyway)
#else
#define print_on_gc 0
#endif

/* if SAFE_GC is defined, some more checks are made after gargage collection */
/* #define SAFE_GC */
#define DEBUG_ASSERTIONS

/* if VERBOSE_GC is defined, garbage collection prints its statistics */
/* #define VERBOSE_GC */


/*---------------------------------------------------------------------------*/
/* global variables for top and bottom of some areas + macro to compute them */
/*---------------------------------------------------------------------------*/

static CPtr heap_bot,heap_top,
  ls_bot,ls_top,
  tr_bot,tr_top,
  cp_bot,cp_top,
  compl_top,compl_bot;

#define stack_boundaries \
  heap_top = hreg; \
  ls_top = top_of_localstk ; \
  if (ls_top < heap_top) xsb_exit("Heap and local stack are clobbered"); \
  heap_bot = (CPtr)glstack.low ; \
  ls_bot = (CPtr)glstack.high - 1 ; \
  tr_top = (CPtr)(top_of_trail) /*- 1*/ ; \
  tr_bot = (CPtr)tcpstack.low ; \
  cp_bot = (CPtr)tcpstack.high - 1 ; \
  cp_top = top_of_cpstack ; \
  compl_top = (CPtr)complstack.low ; /* NOT top_of_complstk !!! */\
  compl_bot = (CPtr)complstack.high ;

#define points_into_heap(p)  ((heap_bot <= p) && (p < heap_top))
#define points_into_ls(p)    ((ls_top <= p) && (p <= ls_bot))
#define points_into_cp(p)    ((cp_top <= p) && (p <= cp_bot))
#define points_into_tr(p)    ((tr_bot <= p) && (p <= tr_top))
#define points_into_compl(p) ((compl_top <= p) && (p <= compl_bot))

/*======================================================================*/
/* global variables used for statistics.                                */
/*======================================================================*/

static double total_time_gc = 0 ;
static unsigned long total_collected = 0 ;

/*----------------------------------------------------------------------*/
/* marker bits in different areas: the mark bit for the CHAT areas is   */
/* in the CHAT areas themselves.                                        */
/*----------------------------------------------------------------------*/

static char *heap_marks  = NULL ;
static char *ls_marks    = NULL ;
static char *tr_marks    = NULL ;
static char *cp_marks    = NULL ;
#ifdef CHAT
static char *compl_marks = NULL ;
#endif

#define INDIRECTION_SLIDE
#ifdef INDIRECTION_SLIDE
static CPtr *slide_buf= NULL;
static unsigned long slide_top = 0;
static int slide_buffering = 0;
static unsigned long slide_buf_size = 0;
#endif

#define MARKED    1
#define TRAIL_PRE 2
#define CHAIN_BIT 4                            

/* in the absence of serious bugs, the test is an invariant of the WAM */
#ifdef DEBUG_ASSERTIONS
#define testreturnit(retp)   if (points_into_heap(retp)) return(retp)
#else
#define testreturnit(retp)   return(retp)
#endif

#ifdef CHAT
/* space to temporarily keep answer template pointers from consumer
   CPs to allow for chaining --lfcastro */
CPtr *at_malloc=NULL, *at_start=NULL; 
unsigned long at_area_size = 0, at_malloc_size = 0;
#endif

/*=========================================================================*/
/* GC-specific includes */
#include "gc_profile.h"
#include "gc_mark.h"
#include "gc_print.h"
#include "gc_slide.h"
#include "gc_copy.h"
/*=========================================================================*/


/*==========================================================================
        new_size = new size of heap + environmentstack
        arity = number of argument registers in use at moment of call

        assumption: the argument registers in use are
                        reg+1 up to reg+arity included

        if you call glstack_realloc with new_size == the current size,
                you will get a reallocated area !

        Re-allocate the space for the Global and Local Stacks' data area
        to "new_size" K-byte blocks.


        Optimizations:
                if the heap hasn't been moved, then there is no need to change:
                        o pointers INTO the heap;
                        o pointers IN the heap (because there shouldn't be
                                any pointing into the local stack).
*/


/*----------------------------------------------------------------------*/
/* reallocation for CHAT areas                                          */
/*----------------------------------------------------------------------*/

#ifdef CHAT

inline static void chat_relocate_region(CPtr *startp, int len,
					int heap_offset, int local_offset)
{ int j;
  Cell cell_val;

  while (len)
    { 
      if (sizeof(CPtr) > len)
	       { j = len; len = 0; }
	  else { j = sizeof(CPtr); len -= sizeof(CPtr); }

      while (j--)
	{ reallocate_heap_or_ls_pointer(startp);
	  startp++;
	}
      startp++;
    }
} /* chat_relocate_region */

/*----------------------------------------------------------------------*/

inline static void chat_clear_marks(void)
{
  register chat_init_pheader initial_pheader;
  register chat_incr_pheader pheader;

  initial_pheader = chat_link_headers;
  if (initial_pheader != NULL) {
    do
      {
	pheader = chat_get_father(initial_pheader);
	while ((pheader != NULL) && (chat_area_imarked(pheader)))
	  {
	    chat_iunmark_area(pheader);
	    pheader = chat_get_ifather(pheader);
	  }
	initial_pheader = initial_pheader->next_header;
      }
    while (initial_pheader != chat_link_headers);
  }
}

/*----------------------------------------------------------------------*/

static void chat_relocate_all(CPtr heap_bot, int heap_offset,
			      CPtr ls_bot, int local_offset)
{
  CPtr *b, *tr;
  int  i, trlen;
  Cell cell_val;
  register chat_incr_pheader pheader;
  register chat_init_pheader initial_pheader;

  initial_pheader = chat_link_headers;
  if (initial_pheader != NULL)
  {
    do
      {
	/* relocate the saved consumer choice points -- this also */
	/* takes care of their Dreg field.  A more refined traversal */
	/* of choice points is possible, but is it worth it ? */
	b = (CPtr *)(&chat_get_cons_start(initial_pheader));
	for (i = NLCP_SIZE; i > 0; i--)
	  { reallocate_heap_or_ls_pointer(b);
	    b++;
	  }

	/* relocate the CHAT trail */
	/* this is more tricky than expected: the marks must be used:  */
	/* a relocated pointer might point to the old area !           */
	/* and the marks have to switched off as well at the end       */
	pheader = chat_get_father(initial_pheader);
	while ((pheader != NULL) && (! chat_area_imarked(pheader)))
	  {
	    chat_imark_area(pheader);
	    tr = chat_get_tr_start(pheader);
	    trlen = chat_get_tr_length(pheader);
	    chat_relocate_region(tr,trlen,heap_offset,local_offset);

	    pheader = chat_get_ifather(pheader);
	  }
	initial_pheader = initial_pheader->next_header;
      }
    while (initial_pheader != chat_link_headers);

    chat_clear_marks();      /* here the marks are switched off */
  }

  /* now relocate pointers to the heap from the completion
   * stack: the SF and Dreg fields for generators */
  { register CPtr compl_fr;
    register CPtr *p;

    compl_fr = openreg;
    while (compl_fr != COMPLSTACKBOTTOM)
      { /* substitution factor is now in the heap for generators */
	p = (CPtr *)(&compl_hreg(compl_fr));
	reallocate_heap_or_ls_pointer(p);
	/* relocate Dreg field too: if non-null, it points to the heap */
	p = (CPtr *)(&compl_pdreg(compl_fr));
	reallocate_heap_or_ls_pointer(p);
	compl_fr = prev_compl_frame(compl_fr);
      }
  }

} /* chat_relocate_all */

#endif /* CHAT */

/*----------------------------------------------------------------------*/

xsbBool glstack_realloc(int new_size, int arity)
{
  CPtr   new_heap_bot ;       /* bottom of new Global Stack area */
  CPtr   new_ls_bot ;         /* bottom of new Local Stack area */

  long   heap_offset ;        /* offsets between the old and new */
  long   local_offset ;       /* stack bottoms, measured in Cells */

  CPtr   *cell_ptr ;
  Cell   cell_val ;

  size_t new_size_in_bytes, new_size_in_cells ; /* what a mess ! */
  long   expandtime ;

  if (new_size <= glstack.size) return 0;

#ifdef REALLOC_DEBUG
  xsb_dbgmsg("Reallocating the Heap and Local Stack data area") ;
  if (glstack.size == glstack.init_size) {
    xsb_dbgmsg("\tBottom:\t\t%p\t\tInitial Size: %ldK",
	       glstack.low, glstack.size) ;
    xsb_dbgmsg("\tTop:\t\t%p", glstack.high) ;
  }
#endif

  expandtime = (long)(1000*cpu_time()) ;

  new_size_in_bytes = new_size*K ;
  new_size_in_cells = new_size_in_bytes/sizeof(Cell) ;
  		/* and let's hope K stays divisible by sizeof(Cell) */

  stack_boundaries ;

  /* Expand the data area and push the Local Stack to the high end. */

  new_heap_bot = (CPtr)realloc(heap_bot, new_size_in_bytes);
  if (new_heap_bot == NULL) {
    xsb_mesg("Not enough core to resize the Heap and Local Stack!");
    return 1; /* return an error output -- will be picked up later */
  }
  heap_offset = new_heap_bot - heap_bot ;
  new_ls_bot = new_heap_bot + new_size_in_cells - 1 ;
  local_offset = new_ls_bot - ls_bot ;
  memmove(ls_top + local_offset,             /* move to */
	  ls_top + heap_offset,              /* move from */
	  (ls_bot - ls_top + 1)*sizeof(Cell) );      /* number of bytes */

  /* Update the Heap links */
  for (cell_ptr = (CPtr *)(heap_top + heap_offset);
       cell_ptr-- > (CPtr *)new_heap_bot;
      )
  { reallocate_heap_or_ls_pointer(cell_ptr) ; }

  /* Update the pointers in the Local Stack */
  for (cell_ptr = (CPtr *)(ls_top + local_offset);
       cell_ptr <= (CPtr *)new_ls_bot;
       cell_ptr++)
  { reallocate_heap_or_ls_pointer(cell_ptr) ; }

  /* Update the trailed variable pointers */
#ifdef WAM_TRAIL
  for (cell_ptr = (CPtr *)top_of_trail - 1;
       cell_ptr >= (CPtr *)tcpstack.low;  /* CHAT needs: >= */
       cell_ptr--)
  { /* the address is the only thing */
    cell_val = (Cell)*cell_ptr ;
    realloc_ref(cell_ptr,(CPtr)cell_val) ;
  }
#else
  for (cell_ptr = (CPtr *)top_of_trail - 1;
       cell_ptr > (CPtr *)tcpstack.low;
       cell_ptr = cell_ptr - 2)
  { /* first the value */
    reallocate_heap_or_ls_pointer(cell_ptr);
    /* now the address */
    cell_ptr-- ;
    cell_val = (Cell)*cell_ptr ;
    realloc_ref(cell_ptr,(CPtr)cell_val) ;
  }
#endif

  /* Update the CP Stack pointers */
  for (cell_ptr = (CPtr *)top_of_cpstack;
       cell_ptr < (CPtr *)tcpstack.high;
       cell_ptr++)
  { reallocate_heap_or_ls_pointer(cell_ptr) ; }

  /* Update the argument registers */
  while (arity)
  { cell_ptr = (CPtr *)(reg+arity) ;
    reallocate_heap_or_ls_pointer(cell_ptr) ;
    arity-- ;  
  }

#ifdef CHAT
  chat_relocate_all(heap_bot,heap_offset,ls_bot,local_offset); 
#endif 

  /* Update the system variables */
  glstack.low = (byte *)new_heap_bot ;
  glstack.high = (byte *)(new_ls_bot + 1) ;
  glstack.size = new_size ;

  hreg = (CPtr)hreg + heap_offset ;
  hbreg = (CPtr)hbreg + heap_offset ;
#if (!defined(CHAT))
  hfreg = (CPtr)hfreg + heap_offset ;
#endif
  ereg = (CPtr)ereg + local_offset ;
  ebreg = (CPtr)ebreg + local_offset ;
#if (!defined(CHAT))
  efreg = (CPtr)efreg + local_offset ;
#endif

  if (islist(delayreg))
    delayreg = (CPtr)makelist(clref_val(delayreg) + heap_offset);

  expandtime = (long)(1000*cpu_time()) - expandtime;

#ifdef REALLOC_DEBUG
  xsb_dbgmsg("\tNew Bottom:\t%p\t\tNew Size: %ldK",
	     glstack.low, glstack.size) ;
  xsb_dbgmsg("\tNew Top:\t%p", glstack.high) ;
  xsb_dbgmsg("Heap/Local Stack data area expansion - finished in %ld msecs\n",
	     expandtime) ;
#endif

  return 0;
} /* glstack_realloc */


/*======================================================================*/
/* The main routine that performs garbage collection.                   */
/*======================================================================*/

int gc_heap(int arity)
{
#ifdef GC
  CPtr p;
  unsigned long  begin_marktime, end_marktime,
    end_slidetime, end_copy_time;
  int  marked = 0, marked_dregs = 0, i;
  int  start_heap_size;
  DECL_GC_PROFILE;
  
  INIT_GC_PROFILE;
  if (flags[GARBAGE_COLLECT] != NO_GC) {
    num_gc++ ;
    GC_PROFILE_PRE_REPORT;
    slide = (flags[GARBAGE_COLLECT] == SLIDING_GC) | 
      (flags[GARBAGE_COLLECT] == INDIRECTION_SLIDE_GC);
    
    if (fragmentation_only) 
      slide = FALSE;
    heap_early_reset = ls_early_reset = 0;
    
    GC_PROFILE_START_SUMMARY;
    
    begin_marktime = cpu_time();
    start_heap_size = hreg+1-(CPtr)glstack.low;
    
    /* make sure the top choice point heap pointer 
       that might not point into heap, does */
    if (hreg == cp_hreg(breg)) {
      *hreg = makeint(666) ;
      hreg++ ;
    }
#ifdef SLG_GC
    /* same for the freeze heap pointer */
    if (hfreg == hreg && hreg == cp_hreg(bfreg)) {
      *hreg = makeint(66600);
      hreg++;
    }
#endif
    
    /* copy the aregs to the top of the heap - only if sliding */
    /* just hope there is enough space */
    /* this happens best before the stack_boundaries are computed */
    if (slide) {
      if (delayreg != NULL) {
	arity++;
	reg[arity] = (Cell)delayreg;
      }
      for (i = 1; i <= arity; i++) {
	*hreg = reg[i];
	hreg++;
      }
    }
    

#ifdef CHAT
    /* copy AT ptrs from consumer CPs into a special chat-like area */
    { 
      chat_init_pheader initial_pheader;
      unsigned long at_num = 0, malloc_size = 0, j;
      CPtr *at_ptr;
      
      /* count # of consumers in CHAT area */
      initial_pheader = chat_link_headers;
      if (initial_pheader != NULL) {
	do {
	  at_num++;
	  initial_pheader = initial_pheader->next_header;
	} while (initial_pheader != chat_link_headers);
	at_area_size = at_num;
	/* compute size of area, allocate and align */
	malloc_size = (((at_num + sizeof(CPtr) - 1)/sizeof(CPtr))*
		       (sizeof(CPtr)+1) + sizeof(CPtr)); /* needed_size(at_num) */
	at_malloc_size = malloc_size;
	at_malloc = malloc(malloc_size * sizeof(CPtr));
	at_start = at_malloc;
	malloc_size = (((long)at_start)/sizeof(CPtr)) % (sizeof(CPtr) + 1);
	if (malloc_size != 0)
	  at_start += sizeof(CPtr) + 1 - malloc_size; /* align at_start */
	
	/* copy A.T. ptrs from consumer CP's */
	at_ptr = at_start;
	initial_pheader = chat_link_headers;
	while (at_num) {
	  at_ptr[sizeof(CPtr)] = 0;
	  if (sizeof(CPtr) >= at_num) {
	    j = at_num; 
	    at_num = 0;
	  } else {
	    j = sizeof(CPtr);
	    at_num -= sizeof(CPtr);
	  }
	  while (j--) {
	    *(at_ptr++) = nlcp_template(&chat_get_cons_start(initial_pheader));
	    initial_pheader = initial_pheader->next_header;
	  }
	  at_ptr++;
	}
      } else {
	at_area_size = 0;
	at_malloc = at_start = NULL;
      }
    }
#endif

#ifdef SLG_GC
    /* in SLGWAM, copy hfreg to the heap */
    if (slide) {
      *hreg = (unsigned long) hfreg;
      hreg++;
    }
#endif

    marked = mark_heap(arity, &marked_dregs);
    
    end_marktime = cpu_time();
    
    if (fragmentation_only) {
      /* fragmentation is expressed as ratio not-marked/total heap in use
	 this is internal fragmentation only.  we print marked and total,
	 so that postprocessing can do what it wants with this info. */
      xsb_dbgmsg("marked_used_missed(%d,%d,%d,%d).",
		 marked,hreg+1-(CPtr)glstack.low,
		 heap_early_reset,ls_early_reset);
    free_marks:
      /* get rid of the marking areas - if they exist */
      if (heap_marks)  { free((heap_marks-1)); heap_marks = NULL; }
      if (tr_marks)    { free(tr_marks); tr_marks = NULL; }
      if (ls_marks)    { free(ls_marks); ls_marks = NULL; }
      if (cp_marks)    { free(cp_marks); cp_marks = NULL; }
#ifdef CHAT
      if (compl_marks) { free(compl_marks); compl_marks = NULL; }
      if (at_malloc) { free(at_malloc) ; at_malloc = NULL; } 
#endif
      goto end;
    }
    
    GC_PROFILE_MARK_SUMMARY;
    
    /* An attempt to add some gc/expansion policy;
       ideally this should be user-controlled */
#if (! defined(GC_TEST))
    if (marked > ((hreg+1-(CPtr)glstack.low)*mark_threshold))
      {
	GC_PROFILE_QUIT_MSG;
        if (slide)
          hreg -= arity;
	total_time_gc += (double) 
	  (end_marktime-begin_marktime)*1000/CLOCKS_PER_SEC;
#ifdef CHAT
	chat_clear_marks();
#endif
        goto free_marks; /* clean-up temp areas and get out of here... */
      }
#endif
    
    total_collected += (start_heap_size - marked);

    if (slide)
      {
	GC_PROFILE_SLIDE_START_TIME;

	hreg = slide_heap(marked) ;

	if (hreg != (heap_bot+marked))
	  xsb_dbgmsg("heap sliding gc - inconsistent hreg");
#ifdef CHAT
	hreg -= marked_dregs;
	{
	  CPtr b, p = hreg;
	  chat_init_pheader initial_pheader = chat_link_headers;
	  
	  if (initial_pheader != NULL)
	    do
	      { /* loop over CHAT areas to restore Dreg fields */
		b = (CPtr)(&chat_get_cons_start(initial_pheader));
		if (cp_pdreg(b) != NULL)
		  cp_pdreg(b) = (CPtr)*p++ ;
		initial_pheader = initial_pheader->next_header;
	      }
	    while (initial_pheader != chat_link_headers);
	}
#endif

#ifdef SLG_GC
	/* copy hfreg back from the heap */
	hreg--;
	hfreg = (unsigned long*) *hreg;
#endif

	/* copy the aregs from the top of the heap back */
	hreg -= arity;
	hbreg = cp_hreg(breg);
	
	p = hreg;
	
	for (i = 1; i <= arity; i++)
	  reg[i] = *p++ ;
	if (delayreg != NULL)
	  delayreg = (CPtr)reg[arity--];

	end_slidetime = cpu_time();
	
	total_time_gc += (double) 
	  (end_slidetime - begin_marktime)*1000/CLOCKS_PER_SEC;
	
	GC_PROFILE_SLIDE_FINAL_SUMMARY;
      }
    else
      { /* else we call the copying collector a la Cheney */
	CPtr begin_new_heap, end_new_heap;
	
	GC_PROFILE_COPY_START_TIME;
	
	begin_new_heap = (CPtr)malloc(marked*sizeof(Cell));
	if (begin_new_heap == NULL)
	  xsb_exit("copying garbage collection could not allocate new heap");
	end_new_heap = begin_new_heap+marked;

	hreg = copy_heap(marked,begin_new_heap,end_new_heap,arity);

	free(begin_new_heap);
#ifdef CHAT
	adapt_hreg_from_choicepoints(hreg);
#else
	adapt_hfreg_from_choicepoints(hreg);
#endif
	hbreg = cp_hreg(breg);

#ifdef SLG_GC
	hfreg = hreg;
#endif
	end_copy_time = cpu_time();
	
	total_time_gc += (double) 
	  (end_copy_time - begin_marktime)*1000/CLOCKS_PER_SEC;
	
	GC_PROFILE_COPY_FINAL_SUMMARY;
      }
    
#ifdef CHAT
    /* copy answer template pointers from special area back to consumers */
    {
      CPtr *at_ptr;
      unsigned long j, at_num;
      chat_init_pheader initial_pheader;

      at_ptr = at_start;
      at_num = at_area_size;
      initial_pheader = chat_link_headers;
      while (at_num) {
	if (sizeof(CPtr) >= at_num) {
	  j = at_num; 
	  at_num = 0;
	} else {
	  j = sizeof(CPtr);
	  at_num -= sizeof(CPtr);
	}
	while (j--) {
	  nlcp_template(&chat_get_cons_start(initial_pheader)) = *(at_ptr++);
	  initial_pheader = initial_pheader->next_header;
	}
	at_ptr++;
      }
    }
#endif

    if (print_on_gc) print_all_stacks(arity);
    
    /* get rid of the marking areas - if they exist */
    if (heap_marks)  { 
      check_zero(heap_marks,(heap_top - heap_bot),"heap") ;
      free((heap_marks-1)) ; /* see its calloc */
      heap_marks = NULL ;
    }
    if (tr_marks)    { 
      check_zero(tr_marks,(tr_top - tr_bot + 1),"tr") ;
      free(tr_marks) ;
      tr_marks = NULL ;
    }
    if (ls_marks)    { 
      check_zero(ls_marks,(ls_bot - ls_top + 1),"ls") ;
      free(ls_marks) ;
      ls_marks = NULL ;
    }
    if (cp_marks)    {  
      check_zero(cp_marks,(cp_bot - cp_top + 1),"cp") ;
      free(cp_marks) ;
      cp_marks = NULL ;
    }
#ifdef CHAT
    if (compl_marks) { 
      i = (compl_bot - top_of_complstk);
      check_zero(compl_marks,i,"compl") ;
      free(compl_marks) ;
      compl_marks = NULL ;
    }
    chat_check_zero();

    if (at_malloc) {
      check_zero(at_start, at_area_size, "at_area");
      free(at_malloc);
      at_malloc = at_start = NULL;
    }
#endif

#ifdef SAFE_GC
    p = hreg;
    while (p < heap_top)
      *p++ = 0;
#endif
    
  } /* if (flags[GARBAGE_COLLECT]) */
#else
  /* for non-CHAT, there is no gc, but stack expansion can be done */
#endif
  
#ifdef GC
 end:
  
  GC_PROFILE_POST_REPORT;
  
#endif /* GC */
  return(TRUE);

} /* gc_heap */

/*--------------------------------------------------------------------------*/


