/* File:      libwww_parse_utils.c
** Author(s): kifer, Yang Yang
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 2000
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
** $Id: libwww_parse_utils.c,v 1.3 2000-03-22 07:33:54 kifer Exp $
** 
*/

#include "HTTPReq.h"
#include "deref.h"


/* This is a per-request termination handler */
PRIVATE int parse_termination_handler (HTRequest   *request,
				       HTResponse  *response,
				       void 	   *param,
				       int 	   status)
{
  prolog_term status_term =
    ((REQUEST_CONTEXT *)HTRequest_context(request))->status_term;
  void *userdata = ((REQUEST_CONTEXT *)HTRequest_context(request))->userdata;

#ifdef LIBWWW_DEBUG
  xsb_dbgmsg("In parse_termination_handler");
#endif

  if (status == HT_TEMP_REDIRECT || status == HT_PERM_REDIRECT ||
	status == HT_FOUND || status == HT_SEE_OTHER) {
    HTAnchor * redirection = HTResponse_redirection(response);
    /* if loaded redirection successfully, then return: the request will be
       processed by the existing event loop; otherwise, drop down and terminate
       the request */ 
    if (YES==HTLoadAnchor(redirection,request))
      return TRUE;
  }

  if (total_number_of_requests > 0)
    total_number_of_requests--;
  /* if the last request has finished, stop the event loop 
     and unregister the callbacks */
  if (total_number_of_requests == 0) {
    HTEventList_stopLoop();
#ifdef LIBWWW_DEBUG
    xsb_dbgmsg("In parse_termination_handler: event loop halted");
#endif
    /*
      HText_unregisterElementCallback();
      HText_unregisterTextCallback();
    */
  }

  if (is_var(status_term))
    c2p_int(status, status_term);
  else
    xsb_warn("LIBWWW_PARSE_HTML/XML: Request %d: Arg 5 (Status) must be a var",
	     REQUEST_ID(request));

  /* Clean Up */
  if (userdata)
    delete_userData((USERDATA *) userdata);

#ifdef LIBWWW_DEBUG
  xsb_dbgmsg("In parse_termination_handler: Cleanup: request %d, status=%d",
	     REQUEST_ID(request), status);
#endif

  free_request_context((REQUEST_CONTEXT *) HTRequest_context(request));
  HTRequest_clear(request);
  return TRUE;
}


PRIVATE void libwww_abort_request(HTRequest *request,
				  int status,
				  char *description, ...)
{
  va_list args;
  USERDATA *userdata =
    (USERDATA *)
    ((REQUEST_CONTEXT *)HTRequest_context(request))->userdata;
  prolog_term status_term =
    ((REQUEST_CONTEXT *)HTRequest_context(request))->status_term;

  va_start(args, description);
  va_end(args);
  xsb_dbgmsg("In Request %d:", REQUEST_ID(request));
  xsb_warn(description,args);

  if (total_number_of_requests > 0)
    total_number_of_requests--;
  /* if the last request has finished, stop the event loop 
     and unregister the callbacks */
  if (total_number_of_requests == 0) {
    HTEventList_stopLoop();
#ifdef LIBWWW_DEBUG
    xsb_dbgmsg("In libwww_abort_request: event loop halted");
#endif
    /* we probably need to unregister the handlers here */
  }

  if (is_var(status_term))
    c2p_int(status, status_term);
  else
    xsb_warn("LIBWWW_PARSE_HTML/XML: Request %d: Arg 5 (Status) must be a var",
	     REQUEST_ID(request));

  /* Clean Up */
  delete_userData(userdata);

#ifdef LIBWWW_DEBUG
  xsb_dbgmsg("In libwww_abort_request: Cleanup: request %d, status=%d",
	     REQUEST_ID(request), status);
#endif

  free_request_context((REQUEST_CONTEXT *) HTRequest_context(request));
  HTRequest_kill(request);
  return;
}


/* note that we use xsb_abort here instead of abort handlers, because the error
   conditions handled here are programmatic mistakes rather than network
   conditions. */
PRIVATE void set_request_context(HTRequest *request,
				 prolog_term prolog_req,
				 int request_id,
				 char *caller)
{
  REQUEST_CONTEXT *context;
  prolog_term selection;

  if ((context=(REQUEST_CONTEXT *)calloc(1,sizeof(REQUEST_CONTEXT))) == NULL)
    xsb_abort("%s: Not enough memory", caller);

  context->request_id = request_id;
  context->userdata = NULL;

  init_htable(&(context->selected_tags_tbl),SELECTED_TAGS_TBL_SIZE,caller);
  init_htable(&(context->suppressed_tags_tbl),SUPPRESSED_TAGS_TBL_SIZE,caller);
  init_htable(&(context->stripped_tags_tbl),STRIPPED_TAGS_TBL_SIZE,caller);

  context->parsed_result = p2p_arg(prolog_req,4);
  if(!is_var(context->parsed_result))
    xsb_abort("%s: Arg 4 (Parse result) must be unbound variable", caller);

  context->status_term = p2p_arg(prolog_req,5);
  if(!is_var(context->status_term))
    xsb_abort("%s: Arg 5 (Request status) must be unbound variable", caller);


  /* get tag selection: f(chosen-list,suppressed-list,strip-list) */
  selection = p2p_arg(prolog_req,3);
  if (is_var(selection)) {
    context->suppress_is_default=FALSE;
  } else if (is_functor(selection) && (p2c_arity(selection)==3)) {
    prolog_term
      select_term=p2p_arg(selection,1),
      suppressed_term=p2p_arg(selection,2),
      strip_term=p2p_arg(selection,3);
    
    if (is_var(select_term))
      context->suppress_is_default=FALSE;
    else if (is_list(select_term)) {
      context->suppress_is_default=TRUE;
      init_tag_table(select_term, &(context->selected_tags_tbl));
    } else
      xsb_abort("%s: Arg 3 (selection) in f(CHOOSE,_,_): CHOOSE must be a var or a list", caller);

    if (is_list(suppressed_term)) {
      init_tag_table(suppressed_term, &(context->suppressed_tags_tbl));
    } else if (!is_var(suppressed_term))
      xsb_abort("%s: Arg 3 (selection) in f(_,SUPPRESS,_): SUPPRESS must be a var or a list", caller);
      
    if (is_list(strip_term)) {
      init_tag_table(strip_term, &(context->stripped_tags_tbl));
    } else if (!is_var(strip_term))
      xsb_abort("%s: Arg 3 (selection) in f(_,_,STRIP): STRIP must be a var or a list", caller);
  } else {
    xsb_abort("%s: Arg 3 (selection) must be a var or f(CHOOSE,SUPPRESS,STRIP)", caller);
  }
  
  /* attach context to the request */
  HTRequest_setContext(request, (void *) context);
  HTRequest_addAfter(request,
		     parse_termination_handler,
		     NULL,
		     NULL,
		     HT_ALL,
		     HT_FILTER_LAST,
		     YES);
  return;
}


PRIVATE void free_request_context (REQUEST_CONTEXT *context)
{
  free_htable(&(context->selected_tags_tbl));
  free_htable(&(context->suppressed_tags_tbl));
  free_htable(&(context->stripped_tags_tbl));
  free(context);
}

    
/* Copy FROM to TO and lowercase on the way; assume TO is large enough */
PRIVATE void strcpy_lower(char *to, const char *from)
{
  int i=0;
  if (from)
    while (from[i]) {
      to[i] = tolower(from[i]);
      i++;
    }
  to[i] = '\0';
}


#ifdef LIBWWW_DEBUG_VERBOSE
PRIVATE void print_prolog_term(prolog_term term, char *message)
{ 
  static vstrDEFINE(StrArgBuf);
  vstrSET(&StrArgBuf,"");
  deref(term);
  print_pterm(term, 1, &StrArgBuf); 
  xsb_dbgmsg("%s = %s", message, StrArgBuf.string);
} 
#endif


PRIVATE void init_htable(HASH_TABLE *htable, int size, char *caller)
{
  int i;
  htable->size = size;
  if ((htable->table=(HKEY *)calloc(size, sizeof(HKEY))) == NULL )
    /* use xsb_abort here, because it is not worth trying to recover from the
       out of memory error */
    xsb_abort("%s: Not enough memory", caller);
  for (i=0; i<size; i++)
    htable->table[i] = HTABLE_CELL_INITIALIZER;
}

PRIVATE int add_to_htable(HKEY item, HASH_TABLE *htable)
{
  int idx = (int) (HASH(item) % htable->size), i = idx;

  while ( htable->table[i] != HTABLE_CELL_INITIALIZER ) {
    i++;
    i = i % htable->size;
    if (i == idx) /* reached full circle */
      return FALSE;
  }
  /* found spot */
  SET_HASH_CELL(htable->table[i], item);
  return TRUE;
}


/* hash table stuff; deals with integers stored in a table; the integers are
   html element numbers */
PRIVATE int is_in_htable(const HKEY item, HASH_TABLE *htable)
{
  int idx = (int) (HASH(item) % htable->size), i = idx;

  while ( htable->table[i] != HTABLE_CELL_INITIALIZER ) {
    if (HASH_CELL_EQUAL(htable->table[i],item)) {
      return TRUE;
    }
    i++;
    i = i % htable->size;
    if (i == idx) /* reached full circle */
      return FALSE;
  }
  return FALSE;
}
