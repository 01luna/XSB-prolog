/* File:      dynamic_stack.c
** Author(s): Ernie Johnson
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
** $Id: dynamic_stack.c,v 1.2 2000-06-26 20:09:08 ejohnson Exp $
** 
*/


#include "xsb_config.h"
#include "xsb_debug.h"

#include "debugs/debug_tries.h"

#include <stdio.h>
#include <stdlib.h>

#include "auxlry.h"
#include "cell_xsb.h"
#include "tst_aux.h"  /* needs cell_xsb.h */
#include "error_xsb.h"


/*-------------------------------------------------------------------------*/

/*
 * Prints the fields of the given DynamicStack.  `comment' is useful during
 * debugging, e.g. "just after initialization".
 */

void dsPrint(DynamicStack ds, char *comment) {

  xsb_dbgmsg("Dynamic Stack: %s (%s)\n"
	     "  Stack Base:    %8p\tFrame Size:   %u bytes\n"
	     "  Stack Top:     %8p\tCurrent Size: %u frames\n"
	     "  Stack Ceiling: %8p\tInitial Size: %u frames",
	     DynStk_Name(ds), comment,
	     DynStk_Base(ds), DynStk_FrameSize(ds),
	     DynStk_Top(ds), DynStk_CurSize(ds),
	     DynStk_Ceiling(ds), DynStk_InitSize(ds));
}

/*-------------------------------------------------------------------------*/

/*
 * Initialize a stack for use.  Allocates a block of memory for the stack
 * area and sets the fields of the DynamicStack structure.
 */

void dsInit(DynamicStack *ds, size_t stack_size, size_t frame_size,
	    char *name) {

  size_t total_bytes;

#ifdef DEBUG_TRIE_STACK
  xsb_dbgmsg("Initializing %s", name);
#endif
  total_bytes = stack_size * frame_size;
  DynStk_Base(*ds) = malloc(total_bytes);
  if ( IsNULL(DynStk_Base(*ds)) )
    xsb_abort("Ran out of memory in allocation of %s", DynStk_Name(*ds));
  DynStk_Top(*ds) = DynStk_Base(*ds);
  DynStk_Ceiling(*ds) = (char *)DynStk_Base(*ds) + total_bytes;
  DynStk_FrameSize(*ds) = frame_size;
  DynStk_InitSize(*ds) = DynStk_CurSize(*ds) = stack_size;
  DynStk_Name(*ds) = name;
}

/*-------------------------------------------------------------------------*/

/*
 * `num_frames' are the number of frames that are needed immediately.
 * Here we make sure that the expanded size can accommodate this need.
 */

void dsExpand(DynamicStack *ds, int num_frames) {

  size_t new_size, total_bytes;
  char *new_base;

  if ( num_frames < 1 )
    return;
  if ( DynStk_CurSize(*ds) > 0 )
    new_size = 2 * DynStk_CurSize(*ds);
  else
    new_size = DynStk_InitSize(*ds);
  if ( new_size < DynStk_CurSize(*ds) + num_frames )
    new_size = new_size + num_frames;
#ifdef DEBUG_TRIE_STACK
  xsb_dbgmsg("Expanding %s: %d -> %d", DynStk_Name(*ds),
	     DynStk_CurSize(*ds), new_size);
  dsPrint(*ds, "Before expansion");
#endif
  total_bytes = new_size * DynStk_FrameSize(*ds);
  new_base = realloc(DynStk_Base(*ds),total_bytes);
  if ( IsNULL(new_base) )
    xsb_abort("Ran out of memory during expansion of %s", DynStk_Name(*ds));
  DynStk_Top(*ds) =
    new_base + ((char *)DynStk_Top(*ds) - (char *)DynStk_Base(*ds));
  DynStk_Base(*ds) = new_base;
  DynStk_Ceiling(*ds) = new_base + total_bytes;
  DynStk_CurSize(*ds) = new_size;
#ifdef DEBUG_TRIE_STACK
  dsPrint(*ds, "After expansion");
#endif
}

/*-------------------------------------------------------------------------*/

/*
 * Reduces the size of the memory block allocated back to the initial size
 * specified during initialization.
 */

void dsShrink(DynamicStack *ds) {

  size_t total_bytes;
  char *new_base;

  if ( DynStk_CurSize(*ds) <= DynStk_InitSize(*ds) )
    return;
  total_bytes = DynStk_InitSize(*ds) * DynStk_FrameSize(*ds);
  new_base = realloc(DynStk_Base(*ds),total_bytes);
#ifdef DEBUG_TRIE_STACK
  xsb_dbgmsg("Shrinking %s: %d -> %d", DynStk_Name(*ds),
	     DynStk_CurSize(*ds), DynStk_InitSize(*ds));
#endif
  if ( IsNULL(new_base) )
    xsb_abort("Ran out of memory during expansion of %s", DynStk_Name(*ds));
  DynStk_Top(*ds) =
    new_base + ((char *)DynStk_Top(*ds) - (char *)DynStk_Base(*ds));
  DynStk_Base(*ds) = new_base;
  DynStk_Ceiling(*ds) = new_base + total_bytes;
  DynStk_CurSize(*ds) = DynStk_InitSize(*ds);
}
