/* File:      token.h
** Author(s): Kostis F. Sagonas, Jiyang Xu
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
** $Id: token.h,v 1.1.1.1 1998-11-05 16:55:24 sbprolog Exp $
** 
*/


#define TK_ERROR    -1
#define TK_PUNC      0       /* punctuation , ; ( ) [ ] ... */
#define TK_VARFUNC   1       /* type is a variable(...  HiLog type */
#define TK_VAR       2       /* type is a variable */
#define TK_FUNC	     3       /* type is atom( */
#define TK_INT       4       /* type is an integer number */
#define TK_ATOM      5       /* type is an atom */
#define TK_EOC	     6       /* END of clause but not of file */
#define TK_VVAR      7       /* underscore '_' */
#define TK_VVARFUNC  8       /* type is underscore _(... HiLog type */
#define TK_REAL	     9       /* type is a real number */	
#define TK_EOF	    10       /* END of file, not end of clause */
#define TK_STR      11       /* type is a char string */
#define TK_LIST     12       /* type is a char string */
#define TK_HPUNC    13       /* punctuation ) followed by a ( in HiLog terms */
#define TK_INTFUNC  14       /* type is an integer number functor */
#define TK_REALFUNC 15       /* type is a real number functor */


	/* The following are for printing purposes only (writeq) */
#define TK_INT_0    16
#define TK_FLOAT_0  17
#define TK_PREOP    18
#define TK_INOP     19
#define TK_POSTOP   20
#define TK_QATOM    21
#define TK_QSTR     22		/* quoted string */

#define DIGIT    0              /* 0 .. 9 */
#define BREAK    1              /* _ */
#define UPPER    2              /* A .. Z */
#define LOWER    3              /* a .. z */
#define SIGN     4              /* -/+*<=>#@$\^&~`:.? */
#define NOBLE    5              /* !; (don't form compounds) */
#define PUNCT    6              /* (),[]|{}% */
#define ATMQT    7              /* ' (atom quote) */
#define LISQT    8              /* " (list quote) */
#define STRQT    9              /* $ (string quote) */
#define CHRQT   10              /* ` (character quote, maybe) */
#define TILDE   11              /* ~ (like character quote but buggy) */
#define SPACE   12              /* layout and control chars */
#define EOLN    13              /* line terminators ^J ^L */
#define REALO   14              /* floating point number */
#define EOFCH   15              /* end of file */
#define ALPHA   DIGIT           /* any of digit, break, upper, lower */
#define BEGIN   BREAK           /* atom left-paren pair */
#define ENDCL   EOLN            /* end of clause token */
#define RREAL	16		/* radix number(real) - overflowed */
#define RDIGIT	17		/* radix number(int) */
 
#define strgetc(p) (--(p)->strcnt>=0? ((int)*(p)->strptr++): -1)

#define GetC(card,instr) (instr ? strgetc(instr) : getc(card))

struct strbuf {
  int strcnt;
  char *strptr;
  char *strbase;
};

#define STRFILE struct strbuf
#define MAXIOSTRS 5
extern STRFILE *iostrs[MAXIOSTRS];
#define iostrdecode(j) (-1-j)
#define strfileptr(desc) iostrs[iostrdecode(desc)]

struct token {
  int type;
  char *value;
  int nextch;
};

extern struct token *token;
extern struct token *GetToken(FILE *, STRFILE *, int);

extern int intype(int);

/*======================================================================*/
/*======================================================================*/

