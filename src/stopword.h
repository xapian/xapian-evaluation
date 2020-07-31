/* stopword.h: stop word access routines
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

#ifndef _STOPS_H_
#define _STOPS_H_

#define KW_SIZE 80         /* maximum size of a keyword */
#define MAX_STOPWORDS 1000  /* max number of words in extra stop word list */

typedef struct sw_store {

  char words[MAX_STOPWORDS][KW_SIZE]; /* list of stop words */
  unsigned nstops;   /* the number of stop words held in core */

} SW_STORE;


extern void Read_SW_File( char sw_file[], SW_STORE * sw_store ); 
  /* read stop word file into  stop word store
     version which goes directly to the file, with need for specifying dir */

extern int IsStopWord( SW_STORE sw_store, char word[KW_SIZE] );
  /* see if word is a stop word  */ 

extern void Init_str( char str[], int size );
/* init str */

#endif

