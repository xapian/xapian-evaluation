/* trec_index.cc: indexer for trec experiments
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

#include <xapian.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "htmlparse.h"
#include "config_file.h"
#include "indextext.h"
#include "P98_gzip.h"
#include <time.h>
#include "timer.h"

using namespace Xapian;
using namespace std;

#define ENDDOC "</DOC>"

// chamber (from hashbld) is where the input bundles are decompressed.
#define CHAMBER_SIZE 30000000
char chamber[CHAMBER_SIZE];

float ttextsize=0;  // total amount of text in mb indexed
int totaldocs=0;		// total number of documents indexed

class SGMLParser : public HtmlParser {
public:
  bool in_script_tag;
  bool in_style_tag;
  string title, sample, keywords, dump;
  bool indexing_allowed;
  void process_text(const string &text);
  void opening_tag(const string &tag, const map<string,string> &p);
  void closing_tag(const string &tag);
  SGMLParser() :
    in_script_tag(false),
    in_style_tag(false),
    indexing_allowed(true) { }
};

void SGMLParser::process_text(const string &text) {
    // some tags are meaningful mid-word so this is simplistic at best...

    if (!in_script_tag && !in_style_tag) {
	string::size_type firstchar = text.find_first_not_of(" \t\n\r");
	if (firstchar != string::npos) {
	    dump += text.substr(firstchar);
	    dump += " ";
	}
    }
}

void
SGMLParser::opening_tag(const string &tag, const map<string,string> &p) {

//   	cout<<"Opening Tag"<<tag;
    if (tag == "meta") {
	map<string, string>::const_iterator i, j;
	if ((i = p.find("content")) != p.end()) {
	    if ((j = p.find("name")) != p.end()) {
		string name = j->second;
		lowercase_term(name);
		if (name == "description") {
		    if (sample.empty()) {
			sample = i->second;
			decode_entities(sample);
		    }
		} else if (name == "keywords") {
		    if (!keywords.empty()) keywords += ' ';
		    string tmp = i->second;
		    decode_entities(tmp);
		    keywords += tmp;
		} else if (name == "robots") {
		    string val = i->second;
		    decode_entities(val);
		    lowercase_term(val);
		    if (val.find("none") != string::npos ||
			val.find("noindex") != string::npos) {
			//indexing_allowed = false;
			//cout << "HELP!) found a robot tag which is difficult to index :("  << endl;
		    }
		}
	    }
	}
    } else if (tag == "script") {
	in_script_tag = true;
    } else if (tag == "style") {
	in_style_tag = true;
    } else if (tag == "body") {
	dump = "";
    }
}

void
SGMLParser::closing_tag(const string &tag)
{
 //	  cout << "closing_tag) : " << tag << endl;
    if (tag == "docno") {
			 if( dump.size() < 100 ) // nasty hack to get round problems on terabyte track with robot tags
			 		 title = dump;
			 else
			     title = "DOCNO-ERROR";
	     dump = "";
    } else if (tag == "script") {
	in_script_tag = false;
    } else if (tag == "style") {
	in_style_tag = false;
    } // END if
}

string getline( int & curpos, int uncolen ) {

  string line;
  for( ; curpos < uncolen && chamber[curpos] !='\n'; curpos++ )
    line += chamber[curpos];
  curpos++;
  return line;

} // END getline

string get_document( int & curpos, int uncolen ) {
  // alternative version of get document

  int end_found=0;
  string document;
  while( !end_found ) {
    string line = getline( curpos, uncolen );
    document += line;
    string::size_type pos = line.find(ENDDOC,0);
    if( pos != string::npos ) end_found=1;
  } // END while

  return document;

} // END get_document

inline static bool
p_plusminus(unsigned int c)
{
    return c == '+' || c == '-';
}


static void
index_file(const string& file,
           CONFIG_TREC& config,
           Xapian::WritableDatabase& db,
           Xapian::TermGenerator& indexer)
{
  // index a file containing a number of SGML/HTML documents

  if (file.empty()) {
    cout << "can't read \"" << file << "\" - skipping\n";
    return;
  } else cout << "Indexing [" << file << "]" << endl;

  int curpos=0;
  int uncolen;

  uncolen = decompress_bundle(file.c_str(), chamber, CHAMBER_SIZE);
  cout << "DEBUG) decompresses file done, size = " << uncolen << endl;

  // accumulate the text size read in
  ttextsize += ( (float) uncolen / 1048576.0);

  while( curpos < uncolen ) {

    // get a document
    string rawdoc = get_document( curpos, uncolen );
//    cout << "DEBUG) got a document, size = " << rawdoc <<
  //    ", curpos = " << curpos << endl;

    if( rawdoc.size() > 1 ) {

      // parse the document for the data
      SGMLParser p;
      p.parse_html(rawdoc);

      // Add postings for terms to the document
      Xapian::Document doc;
	  doc.set_data(p.title);
	  indexer.set_document(doc);
	  indexer.index_text(p.title);
	  indexer.index_text(p.dump);
	  indexer.index_text(p.keywords);
      cout << "DOCID = " << p.title << endl;
      cout<<"DOCID:\t"<<db.add_document(doc)<<"TERMLISTCOUNT"<<doc.termlist_count();
      cout<<"DOCADDED\n"<<doc.get_docid();
      totaldocs++;
      //if( (totaldocs % 10000) == 0 ) cout << "DOCUMENTS PROCESSED) " << totaldocs << endl;
    } // END if

  } // END while

} // END index_file

static void
index_directory(const string& dir,
                CONFIG_TREC& config,
                Xapian::WritableDatabase& db,
                Xapian::TermGenerator& indexer);

static void
index_directory_entry(const string& file,
                      CONFIG_TREC& config,
                      Xapian::WritableDatabase& db,
                      Xapian::TermGenerator& indexer)
{
    struct stat statbuf;
    if (stat(file.c_str(), &statbuf) == -1) {
        cout << "Can't stat \"" << file << "\" - skipping\n";
        return;
    }

    if (S_ISDIR(statbuf.st_mode)) {
        // file is a directory
        try {
            index_directory(file, config, db, indexer);
        }
        catch (...) {
            cout << "Caught unknown exception in index_directory, rethrowing" << endl;
            throw;
        }
        return;
    }

    if (S_ISREG(statbuf.st_mode)) {
        // file is a regular indexable text file
        string ext;
        string::size_type dot = file.find_last_of('.');
        if (dot != string::npos) ext = file.substr(dot + 1);

        index_file(file, config, db, indexer);
        return;
    }

    cout << "Not a regular file \"" << file << "\" - skipping\n";
}

static void
index_directory(const string& dir,
                CONFIG_TREC& config,
                Xapian::WritableDatabase& db,
                Xapian::TermGenerator& indexer)
{
    DIR *d;
    struct dirent *ent;
    string path = dir;

    //cout << "[Entering directory " << dir << "]" << endl;

    d = opendir(path.c_str());
    if (d == NULL) {
	cout << "Can't open directory \"" << path << "\" - skipping\n";
	return;
    }
    while ((ent = readdir(d)) != NULL) {
	// ".", "..", and other hidden files
	if (ent->d_name[0] == '.') continue;
	string file = dir;
	if (!file.empty() && file[file.size() - 1] != '/') file += '/';
	file += ent->d_name;
	index_directory_entry(file, config, db, indexer);
    }

    closedir(d);
}

int main(int argc, char **argv)
{

    // check for proper usage of program
    if(argc < 2) {
        cout << "usage: " << argv[0] << " <config file>" << endl;
        exit(1);
    } // END if

    CONFIG_TREC trec_config;
		trec_config.setup_config( string(argv[1]) );

    if( !trec_config.check_index_config() ) {
      cout << "ERROR - configure file invalid, pls check" << endl;
      exit(1);
    }

    // Catch any Xapian::Error exceptions thrown
    try {
        Xapian::TermGenerator indexer;
        Xapian::Stem stemmer(trec_config.get_language());
        indexer.set_stemmer(stemmer);
        ifstream words(trec_config.get_stopsfile().c_str());
        Xapian::SimpleStopper stopper{istream_iterator<string>(words),
                                      istream_iterator<string>()};
        indexer.set_stopper(&stopper);
        if (trec_config.get_indexbigram()) {
            //indexer.set_bigrams(true);
        }

        // Make the database
        Xapian::WritableDatabase db(trec_config.get_db().c_str(), Xapian::DB_CREATE_OR_OPEN);

        struct timeval start_time, finish_time, timelapse;   /* timing variables */

        // start the timer
        gettimeofday( &start_time, 0 );

        // index the text collection
        index_directory_entry(trec_config.get_textfile(),
                              trec_config,
                              db,
                              indexer);
        db.commit();

        // start the timer
        gettimeofday( &finish_time, 0 );

        // print the total time, and average time per query -
        //diff_time( finish_time, start_time, &timelapse );
        //cout << "Total time for " << totaldocs << " documents is " << time_real( timelapse ) << " secs, text size = " << ttextsize
        //		 << " mb" << endl;
        cout << "Total number of documents in the database is now " << db.get_doccount() << " docs" << endl;

    } catch (const Xapian::Error &e) {
	cout << "Exception: " << e.get_msg() << endl;
	return 1;
    } catch (const string &s) {
	cout << "Exception: " << s << endl;
	return 1;
    } catch (const char *s) {
	cout << "Exception: " << s << endl;
	return 1;
    } catch (...) {
	cout << "Caught unknown exception" << endl;
	return 1;
    } // END catch

} // END main
