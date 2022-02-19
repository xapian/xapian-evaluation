/* trec_query.cc: Example batch query generator for TREC experiments
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


#include <xapian.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include "htmlparse.h"
#include "config_file.h"
#include "indextext.h"
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <errno.h>
#include <cstring>
#include <stdio.h>

using namespace Xapian;
using namespace std;

#define MAX_KEYWORDS 10000
#define BIG_BUFFER 100000
#define NOACTION 0
#define END_TOPIC 1
#define TOPIC_NUMBER 2
#define TITLE 3
#define DESC 4
#define NARR 5
#define SAVETERM 6


typedef struct query {
  /* data structure for query */

  int topic_no;                                      /* topic no for the query */
  char term_set[MAX_KEYWORDS][KW_SIZE];         /* list of query terms */
  double weights[MAX_KEYWORDS];			     /* weights for those terms */
  int nwords;					     /* no of words in the query */

} QUERY;

void skip_spaces( char page[], int *curpos ) {
  /* skip any number of spaces/irrelevant chars in rows/cols */

  /* find an alpha numeric character */
  while( page[*curpos] != '\0' &&
	 !isalnum( page[*curpos] ) && !(page[*curpos] == '<')) {
      *curpos += 1;
  } /*END while*/

} /*END skip_spaces*/

void fold_word( char string[], int len ) {
  /* case fold a word */

  int i;

  for( i= 0; i < len; i++ )
     string[i] = tolower( string[i] );

} /* END fold_word */

int iswordbreak( char c ) {
  /* see if there is a break in the middle of a word, such as a hyper
     less than sign etc, if so return true else false - version2 */
  /* note: used for the chamber: ignores <> chars for tag processing */

  if( c >= 0 && c <= '-' )
    return 1;
  if( c >= '[' && c <= '`' )
    return 1;
  if( c >= '{' && c <= '~' )
    return 1;
  if( c >= ':' && c <= ';' )
    return 1;
  if( c >= '=' && c <= '@' )
    return 1;
  if( c == '/'  )
    return 1;
 return 0;

} /* END iswordbreak */

void get_chars(char buffer[], int *curpos,
		char saved_chars[KW_SIZE]) {
    /* get each character for a word */

    bool found = false;             /* condition for a found word */
    int cur_char;                   /* pos of char in current word */

    /* loop until end of word found */
    for (cur_char = 0; cur_char < KW_SIZE && !found; ++cur_char) {
	if (isspace(buffer[*curpos])) {
	    found = true;
	    break;
	} else if (buffer[*curpos] == '\0' || buffer[*curpos] == '>') {
	    found = true;
	    if (buffer[*curpos] == '>') {
		saved_chars[cur_char] = buffer[*curpos];
		++cur_char;
	    }
	    *curpos += 1;
	    break;
	} else if (buffer[*curpos] == '<' && cur_char != 0) {
	    found = true;
	    ++cur_char;
	    break;
	} else { /* got a saveable character */
	    saved_chars[cur_char] = buffer[*curpos];
	    *curpos += 1;
	} /* END if */
    } /* END for */

    fold_word(saved_chars, cur_char); /* case fold the word */

    /* deal with appostr. (mostly <string>'s really) */
    if (cur_char >= 2) {
	if (saved_chars[cur_char - 2] == '\'' ||
	   saved_chars[cur_char - 2] == '\"') {

	    saved_chars[cur_char - 2] = saved_chars[cur_char - 1];
	    saved_chars[cur_char - 1] = '\0';
	} else {
	    saved_chars[cur_char] = '\0';
	} /* END if */
    } else {
	saved_chars[cur_char] = '\0';
    }
} /* END get_chars */

void get_word( char buffer[], char word[KW_SIZE], int *curpos ) {
  /* get a word from a page */

  /*skip any number of spaces/irrelevant chars in rows/cols*/
  skip_spaces( buffer, curpos  );

  /* now get each character from the line */
  get_chars( buffer, curpos, word );
  word[KW_SIZE-1] = '\0';  /*max size keyword to record */
} /*END get_word*/

void check_code( char word[KW_SIZE], int *code ) {

	*code = SAVETERM; /* assume it's a saveable term */

	if( strncmp( word,"<top>",4) == 0 )
		*code = -1;
	if( strncmp( word,"<num>",5) == 0 )
		*code = TOPIC_NUMBER;
	if( strncmp( word,"</num>",5) == 0 )
		*code = NOACTION;
	if( strncmp( word,"<title>",7) == 0 )
		*code = TITLE;
	if( strncmp( word,"</title>",7) == 0 )
		*code = NOACTION;
	if( strncmp( word,"<desc>",6) == 0 )
		*code = DESC;
	if( strncmp( word,"</desc>",6) == 0 )
		*code = NOACTION;
	if( strncmp( word,"<narr>",6) == 0 )
		*code = NARR;
	if( strncmp( word,"</narr>",6) == 0 )
		*code = NOACTION;
	if( strncmp( word,"</top>",6) == 0 )
		*code = END_TOPIC;

} /* END check_code */

static void
clean_word(char word[KW_SIZE])
{
    for (size_t i = 0, curpos = 0; i < strlen(word); i++ ) {
	if (isalnum(word[i])) word[curpos++] = word[i];
    }
}

void find_field_status( char arg[], int *use_title, int *use_desc, int *use_narr ) {
/* find out what type of search you want the user wants */

  int foundone=0;

  for (size_t i=0; i < strlen(arg); i++) {
    switch(arg[i]) {
    case 't' : *use_title=1; foundone=1;
      break;
    case 'd' : *use_desc=1; foundone=1;
      break;
    case 'n' : *use_narr=1; foundone=1;
      break;
      //default :
    } // END switch
  } /* END for */

  if( foundone ) {
    if(*use_title) fprintf(stderr,"FIELD) you have requested the use of the title field\n");
    if(*use_desc) fprintf(stderr,"FIELD) you have requested the use of the description field\n");
    if(*use_narr) fprintf(stderr,"FIELD) you have requested the use of the narrative field\n");

  } else {
    fprintf(stderr,"ERROR - invalid TREC field entered [%s] please try again\n", arg );
  } /* END if */

} /* END find_field_status */

void Init_QUERY( QUERY *q ) {
/* initalise a query */

	int i;

	for( i=0; i < MAX_KEYWORDS; i++ ) {
		memset(q->term_set[i], 0, KW_SIZE);
		q->weights[i] = 0.0;
	} /* END for */
	q->nwords = 0;

} /* END Init_QUERY */

void Save_QUERY( FILE *qd, QUERY q ) {
/* save query details to disk */

	int i;
	fprintf( qd, "%d ", q.topic_no );
	fprintf( stderr, "got query {%d ", q.topic_no );
	for( i=0; i < q.nwords; i++ ) {
		fprintf( qd, "%s ", q.term_set[i] );
		fprintf(stderr, "%s ", q.term_set[i] );
	} /* END if */
	fprintf( qd, "\n" );
	fprintf( stderr, "}\n" );

} /* END Save_QUERY */


int Isin_query( char word[KW_SIZE], QUERY *q ) {
/* returns 1 if word is in query, 0 otherwise */

	int found=0;
	int i;

	for( i=0; i < q->nwords && !found; i++ ) {
		if( strcmp( word, q->term_set[i] ) == 0 ) found=1;
	} /* END for */

	return found;

} /* END Isin_query */

#define NTOPICSTOPS 66

char topic_stops[NTOPICSTOPS][KW_SIZE] = {
"document", "documents","discuss","discussed","mention","mentions","cite","cited","include","includes",
"included","report","reports","describe","describes","described","relevant","relevance","concern","concerns",
"concerned","reveal","specify","specifying","specified","specifics","announce","announces","announced",
"announcing","event","provided","occur","present","contain","contains","containing","example","examples",
"eg","ie","instance","instances","consider","considered","indicative","note","notes","noted","quote",
"quotes","substantive","unless","find","finds", "identify", "identified","identfies", "evidence",
"continue","define","determine", "determined", "discussing", "references", "reference" };

/* save a word in a query, iff it isn't already there */
static void
Save_Word(char word[KW_SIZE], QUERY *q, Xapian::Stopper& stopper)
{
    if (!Isin_query(word, q) && !stopper(word)) {
        strcpy( q->term_set[q->nwords], word );
        q->nwords += 1;
    }
}

void create_queries( CONFIG_TREC & config ) {

  FILE *query_fd;
  FILE *topic_fd;
  QUERY q;
  int code;
  char word[KW_SIZE];
  int use_desc=0;
  int use_narr=0;
  int use_title=0;
  char topics[BIG_BUFFER];
  int topic_size=0;
  int curpos=0;
  int terms_to_save;
  int save_word=0;

  terms_to_save = config.get_nterms();  /* number of terms to save */
  (void)terms_to_save; // FIXME: Use or remove

  /* find out what type of search you want the user wants */
  find_field_status( (char *) config.get_topicfields().c_str(), &use_title, &use_desc, &use_narr );

  /* open the files for manipulation */
  cout << "QUERY) topic file name is " << config.get_topicfile().c_str() << endl;
  topic_fd = fopen( config.get_topicfile().c_str(), "r" );
  if(!topic_fd) {
    cout << "ERROR - can't open topic file" << config.get_topicfile().c_str() << "for reading" << endl;
    std::exit(-1);
  } /* END if */
  for( topic_size=0; topic_size < BIG_BUFFER && !feof(topic_fd); topic_size++ ){
    topics[topic_size] = fgetc(topic_fd);
  } // END for
  fclose( topic_fd );

  query_fd = fopen( config.get_queryfile().c_str(), "w" );
  if(!query_fd) {
    cout << "ERROR - can't open file [" << config.get_queryfile().c_str()  << "] for writing" << endl;
    std::exit(0);
  } // END if
  cout << "QUERY) query file name is: " <<  config.get_queryfile().c_str() << endl;

  ifstream words(config.get_stopsfile().c_str());
  Xapian::SimpleStopper stopper{istream_iterator<string>(words),
                                istream_iterator<string>()};
  // Add topic stops.
  for (int i = 0; i != NTOPICSTOPS; ++i) {
      stopper.add(topic_stops[i]);
  }

  /* iniatisation */
  Init_QUERY( &q );
  code = NOACTION;

  while( curpos < topic_size ) {

    memset(word, 0, KW_SIZE);
    get_word( topics, word, &curpos );
    check_code( word, &code );
    clean_word( word );
    switch( code ) {
    case NOACTION :
      {
	cout << "No action" << endl;
      }
      break;
    case END_TOPIC :
      {
	/* save the query and clean it */
	Save_QUERY( query_fd, q );
	Init_QUERY( &q );
	save_word=0;
      }
      break;
    case TOPIC_NUMBER :
      {
	get_word( topics, word, &curpos ); // spin past Number
	q.topic_no = atoi(word);
	get_word( topics, word, &curpos );
	// Extracting topic number by reading it from the file
	if(q.topic_no == 0)
	q.topic_no = atoi(word);
	save_word=0;
      }
      break;
    case TITLE :
      {
	if( use_title ) save_word = 1;
	else save_word=0;
      }
      break;
    case DESC :
      {
	get_word( topics, word, &curpos ); /* spin past Description: */
	if( use_desc ) save_word = 1;
	else save_word=0;
      }
      break;
    case NARR :
      {
	get_word( topics, word, &curpos ); /* spin past Narrative: */
	if( use_narr ) save_word = 1;
	else save_word=0;
      }
      break;
    default:  /* save the word if required */
      if (save_word) Save_Word(word, &q, stopper);
      break;
    }
  } /* END while */

} /* END create_queries( */

int main(int argc, char **argv)
{

  // only one parameter is required for this program
  if(argc < 2) {
    cout << "usage: " << argv[0] << " <config file>" << endl;
    std::exit(1);
  }

  // Catch any Xapian::Error exceptions thrown
  try {

    CONFIG_TREC config;
    config.setup_config( string(argv[1]) );
    config.check_query_config();

    create_queries( config );

  } catch(const Xapian::Error &error) {
    cout << "Exception: "  << error.get_msg() << endl;
  }
} // END main
