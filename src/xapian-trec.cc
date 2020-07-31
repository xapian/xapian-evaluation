/*
**  xapian-trec.cc
**
**  AM 8/11/2006
**  An example TREC search mechanism using Xapian
**
*/

#include <xapian.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "split.h"

using namespace Xapian;
using namespace std;

int main (int argc, char *argv[]) {

  // There must be four command line arguments passed in the order:
  // database_name, query file name, results file name, db type, run type,
  if (argc < 5) {
    cerr << "ERROR: Insufficient arguments passed to program\n";
    cerr << "USAGE: xapian-trec <database> <query file> <results file>  <run id>  \n";
    exit(0);
  } // END if

  // Catch any Error exceptions thrown
  try {

    /* set up xapian search and term handling */
    Database db(argv[1]);
    Enquire enquire(db);
    Stem stemmer("english");

    /* open files for reading and writing, and items to use/record during search */
    std::ifstream query_file( argv[2]);
    if(!query_file) cerr << "main) Can't open the query file" << argv[2] << '\n';
    std::ofstream results_file( argv[3]);
    if(!results_file) cerr << "main) Can't open the results file" << argv[3] << '\n';
    string run_id( argv[4] );

    // iterate through query file until none left
    while(!query_file.eof()) {
      string topic_no;		 // the topic number of a given query

      /* get the line */
      char ch;
      string line;
      while( query_file.good() )
						 line += (char) query_file.get();

      /* get the topic no, and terms from the line */
      vector<string> terms;
      split(line, ' ', terms );

      /*get the topic number */
      topic_no = terms[0];
			vector<string>::const_iterator termiter = terms.begin();
      termiter++;

      /*get the terms for querying */
      vector<string> stemmed_terms;
      for( ; termiter != terms.end(); termiter++)
					 stemmed_terms.push_back(stemmer(*termiter));

      /* create the query and get the results for it */
      Query query(Query::OP_OR, stemmed_terms.begin(), stemmed_terms.end());
      enquire.set_query(query);
      MSet matches = enquire.get_mset(0, 1000 );

      // get the results and save them to the file
			int next=0;
      for ( MSetIterator i = matches.begin(); i != matches.end(); ++i ) {
					results_file << topic_no << "Q0" << i.get_document().get_data() << next << i.get_weight() << run_id << '\n';
					next++;
			} // END for

    } /* END while */

  } catch (const Error &error) {
    cout << "Exception: "  << error.get_msg() << endl;
    exit(1);
  } // END try/catch

} // END main
