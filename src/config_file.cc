/* config_file.cc: configuration load for trec experiments
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

#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "config_file.h"
#include "split.h"

using namespace std;

void
CONFIG_TREC::handle_item(const string& name, const string& value)
{
    if (name == "textfile") {
        textfile = value;
        return;
    }
    if (name == "stopsfile") {
        stopsfile = value;
        return;
    }
    if (name == "language") {
        language = value;
        return;
    }
    if (name == "db") {
        db = value;
        return;
    }
    if (name == "querytype") {
        querytype = value;
        return;
    }
    if (name == "queryfile") {
        queryfile = value;
        return;
    }
    if (name == "resultsfile") {
        resultsfile = value;
        return;
    }
    if (name == "transfile") {
        transfile = value;
        return;
    }
    if (name == "noresults") {
        noresults = atoi(value.c_str());
        return;
    }
    if (name == "topicfile") {
        topicfile = value;
        return;
    }
    if (name == "topicfields") {
        topicfields = value;
        return;
    }
    if (name == "relfile") {
        relfile = value;
        return;
    }
    if (name == "runname") {
        runname = value;
        return;
    }
    if (name == "nterms") {
        nterms = atoi(value.c_str());
        return;
    }
    if (name == "evaluationfiles") {
        evaluationfiles = value;
        return;
    }

    if (name == "indexbigram" ) {
        indexbigram = value;
        return;
    }
    if (name == "queryparsebigram" ) {
        queryparsebigram = value;
        return;
    }
    if (name == "weightingscheme") {
        weightingscheme = value;
        return;
    }

    cout << "ERROR: unknown config item '" << name << "' with value '"
         << value << "'\n";
}

void
CONFIG_TREC::setup_config(string filename)
{
    // set defaults
    textfile = "noneassigned";     // must enter a file/dir for text
    language = "english";          // corpus language
    db = "noneassigned";           // must enter path of database
    querytype = "n";               // type of query: default is n=normal
    queryfile = "noneassigned";    // must enter path/filename of query file
    resultsfile = "trec.log";      // path/filename of results file
    transfile = "transaction.log";  // transaction log file (timings etc)
    noresults = 1000;              // no of results to save in results log file
    topicfile = "noneassigned";    // path/filename of topic file
    topicfields = "t";             // fields of topic to use from topic file: default title
    relfile= "noneassigned";       // path/filename of relevance judgements file
    runname = "xapiantrec";         // name of the run
    nterms = 100;                  // no of terms to pick from the topic
    evaluationfiles = "eval.log";  //Name of file to which evaluation to be written.
    indexbigram = "false";   // Default value of index bigram
    queryparsebigram = "false"; //Default value of parse bigram in query.
    weightingscheme = "noneassigned";

    std::ifstream configfile( filename.c_str() );

    if (!configfile) {
        cerr << "ERROR: you must specify a valid configuration file name" << endl;
        exit(0);
    }

    while (!configfile.eof()) {
        // Read in lines from the configuration file.
        char line[256];
        if (!configfile.getline(line, sizeof(line))) {
            if (!configfile.eof())
                cout << "Error reading config file\n";
            return;
        }

        char* name = strtok(line, " \t");
        if (name == NULL) {
            // Blank line.
            continue;
        }
        if (name[0] == '#') {
            // Comment.
            continue;
        }

        char* value = strtok(NULL, "");
        if (value == NULL) {
            cout << "No value for tag \"" << name << "\"\n";
            continue;
        }
        // Trim leading whitespace.
        while (*value == ' ' || *value == '\t') {
            ++value;
        }
        // Trim trailing whitespace.
        char* p = value + strlen(value);
        while (p[-1] == ' ' || p[-1] == '\t') --p;
        *p = '\0';

        // cout << "GOT) values [" << name << "] and [" << value << "]" << endl;

        handle_item(name, value);
    }
}

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
