/* File:      io_builtins_xsb.h
** Author(s): kifer
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
** $Id: io_builtins_xsb.h,v 1.5 2004-08-19 22:15:25 tswift Exp $
** 
*/


#include "io_defs_xsb.h"
#include "token_xsb.h"

#define MAX_OPEN_FILES    55
#define MIN_USR_OPEN_FILE 7     /* Where user files start in the XSB
				   open files table */

/* TLS: The following data structure holds all info abt. streams that
   we might eventually want to access from Prolog.  Names, etc come
   from the definition of stream properties in "Prolog: the standard",
   pg. 207-209.  Not everything is implemented, but I figured I'd set
   things up so we can implement them when/as needed */

#define TEXT_STREAM        0
#define BINARY_STREAM    1

#define ERROR_ON_EOF         0 
#define EOF_CODE_ON_EOF   1
#define RESET_ON_EOF          2

typedef struct  {
  FILE *file_ptr;
  char *file_name;
  char io_mode;
  int stream_type;
  int reposition;
  int eof_action;
} stream_record;

extern stream_record open_files[];      /* Table of file pointers for open files */

extern int old_xsb_intern_file(FILE *file,char *c);
extern int xsb_intern_file(char *c1,char *c2,int *i,char *strmode);

extern void write_quotedname(FILE *file, char *string);
extern void double_quotes(char *string, char *new_string);
extern xsbBool no_quotes_needed(char *string);

int read_canonical_term(FILE *, STRFILE *, int);

void print_term_canonical(FILE *, Cell, int);
