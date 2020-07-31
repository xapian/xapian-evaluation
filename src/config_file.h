/* config_file.h: configuration class for trec experiments
 *
 * ----START-LICENCE----
 * Copyright 2003 Andy MacFarlane, City University
 * Copyright 2012 Gaurav Arora
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>

using namespace std;

/* maximum size of a keyword */
#define KW_SIZE 80

class CONFIG_TREC {

private:

	// aspects of TREC config experiments
	string textfile;     // path/filename of text file
	string language;     // corpus language
	string db;           // path of database
	string querytype;    // type of query
	string queryfile;    // path/filename of query file
	string resultsfile;  // path/filename of results file
	string transfile;    // path/filename of transaction file
	int noresults;       // no of results to save in results log file
	string topicfile;    // path/filename of topic file
	string topicfields;  // fields of topic to use from topic file
	string relfile;      // path/filename of relevance judgements file
	string runname;      // name of the run
	int nterms;          // no of terms to pick from the topic
	string stopsfile;    // name of the stopword file
	string evaluationfiles; //path/filename of the evaluation files.
	string indexbigram;  // Index Bigram in the index.
	string queryparsebigram; //Parse and add bigrams to the Query.
	string weightingscheme; //which weighting scheme to select

	// private access routines
	void record_tag( string config_tag, string config_value );

public:

	// base constructor
	//CONFIG_TREC();

	// setup routines
	void setup_config( string filename );

	// validation routines
	int check_query_config();
	int check_index_config();
	int check_search_config();

	// access routines
	string get_textfile() { return textfile; }
	string get_language() { return language; }
	string get_db() { return db; }
	string get_querytype() { return querytype; }
	string get_queryfile() { return queryfile; }
	string get_resultsfile() { return resultsfile; }
	string get_transfile() { return transfile; }
	int get_noresults() { return noresults; }
	string get_topicfile() { return topicfile; }
	string get_topicfields() { return topicfields; }
	string get_relfile() { return relfile; }
	string get_runname() { return runname; }
	int get_nterms() { return nterms; }
	string get_stopsfile() { return stopsfile; }
	string get_evaluationsfile() { return evaluationfiles; }
	string get_weightingscheme() const { return weightingscheme; }

	bool get_indexbigram() { return indexbigram == "true"; }

	bool get_queryparsebigram() {
	    return indexbigram == "true" && queryparsebigram == "true";
	}
}; // END class CONFIG

#endif
