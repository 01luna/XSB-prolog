/* File:      xsbpattern.c -- XSB-side interface to match() and substitute()
** Author(s): Jin Yu
** Contact:   xsb-contact@cs.sunysb.edu
** 
** Copyright (C) The Research Foundation of SUNY, 1998
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
** $Id: xsbpattern.c,v 1.1.1.1 1998-11-05 17:01:24 sbprolog Exp $
** 
*/


/*----------------------------------------------------------------------------
       try_match__() -- find the match pattern,
       next_match__() -- find the next match pattern,
       do_bulk_match__() -- find the global match patterns,
       string_substitute__() -- substitute the string with expected pattern,
       load_perl__() -- load the perl interpretor,
       unload_perl__() -- release the perl interpretor object,
       get_match_resultC__() -- get the perl pattern match C function,
       get_bulk_match_result__() --  get Perl global pattern match results.
       
----------------------------------------------------------------------------*/

/**------------------------------------------------------------------------**/

void build_sub_match_spec( void );


#define xsb_warn(warning)	fprintf(stderr, "++Warning: %s\n", warning)

#include "perlpattern.c"          /*pattern match basic functions */   


/*----------------------------------------------------------------------------
try_match__()
The pattern matching function which includes loading perl interpreter and 
trying the perl pattern matching.
arguments: 
  input: char* string,    -- input text
	 char* pattern    --  match pattern
  output:if no match found, return FAILURE (0).
----------------------------------------------------------------------------*/
int try_match__( void )
{

   SV *text=newSV(0);       /*the storage for the string in embeded Perl*/
   SV *string_buff=newSV(0);/*the storage for the string in embeded Perl*/
   int num_match;           /*number of the matches*/
   char *string = ptoc_string(1),
        *pattern = ptoc_string(2);

   sv_setpv(text, string );  /*store the string in the SV */

   if (perlObjectStatus == UNLOADED ) load_perl__();  /*load the perl interpreter*/

   num_match=match(text, pattern );
   
   SvREFCNT_dec(string_buff);
   SvREFCNT_dec(text);
  
   if (num_match==0) return(FAILURE); /*no match, return FAILURE*/
}

/*----------------------------------------------------------------------------
next_match__()
The pattern match function which repeats pattern match after 
the pattern match of the function try_match__().
If there is no calling of function try_match__() before, give warning! 
   output: if no match found, return FAILURE.
----------------------------------------------------------------------------*/
int next_match__( void )
{

   int num_match;        /* the number of the matches */

   if ( matchPattern == NULL ) { /*didn't try_match__ before*/
     xsb_warn("call try_match first!");
     num_match = 0;
   }
   else num_match=match_again( ); /*doing next match*/

   if (num_match==0) return(FAILURE); /* no match is found, return FAILURE */

}

/*----------------------------------------------------------------------------
do_bulk_match__()
The pattern match function which includes loading perl interpreter and 
doing the global perl pattern match, and storing the results in the global 
array of bulkMatchList.
argument: 
  input: char* string	     	     -- input text
	 char* pattern	     	     --  match pattern
  output: int* num_match     	     --  the number of the matches	 
----------------------------------------------------------------------------*/
int do_bulk_match__( void )
{

  AV *match_list;           /* AV storage of matches list*/
  SV *text=newSV(0);        /* storage for the embeded perl cmd */
  SV *string_buff=newSV(0); /* storage for the embedded perl cmd */
  int num_match;            /* the number of the matches */
  int i;
 
  sv_setpv(text, ptoc_string(1) );  /*put the string into an SV */
 
  if (perlObjectStatus == UNLOADED) load_perl__();                   
        /*load perl interpreter*/

  /*------------------------------------------------------------------------
    free the old match list space and allocate new space for current match list
    -----------------------------------------------------------------------*/
  for ( i=0; i<preBulkMatchNumber; i++ ) 
    free(bulkMatchList[i]);
  if (bulkMatchList != NULL ) free(bulkMatchList);
  bulkMatchList = NULL;   

  /*------------------------------------------------------------------------
    do bulk match
    ----------------------------------------------------------------------*/
  num_match = matches(text, ptoc_string(2),&match_list);
    
  /* allocate the space to store the matches */
  if ( num_match != 0 ) {
    preBulkMatchNumber = num_match; /* reset the pre bulk match number */
    bulkMatchList = (char **)malloc(num_match*sizeof(char *)); 
    if ( bulkMatchList == NULL ) 
      xsb_abort("Cannot alocate memory to store the results for bulk match");
  }

  /*get the matches from the AV */
  for ( i=0;i<num_match;i++ ) {
    string_buff = av_shift(match_list);
    bulkMatchList[i] = (char *)malloc( strlen(SvPV(string_buff,na))+1 ); 
    strcpy((char *)bulkMatchList[i], SvPV(string_buff,na) );   
  } 

  SvREFCNT_dec(string_buff); /* release space*/
  SvREFCNT_dec(text);
  
  ctop_int(3, num_match);           /*return the number of matches*/

}

/*----------------------------------------------------------------------------
string_substitute__()
The pattern substitution function which includes loading perl interpreter 
and doing the pattern substitution, then returning the replaced string.
arguments: 
  input: char* string, input text
	 char* pattern, match pattern
  output:char* string, output text
----------------------------------------------------------------------------*/
int string_substitute__( void )
{

  SV *text=newSV(0);    /* storage for the embeded perl cmd */
  int i;                

  sv_setpv(text, ptoc_string(1) );  /*put the string to the SV */
  
  if ( perlObjectStatus == UNLOADED ) load_perl__(); 
  /*load perl interpreter*/
   
  if( !substitute(&text, ptoc_string(2)) )
    {
      return(FAILURE);
    }     
  
  if (substituteString != NULL ) free(substituteString);
  substituteString = malloc(strlen(SvPV(text,na))+1);
  strcpy(substituteString,SvPV(text,na));
  
  SvREFCNT_dec(text);  /*release space*/
  
  ctop_string(3, string_find(substituteString,1));  /*return changed text*/
  
}

/*----------------------------------------------------------------------------
load_perl__():
The function to implement the interface of C and Perl, load the perl 
interpreter and initial the global variables. Then the Perl function
is ready to run.
----------------------------------------------------------------------------*/

int load_perl__( void ) 
{

  char *embedding[] = {"","-e","0"};  /* perl interpreter config params */
  int i;

  /* check if the perl interpreter is loaded already*/
  if ( perlObjectStatus == LOADED ) return;

  /*------------------------------------------------------------------------
    initial the global variables
    ----------------------------------------------------------------------*/
  for ( i=0; i<MAX_TOTAL_MATCH; i++ ) 
    matchResults[i] = NULL;  
  preBulkMatchNumber = 0;
  bulkMatchList = NULL;
  matchPattern = NULL;
  substituteString = NULL;
  build_sub_match_spec();    /*build the submatch arguments string constant*/  

  my_perl = perl_alloc();
  perl_construct( my_perl );
  perl_parse( my_perl, NULL, 3, embedding, (char **)NULL );
  perl_run(my_perl);

  perlObjectStatus = LOADED;

  return (SUCCESS); 

}

/*---------------------------------------------------------------------------
unload_perl__():
The function to release the Perl interpreter, and deallocat the memory
---------------------------------------------------------------------------*/

int unload_perl__( void )
{
  int i;

  perl_destruct_level = 1;
  perl_destruct( my_perl );
  perl_free( my_perl );

  /*-------------------------------------------------------------------------
   free all the space allocated for perl match functions
  -------------------------------------------------------------------------*/
  for ( i=0; i<preBulkMatchNumber; i++ ) 
    free(bulkMatchList[i]);
  if (bulkMatchList != NULL ) free(bulkMatchList);
  if (matchPattern != NULL ) free(matchPattern);
  if (substituteString != NULL ) free(substituteString);
  free(subMatchSpec);

  perlObjectStatus = UNLOADED;

  return SUCCESS;
}  

/*----------------------------------------------------------------------------
get_bulk_match_result__(order, argumentValue):
The function to get the values of all the matches.
input: is the position of the argument;
output: is the string of match result.
----------------------------------------------------------------------------*/

int get_bulk_match_result__( void ) {

  if (perlObjectStatus == UNLOADED ) {
    load_perl__();
    return(FAILURE);
  }

  if ( bulkMatchList[ptoc_int(1)] == NULL )
    return(FAILURE);        /*no match*/
  else{
    int match_seq_number= ptoc_int(1);
    int match_array_sz= ptoc_int(3);
    if (match_seq_number < match_array_sz)
      c2p_string( bulkMatchList[match_seq_number], reg_term(2));
    else return(FAILURE);
  }
}

/*----------------------------------------------------------------------------
get_match_resultC__(matchCode, matchResult):
Get the value of the submatch string $1, $2, ... from 
the global string array of matchResults

input: is the match code. Match codes correspond to Perl match variables as
follows: 
             -1 -- $&
             -2 -- $`
             -3 -- $'
             -4 -- $+ 
              1 -- $1
              2 -- $2
              3 -- $3
              ....
              9 -- $9
              .... MAX_SUB_MATCH

output: the string of match result.

The results of the matches (the values of Perl vars) are in consecutive
cells of the matchResults array as follows:
     cell#  Perl var
       0 -- $&
       1 -- $`
       2 -- $'
       3 -- $+
       4 -- $1
       5 -- $2
        .......

----------------------------------------------------------------------------*/

int get_match_resultC__( void ) {

  int order; 
 
  int submatch_number=ptoc_int(1);
  
  /*--------------------------------------------------------------------------
   get the order of the argument in the match result storage
  --------------------------------------------------------------------------*/
  switch (submatch_number) {
  case MATCH:     /*MATCH = -1*/
    order = 0;    /* actual position in the memory */
    break;
  case PREMATCH:  /*PREMATCH = -2*/
    order = 1;
    break; 
  case POSTMATCH:  /*POSTMATCH = -3*/
    order = 2;
    break;
  case LAST_PAREN_MATCH:  /*LAST_PAREN_MATCH = -4*/
    order = 3;
    break;
  default:
    if ( submatch_number > MAX_SUB_MATCH ) {
      char message[120];
      sprintf(message,
	      "Specified submatch number %d exceeds the limit: %d\n",
	      submatch_number, MAX_SUB_MATCH);
      xsb_warn(message);
      order = -99;
    }
    else order = submatch_number+3;  /* actual position in the memory */
    break;
  }

  if (order == -99) return(FAILURE);

  if ( matchPattern == NULL ) { /*didn't try_match before*/
     xsb_warn("call try_match first!");
     return(FAILURE);
   } else if ( !strcmp(matchResults[order],"") || matchResults[order] == NULL )
     return(FAILURE);           /*no match found, return FAILURE */
  else {
    c2p_string( matchResults[order], reg_term(2));
    return(SUCCESS);
  }

}

/*----------------------------------------------------------------------------
void build_sub_match_string( void )
This function is used to build the submatch arguments list string, 
"($&,$`,$',$+,$1,$2,$3.....,$MAX_SUB_MATCH)"
here the value of MAX_SUB_MATCH is defined in the include file
----------------------------------------------------------------------------*/

void build_sub_match_spec( void ) {

  int i,j,k;          /*counter flags*/
  int spaceSize;      /*memory space size for the submatch string*/

  /*get the size of the submatch string, the size of string $1, $2 etc., is 2,
    when the digit is bigger than 10, the size of string $10, $11 etc., is 3,
    so whenever the digit increases by 10, the size of the string $digit will
    increase by 1. Following code is to calculate the size of string $1,$2...*/
  j = 1;
  k = 10;
  spaceSize=0;
  for (i=1;i<=MAX_SUB_MATCH;i++) {
    if ( i%k==0 ) {
      j++;
      k*=10;
    }
    spaceSize += 2+j;    /* the size of ",$" is 2 */
  }
  spaceSize+=(sizeof(FIXEDSUBMATCHSPEC)+1);


  /*build the submatch string*/
  subMatchSpec=(char *)malloc(spaceSize);
  strcpy(subMatchSpec, FIXEDSUBMATCHSPEC); /*build the fixed part $&,$`,$',$+*/

  /* add string $1, $2 etc., to the end of the string */
  for (i=1; i<=MAX_SUB_MATCH;i++)
    sprintf(&(subMatchSpec[strlen(subMatchSpec)]), ",$%d\0", i);
            /*add one of $1, $2 etc., to the string each time by order*/
  strcat(subMatchSpec, ")");

  return;
  
}







