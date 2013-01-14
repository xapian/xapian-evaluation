/* stopword.cc: stop word manipulation routines
 *
 * ----START-LICENCE----
 * Copyright 2003 Andy MacFarlane, City University
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 * -----END-LICENCE-----
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "stopword.h"

#define LSTRING 80         /* A Long String of characters */
#define TRUE 1             /* a condition is true */
#define FALSE 0            /* a condition is false */
#define IDENTICAL 0        /* two words are equal in strcmp */
#define LATER 1            /* 1 word is lexico.  later than another */
#define EARLIER -1         /* 1 word is lexico.  ealier than another */
#define NL '\n'            /* newline marker */
#define KW_SIZE 80         /* maximum size of a keyword */

void Init_str( char str[], int size ) {
  /* initialise a string */

  int i;

  for(i = 0; i < size; i++)
    str[i] = '\0';

} /* END Init_str */

void Read_Extra_SWord( FILE *f_id, char word[KW_SIZE] ) { 
  /* read a stop word from the extra stop word file*/ 
 
  int i; 
  char c;                         /* character read in */ 
 
  /*initialize word*/ 
  Init_str( word, KW_SIZE ); 
 
  /* get character from file until  newline or end of file found */ 
  for( i=0; i < KW_SIZE && (c=fgetc(f_id)) != EOF && c != NL; i++ ) 
    word[i] = c; 
  /* END for */ 
  
} /*END Read_Extra_SWord */ 


void Read_SW_File( char sw_file[], SW_STORE * sw_store ) { 
  /* read stop word file into  stop word store
     version which goes directly to the file, with need for specifiying dir */ 

  FILE *swf_id;	/* file id for stopword file */ 
  int i; 
  char sw_fname[LSTRING];  /* full path and name of sw file */ 

  sw_store->nstops = 0;  /* init just in case! */

  strcpy( sw_fname, sw_file );

  swf_id = fopen( sw_fname, "r" );	/*open stop word file*/
  if( !swf_id ) 
    {
      fprintf(stderr,"ERROR - can't open the extra stop word file [%s]\n",sw_fname ); 
      exit( 0 );
    } /* END if */ 

  /*read stop words into stop word store*/
  for( i=0; i < MAX_STOPWORDS && !feof(swf_id); i++) {
    Read_Extra_SWord( swf_id, sw_store->words[i] );
    if(!feof(swf_id)) 
      {
	sw_store->nstops++;
	//fprintf(stderr,"DEBUG) sw(%d) %s\n", i, 
	  // sw_store->words[i] );
      } /* END if */
  } /*END for*/
  //fprintf(stderr,"STOP_WORDS) read in %d stop words\n", sw_store->nstops );

  fclose( swf_id );	/*close down stop word file*/ 
 
} /* END Read_SW_File */ 

int IsStopWord( SW_STORE sw_store, char word[KW_SIZE] ) { 
  /* see if word is a extra stop word / stop integer during iterative search or 
     relevance feedback process */ 
 
  int found = FALSE;		/* word found status */ 
  int low = 0;			/* min position of search */ 
  int high;	                /* max position of search */ 
  int middle;			/* middle position of current search */ 
  int result;			/* result of compare between two words */ 
 
  high = sw_store.nstops;
 
  if( isdigit(word[0]) )  
    { /* if its a digit don't index it */ 
      found = TRUE; 
    } 
  else /* its a word, so compare it with the full stop word list - using binary chop */ 
    { 
      while( low <= high && !found ) 
	{ 
	  middle = (high + low) / 2; 
	   
	  result = strcmp( word, sw_store.words[middle] ); 
		//fprintf(stderr,"COMPARE) [%s] with sw[%s], RESULT=%d\n", word, sw_store.words[middle], result );
	  if (result == IDENTICAL) 
	    found = 1; 
	  else if (result >= LATER) 
	    low = middle + 1; 
	  else 
	    high = middle - 1; 
	  /*END if*/ 
	} /*END while*/ 
    } /* END if */ 
  //fprintf(stderr,"RESULT) %d\n", found );

  return found;	/* result of search */ 
 
} /* END IsStopWord */ 
