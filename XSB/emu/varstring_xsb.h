/* File:      varstring.h
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
** $Id: varstring_xsb.h,v 1.1 1999-11-29 00:30:21 kifer Exp $
** 
*/


#ifndef VARSTRING_INCLUDED

struct varstr;
typedef struct varstr VarString;

struct varstr_ops {
  void (*set)(VarString*,char*);       /* copy a char* to VarString; increase
					  or shrink space, if needed        */ 
  void (*setv)(VarString*,VarString*); /* like set; 2nd arg is a VarString* */
  void (*append)(VarString*,char*);    /* append a char* to VarString;
					  increase space, if needed         */
  void (*prepend)(VarString*,char*);   /* like append, but prepend instead  */
  void (*appendv)(VarString*,VarString*);  /* append 2nd VarString to 1st   */
  void (*prependv)(VarString*,VarString*); /* prepend 2nd VarString to 1st  */
  int  (*compare)(VarString*,VarString*);  /* like strcmp for VarStrings    */
  int  (*strcmp)(VarString*,char*);        /* compare VarString to a char*  */

  /* append block of N chars; don't NULL-terminate */
  void (*appendblk)(VarString*,char*,int); 
  /* append block of N chars; don't NULL-terminate */
  void (*prependblk)(VarString*,char*,int);
  void (*null_terminate)(VarString*);  /* Null-terminate VarString           */
  void (*ensure_size)(VarString*,int); /* Make sure size is at least N       */
  void (*shrink)(VarString*,int);      /* 2nd arg becomes the increment.
					  Space shrinks to the minimum needed
					  to accommodate existing data       */
  void  (*destroy)(VarString*);        /* release the space, uninitialize    */
};



/* All attributes are read-only;
   it is not recommended to refer to private attrs */
struct varstr {
  /* PRIVATE */
  int   size;    	       /* size of the allocated chunk 	      	     */
  int   increment;    	       /* increment by which to incr string size     */
  
  /* PUBLIC */
  int   length;    	       /* memory currently allocated for the string  */
  char  *string;    	       /* memory currently allocated for the string  */

  struct varstr_ops *op;       /* structure that defines valid VarString ops */
};


extern struct varstr_ops VarStrOps;

/* calling sequence shortcuts; all expect a VarString pointer */
#define vstrSET(vstr,str)           (vstr)->op->set(vstr,str)
#define vstrSETV(vstr1,vstr2)       (vstr1)->op->setv(vstr1,vstr2)
#define vstrAPPEND(vstr,str)        (vstr)->op->append(vstr,str)
#define vstrPREPEND(vstr,str)       (vstr)->op->prepend(vstr,str)
#define vstrAPPENDV(vstr1,vstr2)    (vstr)->op->appendv(vstr1,vstr2)
#define vstrPREPENDV(vstr1,vstr2)   (vstr)->op->prependv(vstr1,vstr2)
#define vstrCOMPARE(vstr1,vstr2)    (vstr1)->op->compare(vstr1,vstr2)
#define vstrSTRCMP(vstr,str)        (vstr)->op->strcmp(vstr,str)
#define vstrAPPENDBLK(vstr,blk,sz)  (vstr)->op->appendblk(vstr,blk,sz)
#define vstrPREPENDBLK(vstr,blk,sz) (vstr)->op->prependblk(vstr,blk,sz)
#define vstrNULL_TERMINATE(vstr)    (vstr)->op->null_terminate(vstr)
#define vstrENSURE_SIZE(vstr,size)  (vstr)->op->ensure_size(vstr,size)
#define vstrSHRINK(vstr,incr)       (vstr)->op->shrink(vstr,incr)
/* destruction is necessary for automatic VarString's */
#define vstrDESTROY(vstr)           (vstr)->op->destroy(vstr)


#define vstrDEFINE(vstr)          VarString vstr = {0,0,0,NULL,&VarStrOps}


#define VARSTRING_INCLUDED

#endif
