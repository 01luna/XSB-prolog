/* File:      tst_utils.h
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
** $Id: tst_utils.h,v 1.4 2000-05-29 04:23:42 ejohnson Exp $
** 
*/


/* Debugging Routines
   ------------------ */
void printNodeType(byte);
void printTrieType(byte);
void printTrieSymbol(Cell);
void tnSymbolicPrint(BTNptr);
void thtPrintSymbols(BTHTptr);

void printCall(TabledCallInfo *);
CPtr decode_ptr(Cell);
int decode_int(Cell);
int decode_tag(Cell);

VariantSF findSF_forAnswerRoot(TSTNptr);
VariantSF findSF_forAnswerLeaf(TSTNptr);
VariantSF findProdSF_for_ConsSF(VariantSF);
int containsSF(VariantSF, VariantSF);

void triePrintPath(BTNptr, xsbBool);
void sfPrintGoal(VariantSF, xsbBool);
void sfPrintConsGoals(VariantSF);

void printAnswerTemplate(CPtr, int);
void printAnswerList(ALNptr);
