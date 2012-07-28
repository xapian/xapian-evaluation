/* config_file.cc: configuration load for trec experiments
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

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <xapian.h>
#include "config_file.h"
#include "split.h" 

using namespace Xapian;
using namespace std;

void CONFIG_TREC::record_tag( string config_tag, string config_value ) {

  int found=0;

  if( config_tag == "textfile" ) {
    textfile = config_value;
    found = 1;
  } // END if
  if( config_tag == "stopsfile" ) {
    stopsfile = config_value;
    found = 1;
  } // END if
  if( config_tag == "language" ) {
    language = config_value;
    found = 1;
  } // END if
  if( config_tag == "db" ) {
    db = config_value;
    found = 1;
  } // END if
  if( config_tag == "querytype" ) {
    querytype = config_value;
    found = 1;
  } // END if
  if( config_tag == "queryfile" ) {
    queryfile = config_value;
    found = 1;
  } // END if
  if( config_tag == "resultsfile" ) {
    resultsfile = config_value;
    found = 1;
  } // END if
  if( config_tag == "transfile" ) {
    transfile = config_value;
    found = 1;
  } // END if
  if( config_tag == "noresults" ) {
    noresults = atoi(config_value.c_str());
    found = 1;
  } // END if
  if( config_tag == "const_k1" ) {
    const_k1 = atof(config_value.c_str());
    found = 1;
  } // END if
  if( config_tag == "const_b" ) {
    const_b = atof(config_value.c_str());
    found = 1;
  } // END if
  if( config_tag == "topicfile" ) {
    topicfile = config_value;
    found = 1;
  } // END if
  if( config_tag == "topicfields" ) {
    topicfields = config_value;
    found = 1;
  } // END if
  if( config_tag == "relfile" ) {
    relfile = config_value;
    found = 1;
  } // END if
  if( config_tag == "runname" ) {
    runname = config_value;
    found = 1;
  } // END if
  if( config_tag == "nterms" ) {
    nterms = atoi(config_value.c_str());
    found = 1;
  } // END if
  if ( config_tag == "evaluationfiles" ) {
  evaluationfiles = config_value;
  } // END if
  
  if( !found ) {
    cout << "ERROR: could not locate tag [" << config_tag << "] for value [" << config_value
	 << "]" << endl;
  } // END if
  
} // END record_tag

void CONFIG_TREC::setup_config( string filename ) {

  // set defaults
  textfile = "noneassigned";     // must enter a file/dir for text
  language = "english";          // corpus language
  db = "noneassigned";           // must enter path of database
  querytype = "n";               // type of query: default is n=normal
  queryfile = "noneassigned";    // must enter path/filename of query file
  resultsfile = "trec.log";      // path/filename of results file
  transfile = "transaction.log";  // transaction log file (timings etc)
  noresults = 1000;              // no of results to save in results log file
  const_k1 = 1.2;                // value for K1 constant (BM25)
  const_b = 0.75;                // value for B constant (BM25)
  topicfile = "noneassigned";    // path/filename of topic file
  topicfields = "t";             // fields of topic to use from topic file: default title
  relfile= "noneassigned";       // path/filename of relevance judgements file
  runname = "xapiantrec";         // name of the run
  nterms = 100;                  // no of terms to pick from the topic
  evaluationfiles = "eval.log";
  
  std::ifstream configfile( filename.c_str() );
  
  if( !configfile ) {
    cerr << "ERROR: you must specify a valid configuration file name" << endl;
    exit(0);
  } //else cout << "CONFIG) Opened configuration file: " << filename << endl;

  while( !configfile.eof() ) {
    
    // read in lines from the configuration file
    string data; 
    //  the tag
    string config_tag;
    // get the value
    string config_value;
    
    // identify and save information from the configuration file
    if( !configfile.eof() ) {
      configfile >> data;
      config_tag = data;
      configfile >> data;
      config_value = data;
      //cout << "GOT) values [" << config_tag << "] and [" << config_value << "]" << endl;
      
      // record the tag
      if( !configfile.eof() ) record_tag( config_tag, config_value );
    } // END if
    
  } // END while
  
} // END setup_config

int CONFIG_TREC::check_query_config() {
// ensure that all the information required by query generator has been entered in config file

	if( queryfile == "noneassigned" ) {
		cerr << "ERROR: you must specify the query file" << endl;
		return 0;
	} // END if
	if( stopsfile == "noneassigned" ) {
		cerr << "ERROR: you must specify the stop word file" << endl;
		return 0;
	} // END if
	if( topicfile == "noneassigned" ) {
		cerr << "ERROR: you must specify the topic file" << endl;
		return 0;
	} // END if
  if( db == "noneassigned" ) {
		cerr << "ERROR: you must specify the db path" << endl;
		return 0;
	} // END if
	
	return 1;
	
} // END check_query_config

int CONFIG_TREC::check_index_config( ) {
// ensure that all the information required by indexer has been entered in config file

	if( stopsfile == "noneassigned" ) {
		cerr << "ERROR: you must specify the stops file" << endl;
		return 0;
	} // END if

 if( db == "noneassigned" ) {
		cerr << "ERROR: you must specify the db path" << endl;
		return 0;
	} // END if
	
 if( textfile == "noneassigned" ) {
		cerr << "ERROR: you must specify the db path" << endl;
		return 0;
	} // END if

	return 1;
	
} // END check_index_config

int CONFIG_TREC::check_search_config( ) {
// ensure that all the information required by search program has been entered in config file

	if( queryfile == "noneassigned" ) {
		cerr << "ERROR: you must specify the " << endl;
		return 0;
	} // END if
	if( stopsfile == "noneassigned" ) {
		cerr << "ERROR: you must specify the " << endl;
		return 0;
	} // END if

	return 1;
	
} // END check_search_config
