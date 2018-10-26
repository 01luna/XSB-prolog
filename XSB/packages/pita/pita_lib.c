/* File:      pita_lib.c
** Author(s): Fabrizio Riguzzi and Terrance Swift
** Contact:   fabrizio.riguzzi@unife.it, xsb-contact@cs.sunysb.edu
**
** Copyright (C) Copyright: Fabrizio Riguzzi and Terrance Swift
**                          ENDIF - University of Ferrara
**                          Centro de Inteligencia Artificial,
**                          Universidade Nova de Lisboa, Portugal
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
*/

/* 
This package uses the library cudd, see http://vlsi.colorado.edu/~fabio/CUDD/
for the relative license.
*/
#include "util.h"
#include "cudd.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "cinterf.h"



typedef struct
  {
    int nVal,nBit;
    int firstBoolVar;
  } variable;

typedef struct
{
  DdNode *key;
  double value;
} rowel;

typedef struct
{
  int cnt;
  rowel *row;
} tablerow;


void init_my_predicates(void);
int compare(char *a, char *b);
FILE *open_file (char *filename, const char *mode);

tablerow* init_table(int varcnt);
double * get_value(tablerow *tab,  DdNode *node);
void add_or_replace_node(tablerow *tab, DdNode *node, double value);
void add_node(tablerow *tab, DdNode *node, double value);
void destroy_table(tablerow *tab,int varcnt);


static variable * vars;
double * probs;
static DdManager * mgr;
static int nVars;
static int  boolVars;

int _debug = 0;

double Prob(DdNode *node,  tablerow * table);



void  initc(int reorder)
{

  nVars=0;
  boolVars=0;
  mgr=Cudd_Init(nVars,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);

  if (reorder != CUDD_REORDER_NONE)
    Cudd_AutodynEnable(mgr,reorder);

  vars= (variable *) malloc(nVars * sizeof(variable));
  probs=(double *) malloc(0);
}

void reorderc(int method)
{

  Cudd_ReduceHeap(mgr,method,0);
}

void endc(void)
{
    Cudd_Quit(mgr);
  free(vars);
  free(probs);
}

double ret_probc(long node_in)
{
    double prob;
    tablerow * table;
    DdNode * node;

    node=(DdNode *) node_in;
if (!Cudd_IsConstant(node))
  {
    table=init_table(boolVars);
    prob=Prob(node,table);
    if (Cudd_IsComplement(node))
      prob=1.0-prob;
    destroy_table(table,boolVars);
  }
  else
  {
    if (node==Cudd_ReadOne(mgr))
    {
      prob=1.0;
    }
    else
    {
      prob=0.0;
    }
  }

  return(prob);
}

int add_varc(int nVal,prolog_term probabilities)
{
  variable * v;
  int i;
  double p,p0;

  nVars=nVars+1;
  vars=(variable *) realloc(vars,nVars * sizeof(variable));
  v=&vars[nVars-1];
  v->nVal=nVal;

  probs=(double *) realloc(probs,(((boolVars+v->nVal-1)* sizeof(double))));
  p0=1;
  for (i=0;i<v->nVal-1;i++)
  {
    p=p2c_float(p2p_car(probabilities));
    probs[boolVars+i]=p/p0;
    probabilities=p2p_cdr(probabilities);

    p0=p0*(1-p/p0);
  }
  v->firstBoolVar=boolVars;
  boolVars=boolVars+v->nVal-1;

  return  nVars-1;
}

long equalityc(int varIndex, int value)
{
  int i;
  variable v;
  DdNode * node,  * tmp,  *var ;

  v=vars[varIndex];
  i=v.firstBoolVar;
  node=NULL;
  tmp=Cudd_ReadOne(mgr);
  Cudd_Ref(tmp);
  for (i=v.firstBoolVar;i<v.firstBoolVar+value;i++)
  {
    var=Cudd_bddIthVar(mgr,i);
    node=Cudd_bddAnd(mgr,tmp,Cudd_Not(var));
    Cudd_Ref(node);
    Cudd_RecursiveDeref(mgr,tmp);
    tmp=node;
  }
  if (!(value==v.nVal-1))
  {
    var=Cudd_bddIthVar(mgr,v.firstBoolVar+value);
    node=Cudd_bddAnd(mgr,tmp,var);
    Cudd_Ref(node);
    Cudd_RecursiveDeref(mgr,tmp);
  }
  return (long) node;
}

long onec(void)
{
  DdNode * node;

  node =  Cudd_ReadOne(mgr);
  Cudd_Ref(node);
  return (long) node;
}

long zeroc(void)
{
  DdNode * node;

  node=Cudd_ReadLogicZero(mgr);
  Cudd_Ref(node);
  return (long) node;
}

long bdd_notc(long node)
{
  return (long) Cudd_Not((DdNode *)node);
}

long andc(long nodea, long nodeb)
{
  DdNode * node1, *node2,*nodeout;

  node1=(DdNode *)nodea;
  node2=(DdNode *)nodeb;
  nodeout=Cudd_bddAnd(mgr,node1,node2);
  Cudd_Ref(nodeout);
  return (long)nodeout;
}

long orc(long nodea, long nodeb)
{
  DdNode * node1,*node2,*nodeout;

  node1=(DdNode *)nodea;
  node2=(DdNode *)nodeb;
  nodeout=Cudd_bddOr(mgr,node1,node2);
  Cudd_Ref(nodeout);
  return (long)nodeout;
}


void create_dotc(long bdd,prolog_term filenameatom)
{
  char * onames[]={"Out"};
  char ** inames;
  int i,b,index,nv;
  variable v;
  char numberVar[11],numberBit[11];
  FILE * file;
  char * filename;
  DdNode * array[1];

  filename=p2c_string(filenameatom);
  file = open_file(filename, "w");
  array[0]=(DdNode *)bdd;
  inames= (char **) malloc(sizeof(char *)*boolVars);
  index=0;
  for (i=0;i<nVars;i++)
  {
    v=vars[i];
        nv=v.nVal-1;
    for (b=0;b<nv;b++)
    {
      inames[b+index]=(char *) malloc(sizeof(char)*20);
      strcpy(inames[b+index],"X");
      sprintf(numberVar,"%d",i);
      strcat(inames[b+index],numberVar);
      strcat(inames[b+index],"_");
      sprintf(numberBit,"%d",b);
      strcat(inames[b+index],numberBit);
    }
    index=index+nv;
  }
  Cudd_DumpDot(mgr,1,array,(const char * const *)inames,(const char * const *)onames,file);
  index=0;
  for (i=0;i<nVars;i++)
  {
    v=vars[i];
      nv=v.nVal-1;
    for (b=0;b<nv;b++)
    {
      free(inames[b+index]);
    }
    index=index+nv;
  }
  free(inames);
}
double Prob(DdNode *node,  tablerow * table)
/* compute the probability of the expression rooted at node.
table is used to store nodeB for which the probability has alread been computed
so that it is not recomputed
 */
{
  int index;
  double res;
  double p,pt,pf,BChild0,BChild1;
  double * value_p;
  DdNode *nodekey,*T,*F;
  //comp=(comp && !comp_par) ||(!comp && comp_par);
  if (Cudd_IsConstant(node))
  {
      return 1.0;
  }
  else
  {
    nodekey=Cudd_Regular(node);
    value_p=get_value(table,nodekey);
    if (value_p!=NULL)
        return *value_p;
    else
    {
      index=Cudd_NodeReadIndex(node);  //Returns the index of the node. The node pointer can be either regular or complemented.
      //The index field holds the name of the variable that labels the node. The index of a variable is a permanent attribute that reflects the order of creation.
      p=probs[index];
      T = Cudd_T(node);
      F = Cudd_E(node);
      pf=Prob(F,table);
      pt=Prob(T,table);
      if (Cudd_IsComplement(F))
        pf=1.0-pf;

      BChild0=pf*(1-p);
      BChild1=pt*p;
      res=BChild0+BChild1;
      add_node(table,nodekey,res);
      return res;
    }
  }
}


FILE * open_file(char *filename, const char *mode)
/* opens a file */
{
  FILE *fp;

  if ((fp = fopen(filename, mode)) == NULL) 
  {
      perror(filename);
      exit(1);
    }
    return fp;
}


tablerow* init_table(int varcnt) {
  int i;
  tablerow *tab;

  tab = (tablerow *) malloc(sizeof(rowel) * varcnt);
  for (i = 0; i < varcnt; i++)
  {
    tab[i].row = NULL;
    tab[i].cnt = 0;
  }
  return tab;
}


void add_node(tablerow *tab, DdNode *node, double value) {
  int index = Cudd_NodeReadIndex(node);

  tab[index].row = (rowel *) realloc(tab[index].row,
    (tab[index].cnt + 1) * sizeof(rowel));
  tab[index].row[tab[index].cnt].key = node;
  tab[index].row[tab[index].cnt].value = value;
  tab[index].cnt += 1;
}

void add_or_replace_node(tablerow *tab, DdNode *node, double value)
{
  int i;
  int index = Cudd_NodeReadIndex(node);
  for(i = 0; i < tab[index].cnt; i++)
  {
    if (tab[index].row[i].key == node)
    {
      tab[index].row[i].value=value;
      return;
    }
  }
  tab[index].row = (rowel *) realloc(tab[index].row,
    (tab[index].cnt + 1) * sizeof(rowel));
  tab[index].row[tab[index].cnt].key = node;
  tab[index].row[tab[index].cnt].value = value;
  tab[index].cnt += 1;
}

double * get_value(tablerow *tab,  DdNode *node) {
  int i;
  int index = Cudd_NodeReadIndex(node);

  for(i = 0; i < tab[index].cnt; i++)
  {
    if (tab[index].row[i].key == node)
    {
      return &tab[index].row[i].value;
    }
  }
  return NULL;
}

void destroy_table(tablerow *tab,int varcnt)
{
  int i;

  for (i = 0; i < varcnt; i++)
  {
    free(tab[i].row);
  }
  free(tab);
}
