/* File:      cinterf.c
** Author(s): Jiyang Xu
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
** $Id: cinterf.c,v 1.37 2001-07-06 18:19:42 kifer Exp $
** 
*/


#include "xsb_config.h"
#include "xsb_debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "auxlry.h"
#include "cell_xsb.h"
#include "memory_xsb.h"
#include "register.h"
#include "psc_xsb.h"
#include "flags_xsb.h"
#include "deref.h"
#include "heap_xsb.h"
#include "binding.h"
#include "tries.h"
#include "choice.h"
#include "subp.h"
#include "emuloop.h"
#include "cinterf.h"
#include "error_xsb.h"
#include "orient_xsb.h"

/* the following really belongs somewhere else */
extern char *expand_filename(char *);
extern void xsb_sprint_variable(char *sptr, CPtr var);


char *p_charlist_to_c_string(prolog_term term, VarString *buf,
			     char *in_func, char *where);
void c_string_to_p_charlist(char *name, prolog_term list,
			    char *in_func, char *where);

/*======================================================================*/
/* Low level C interface						*/
/*======================================================================*/

DllExport xsbBool call_conv is_var(prolog_term term)
{
    Cell t = (Cell)term;
    return isref(t);
}

DllExport xsbBool call_conv is_int(prolog_term term)
{
    Cell t = (Cell)term;
    return isinteger(t);
}

DllExport xsbBool call_conv is_float(prolog_term term)
{
    Cell t = (Cell)term;
    return isfloat(t);
}

DllExport xsbBool call_conv is_string(prolog_term term)
{
    Cell t = (Cell)term;
    return isstring(t);
}

DllExport xsbBool call_conv is_list(prolog_term term)
{
    Cell t = (Cell)term;
    return islist(t);
}

DllExport xsbBool call_conv is_nil(prolog_term term)
{
    Cell t = (Cell)term;
    return isnil(t);
}

DllExport xsbBool call_conv is_functor(prolog_term term)
{
    Cell t = (Cell)term;
    return isconstr(t);
}

DllExport xsbBool call_conv is_attv(prolog_term term)
{
    Cell t = (Cell)term;
    return isattv(t);
}

DllExport prolog_term call_conv reg_term(reg_num regnum)
{
    register Cell addr;

    addr = cell(reg+regnum);
    XSB_Deref(addr);
    return (prolog_term)addr;
}

DllExport xsbBool call_conv c2p_int(Integer val, prolog_term var)
{
    Cell v = (Cell)var;
    if (is_var(v)) {
	bind_int(vptr(v), val);
	return TRUE;
    } else {
	xsb_warn("[C2P_INT] Argument 2 must be a variable");
	return FALSE;
    }
}

DllExport xsbBool call_conv c2p_float(double val, prolog_term var)
{
    Cell v = (Cell)var;
    if (is_var(v)) {
	bind_float(vptr(v), (Float)(val));
	return TRUE;
    } else {
	xsb_warn("[C2P_FLOAT] Argument 2 must be a variable");
	return FALSE;
    }
}

DllExport xsbBool call_conv c2p_string(char *val, prolog_term var)
{
    Cell v = (Cell)var;
    if (is_var(v)) {
	bind_string(vptr(v), (char *)string_find(val, 1));
	return TRUE;
    } else {
	xsb_warn("[C2P_STRING] Argument 2 must be a variable");
	return FALSE;
    }
}

DllExport xsbBool call_conv c2p_list(prolog_term var)
{
    Cell v = (Cell)var;
    if (is_var(v)) {
	sreg = hreg;
	new_heap_free(hreg);
	new_heap_free(hreg);
	bind_list(vptr(v), sreg);
	return TRUE;
    } else {
	xsb_warn("[C2P_LIST] Argument must be a variable");
	return FALSE;
    }
}

DllExport xsbBool call_conv c2p_nil(prolog_term var)
{
    Cell v = (Cell)var;
    if (is_var(v)) {
       bind_nil(vptr(v));
       return TRUE;
    } else {
	xsb_warn("[C2P_NIL] Argument must be a variable");
	return FALSE;
    }
}

DllExport void call_conv c2p_setfree(prolog_term var)
{
    CPtr v = (CPtr)var;
    bld_free(v);
}

DllExport xsbBool call_conv c2p_functor(char *functor, int arity, prolog_term var)
{
    Cell v = (Cell)var;
    Pair sym;
    int i;
    if (is_var(v)) {
	sym = (Pair)insert(functor, (byte)arity, (Psc)flags[CURRENT_MODULE], &i);
	sreg = hreg;
	hreg += arity + 1;
	bind_cs(vptr(v), sreg);
	new_heap_functor(sreg, sym->psc_ptr);
	for (i=0; i<arity; sreg++,i++) { bld_free(sreg); }
	return TRUE;
    } else {
	xsb_warn("[C2P_FUNCTOR] Argument 3 must be a variable");
	return FALSE;
    }
}

DllExport Integer call_conv p2c_int(prolog_term term)
{
    Cell t = (Cell)term;
    return int_val(t);
}

DllExport double call_conv p2c_float(prolog_term term)
{
    Cell t = (Cell)term;
    return (double)(float_val(t));
}

DllExport char* call_conv p2c_string(prolog_term term)
{
    Cell t = (Cell)term;
    return string_val(t);
}

DllExport char* call_conv p2c_functor(prolog_term term)
{
    Cell t = (Cell)term;
    return get_name(get_str_psc(t));
}

DllExport int call_conv p2c_arity(prolog_term term)
{
    Cell t = (Cell)term;
    return get_arity(get_str_psc(t));
}

DllExport prolog_term call_conv p2p_arg(prolog_term term, int argno)
{
    Cell t = (Cell)term;
    XSB_Deref(t);
    t = cell(clref_val(t)+argno);
    XSB_Deref(t);
    return (prolog_term)t;
}

DllExport prolog_term call_conv p2p_car(prolog_term term)
{
    Cell t = (Cell)term;
    XSB_Deref(t);
    t = cell(clref_val(t));
    XSB_Deref(t);
    return (prolog_term)t;
}

DllExport prolog_term call_conv p2p_cdr(prolog_term term)
{
    Cell t = (Cell)term;
    XSB_Deref(t);
    t = cell(clref_val(t)+1);
    XSB_Deref(t);
    return (prolog_term)t;
}

DllExport prolog_term call_conv p2p_new(void)
{
    CPtr t = hreg;
    new_heap_free(hreg);
    return (prolog_term)(cell(t));
}

DllExport xsbBool call_conv p2p_unify(prolog_term term1, prolog_term term2)
{
    return unify(term1, term2);
}

DllExport prolog_term call_conv p2p_deref(prolog_term term)
{
    Cell t = (Cell)term;
    XSB_Deref(t);
    return (prolog_term)t;
}


/* convert Arg 1 -- prolog list of characters (a.k.a. prolog string) into C
   string and return this string.
   Arg 2: ptr to string buffer where the result is to be returned.
          Space for this buffer must already be allocated.
   Arg 3: which function was called from.
   Arg 4: where in the call this happened.
   Args 3 and 4 are used for error reporting.
   This function converts escape sequences in the Prolog string
   (except octal/hexadecimal escapes) into the corresponding real characters.
*/
char *p_charlist_to_c_string(prolog_term term, VarString *buf,
			     char *in_func, char *where)
{
  int head_val;
  char head_char[1];
  int escape_mode=FALSE;
  prolog_term list = term, list_head;

  if (!is_list(list) && !is_nil(list)) {
    xsb_abort("[%s] %s is not a list of characters", in_func, where);
  }

  XSB_StrSet(buf, "");

  while (is_list(list)) {
    if (is_nil(list)) break;
    list_head = p2p_car(list);
    if (!is_int(list_head)) {
      xsb_abort("[%s] A Prolog string (a character list) expected, %s",
		in_func, where);
    }
    head_val = int_val(list_head);
    if (head_val < 0 || head_val > 255) {
      xsb_abort("[%s] A Prolog string (a character list) expected, %s",
		in_func, where);
    }

    *head_char = (char) head_val;
    /* convert ecape sequences */
    if (escape_mode)
      switch (*head_char) {
      case 'a':
	XSB_StrAppendBlk(buf, "\a", 1);
	break;
      case 'b':
	XSB_StrAppendBlk(buf, "\b", 1);
	break;
      case 'f':
	XSB_StrAppendBlk(buf, "\f", 1);
	break;
      case 'n':
	XSB_StrAppendBlk(buf, "\n", 1);
	break;
      case 'r':
	XSB_StrAppendBlk(buf, "\r", 1);
	break;
      case 't':
	XSB_StrAppendBlk(buf, "\t", 1);
	break;
      case 'v':
	XSB_StrAppendBlk(buf, "\v", 1);
	break;
      default:
	XSB_StrAppendBlk(buf, head_char, 1);
      }
    else
      XSB_StrAppendBlk(buf, head_char, 1);

    if (*head_char == '\\' && !escape_mode) {
      escape_mode = TRUE;
      buf->length--; /* backout the last char */
    }
    else {
      escape_mode = FALSE;
    }
    list = p2p_cdr(list);
  } /* while */

  XSB_StrNullTerminate(buf);

  return (buf->string);
}


/* convert a C string into a prolog list of characters.
   LIST must be a Prolog variable. IN_FUNC is a string that should indicate the
   high-level function from this c_string_to_p_charlist was called.
   WHERE is another string with additional info. These two are used to provide
   informative error messages to the user. */
void c_string_to_p_charlist(char *name, prolog_term list,
			    char *in_func, char *where)
{
  Cell new_list;
  CPtr top = 0;
  int len=strlen(name), i;

  if (isnonvar(list)) {
    xsb_abort("[%s] A variable expected, %s", in_func, where);
  }
  if (len == 0) {
    bind_nil((CPtr)(list));
  } else {
    new_list = makelist(hreg);
    for (i = 0; i < len; i++) {
      follow(hreg++) = makeint(*(unsigned char *)name);
      name++;
      top = hreg++;
      follow(top) = makelist(hreg);
    } follow(top) = makenil;
    unify(list, new_list);
  } 
}


/* The following function checks if a given term is a prolog string.
   It also counts the size of the list.
   It deals with the same escape sequences as p_charlist_to_c_string */

DllExport xsbBool call_conv is_charlist(prolog_term term, int *size)
{
  int escape_mode=FALSE, head_char;
  prolog_term list, head;

  list = term;
  *size = 0;
  
  /* apparently, is_nil can be true and is_list false?? */
  if(is_nil(list))
    return TRUE;

  if (!is_list(list)) 
    return FALSE;

  while (is_list(list)) {
    if (is_nil(list)) break;

    head = p2p_car(list);
    if (!is_int(head)) 
      return FALSE;
    
    head_char = (char) int_val(head);
    if (head_char < 0 || head_char > 255) 
      return FALSE;

    if (escape_mode)
      switch (head_char) {
      case 'a':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
      case 'v':
	(*size)++;
	escape_mode=FALSE;
	break;
      default:
	(*size) += 2;
      }
    else
      if (head_char == '\\') 
	escape_mode = TRUE;
      else
	(*size)++;
    list = p2p_cdr(list);
  }
  return TRUE;
}

/* the following two functions were introduced by Luis Castro */
/* they extend the c interface to allow for an easy interface for 
lists of characters */
    
DllExport char *call_conv p2c_chars(prolog_term term, VarString *buf)
{
  return p_charlist_to_c_string(term, buf, "p2c_chars", "list -> char*");
}

DllExport void call_conv c2p_chars(char *str, prolog_term term)
{
  c_string_to_p_charlist(str,term,"c2p_chars", "char* -> list");
}


/*
** Constaints and internal data structures
**
*/

#include "setjmp_xsb.h"

static char *subformat[10];
static char *c_dataptr_rest;
static jmp_buf env;

/*
** procedure cppc_error
**
*/

static void cppc_error(int num)
{
    longjmp(env, num);
}

/*
** procedure skip_subfmt
**
*/

static char *skip_subfmt(char *ptr, char quote)
{
    while (*ptr) {
	if (*ptr == quote) return ++ptr;
	else if (*ptr == '[') ptr = skip_subfmt(++ptr, ']');
	else if (*ptr == '(') ptr = skip_subfmt(++ptr, ')');
	else ptr++;
    }
    cppc_error(6);
    return ptr;	/* never reach here */
}

/*
** procedure count_arity
**
** count Prolog term size (arity). Ignored fields are not counted
*/

static int count_arity(char *ptr, int quote)
{
    int arity = 0;

    while (*ptr && arity <= MAX_ARITY) {
	if (*ptr == quote) return arity;
	else if (*ptr == '%') {
	    if (*(++ptr)!='*') arity++;
	} else if (*ptr == '[') ptr = skip_subfmt(++ptr, ']');
	else if (*ptr == '(') ptr = skip_subfmt(++ptr, ')');
	else ptr++;
    }
    cppc_error(5);
    return -1;	/* never reach here */
}

/*
** procedure count_fields
**
** count number of fields in the primary structure.
** should be the same as arity + ignored fields.
*/

static int count_fields(char *ptr, int quote)
{
    int fields = 0;

    while (*ptr && fields <= MAX_ARITY) {
	if (*ptr == quote) return fields;
	else if (*ptr == '%') { fields++; ptr++; }
	else if (*ptr == '[') ptr = skip_subfmt(++ptr, ']');
	else if (*ptr == '(') ptr = skip_subfmt(++ptr, ')');
	else ptr++;
    }
    cppc_error(5);
    return -1;	/* never reach here */
}

/*
** procedure count_csize
**
** count C struct size (number of bytes). Ignored fields are also counted
*/

static int count_csize(char *ptr, int quote)
{
    int size = 0;

    while (*ptr) {
	if (*ptr == quote) return size;
	else if (*ptr == '%') {
	    if (*(++ptr)=='*') ptr++;
	    switch (*ptr) {
		case 'f': size += sizeof(float); ptr++; break;
		case 'd': size += sizeof(double); ptr++; break;
		case 'i': size += sizeof(int); ptr++; break;
		case 'c': size += 1; ptr++; break;
		case 's': size += sizeof(char *); ptr++; break;
		case 'z': ptr++; size += 4 * (*ptr-'0'); ptr++; break;
		case 't': size += sizeof(int *);
		    ptr += 2;
		    skip_subfmt(ptr, ')');
		    break;
		case 'l': size += sizeof(int *);
		    ptr += 2;
		    skip_subfmt(ptr, ')');
		    break;
		case '[': 
		    size += count_csize(++ptr, ']');
		    skip_subfmt(ptr, ']');
		    break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		    size += sizeof(int *); ptr++; break;
		default: cppc_error(7); break;
	    }
	}
    }
    cppc_error(8);
    return -1;	/* never reach here */
}

/*
** procedure ctop_term0
**
*/

static char *ctop_term0(char *ptr, char *c_dataptr,
			prolog_term variable, int ignore)
{
    char ch;
    int fmtnum;
    char *cdptr2;
    int  argno, fields, i;
    int ignore1;

    if (*ptr++!= '%') cppc_error(1);
    ch = *ptr++;
    if (ch=='*') ch = *ptr++;
    switch (ch) {
	case 'i':			/* int */
	
	if (!ignore) c2p_int(*((int *)(c_dataptr)), variable);
	c_dataptr_rest = c_dataptr + sizeof(int);
	break;

	case 'c':

	if (!ignore) c2p_int((int)(*(char *)(c_dataptr)), variable);
	c_dataptr_rest = c_dataptr + 1;
	break;

	case 's':

	if (!ignore) c2p_string(*(char **)(c_dataptr), variable);
	c_dataptr_rest = c_dataptr + sizeof(char*);
	break;

	case 'z':

	if (!ignore) c2p_string(c_dataptr, variable);
	ch = *ptr++;
	c_dataptr_rest = c_dataptr + (ch -'0')*4;
	break;

	case 'f':

	if (!ignore) c2p_float((double)(*((float *)(c_dataptr))), variable);
	c_dataptr_rest = c_dataptr + sizeof(float);
	break;

	case 'd':

	if (!ignore) c2p_float(*((double *)(c_dataptr)), variable);
	c_dataptr_rest = c_dataptr + sizeof(double);
	break;

	case '[':

	fields = count_fields(ptr, ']');
	if (!ignore) {
	    argno = count_arity(ptr, ']');
	    if (!is_functor(variable)) c2p_functor("c2p", argno, variable);
	}
	argno = 0;
	for (i = 1; i <= fields; i++) {
	    if (*(ptr+1)=='*') ignore1 = 1;
	    else { ignore1 = ignore; argno++; }
	    ptr = ctop_term0(ptr,c_dataptr,p2p_arg(variable,argno),ignore1);
	    c_dataptr = c_dataptr_rest;
	}
	ptr = skip_subfmt(ptr, ']');
	break;

	case 't':

	if (!ignore) {
	    if (*(char **)(c_dataptr)) {
		fmtnum = (int)(*ptr-'0');
		subformat[fmtnum] = ptr-2;
		ptr++;
		if (*(ptr++) !='(') cppc_error(2);
		argno = count_arity(ptr, ')');
		fields = count_fields(ptr, ')');
		if (!is_functor(variable)) c2p_functor("c2p", argno, variable);
		cdptr2 = * (char **)(c_dataptr);
		argno = 0;
		for (i = 1; i <= fields; i++) {
		    if (*(ptr+1)=='*') ignore = 1;
		    else { ignore = 0; argno++; }
		    ptr = ctop_term0(ptr,cdptr2,p2p_arg(variable,argno),ignore);
		    cdptr2 = c_dataptr_rest;
		}
	    } else c2p_nil(variable);
	}
	ptr = skip_subfmt(ptr, ')');
	c_dataptr_rest = c_dataptr + 4;
	break;

	case 'l':
	if (!ignore) {
	    if (*(char **)(c_dataptr)) {
		fmtnum = (int)(*ptr-'0');
		subformat[fmtnum] = ptr-2;
		ptr++;
		if (*(ptr++) != '(') cppc_error(3);
		argno = count_arity(ptr, ')');
		fields = count_fields(ptr, ')');
		if (!is_list(variable)) c2p_list(variable);
		cdptr2 = * (char **)(c_dataptr);
		argno = 0;
		for (i = 1; i <= fields; i++) {
		    if (*(ptr+1)=='*') ignore = 1;
		    else { ignore = 0; argno++; }
		    if (argno==1) 
		       ptr = ctop_term0(ptr,cdptr2,p2p_car(variable),ignore);
		    else if (argno==2)
		       ptr = ctop_term0(ptr,cdptr2,p2p_cdr(variable),ignore);
		    else if (argno==0)
		       ptr = ctop_term0(ptr,cdptr2,p2p_car(variable),ignore);
		       /* always ignored */
		    else cppc_error(30);
		    cdptr2 = c_dataptr_rest;
		}
	    } else c2p_nil(variable);
	}
	ptr = skip_subfmt(ptr, ')');
	c_dataptr_rest = c_dataptr + 4;
	break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':

	if (!ignore) {
	    if (*(char **)(c_dataptr)) {
		ctop_term0(subformat[ch-'0'], c_dataptr, variable, 0);
	    } else c2p_nil(variable);
	}
	c_dataptr_rest = c_dataptr + 4;
	break;

	default: cppc_error(4);
    }
    return ptr;
}

/*
** procedure ptoc_term0
**
*/

static char *ptoc_term0(char *ptr, char *c_dataptr,
			prolog_term variable, int ignore)
{
    char ch;
    int fmtnum;
    char *cdptr2;
    int  argno, fields, i, size;
    int ignore1;

    if (*ptr++!= '%') cppc_error(9);
    ch = *ptr++;
    if (ch=='*') ch = *ptr++;
    switch (ch) {
	case 'i':			/* int */

	if (!ignore) {
	    if (is_int(variable)) *((int *)(c_dataptr)) = p2c_int(variable);
	    else cppc_error(10);
	}
	c_dataptr_rest = c_dataptr + sizeof(int);
	break;

	case 'c':

	if (!ignore) {
	    if (is_int(variable)) *((char *)(c_dataptr)) = 
	       (char)p2c_int(variable);
	    else cppc_error(11);
	}
	c_dataptr_rest = c_dataptr + 1;
	break;

	case 's':

	if (!ignore) {
	    if (is_string(variable)) *((char **)(c_dataptr)) =
	       p2c_string(variable);		/* should make a copy??? */
	    else cppc_error(12);
	}
	c_dataptr_rest = c_dataptr + 4;
	break;

	case 'z':

	ch = *ptr++;
	size = 4 * (ch - '0');
	if (!ignore) {
	    if (is_string(variable)) 
	       strncpy(c_dataptr, p2c_string(variable), size);
	    else cppc_error(12);
	}
	c_dataptr_rest = c_dataptr + size;
	break;

	case 'f':

	if (!ignore) {
	    if (is_float(variable)) 
	      *((float *)(c_dataptr)) = (float)p2c_float(variable);
	    else cppc_error(13);
	}
	c_dataptr_rest = c_dataptr + sizeof(float);
	break;

	case 'd':

	if (!ignore) {
	    if (is_float(variable)) *((double *)(c_dataptr)) =
	       p2c_float(variable);
	    else cppc_error(14);
	}
	c_dataptr_rest = c_dataptr + sizeof(double);
	break;

	case '[':

	fields = count_fields(ptr, ']');
	argno = 0;
	for (i = 1; i <= fields; i++) {
	    if (*(ptr+1)=='*') ignore1 = 1;
	    else { ignore1 = ignore; argno++; }
	    ptr = ptoc_term0(ptr, c_dataptr,p2p_arg(variable,argno),ignore1);
	    c_dataptr = c_dataptr_rest;
	}
	ptr = skip_subfmt(ptr, ']');
	break;

	case 't':

	if (!ignore) {
	    fmtnum = (int)(*ptr-'0');
	    subformat[fmtnum] = ptr-2;
	    ptr++;
	    if (*(ptr++) != '(') cppc_error(15);
	    fields = count_fields(ptr, ')');
	    size = count_csize(ptr, ')');
	    cdptr2 = (char *)malloc(size);
	    *((char **)c_dataptr) = cdptr2;
	    argno = 0;
	    for (i = 1; i <= fields; i++) {
		if (*(ptr+1)=='*') ignore = 1;
		else { ignore = 0; argno++; }
		ptr = ptoc_term0(ptr,cdptr2,p2p_arg(variable,argno),ignore);
		cdptr2 = c_dataptr_rest;
	    }
	}
	ptr = skip_subfmt(ptr, ')');
	c_dataptr_rest = c_dataptr + 4;
	break;

	case 'l':
	if (!ignore) {
	    fmtnum = (int)(*ptr-'0');
	    subformat[fmtnum] = ptr-2;
	    ptr++;
	    if (*(ptr++)!='(') cppc_error(16);
	    fields = count_fields(ptr, ')');
	    size = count_csize(ptr, ')');
	    cdptr2 = (char *)malloc(size);
	    *((char **)c_dataptr) = cdptr2;
	    argno = 0;
	    for (i = 1; i <= fields; i++) {
		if (*(ptr+1)=='*') ignore = 1;
		else { ignore = 0; argno++; }
		if (argno==1)
		   ptr = ptoc_term0(ptr,cdptr2,p2p_car(variable),ignore);
		else if (argno==2)
		   ptr = ptoc_term0(ptr,cdptr2,p2p_cdr(variable),ignore);
		else cppc_error(31);
		cdptr2 = c_dataptr_rest;
	    }
	}
	ptr = skip_subfmt(ptr, ')');
	c_dataptr_rest = c_dataptr + 4;
	break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':

	if (!ignore) {
	    if (!is_nil(variable)) {
		ptoc_term0(subformat[ch-'0'], c_dataptr, variable, 0);
	    } else *(int *)(c_dataptr) = 0;
	}
	c_dataptr_rest = c_dataptr + 4;
	break;

	default: cppc_error(17);
    }
    return ptr;
}

/*
** procedure ctop_term
**
*/

int ctop_term(char *fmt, char *c_dataptr, reg_num regnum)
{
    prolog_term variable;
    int my_errno;

    variable = reg_term(regnum);
    if ((my_errno = setjmp(env))) return my_errno;  /* catch an exception */
    ctop_term0(fmt, c_dataptr, variable, 0);
    return 0;
}

/*
** procedure ptoc_term
**
*/

int ptoc_term(char *fmt, char *c_dataptr, reg_num regnum)
{
    prolog_term variable;
    int my_errno;

    variable = reg_term(regnum);
    if ((my_errno = setjmp(env))) return my_errno;  /* catch an exception */
    ptoc_term0(fmt, c_dataptr, variable, 0);
    return 0;
}

/*
** procedure c2p_term
**
*/

int c2p_term(char *fmt, char *c_dataptr, prolog_term variable)
{
    int my_errno;

    if ((my_errno = setjmp(env))) return my_errno;  /* catch an exception */
    ctop_term0(fmt, c_dataptr, variable, 0);
    return 0;
}

/*
** procedure p2c_term
**
*/

int p2c_term(char *fmt, char *c_dataptr, prolog_term variable)
{
    int my_errno;

    if ((my_errno = setjmp(env))) return my_errno;  /* catch an exception */
    ptoc_term0(fmt, c_dataptr, variable, 0);
    return 0;
}
/* quick test to see whether atom must be quoted */
int mustquote(char *atom)
{
    int i;

    if (!(atom[0] >= 'a' && atom[0] <= 'z')) return TRUE;
    for (i=1; atom[i] != '\0'; i++) {
        if (!((atom[i] >= 'a' && atom[i] <= 'z') ||
             (atom[i] >= 'A' && atom[i] <= 'Z') ||
             (atom[i] == '_') ||
             (atom[i] >= '0' && atom[i] <= '9')
             )) return TRUE;
    }
    return FALSE;
}

/* copy a string (quoted if !toplevel and necessary) into a buffer.  */
void printpstring(char *atom, int toplevel, VarString *straddr)
{
    int i;
   
    if (toplevel || !mustquote(atom)) {
      XSB_StrAppend(straddr,atom);
    } else {
      XSB_StrAppendBlk(straddr, "'", 1);
      for (i = 0; atom[i] != '\0'; i++) {
	XSB_StrAppendBlk(straddr, atom+i, 1);
	if (atom[i] == '\'')
	  /* double the quotes in a quoted string */
	  XSB_StrAppendBlk(straddr, "'", 1);
      }
      XSB_StrAppend(straddr, "'");
    }
}
    
/* calculate approximate length of a printed term.  For space alloc. */
int clenpterm(prolog_term term)
{
  int i, clen;

  if (is_var(term)) return 11;
  else if (is_int(term)) return 12;
  else if (is_float(term)) return 12;
  else if (is_nil(term)) return 2;
  else if (is_string(term)) return strlen(p2c_string(term))+5;
  else if (is_list(term)) {
      clen = 1;
      clen += clenpterm(p2p_car(term)) + 1;
      while (is_list(term)) {
          clen += clenpterm(p2p_car(term)) + 1;
          term = p2p_cdr(term);
      }
      if (!is_nil(term)) {
          clen += clenpterm(term) + 1;
      }
      return clen+1;
  } else if (is_functor(term)) {
      clen = strlen(p2c_functor(term))+5;
      if (p2c_arity(term) > 0) {
          clen += clenpterm(p2p_arg(term,1)) + 1;
          for (i = 2; i <= p2c_arity(term); i++) {
              clen += clenpterm(p2p_arg(term,i)) + 1;
          }
          return clen + 1;
      } else return clen;
  } else {
      xsb_warn("Unrecognized prolog term type");
      return 0;
  }
}

char tempstring[MAXBUFSIZE];

/* print a prolog_term into a buffer.
   Atoms are quoted if !toplevel -- necessary for Prolog reading 
   Buffer is a VarString. If the VarString is non-empty, the term is appended
   to the current contents of the VarString.
*/
DllExport void call_conv print_pterm(prolog_term term, int toplevel, VarString *straddr)
{
  int i;

  if (is_var(term)) {
    xsb_sprint_variable(tempstring, (CPtr) term);
    XSB_StrAppend(straddr,tempstring);
  } else if (is_int(term)) {
    sprintf(tempstring,"%d", (int) p2c_int(term));
    XSB_StrAppend(straddr,tempstring);
  } else if (is_float(term)) {
    sprintf(tempstring,"%f", (float) p2c_float(term));
    XSB_StrAppend(straddr,tempstring);
  } else if (is_nil(term)) {
    XSB_StrAppend(straddr,"[]");
  } else if (is_string(term)) {
    printpstring(p2c_string(term),toplevel,straddr);
  } else if (is_list(term)) {
    XSB_StrAppend(straddr, "[");
    print_pterm(p2p_car(term),FALSE,straddr);
    term = p2p_cdr(term);
    while (is_list(term)) {
      XSB_StrAppend(straddr, ",");
      print_pterm(p2p_car(term),FALSE,straddr);
      term = p2p_cdr(term);
    }
    if (!is_nil(term)) {
      XSB_StrAppend(straddr, "|");
      print_pterm(term,FALSE,straddr);
    }
    XSB_StrAppend(straddr, "]");
  } else if (is_functor(term)) {
    printpstring(p2c_functor(term),FALSE,straddr);
    if (p2c_arity(term) > 0) {
      XSB_StrAppend(straddr, "(");
      print_pterm(p2p_arg(term,1),FALSE,straddr);
      for (i = 2; i <= p2c_arity(term); i++) {
	XSB_StrAppend(straddr, ",");
	print_pterm(p2p_arg(term,i),FALSE,straddr);
      }
      XSB_StrAppend(straddr, ")");
    }
  } else xsb_warn("[PRINT_PTERM] Unrecognized prolog term type");
}

/************************************************************************/
/*                                                                      */
/*	xsb_answer_string copies an answer from reg 2 into ans.		*/
/*                                                                      */
/************************************************************************/
int xsb_answer_string(VarString *ans, char *sep) {
  int i;
  
  for (i=1; i<p2c_arity(reg_term(2)); i++) {
    print_pterm(p2p_arg(reg_term(2),i),TRUE,ans);
    XSB_StrAppend(ans,sep);
  }
  print_pterm(p2p_arg(reg_term(2),p2c_arity(reg_term(2))),TRUE,ans);
  return 0;
}


/************************************************************************/
/*                                                                      */
/* xsb_init(argc,argv) initializes XSB to be called from C.             */
/* argc and argv are the arg count and arg vector as passed from the    */
/* command line.  The parameters are used to set space sizes in xsb.    */
/* The parameters MUST include -i, which indicates that the main        */
/* interpreter is to be loaded, AND -n, which indicates that the        */
/* interpreter should not enter the usual read-eval-print loop, but     */
/* instead support the interface to the C caller.                       */
/*   If xsb has been previously initialized, nothing is done and 1 is   */
/* returned.                                                            */
/*                                                                      */
/************************************************************************/

int xsb_initted = 0;   /* if xsb has been called */

DllExport int call_conv xsb_init(int argc, char *argv[])
{
  char executable1[MAXPATHLEN];
  if (xsb_initted) return(1);

  /* we rely on the caller to tell us in argv[0]
     the absolute or relative path name to the XSB installation directory */
  sprintf(executable1, "%s%cconfig%c%s%cbin%cxsb",
	  argv[0], SLASH, SLASH, FULL_CONFIG_NAME, SLASH, SLASH);
  strcpy(executable, expand_filename(executable1));


  xsb(0, argc,argv);  /* initialize xsb */

  xsb(1, 0, 0);       /* enter xsb to set up regs */
  xsb_initted = 1;
  return(0);
}

/************************************************************************/
/*								        */
/*  int xsb_cmd_string(char *cmdline, char **argv) takes a		*/
/*  command line string in cmdline, and parses it to return an argv	*/
/*  vector in its second argument, and the argc count as the value of   */
/*	the function.  (Will handle a max of 19 args.)			*/
/*									*/
/************************************************************************/
/*FILE *stream_err, *stream_out;*/

DllExport int call_conv xsb_init_string(char *cmdline_param) {
	int i = 0, argc = 0;
	char **argv, delim;
	char cmdline[2*MAXPATHLEN+1];

	/*stream_err = freopen("XSB_errlog", "w", stderr);
	  stream_out = freopen("XSB_outlog", "w", stdout);*/

	if (strlen(cmdline_param) > 2*MAXPATHLEN) {
	    xsb_warn("**************************************************************************");
	    xsb_warn("[XSB_INIT_STRING] %18s...: command used to call XSB server is too long",
		    cmdline_param);
	    xsb_warn("**************************************************************************");
	    exit(1);
	}
	strncpy(cmdline, cmdline_param, 2*MAXPATHLEN - 1);
	argv = (char **) malloc(20*sizeof(char *));

	while (cmdline[i] == ' ') i++;
	while (cmdline[i] != '\0') {
		if ((cmdline[i] == '"') || (cmdline[i] == '\'')) {
			delim = cmdline[i];
			i++;
		} else delim = ' ';
		argv[argc] = &(cmdline[i]);
		argc++;
		if (argc >= 19) {argc--; break;}
		while ((cmdline[i] != delim) && (cmdline[i] != '\0')) i++;
		if (cmdline[i] == '\0') break;
		cmdline[i] = '\0';
		i++;
		while (cmdline[i] == ' ') i++;
	}
	argv[argc] = 0;
	return xsb_init(argc,argv);
}

/************************************************************************/
/*                                                                      */
/* xsb_command() passes the command (i.e. query with no variables) to   */
/* xsb.  The command must be put into xsb's register 1 as a term, by    */
/* the caller who uses the c2p_* (and perhaps p2p_*) functions.         */
/*   It returns 0 if it succeeds, 1 if it fails, in either case         */
/* resetting register 1 back to a free variable.  It returns 2 if there */
/* is an error.                                                         */
/*                                                                      */
/************************************************************************/

int xsb_inquery = 0;

DllExport int call_conv xsb_command()
{
  if (xsb_inquery) return(2);  /* error */
  c2p_int(0,reg_term(3));  /* command for calling a goal */
  xsb(1,0,0);
  if (is_var(reg_term(1))) return(1);  /* goal failed, so return 1 */
  c2p_int(1,reg_term(3));  /* command for next answer */
  xsb(1,0,0);
  if (is_var(reg_term(1))) return(0);  /* goal succeeded */
  (void) xsb_close_query();
  return(2);
}

/************************************************************************/
/*                                                                      */
/* xsb_command_string(char *goal) passes the command (e.g. a query      */
/* which only succeeds or fails) to xsb.  The command must a string     */
/* passed in the argument.  It returns 0 if it succeeds, 1 if it        */
/* fails, in either case resetting register 1 back to a free            */
/* variable.  It returns 2 if there is an error.                        */
/*                                                                      */
/************************************************************************/

DllExport int call_conv xsb_command_string(char *goal)
{
  if (xsb_inquery) return(2);  /* error */
  c2p_string(goal,reg_term(1));
  c2p_int(2,reg_term(3));  /* command for calling a string goal */
  xsb(1,0,0);
  if (is_var(reg_term(1))) return(1);  /* goal failed, so return 1 */
  c2p_int(1,reg_term(3));  /* command for next answer */
  xsb(1,0,0);
  if (is_var(reg_term(1))) return(0);  /* goal succeeded */
  (void) xsb_close_query();
  return(2);
}

/************************************************************************/
/*                                                                      */ 
/* xsb_query() submits a query to xsb. The query must have been put into*/
/* xsb's register 1 by the caller, using p2c_* (and perhaps p2p_*).  Xsb*/
/* will evaluate the query and return with the variables in the query   */
/* bound to the first answer.  In addition, register 2 will contain a   */
/* Prolog term of the form ret(V1,V2,..,Vn) with as many Vi's as        */
/* variables in the original query and with Vi bound to the value for   */
/* that variable in the first answer.  If the query fails, it returns 1.*/
/* If the query succeeds, it returns 0. If there is an error, it returns*/
/* 2.                                                                   */
/*                                                                      */
/************************************************************************/

DllExport int call_conv xsb_query()
{
  if (xsb_inquery) return(2);
  c2p_int(0,reg_term(3));  /* set command for calling a goal */
  xsb(1,0,0);
  if (is_var(reg_term(1))) return(1);
  xsb_inquery = 1;
  return(0);
}

/************************************************************************/
/*                                                                      */ 
/* xsb_query_string(char *) submits a query to xsb.  The string must
   be a goal that will be correctly read by xsb's reader, and it must
   be terminated with a period (.).  Register 2 may be a variable or
   it may be a term of the form ret(X1,X2,...,Xn), where n is the
   number of variables in the query.  The query will be parsed, and an
   answer term of the form ret(Y1,Y2,...,Yn) will be constructed where
   Y1, .... Yn are the variables in the parsed goal (in left-to-right
   order).  This answer term is unified with the argument in register
   2.  Then the goal is called.  If the goal succeeds,
   xsb_query_string returns 0 and the first answer is in register 2.
   If it fails, xsb_query_string returns 1.                             */
/*                                                                      */
/************************************************************************/

DllExport int call_conv xsb_query_string(char *goal)
{
  if (xsb_inquery) return(2);
  c2p_string(goal,reg_term(1));
  c2p_int(2,reg_term(3));  /* set command for calling a string goal */
  xsb(1,0,0);
  if (is_var(reg_term(1))) return(1);
  xsb_inquery = 1;
  return(0);
}

/************************************************************************/
/*                                                                      */
/*  xsb_query_string_string calls xsb_query_string and returns          */
/*	the answer in a string.  The answer is copied into ans,	        */
/*	a VarString provided by the caller.  Variable	    	    	*/
/*	values are separated by the string sep.				*/
/*                                                                      */
/************************************************************************/

DllExport
int call_conv xsb_query_string_string(char *goal, VarString *ans, char *sep) {
  int rc;
  
  rc = xsb_query_string(goal);
  if (rc > 0) return rc;
  return xsb_answer_string(ans,sep);
}

/************************************************************************/
/*                                                                      */
/*  xsb_query_string_string_b calls xsb_query_string and returns        */
/*	the answer in a string.  The caller provides a buffer and its   */
/*      length.  If the answer fits in the buffer, it is returned       */
/*      there, and its length is returned.  If not, then the length is  */ 
/*      returned, and the answer can be obtained by calling             */
/*      xsb_get_last_answer.                                            */
/*                                                                      */
/************************************************************************/
static XSB_StrDefine(last_answer);

DllExport
int call_conv xsb_query_string_string_b(
	     char *goal, char *buff, int buflen, int *anslen, char *sep) {
  int rc;
  
  XSB_StrSet(&last_answer,"");
  rc = xsb_query_string_string(goal,&last_answer,sep);
  if (rc > 0) return rc;
  *anslen = last_answer.length;
  XSB_StrNullTerminate(&last_answer);
  if (last_answer.length < buflen) {
    strcpy(buff,last_answer.string);
    return rc;
  } else return 3;
}

/************************************************************************/
/*                                                                      */
/*	xsb_get_last_answer_string returns previous answer.             */
/*                                                                      */
/************************************************************************/
DllExport int call_conv 
   xsb_get_last_answer_string(char *buff, int buflen, int *anslen) {

 *anslen = last_answer.length;
  if (last_answer.length < buflen) {
    strcpy(buff,last_answer.string);
    return 0;
  } else 
    return 3;
}    

/************************************************************************/
/*                                                                      */
/* xsb_next() causes xsb to return the next answer.  It (or             */
/* xsb_close_query) must be called after xsb_query.  If there is        */
/* another answer, xsb_next returns 0 and the variables in goal term    */
/* (in xsb register 1) are bound to the answer values.  In addition     */
/* xsb register 2 will contain a term of the form ret(V1,V2,...,Vn)     */
/* where the Vi's are the values for the variables for the next         */
/* answer.                                                              */
/* xsb_next returns 0 if the next is found, 1 if there are no more      */
/* answers, and 3 if an error is encountered. If 1 is returned, then    */
/* the query is automatically closed.                                   */
/*                                                                      */
/************************************************************************/

DllExport int call_conv xsb_next()
{
  if (!xsb_inquery) return(2);
  c2p_int(0,reg_term(3));  /* set command for next answer */
  xsb(1,0,0);
  if (is_var(reg_term(1))) {
    xsb_inquery = 0;
    return(1);
  } else return(0);
}

/************************************************************************/
/*                                                                      */
/*	xsb_next_string(ans,sep) calls xsb_next() and returns the       */
/*	answer in the VarString ans, provided by the caller.            */
/*	sep is a separator for the fields of the answer.        	*/
/*                                                                      */
/************************************************************************/

DllExport int call_conv xsb_next_string(VarString *ans, char *sep) {
  int rc = xsb_next();
  if (rc > 0) return rc;
  return xsb_answer_string(ans,sep);
}

/************************************************************************/
/*                                                                      */
/*	xsb_next_string_b(buff,buflen,anslen,sep) calls xsb_next() and  */
/*      returns the answer in buff, provided by the caller.  The length */
/*      of buff is buflen.  The length of the answer is put in anslen.  */
/*      If the buffer is too small for the answer, nothing is put in    */
/*      the buffer.  In this case the caller can allocate a larger      */
/*      and retrieve the buffer using xsb_get_last_answer.              */
/*                                                                      */
/************************************************************************/

DllExport int call_conv xsb_next_string_b(
		     char *buff, int buflen, int *anslen, char *sep) {
  int rc;

  XSB_StrSet(&last_answer,"");
  rc = xsb_next_string(&last_answer,sep);
  if (rc > 0) return rc;
  *anslen = last_answer.length;
  XSB_StrNullTerminate(&last_answer);
  if (last_answer.length < buflen) {
    strcpy(buff,last_answer.string);
    return rc;
  } else return 1;
}


/************************************************************************/
/*                                                                      */
/* xsb_close_query() closes the current query, so that no more answers  */
/* will be returned, and another query can be opened.                   */
/* If the query was correctly closed, it resets xsb registers 1 and 2   */
/* to be variables, and returns 0.  If there is some error, it returns  */
/* 2.                                                                   */
/*                                                                      */
/************************************************************************/

DllExport int call_conv xsb_close_query()
{
  if (!xsb_inquery) return(2);
  c2p_int(1,reg_term(3));  /* set command for cut */
  xsb(1,0,0);
  if (is_var(reg_term(1))) {
    xsb_inquery = 0;
    return(0);
  } else return(2);
}

/************************************************************************/
/*                                                                      */
/*  xsb_close() is currently just a noop, since it doesn't clean        */
/*  anything up, to allow a re-init.                                    */
/*                                                                      */
/************************************************************************/

DllExport int call_conv xsb_close()
{
  if (xsb_initted) return(0);
  else return(1);
}

