/* File:      binding.h
** Author(s): Jiyang Xu, Terrance Swift, Kostis Sagonas, Baoqiu Cui
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
** $Id: binding.h,v 1.12 2000-04-29 21:53:49 kifer Exp $
** 
*/

/*
 * Structures Of Different Trail Frames:
 *
 * In SLG-WAM, trreg and trfreg always point to the trail_parent field of
 * a trail frame, and the trail stack may contain both 3-word trail frames 
 * and 4-word ones.  After untrailing a trail frame, trreg can be reset by 
 * following the trail_parent pointer.
 *
 * In CHAT, trreg always points to the address field of a trail frame, and
 * the trail stack may contain both 1-word trail frames or 2-word ones.
 * After untrailing a trail frame, trreg is set to trreg-1 or trreg-2
 * (depending on the lowest bit of the address field of the trail frame).
 *
 * 1) Regular (forward) trail frame in SLG-WAM (3-word trail frame)
 * ----------------------------------------------------------------
 *    low memory
 *     +-----+
 *     |     | trail_variable : Address of the trailed variable
 *     +-----+
 *     |     | trail_value    : New value to which the trailed variable is
 *     +-----+                  to be bound
 *     |     | trail_parent   : Pointer to the trail_parent cell of the
 *     +-----+                  previous trail frame)
 *    high memory
 *
 * 2) Pre-image trail frame in SLG-WAM (4-word trail frame)
 * --------------------------------------------------------
 *    low memory
 *     +-----+
 *     |     | trail_pre_image: Old value of the trailed variable (cell)
 *     +---+-+
 *     |   |1| trail_variable : Address of the trailed variable
 *     +---+-+                  (with PRE_IMAGE_MARK)
 *     |     | trail_value    : New value to which the trailed variable is
 *     +-----+                  to be bound
 *     |     | trail_parent   : Pointer to the trail_parent cell of the
 *     +-----+                  previous trail frame)
 *    high memory
 *
 * 3) Regular WAM-TRAIL frame in CHAT (1-word trail frame)
 * -------------------------------------------------------
 *     +-----+
 *     |     | Address of the trailed variable
 *     +-----+
 *
 * 4) Pre-image trail frame in CHAT (2-word trail frame)
 * -----------------------------------------------------
 *     +-----+
 *     |     | Old value of the trailed variable (cell)
 *     +---+-+
 *     |   |1| Address of the trailed variable (with PRE_IMAGE_MARK)
 *     +---+-+
 */

#define PRE_IMAGE_TRAIL

#define PRE_IMAGE_MARK   1

#ifdef WAM_TRAIL

#define pushtrail0(addr,val) {						\
  if ((char *)(top_of_trail) > ((char *)(top_of_cpstack) - 10)) {	\
    handle_tcpstack_overflow();						\
  }									\
  *(trreg++) = addr;							\
}

#ifdef PRE_IMAGE_TRAIL
#define push_pre_image_trail0(addr, new_value) {			\
  if ((char *)(top_of_trail) > ((char *)(top_of_cpstack) - 10)) {	\
    handle_tcpstack_overflow();						\
  }									\
  *(trreg++) = (CPtr) (cell(addr));					\
  *(trreg++) = (CPtr) ((Cell) (addr) | PRE_IMAGE_MARK);			\
}
#endif /* PRE_IMAGE_TRAIL */

#else  /* WAM_TRAIL */

#define TRAIL_FRAME_SIZE  4

#define pushtrail0(addr,val)  \
   if (trfreg > trreg) {\
     if ((char *)trfreg > \
	 ((char *)(top_of_cpstack) - (TRAIL_FRAME_SIZE*sizeof(CPtr)))) {\
       handle_tcpstack_overflow();\
     }\
     *(trfreg+3) = (CPtr) trreg;\
     trreg = trfreg + 3;\
     *(trreg-1) = (CPtr) val;\
     *(trreg-2) = addr;\
   }\
   else {\
     if ((char *)trreg > \
	 ((char *)(top_of_cpstack) - (TRAIL_FRAME_SIZE*sizeof(CPtr)))) {\
       handle_tcpstack_overflow();\
     }\
     trreg = trreg+3;\
     *trreg = (CPtr) trreg-3;\
     *(trreg-1) = (CPtr) val;\
     *(trreg-2) = addr;\
   }

#ifdef PRE_IMAGE_TRAIL
#define push_pre_image_trail0(addr, new_value)			\
  if (trfreg > trreg) {						\
    if ((char *)trfreg > ((char *)(top_of_cpstack) -		\
                          TRAIL_FRAME_SIZE*sizeof(CPtr))) {	\
      handle_tcpstack_overflow();				\
    }								\
    *(trfreg + 4) = (CPtr) trreg;				\
    trreg = trfreg + 4;						\
    *(trreg - 1) = (CPtr) (new_value);				\
    *(trreg - 2) = (CPtr) ((Cell) (addr) | PRE_IMAGE_MARK);	\
    *(trreg - 3) = (CPtr) (cell(addr));				\
  }								\
  else {							\
    if ((char *)trreg > ((char *)(top_of_cpstack) -		\
                          TRAIL_FRAME_SIZE*sizeof(CPtr))) {	\
      handle_tcpstack_overflow();				\
    }								\
    trreg = trreg + 4;						\
    *trreg = (CPtr) trreg - 4;					\
    *(trreg - 1) = (CPtr) (new_value);				\
    *(trreg - 2) = (CPtr) ((Cell) (addr) | PRE_IMAGE_MARK);	\
    *(trreg - 3) = (CPtr) (cell(addr));				\
  }
#endif /* PRE_IMAGE_TRAIL */

#endif /* WAM_TRAIL */

#ifdef CHAT
#define conditional(a)	( ((a) >= ebreg) || ((a) < hbreg) )
#else
#define conditional(a)	( ((a) >= ebreg || (a) >= efreg) || \
			  ((a) < hbreg  || (a) < hfreg) )
#endif

#define pushtrail(a,v)	if (conditional(a)) { pushtrail0(a,v); }
#define dpushtrail(a,v) pushtrail0(a,v)

#ifdef PRE_IMAGE_TRAIL
#define push_pre_image_trail(a, new_v)			\
  if (conditional(a)) {push_pre_image_trail0(a, new_v)}
#endif /* PRE_IMAGE_TRAIL */

/* --- binding -------------------------------------------------------- */

#define bind_int(addr, val)	pushtrail(addr, makeint(val));\
				bld_int(addr, val)

#define bind_float(addr, val)	pushtrail(addr, (Cell) makefloat(val)); \
				bld_float(addr, val)

#define bind_ref(addr, val)	pushtrail(addr, val);\
				bld_ref(addr, val)

#define dbind_ref(addr, val)	dpushtrail(addr, val);\
				bld_ref(addr, val)

#define bind_cs(addr, str)	pushtrail(addr, makecs(str));\
				bld_cs(addr, str)

#define bind_string(addr, str)	pushtrail(addr, makestring(str));\
				bld_string(addr,str)

#define bind_nil(addr)		pushtrail(addr, makenil);\
				bld_nil(addr)

#define bind_list(addr, list)	pushtrail(addr, makelist(list));\
				bld_list(addr, list)

#define bind_attv(addr, attv)	pushtrail(addr, makeattv(attv));\
				bld_attv(addr, attv)

#define bind_copy(addr, val)	pushtrail(addr, val); *(addr) = val

/* value trail MUST be used because first CP cell has cp_trreg = 0 !!!! */

#define untrail(addr) bld_free(addr)

#ifdef PRE_IMAGE_TRAIL	/* untrail2 is for pre_image trail. */

#ifdef WAM_TRAIL
#define untrail2(trail_ptr, addr) {		\
  if ((addr) & PRE_IMAGE_MARK) {		\
    bld_copy0((CPtr)((addr) - PRE_IMAGE_MARK),	\
              cell((CPtr)trail_ptr - 1));	\
    trreg--;					\
  }						\
  else						\
    bld_free((CPtr)(addr));			\
}
#else  /* WAM_TRAIL */
#define untrail2(trail_ptr, addr)		\
  if ((addr) & PRE_IMAGE_MARK) {		\
    bld_copy0((CPtr)((addr) - PRE_IMAGE_MARK),	\
	      cell((CPtr)trail_ptr - 3));	\
  }						\
  else						\
    bld_free((CPtr)(addr))
#endif /* WAM_TRAIL */

#endif /* PRE_IMAGE_TRAIL */


/* --- testing location of variable ----------------------------------- */

#define ereg_on_top(t_ereg)	t_ereg < ebreg

#define efreg_on_top(t_ereg) efreg < ebreg  && efreg < t_ereg

/* --- for building vals on the heap ---------------------------------- */

#define nbldval(OP1) {							  \
   XSB_Deref(OP1);								  \
   if ( isnonvar(OP1) ||						  \
	( /* (CPtr)(OP1) >= glstack.low && */				  \
	  (CPtr)(OP1) <= top_of_heap ) ) {			 	  \
     new_heap_node(hreg, OP1);						  \
   }									  \
   else {  /* local stack vars point to heap vars and not vice-versa! */  \
     bind_ref((CPtr)(OP1), hreg);					  \
     new_heap_free(hreg);						  \
   }									  \
}
