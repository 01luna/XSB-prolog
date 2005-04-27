/* File:      flag_defs_xsb.h
** Author(s): Jiyang Xu, Kostis Sagonas, Ernie Johnson
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1986, 1993-1999
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
** $Id: flag_defs_xsb.h,v 1.14 2005-04-27 04:20:15 evansbj Exp $
** 
*/


/* -------- system flags --------------------------------------------------*/

/*	used by interpreter ->  Y/N ; read only -> R/W/N (no access)	   */

#define PIL_TRACE 	 0	/* 0 = pil trace off, 1 = on		YW */
#define HITRACE		 1	/* 0 = hitrace off, 1 = on		YW */
#define STACK_REALLOC    2      /* 1 = allow realloc of stacks on Oflow YW */
#define TRACE_STA	 3	/* 1 = keep max stack size stats	YW */
#define DEBUG_ON	 4	/* 1 = debug on; 0 = off 		YW */
#define HIDE_STATE	 5	/* 0 = no hide, >0 = hide level 	YW */
#define TRACE		 6	/* 1 = trace on, 0 = trace off	    	YW */
#define INVOKE_NUM	 7	/* debugger, the ordinal invoke number 	NW */
#define SKIPPING	 8	/* debugger, 1 = skip, 0 = not	   	NW */
#define QUASI_SKIPPING	 9	/* debugger, 1 = quasi skip, 0 = not	NW */
#define CURRENT_INPUT	10	/* current input file descriptor	NW */
#define CURRENT_OUTPUT	11	/* current output file descriptor	NW */
#define CURRENT_MODULE	12	/* current module. USERMOD_PSC=usermod  YW */
#define MOD_LIST	13	/* the list of module (Psc) entries	YR */

#define SYS_TIMER       16      /* XSB Timer	    	    	    	MK */
#define BANNER_CTL      17      /* Controls whether banner, "loaded" msgs
				   are displayed; passed to Prolog side.
				   Check BANNER_CTL values at the end      */
#define DCG_MODE        18      /* DGC mode: standard or xsb	           */

#define VERBOSENESS_LEVEL 19    /* how verbose debug messages should be    */

#define GARBAGE_COLLECT 20      /* type of garbage collection employed:    */
                                /* 0 = none; 1 = sliding; 2 = copying.  NW */
#define CMD_LINE_GOAL  	21	/* The Prolog goal passed on cmd 
				   line with -e	       	       	           */
#define USER_HOME  	22	/* $HOME, if not null. Else INSTALL_DIR    */
#define INSTALL_DIR	23	/* set dynamically in orient_xsb.c         */

#define CLAUSE_INT	24	/* for clause interrupt			YW */

#define BACKTRACE	25	/* for backtracing on error      Y/N              */

#define CONFIG_FILE	26	/* Where xsb_configuration.P lives	   */
/* loader uses CONFIG_NAME flag before xsb_configuration is loaded */
#define CONFIG_NAME	28	/* this looks like this: cpu-vendor-os	   */

/* the following three flags are only in effect when
   the system is compiled with --enable-debug         --lfcastro           */
#define VERBOSE_GC      29      /* be verbose on garbage collection        */
#define COUNT_CHAINS    30      /* count size of chains on GC              */
#define EXAMINE_DATA    31      /* examine data on GC                      */

/*
 *  Flags 32-48 are reserved for Interrupt Handler PSCs.
 */
#define INT_HANDLERS_FLAGS_START   32  /* the first interrupt flag */

/* ----------------------------------------------------------------------------
   The following exist/are defined in sig_xsb.h:

 MYSIG_UNDEF      0    // _$load_undef
 MYSIG_KEYB       1    // _$keyboard_int
 MYSIG_SPY        3    // _$deb_spy
 MYSIG_TRACE      4    // _$deb_trace
 MYSIG_ATTV       8    // _$attv_int
 MYSIG_CLAUSE    16    // _$clause_int

These values are added to INT_HANDLERS_FLAGS_START to obtain the actual
interrupt flag
---------------------------------------------------------------------------- */


/* This flag is used by the loader to tell itself whether it should look into
   user-supplied library search paths or not. If 0, the loader will look only
   in lib/syslib/cmplib. If 1, the loader will look in library_directory/1
   before checking the standard places. */
#define LIBS_LOADED	  50

#define PROFFLAG          52

#define LETTER_VARS	  55      /* For printing vars in the interpreter */
#define BOOT_MODULE       56      /* First file loaded; usually loader.P  */
#define CMD_LOOP_DRIVER   57      /* File that contains top-level command
				     loop driver */

#define ORA_INPUTARRAY_LENGTH     58   /* max # simultaneous input tuples */
#define ORA_INPUTARRAY_WIDTH      59   /* max size of each input value    */
#define ORA_OUTPUTARRAY_LENGTH    60   /* max # simultaneous output tuples */

#define TABLING_METHOD		61      /* Default method used to evaluate tabled */

#define NUM_THREADS     	62      /* always 1 in the sequential system     NW*/
#define THREAD_RUN			63      /* PSC for the thread handler predicate  NN*/
#define STDERR_BUFFERED		65      /* Allow Cinterface to read Stderr back from buffer*/

#define MAXTOINDEX 5              /* depth in term to go when using deep indexing */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Banner control values recognized on the Prolog side. 
   MUST BE PRIME NUMBERS */
#define NOBANNER          2   /* don't display XSB banner */
#define QUIETLOAD    	  3   /* don't display "module loaded" msgs */
#define NOPROMPT    	  5   /* display no prompt--useful in spawned
				 subprocesses */
