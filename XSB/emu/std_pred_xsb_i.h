/* File:      std_pred_xsb_i.h
** Author(s): Kostis F. Sagonas
** Modified by Swift 
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
** $Id: std_pred_xsb_i.h,v 1.19 2004-03-18 16:26:01 dwarren Exp $
** 
*/


/*----------------------------------------*/
#include "builtin.h"
#include "sp_unify_xsb_i.h"
/*----------------------------------------*/

static xsbBool atom_to_list(int call_type);
static xsbBool number_to_list(int call_type);

/* TLS 10/01 changed functor so that it did not core dump on 
   functor(X,1,2) */
inline static xsbBool functor_builtin(void)
{
  /* r1: ?term; r2: ?functor; r3: ?arity (int)	*/
  int  new_indicator, arity, value, disp;
  Psc psc;
  char *name;
  Cell functor, term;
  Pair sym;

  term = ptoc_tag(1);
  if (isnonvar(term)) {
    if (isconstr(term) && !isboxedinteger(term)) {
      psc = get_str_psc(term);
      name = get_name(psc);
      arity = get_arity(psc);
      return (atom_unify(makestring(name), ptoc_tag(2)) &&
	      int_unify(makeint(arity), ptoc_tag(3)));
    } else if (islist(term))
      return (atom_unify(makestring(list_dot), ptoc_tag(2)) &&
	      int_unify(makeint(2), ptoc_tag(3)));
    else return (unify(term, ptoc_tag(2)) &&
		 int_unify(makeint(0), ptoc_tag(3)));
  } else {	/* term is a variable */
    functor = ptoc_tag(2);
    if (isstring(functor) || isinteger(functor) || isfloat(functor) ||
	isboxedinteger(functor) ||
	(isconstr(term) && get_arity(get_str_psc(term)) == 0)) {
      arity = ptoc_tag(3);
      /* tls: added !isnumber conjunct */
      if (arity_integer(arity) && !isnumber(functor)) {
	value = int_val(arity);
	if (value == 0) return unify(functor, term);
	else {
	  if (value == 2 && isstring(functor) 
	      && string_val(functor) == list_dot) {
	    /* term is a variable and I bind it to a list. */
	    bind_list((CPtr)term, hreg);
	    new_heap_free(hreg);
	    new_heap_free(hreg);
	  } else { 
	    /* functor always creates a psc in the current module */
	    sym = (Pair)insert(string_val(functor), (char)value, 
			       (Psc)flags[CURRENT_MODULE],
			       &new_indicator);
	    sreg = hreg;
	    hreg += value+1;	/* need (arity+1) new cells */
	    bind_cs((CPtr)term, sreg);
	    new_heap_functor(sreg, sym->psc_ptr);
	    for (disp=0; disp<value; disp++) {
	      new_heap_free(sreg);
	    }
	  }
	  return TRUE;	/* always succeed! */
	}
	/* TLS rearranged order of the two elses below */
      } else {
	  if (isnumber(functor))
	    return (unify(term, functor) && 
		    int_unify(makeint(0), arity));
	  else {
	    if (isnonvar(arity)) {
	      if (isinteger(arity))
		err_handle(RANGE, 3, "functor", 3,
		       "integer in the range 0..255", arity);
	      else err_handle(TYPE, 3, "functor", 3, "integer",arity);
	    }
	  else err(INSTANTIATION, 3, "functor", 3);
	  }
      }
    }
      else {
      if (isnonvar(functor))
	err_handle(TYPE, 2, "functor", 3, "atom", functor);
      else err(INSTANTIATION, 2, "functor", 3);
      }
  }
  return TRUE;
}


inline static xsbBool arg_builtin(void)
{
  /* r1: +index (int); r2: +term; r3: ?arg (term) */
  Cell index;
  Cell term;
  int disp;

  index = ptoc_tag(1);
  if (isinteger(index)) {
    if ((disp = int_val(index)) > 0) {
      term = ptoc_tag(2);
      if (isnonvar(term)) {
	if (isconstr(term)) {
	  if (disp <= (int)get_arity(get_str_psc(term))) {
	    return unify((Cell)(clref_val(term)+disp),
			 ptoc_tag(3));
	  } 
	  else return FALSE;	/* fail */
	} else if (islist(term) && (disp==1 || disp==2)) {
	  return unify((Cell)(clref_val(term)+disp-1),
		       ptoc_tag(3));
	} else return FALSE;	/* fail */
      } else err(INSTANTIATION, 2, "arg", 3);
    } else return FALSE;	/* fail */
  } else {
    if (isnonvar(index)) err_handle(TYPE, 1, "arg", 3, "integer",index);
    else err(INSTANTIATION, 1, "arg", 3);
  }
  return TRUE;
}


inline static xsbBool univ_builtin(void)
{
  /* r1: ?term; r2: ?list	*/
  int i, arity;
  int  new_indicator;
  char *name;
  Cell list, new_list, term;
  CPtr head, top = 0;
  Pair sym;

  term = ptoc_tag(1);
  list = ptoc_tag(2);
  if (isnonvar(term)) {	/* Usage is deconstruction of terms */
    new_list = makelist(hreg);
    if (isatomic(term) || isboxedinteger(term)) { follow(hreg++) = term; top = hreg++; }
    else if (isconstr(term) && (arity = (get_arity(get_str_psc(term))))) {
      follow(hreg++) = makestring(get_name(get_str_psc(term)));
      top = hreg++;
      for (i = 1 ; i <= arity ; i++) {
	follow(top) = makelist(hreg); top = hreg++;
	follow(top) = (Cell)(clref_val(term)+i); top = hreg++;
      }
    }
    else { /* term is list */
      follow(hreg++) = makestring(list_dot);
      top = hreg++;
      follow(top) = makelist(hreg); top = hreg++;
      follow(top) = (Cell)(clref_val(term)); top = hreg++;
      follow(top) = makelist(hreg); top = hreg++;
      follow(top) = (Cell)(clref_val(term)+1); top = hreg++;
    }
    follow(top) = makenil;
    return unify(list, new_list);
  } else { /* usage is construction; term is known to be a variable */
    if (islist(list)) {
      head = clref_val(list);
      XSB_Deref(cell(head));
      if (isatom(cell(head))) {
	if (isnil(cell(head+1))) {	/* atom construction */
	  bind_copy((CPtr)term, cell(head));
	  return TRUE;	/* succeed */
	} else {
	  xsbBool list_construction = FALSE;
	  name = string_val(cell(head));
	  if (!strcmp(name, ".")) { /* check for list construction */
	    list = cell(head+1); XSB_Deref(list);
	    if (islist(list)) {
	      list = cell(clref_val(list)+1); XSB_Deref(list);
	      if (islist(list)) {
		list = cell(clref_val(list)+1); XSB_Deref(list);
		if (isnil(list)) list_construction = TRUE;
	      }
	    }
	  }
	  if (list_construction) { /* no errors can occur */
	    bind_list((CPtr)term, hreg);
	    list = cell(head+1);
	    XSB_Deref(list);
	    bld_copy(hreg, cell(clref_val(list))); hreg++;
	    list = cell(clref_val(list)+1);
	    XSB_Deref(list);
	    bld_copy(hreg, cell(clref_val(list))); hreg++;
	  } else { /* compound term construction */
	    sreg = hreg;
	    bind_cs((CPtr)term, sreg); hreg = sreg; sreg++;
	    for (arity = 0, list = cell(head+1); ;
		 arity++, list = cell(clref_val(list)+1)) {
	      XSB_Deref(list); /* necessary */
	      if (!islist(list)) break; /* really ugly */
	      bld_copy(sreg, cell(clref_val(list))); sreg++;
	    }
	    if (isnil(list) && arity <= MAX_ARITY) {
	      /* '=..'/2 always creates a psc in the current * module */
	      sym = (Pair)insert(name, (char)arity,
				 (Psc)flags[CURRENT_MODULE],
				 &new_indicator);
	      new_heap_functor(hreg, sym->psc_ptr);
	      hreg = sreg+1;
	    } else {
	      hreg = hreg-1;	/* restore hreg */
	      if (arity > MAX_ARITY)
		xsb_abort("[In =..] Attempt to construct a functor with arity %d > %d",
			  arity, MAX_ARITY);
	      else err_handle(TYPE, 2, "=..", 2,
			      "[]-terminated list", list);
	      return FALSE;
	    }
	  }
	} return TRUE;
      }
      if ((isnumber(cell(head)) || isboxedinteger(cell(head))) && isnil(cell(head+1))) { /* list=[num] */
	bind_copy((CPtr)term, cell(head));	 /* term<-num  */
	return TRUE;	/* succeed */
      }
      else
	{
	  err_handle(TYPE, 2, "=..", 2,
		     "[]-terminated list whose first element is atomic",
		     list);
	  return(FALSE);
	}
    }
    if (isnonvar(list))
      err_handle(TYPE, 2, "=..", 2,
		 "[]-terminated list whose first element is atomic",
		 list);
    else err(INSTANTIATION, 2, "=..", 2);
  }
  return TRUE;
}


inline static xsbBool hilog_arg(void)
{
  /* r1: +index (int); r2: +term; r3: ?arg (term) */
  Cell index, term;
  int disp;

  index = ptoc_tag(1);
  if (isinteger(index)) {
    if ((disp = int_val(index)) > 0) {
      term = ptoc_tag(2);
      if (isnonvar(term)) {
	if (isconstr(term)) {
	  if (hilog_cs(term)) disp++;
	  if (disp <= (int)get_arity(get_str_psc(term))) {
	    return unify((Cell)(clref_val(term)+disp),
			 ptoc_tag(3));
	  } return FALSE;		/* fail */
	} else if (islist(term) && (disp==1 || disp==2)) {
	  return unify((Cell)(clref_val(term)+disp-1),
		       ptoc_tag(3));
	} else return FALSE;	/* fail */
      } else err(INSTANTIATION, 2, "hilog_arg", 3);
    } else return FALSE;	/* fail */
  } else {
    if (isnonvar(index))
      err_handle(TYPE, 1, "hilog_arg", 3, "integer", index);
    else err(INSTANTIATION, 1, "hilog_arg", 3);
  }
  return TRUE;
}

#define INITIAL_NAMELEN 256

inline static xsbBool atom_to_list(int call_type)
{
  /* r1: ?term; r2: ?character list	*/
  int i, len;
  long c;
  char *atomname, *atomnamelast;
  static char *atomnameaddr = NULL;
  static int atomnamelen;
  char tmpstr[2], *tmpstr_interned;
  Cell heap_addr, term, term2;
  Cell list, new_list;
  CPtr top = 0;
  char *call_name = (call_type == ATOM_CODES ? "atom_codes" : "atom_chars");
  char *elt_type = (call_type == ATOM_CODES ? "ASCII code" : "character atom");

  term = ptoc_tag(1);
  list = ptoc_tag(2);
  if (!isnonvar(term)) {	/* use is: CODES/CHARS --> ATOM */
    if (atomnameaddr == NULL) {
      atomnameaddr = (char *)malloc(INITIAL_NAMELEN);
      atomnamelen = INITIAL_NAMELEN;
      /* printf("Allocated namebuf: %p, %d\n",atomnameaddr,atomnamelen);*/
    }
    atomname = atomnameaddr;
    atomnamelast = atomnameaddr + (atomnamelen - 1);
    term2 = list;	/* DON'T use heap for temp storage */
    do {
      XSB_Deref(term2);
      if (isnil(term2)) {
	*atomname++ = '\0';
	break;
      }
      if (islist(term2)) {
	heap_addr = cell(clref_val(term2)); XSB_Deref(heap_addr);
	if (((call_type==ATOM_CODES) && !isinteger(heap_addr))
	    || ((call_type==ATOM_CHARS) && !isstring(heap_addr))) {
	  if (isnonvar(heap_addr))
	    err_handle(TYPE, 2, call_name, 2, elt_type, list);
	  else err(INSTANTIATION, 2, call_name, 2);
	  return FALSE;	/* fail */
	}
	if (isinteger(heap_addr))
	  c = int_val(heap_addr);
	else /* ATOM CHARS */
	  c = *string_val(heap_addr);

	if (c < 0 || c > 255) {
	  err_handle(RANGE, 2, call_name, 2, "ASCII code", heap_addr);
	  return FALSE;	/* fail */
	}
	if (atomname >= atomnamelast) {
	  atomnameaddr = (char *)realloc(atomnameaddr, (atomnamelen << 1));
	  atomname = atomnameaddr + (atomnamelen - 1);
	  atomnamelen = atomnamelen << 1;
	  atomnamelast = atomnameaddr + (atomnamelen - 1);
	  /*printf("Allocated namebuf: %p, %d\n",atomnameaddr,atomnamelen);*/
	}
	*atomname++ = (char)c;
	term2 = cell(clref_val(term2)+1);
      } else {
	if (isref(term2)) err(INSTANTIATION, 2, call_name, 2);
	else err_handle(TYPE, 2, call_name, 2, "list", term2);
	return FALSE;	/* fail */
      }
    } while (1);
    bind_string((CPtr)(term), (char *)string_find((char *)atomnameaddr, 1));
    return TRUE;
  } else {	/* use is: ATOM --> CODES/CHARS */
    if (isstring(term)) {
      atomname = string_val(term);
      len = strlen(atomname);
      if (len == 0) {
	if (!isnonvar(list)) {
	  bind_nil((CPtr)(list)); return TRUE;
	}
	else return isnil(list);
      } else {
	/* check that there is enough space on the heap! */
	check_glstack_overflow(2, pcreg, 2*len*sizeof(Cell)) ;
	list = ptoc_tag(2);   /* in case it changed */

	new_list = makelist(hreg);
	for (i = 0; i < len; i++) {
 	  if (call_type==ATOM_CODES)
	    follow(hreg++) = makeint(*(unsigned char *)atomname);
	  else {
	    tmpstr[0]=*atomname;
	    tmpstr[1]='\0';
	    tmpstr_interned=string_find(tmpstr,1);
	    follow(hreg++) = makestring(tmpstr_interned);
	  }
	  atomname++;
	  top = hreg++;
	  follow(top) = makelist(hreg);
	}
	follow(top) = makenil;
	return unify(list, new_list);
      } 
    } else err_handle(TYPE, 1, call_name, 2, "atom", term);
  }
  return TRUE;
}

inline static xsbBool number_to_list(int call_type)
{
  int i, tmpval;
  long c;
  char tmpstr[2], *tmpstr_interned;
  char str[256];	
  int StringLoc = 0;
  Cell heap_addr, term, term2;
  Cell list, new_list;
  char hack_char;	
  CPtr top = 0;
  char *call_name =
    (call_type == NUMBER_CODES ?
     "number_codes" : (call_type == NUMBER_DIGITS?
		       "number_digits" : "number_chars"));
  char *elt_type =
    (call_type == NUMBER_CODES ?
     "integer" : (call_type == NUMBER_DIGITS? "digit" : "digit atom"));


  term = ptoc_tag(1);
  list = ptoc_tag(2);
  if (!isnonvar(term)) {	/* use is: CHARS/CODES --> NUMBER */
    term2 = list;
    do {
      XSB_Deref(term2);
      if (isnil(term2)) {
	str[StringLoc++] = '\0';
	break;
      }
      if (islist(term2)) {
	heap_addr = cell(clref_val(term2)); XSB_Deref(heap_addr);
	if (((call_type==NUMBER_CODES) && (!isinteger(heap_addr)))
	    || ((call_type==NUMBER_CHARS) && !isstring(heap_addr))
	    || ((call_type==NUMBER_DIGITS)
		&& !isstring(heap_addr)
		&& !isinteger(heap_addr))) {
	  if (isnonvar(heap_addr))
	    err_handle(TYPE, 2, call_name, 2, elt_type, list);
	  else err(INSTANTIATION, 2, call_name, 2);
	  return FALSE;	/* fail */
	}
	if (call_type==NUMBER_CODES)
	  c = int_val(heap_addr);
	else if ((call_type==NUMBER_DIGITS) && (isinteger(heap_addr))) {
	  tmpval = int_val(heap_addr);
	  if ((tmpval < 0) || (tmpval > 9)) {
	    err_handle(TYPE, 2, call_name, 2, elt_type, list);
	    return FALSE;	/* fail */
	  }
	  c = (long) '0' + int_val(heap_addr);
	} else if (isstring(heap_addr))
	  c = *string_val(heap_addr);
	else {
	  err_handle(TYPE, 2, call_name, 2, "integer, digit, or atom", list);
	  return FALSE;	/* fail */
	}

	if (c < 0 || c > 255) {
	  err_handle(RANGE, 2, call_name, 2, "ASCII code", heap_addr);
	  return FALSE;	/* fail */
	}
	if (StringLoc > 200) return FALSE;
	str[StringLoc++] = (char)c;
	term2 = cell(clref_val(term2)+1);
      } else {
	if (isref(term2))
	  err(INSTANTIATION, 2, call_name, 2);
	else
	  err_handle(TYPE, 2, call_name, 2, "list", term2);
	return FALSE;	/* fail */
      }
    } while (1);

    if (sscanf(str, "%ld%c", &c, &hack_char) == 1) {
      bind_int((CPtr)(term), c);
    } else {
      Float float_temp;
#ifdef BITS64
      if (sscanf(str, "%le%c", &float_temp, &hack_char) == 1)
#else
      if (sscanf(str, "%e%c", &float_temp, &hack_char) == 1)
#endif
	{
	  bind_float((CPtr)(term), float_temp);
	}
      else return FALSE;	/* fail */
    }
  } else {	/* use is: NUMBER --> CHARS/CODES/DIGITS */
    if (isinteger(term)) {
      sprintf(str, "%ld", (long)int_val(term));
    } else {
      if (isfloat(term)) {
	sprintf(str, "%e", float_val(term));
      } else {
	if (isboxedinteger(term)) {
	  sprintf(str,"%ld",(long)boxedint_val(term));
	} else {
	  err_handle(TYPE, 1, call_name, 2, "number", term);
	  return FALSE;	/* fail */
	}
      }
    }
    new_list = makelist(hreg);
    for (i=0; str[i] != '\0'; i++) {
      if (call_type==NUMBER_CODES)
	follow(hreg++) = makeint((unsigned char)str[i]);
      else if (call_type==NUMBER_CHARS) {
	tmpstr[0] = str[i];
	tmpstr[1] = '\0';
	tmpstr_interned=string_find(tmpstr,1);
	follow(hreg++) = makestring(tmpstr_interned);
      } else { /* NUMBER_DIGITS */
	tmpval = str[i] - '0';
	if (0 <= tmpval && tmpval < 10)
	  follow(hreg++) = makeint((unsigned char)str[i] - '0');
	else {
	  tmpstr[0] = str[i];
	  tmpstr[1] = '\0';
	  tmpstr_interned=string_find(tmpstr,1);
	  follow(hreg++) = makestring(tmpstr_interned);
	}
      }
      top = hreg++;
      follow(top) = makelist(hreg);
    } follow(top) = makenil;
    return unify(list, new_list);
  }
  return TRUE;
}


inline static xsbBool sort(void)
{
  /* r1: +list of terms; r2: ?sorted list of terms */
  int i, len;
  Cell *cell_tbl;
  Cell heap_addr, term, term2;
  Cell list, new_list;
  CPtr top = 0;

  list = ptoc_tag(1);
  term2 = list; len = 0;
  do {
    XSB_Deref(term2);
    if (isnil(term2)) break;
    if (islist(term2)) {
      len++; term2 = cell(clref_val(term2)+1);
    } else {
      if (isref(term2)) err(INSTANTIATION, 1, "sort", 2);
      else err_handle(TYPE, 1, "sort", 2, "list", list);
      return FALSE;	/* fail */
    }
  } while(1);
  check_glstack_overflow(3, pcreg, (2*len)*sizeof(Cell)) ;
  list = ptoc_tag(1); /* reset in case moved */
  if (len > 0) {
    term2 = list;
    cell_tbl = (Cell *)malloc((len * sizeof(Cell)));
    if (!cell_tbl)
      xsb_abort("Cannot allocate temporary memory for sort/2");
    for (i=0 ; i < len ; ++i) {
      XSB_Deref(term2);	/* Necessary for correctness.	*/
      heap_addr = cell(clref_val(term2)); XSB_Deref(heap_addr);
      cell_tbl[i] = heap_addr;
      term2 = cell(clref_val(term2)+1);
    }
    qsort(cell_tbl, len, sizeof(Cell), compare);
    new_list = makelist(hreg);
    follow(hreg++) = cell_tbl[0]; top = hreg++;
    follow(top) = makelist(hreg);
    for (i=1 ; i < len ; i++) {
      if (compare((void*)cell_tbl[i], (void*)cell_tbl[i-1])) {
	follow(hreg++) = cell_tbl[i];
	top = hreg++;
	follow(top) = makelist(hreg);
      }
    } follow(top) = makenil;
    free(cell_tbl);
    term = ptoc_tag(2);
    return unify(new_list, term);
  }
  term = ptoc_tag(2);
  return unify(list, term);
}

inline static xsbBool keysort(void)
{
  /* r1: +list of terms of the form Key-Value;	*/
  /* r2: ?sorted list of terms			*/
  int i, len;
  Cell heap_addr, term, term2;
  Cell list, new_list;
  Cell *cell_tbl;
  CPtr top = 0;

  list = ptoc_tag(1);
  term2 = list; len = 0;
  do {
    XSB_Deref(term2);
    if (isnil(term2)) break;
    if (islist(term2)) {
      heap_addr = cell(clref_val(term2)); XSB_Deref(heap_addr);
      if (isconstr(heap_addr) && 
	  get_arity(get_str_psc(heap_addr)) == 2 &&
	  !strcmp(get_name(get_str_psc(heap_addr)), "-")) {
	len++; term2 = cell(clref_val(term2)+1);
      } else {
	err_handle(TYPE, 1, "keysort", 2,
		   "pair of the form Key-Value", (Cell)NULL);
	return FALSE;	/* fail */
      }
    } else {
      if (isref(term2)) err(INSTANTIATION, 1, "keysort", 2);
      else err_handle(TYPE, 1, "keysort", 2, "list", list);
      return FALSE;	/* fail */
    }
  } while(1);
  check_glstack_overflow(3, pcreg, (2*len)*sizeof(Cell)) ;
  list = ptoc_tag(1);  /* reset in case moved */
  term = ptoc_tag(2);
  if (len > 0) {
    term2 = list;
    cell_tbl = (Cell *)malloc(len * sizeof(Cell));
    if (!cell_tbl)
      xsb_abort("Cannot allocate temporary memory for keysort/2");
    for (i=0 ; i < len ; ++i) {
      XSB_Deref(term2);	/* Necessary for correctness.	*/
      heap_addr = cell(clref_val(term2)); XSB_Deref(heap_addr);
      cell_tbl[i] = heap_addr;
      term2 = cell(clref_val(term2)+1);
    }
    qsort(cell_tbl, len, sizeof(Cell), key_compare);
    new_list = makelist(hreg);
    for (i=0 ; i < len ; i++) {
      follow(hreg++) = cell_tbl[i];
      top = hreg++;
      follow(top) = makelist(hreg);
    } follow(top) = makenil;
    free(cell_tbl);
    return unify(new_list, term);
  }
  return unify(list, term);
}

long sort_par_dir[10];
long sort_par_ind[10];
long sort_num_pars;

int par_key_compare(const void * t1, const void * t2) {
  long ipar, cmp, ind1, ind2;
  Cell term1 = (Cell) t1 ;
  Cell term2 = (Cell) t2 ;

  XSB_Deref(term1);		/* term1 is not in register! */
  XSB_Deref(term2);		/* term2 is not in register! */
  if (sort_num_pars > 0) {
    ipar = 0;
    while (ipar < sort_num_pars) {
      ind1 = ind2 = sort_par_ind[ipar];
      if (islist(term1)) ind1--;
      if (islist(term2)) ind2--;
      cmp = compare((void*)cell(clref_val(term1)+ind1),
		    (void*)cell(clref_val(term2)+ind2));
      if (cmp) {
	if (sort_par_dir[ipar]) return cmp;
	else return -cmp;
      } else ipar++;
    }
    return 0;
  } else if (sort_num_pars == 0) {
    return compare((void*)term1, (void*)term2);
  } else
    return -compare((void*)term1, (void*)term2);
}

inline static xsbBool parsort(void)
{
  /* r1: +list of terms;				*/
  /* r2: +list of sort indicators: asc(I) or desc(I)	*/
  /* r3: 1 if eliminate dupls, 0 if not			*/
  /* r4: ?sorted list of terms				*/
  int i, len;
  int max_ind = 0, elim_dupls;
  Cell heap_addr, term, term2, tmp_ind;
  Cell list, new_list;
  Cell *cell_tbl;
  CPtr top = 0;
  char ermsg[50];

  elim_dupls = ptoc_int(3);

  list = ptoc_tag(2);
  term2 = list; sort_num_pars = 0;

  XSB_Deref(term2);
  if (isstring(term2) && !strcmp(string_val(term2),"asc")) sort_num_pars = 0;
  else if (isstring(term2) && !strcmp(string_val(term2),"desc")) sort_num_pars = -1;
  else
    while (TRUE) {
      if (isnil(term2)) break;
      if (islist(term2)) {
	heap_addr = cell(clref_val(term2)); XSB_Deref(heap_addr);
	if (isconstr(heap_addr) && 
	    get_arity(get_str_psc(heap_addr)) == 1 &&
	    !strcmp(get_name(get_str_psc(heap_addr)),"asc")) {
	  sort_par_dir[sort_num_pars] = 1;
	} else if (isconstr(heap_addr) && 
		   get_arity(get_str_psc(heap_addr)) == 1 &&
		   !strcmp(get_name(get_str_psc(heap_addr)),"desc")) {
	  sort_par_dir[sort_num_pars] = 0;
	} else err_handle(TYPE,2,"parsort",4,"asc/1 or desc/1 term",heap_addr);
	tmp_ind = cell(clref_val(heap_addr)+1); XSB_Deref(tmp_ind);
	if (!isinteger(tmp_ind)) err_handle(TYPE,2,"parsort",4,"integer",tmp_ind);
	i = int_val(tmp_ind);
	if (i < 1 || i > 255) err_handle(TYPE,2,"parsort",4,"arity-sized integer",tmp_ind);
	sort_par_ind[sort_num_pars] = i;
	if (i > max_ind) max_ind = i;
	sort_num_pars++;
	term2 = cell(clref_val(term2)+1);
	XSB_Deref(term2);
      } else err_handle(TYPE, 2, "parsort", 4, "list or asc or desc", list);
    }
      
  list = ptoc_tag(1);
  term2 = list; len = 0;
  do {
    XSB_Deref(term2);
    if (isnil(term2)) break;
    if (islist(term2)) {
      heap_addr = cell(clref_val(term2)); XSB_Deref(heap_addr);
      if (sort_num_pars == 0 || 
	  (isconstr(heap_addr) && get_arity(get_str_psc(heap_addr)) >= max_ind) ||
	  (islist(heap_addr) && max_ind <=2)) {
	len++; term2 = cell(clref_val(term2)+1);
      } else {
	sprintf(ermsg,"Term with arity at least %d", max_ind);
	err_handle(TYPE, 1, "parsort", 4, ermsg, (Cell)NULL);
	return FALSE;	/* fail */
      }
    } else {
      if (isref(term2)) err(INSTANTIATION, 1, "parsort", 4);
      else err_handle(TYPE, 1, "parsort", 4, "list", list);
      return FALSE;	/* fail */
    }
  } while(1);

  check_glstack_overflow(4, pcreg, (2*len)*sizeof(Cell)) ;
  list = ptoc_tag(1);  /* reset in case moved */
  term = ptoc_tag(4);
  if (len > 0) {
    term2 = list;
    cell_tbl = (Cell *)malloc(len * sizeof(Cell));
    if (!cell_tbl)
      xsb_abort("Cannot allocate temporary memory for parsort/4");
    for (i=0 ; i < len ; ++i) {
      XSB_Deref(term2);	/* Necessary for correctness.	*/
      heap_addr = cell(clref_val(term2)); XSB_Deref(heap_addr);
      cell_tbl[i] = heap_addr;
      term2 = cell(clref_val(term2)+1);
    }
    qsort(cell_tbl, len, sizeof(Cell), par_key_compare);
    new_list = makelist(hreg);
    if (elim_dupls) {
      follow(hreg++) = cell_tbl[0]; top = hreg++;
      follow(top) = makelist(hreg);
      for (i=1 ; i < len ; i++) {
	if (compare((void*)cell_tbl[i], (void*)cell_tbl[i-1])) {
	  follow(hreg++) = cell_tbl[i];
	  top = hreg++;
	  follow(top) = makelist(hreg);
	}
      } 
    } else {
      for (i=0 ; i < len ; i++) {
	follow(hreg++) = cell_tbl[i];
	top = hreg++;
	follow(top) = makelist(hreg);
      } 
    }
    follow(top) = makenil;
    free(cell_tbl);
    return unify(new_list, term);
  }
  return unify(list, term);
}

