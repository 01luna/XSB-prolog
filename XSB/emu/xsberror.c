/* File:      xsberror.c
** Author(s): Kostis F. Sagonas
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
** $Id: xsberror.c,v 1.3 1999-03-23 05:30:15 kifer Exp $
** 
*/


#include <stdio.h>

#include "configs/config.h"

#include "auxlry.h"
#include "cell.h"
#include "psc.h"
#include "subp.h"
#include "register.h"
#include "xsberror.h"

/*----------------------------------------------------------------------*/


static char *err_msg[] = {
	"Calculation", "Database", "Evaluation", "Implementation",
	"Instantiation", "I/O Control", "I/O End-of-file", "I/O Formatting",
	"Operator", "Overflow", "Range", "Syntax", "Type",
	"Undefined predicate/function", "Undefined value",
	"Underflow", "Zero division" };

/*----------------------------------------------------------------------*/

void xsb_abort(char *description)
{
    char message[240];

    sprintf(message, "++Error: %s\n", description);
    pcreg = exception_handler(message);
}

/*----------------------------------------------------------------------*/

void xsb_exit(char *description)
{
    fprintf(stderr, "%s\nExiting XSB abnormally...\n", description);
    exit(1);
}

/*----------------------------------------------------------------------*/

void err_handle(int description, int arg, char *f,
		int ar, char *expected, Cell found)
{
  char message[160];	/* Allow 2 lines of error reporting.	*/
  
  switch (description) {
  case INSTANTIATION:
    sprintf(message, 
	    "! %s error in argument %d of %s/%d\n",
	    err_msg[description], arg, f, ar);
    break;
  case RANGE:	/* I assume expected != NULL */
    sprintf
      (message,
       "! %s error: in argument %d of %s/%d\n! %s expected, but %d found\n",
       err_msg[description], arg, f, 
       ar, expected, (int) int_val(found));
    break;
  case TYPE:
    if (expected == NULL) {
      sprintf(message, 
	      "! %s error in argument %d of %s/%d\n",
	      err_msg[description], arg, f, ar);
    } else  
      sprintf
	(message,
	 "! %s error: in argument %d of %s/%d\n! %s expected, but %s found\n",
	 err_msg[description], arg, f, ar, expected,
	 "something else");
    break;
  case ZERO_DIVIDE:
    sprintf(message,
	    "! %s error in %s\n! %s expected, but %lx found\n",
	    err_msg[description], f, expected, found);
    break;
  default:
    sprintf(message, 
	    "! %s error (not completely handled yet)\n",
	    err_msg[description]);
    break;
  }
  pcreg = exception_handler(message);
}

/*----------------------------------------------------------------------*/
