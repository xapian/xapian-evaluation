/* trec_search.cc: Example batch search for TREC experiments
 *
 * ----START-LICENCE----
 * Copyright 1999,2000,2001 BrightStation PLC
 * Copyright 2003 Olly Betts
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

#include "config_file.h"
#include <fstream>
#include <xapian.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <time.h>
#include "split.h"
#include "timer.h"

using namespace Xapian;
using namespace std;

CONFIG_TREC config;

void get_stopper(Xapian::SimpleStopper &stopper) {
ifstream stopfile(config.get_stopsfile().c_str(),ifstream::in);
while ( !stopfile.eof() ) {
	string stopword;
	getline(stopfile,stopword);
	stopper.add(stopword);
	}
	stopfile.close();
}

int load_query( std::ifstream & queryfile, int & topicno,Xapian::Query & query, Xapian::Stem & stemmer ) {
// load a query and record its terms

	if( queryfile.eof() ) return 0;

	int found_topicno=0;
	string line;
	getline(queryfile,line);
	line[line.size()-1] ='\0';
	Xapian::QueryParser qp;
	Xapian::SimpleStopper stopper;
	get_stopper(stopper);
	qp.set_stopper(&stopper);
	qp.set_stemmer(stemmer);
	qp.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);
	query = qp.parse_query(line);
	cout<<"Parsed Query is :\t"<<query.get_description()<<endl;
	vector<string> data;
	split(line, ' ', data );
	vector <string> terms;
	for( vector<string>::const_iterator start = data.begin(); start != data.end(); start++ ) {
	  string queryword;
		if( !found_topicno ) {
			topicno = atoi( start->c_str());
			found_topicno=1;
		}

	} // END for


	return 1;

} // END load_query

int main(int argc, char **argv)
{
  // Simplest possible options parsing: we just require two or more
  // parameters.
  if(argc < 2) {
    cout << "usage: " << argv[0] << " <config file>" << endl;
    exit(1);
  }

  // Catch any Xapian::Error exceptions thrown
  try {
    // load the TREC experiment configuration file
    config.setup_config( string(argv[1]) );
    config.check_search_config();
    Xapian::Stem stemmer( config.get_language() );
    struct timeval start_time, finish_time, timelapse;   /* timing variables */

    // Make the database
    Xapian::Database db(config.get_db().c_str());

    // Start an enquire session
    Xapian::Enquire enquire(db);

    // open the query file
    std::ifstream queryfile( config.get_queryfile().c_str() );

    // open the results file
    std::ofstream resultsfile( config.get_resultsfile().c_str() );

    // open the transaction file
    std::ofstream transfile( config.get_transfile().c_str() );


    // count of no queries done
    int count=0;

    // total query time
    float total_qp_time = 0.0;

    // process the queries
    while( !queryfile.eof() ) {

      int topicno;  // topic number for the query
      int len=0;

      // Build the query object
      Xapian::Query query;
      int gotquery = load_query( queryfile, topicno, query, stemmer );

      if(gotquery && !queryfile.eof()) {

	// start the timer
	gettimeofday( &start_time, 0 );

	cout << "Running " << topicno << ", query = [" << query.get_description() << "] getting " << config.get_noresults() << " docs" << endl;

	// Give the query object to the enquire session
	enquire.set_query(query);

	auto wt = Xapian::Weight::create(config.get_weightingscheme());
	enquire.set_weighting_scheme(*wt);
	delete wt;

	// Get the top n results of the query
	Xapian::MSet matches = enquire.get_mset( 0, config.get_noresults() );

	// record the number of matches made in this query
	//int queryweightings = enquire.get_totalweightings();
	//cout << "W's) for this query is -> " << queryweightings << endl;

	// Display the results cout << matches.size() << " results found" << endl;
	count++;
	if( (count % 1000) == 0 ) cout << "QUERIES PROCESSED) " << count << endl;

	// record the results in a 'trec.log' file
	for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); i++) {
	  Xapian::Document doc = i.get_document();
	  resultsfile << topicno << " Q0 " << doc.get_data() << " " << i.get_rank() << " " <<
	    i.get_weight() << " " << config.get_runname() << endl;
	  len++;
	} // END for

	// record the finish
	gettimeofday( &finish_time, 0 );
	diff_time( finish_time, start_time, &timelapse );
	float qp_time = time_real( timelapse );
	total_qp_time += qp_time;
	transfile << topicno << "," << qp_time << "," << len << endl;
	cout << topicno << "," << qp_time << "," << len << endl;


      } // END if
    } // END while

    // print the total time, and average time per query

    float avg_qp_time = total_qp_time /(float) count;
    cout << "Average query time for " << count << " Queries is " <<
      avg_qp_time << " secs, took a total of " << total_qp_time << " secs" << endl;

  } catch( const Xapian::Error &error) {
    cout << "Exception: "  << error.get_msg() << endl;
  } // END try/catch block

} // END main
