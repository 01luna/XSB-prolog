/* File:      tr_utils.h
** Author(s): Prasad Rao, Kostis Sagonas
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
** $Id: tr_utils.h,v 1.1.1.1 1998-11-05 16:55:24 sbprolog Exp $
** 
*/


extern void aux_breg_retskel(void);
extern void trie_node_element(void);
extern void delete_return(NODEptr, CPtr);
extern void delete_predicate_table(void);
extern void reclaim_del_ret_list(SGFrame);
extern void reclaim_ans_list_nodes(SGFrame);
extern void variant_call_search_rdonly(int, CPtr, CPtr *, int *, byte *);
extern void init_newtrie();
extern int  delete_branch(NODEptr, CPtr);
extern void clear_interned_tries();
extern void safe_delete_branch(NODEptr);


extern bool has_unconditional_answers(SGFrame);



extern void newtrie();
extern void trie_intern();
extern int trie_interned();
extern void trie_dispose();
extern void delete_trie(NODEptr);

extern NODEptr *Set_ArrayPtr;
extern void free_node_function(NODEptr);
