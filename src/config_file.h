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
#include <xapian.h>
using namespace std;

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
	float const_k1;      // value for K1 constant (BM25)
	float const_b;       // value for B constant (BM25)
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

	//Parameters for BM25 Weighting Scheme.
	double bm25param_k1;
	double bm25param_k2;
	double bm25param_k3;
	double bm25param_b;
	double bm25param_min_normlen;

	//Parameters for BM25+ Weighting Scheme.
	double bm25plusparam_k1;
	double bm25plusparam_k2;
	double bm25plusparam_k3;
	double bm25plusparam_b;
	double bm25plusparam_min_normlen;
	double bm25plusparam_delta;

	//Parameters for PL2+ Weighting Scheme.
	double pl2plusparam_c;
	double pl2plusparam_delta;

	//Parameters for Tras Weighting scheme.
	double tradparam_k;

	//Parameters for LMWeight Weighting Scheme.

	double lmparam_log;
	Xapian::Weight::type_smoothing lmparam_select_smoothing;
	double lmparam_smoothing1;
	double lmparam_smoothing2;
	double lmparam_mixture;

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
	bool check_bm25();
	bool check_trad();
	bool check_lmweight();
	bool check_bm25plus();
	bool check_pl2plus();

	// access routines
	string get_textfile() { return textfile; }
	string get_language() { return language; }
	string get_db() { return db; }
	string get_querytype() { return querytype; }
	string get_queryfile() { return queryfile; }
	string get_resultsfile() { return resultsfile; }
	string get_transfile() { return transfile; }
	int get_noresults() { return noresults; }
	float get_const_k1() { return const_k1; }
	float get_const_b() { return const_b; }
	string get_topicfile() { return topicfile; }
	string get_topicfields() { return topicfields; }
	string get_relfile() { return relfile; }
	string get_runname() { return runname; }
	int get_nterms() { return nterms; }
	string get_stopsfile() { return stopsfile; }
	string get_evaluationsfile() { return evaluationfiles; }

	bool get_indexbigram() { return (indexbigram.compare("true") == 0);
	}

	bool get_queryparsebigram() { return ((indexbigram.compare("true") == 0) && (queryparsebigram.compare("true") == 0));
	}
	
	bool use_weightingscheme(string scheme);

	double get_bm25param_k1() { return bm25param_k1; }
	double get_bm25param_k2() { return bm25param_k2; }
	double get_bm25param_k3() { return bm25param_k3; }
	double get_bm25param_b()  { return bm25param_b;  }
	double get_bm25param_min_normlen() { return bm25param_min_normlen; }

	double get_bm25plusparam_k1() { return bm25plusparam_k1; }
	double get_bm25plusparam_k2() { return bm25plusparam_k2; }
	double get_bm25plusparam_k3() { return bm25plusparam_k3; }
	double get_bm25plusparam_b()  { return bm25plusparam_b;  }
	double get_bm25plusparam_min_normlen() { return bm25plusparam_min_normlen; }
	double get_bm25plusparam_delta() { return bm25plusparam_delta; }

	double get_pl2plusparam_c() { return pl2plusparam_c; }
	double get_pl2plusparam_delta() { return pl2plusparam_delta; }

	double get_tradparam_k() { return tradparam_k; }
	
	double get_lmparam_log() { return lmparam_log; }

	Xapian::Weight::type_smoothing get_lmparam_select_smoothing() { return lmparam_select_smoothing; }

	double get_lmparam_smoothing1() { return lmparam_smoothing1; }
	
	double get_lmparam_smoothing2() { return lmparam_smoothing2; }

	double get_lmparam_mixture() { return lmparam_mixture; }


}; // END class CONFIG

#endif
