/* File:      timer_xsb.h
** Author(s): Songmei Yu, kifer
** Contact:   xsb-contact@cs.sunysb.edu
**
** Copyright (C) The Research Foundation of SUNY, 1999
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
** $Id: timer_xsb.h,v 1.9 2000-06-19 07:05:32 ruim Exp $
** 
*/


#ifndef CONFIG_INCLUDED
#error "File xsb_config.h must be included before this file"
#endif

#include "setjmp_xsb.h" 

#ifndef CELL_DEFS_INCLUDED
#error "File cell_xsb.h must be included before this file"
#endif

#ifndef SYSTEM_FLAGS
#include "flags_xsb.h"
#endif

#include "timer_defs_xsb.h"

#ifdef WIN_NT
#define NORMAL_TERMINATION  -1
#define TIMED_OUT            1
#define STILL_WAITING        0
#else  /* UNIX */
extern sigjmp_buf xsb_timer_env;
#endif

typedef struct xsb_timeout xsbTimeout;


#ifdef WIN_NT
VOID CALLBACK xsb_timer_handler(HWND wind, UINT msg, UINT eventid, DWORD time);
unsigned long _beginthread(void(_cdecl *start_address) (void *),
			   unsigned stack_size,
			   void *arglist);
int message_pump();
UINT xsb_timer_id; 
#else  /* UNIX */
void xsb_timer_handler(int signo);
#endif

/* generic function to control the timeout */
int make_timed_call(xsbTimeout*, void (*) (xsbTimeout*));

#define NEW_TIMEOUT_OBJECT  (xsbTimeout *)malloc(sizeof(xsbTimeout))

#ifdef WIN_NT
#define SETALARM            ;  /* no-op */
#define TURNOFFALARM        KillTimer(NULL, xsb_timer_id); flags[SYS_TIMER] = 0
#define CHECK_TIMER_SET     ((int)flags[SYS_TIMER] > 0)
#define NOTIFY_PARENT_THREAD(timeout)       \
   if (CHECK_TIMER_SET) {\
      /* Send msg to the timer thread immediately, \
	 so it won't wait to time out */ \
      PostThreadMessage((DWORD)(timeout->parent_thread), \
                                WM_COMMAND,NORMAL_TERMINATION,0);\
   }
#define OP_TIMED_OUT        (message_pump())

#else  /* Unix */

#define SETALARM     	              (signal(SIGALRM, xsb_timer_handler))
#define TURNOFFALARM                  alarm(0); flags[SYS_TIMER] = 0
#define CHECK_TIMER_SET               ((int)flags[SYS_TIMER] > 0)
#define NOTIFY_PARENT_THREAD(timeout)  ;  /* no-op */
#define SET_TIMER                     alarm((int)flags[SYS_TIMER])
#define OP_TIMED_OUT                  (sigsetjmp(xsb_timer_env,1) != 0)

#endif
