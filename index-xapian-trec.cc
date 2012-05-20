/*
**  index-xapian-trec.cc
**
**  AM 6/12/2006 
**  Internal use only: for xapian paper experiments
**
*/

#include <xapian.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "htmlparse.h"
#include "P98_gzip.h"  
#include "stopword.h"
#include "indextext.h"

using namespace Xapian;
using namespace std;

/* #define MAXPATH 100    maximum pathlength */
#define NO_OF_FILES 0  /* no of file to process in every directory */
#define START_FROM 0    /* which file to start from */
#define SW_FILE "stop.words"
#define LINE_SIZE 80       /* maximum size of a line in a document */
#define MAXPATH 100

/* chamber (from hashbld) is where the input bundles are decompressed.*/
#define CHAMBER_SIZE 10000000
char chamber[CHAMBER_SIZE]; 

class TrecHtmlParser : public HtmlParser {
    public:
	bool in_script_tag;
	bool in_style_tag;
    	string docno, sample, keywords, dump;
	bool indexing_allowed;
	void process_text(const string &text);
	void opening_tag(const string &tag, const map<string,string> &p);
	void closing_tag(const string &tag);
	TrecHtmlParser() :
		in_script_tag(false),
		in_style_tag(false),
		indexing_allowed(true) { }
};

void
TrecHtmlParser::process_text(const string &text)
{
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
TrecHtmlParser::opening_tag(const string &tag, const map<string,string> &p)
{
#if 0
    cout << "<" << tag;
    map<string, string>::const_iterator x;
    for (x = p.begin(); x != p.end(); x++) {
	cout << " " << x->first << "=\"" << x->second << "\"";
    }
    cout << ">\n";
#endif
    
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
			indexing_allowed = false;
			throw true;
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
TrecHtmlParser::closing_tag(const string &tag)
{
    if (tag == "docno") {
      docno = dump;
      dump = "";
    } else if (tag == "script") {
      in_script_tag = false;
    } else if (tag == "style") {
      in_style_tag = false;
    } else if (tag == "body") {
      throw true;
    }
}

inline static bool
p_notalnum(unsigned int c)
{
    return !isalnum(c);
}

inline static bool
p_plusminus(unsigned int c)
{
    return c == '+' || c == '-';
}

void index_text(const string &s, Xapian::Document &doc )
{
 
    AccentNormalisingItor j(s.begin());
    const AccentNormalisingItor s_end(s.end());
    while (true) {
	AccentNormalisingItor first = j;
	while (first != s_end && !isalnum(*first)) ++first;
	if (first == s_end) break;
	AccentNormalisingItor last;
	string term;
	if (isupper(*first)) {
	    j = first;
	    term = *j;
	    while (++j != s_end && *j == '.' && ++j != s_end && isupper(*j)) {
		term += *j;
	    } 
	    if (term.length() < 2 || (j != s_end && isalnum(*j))) {
		term = "";
	    }
	    last = j;
	}
	if (term.empty()) {
	    j = first;
	    while (isalnum(*j)) {
		term += *j;
		++j;
		if (j == s_end) break;
		if (*j == '&') {
		    AccentNormalisingItor next = j;
		    ++next;
		    if (next == s_end || !isalnum(*next)) break;
		    term += '&';
		    j = next;
		}
	    }
	    string::size_type len = term.length();
	    last = j;
	    while (j != s_end && p_plusminus(*j)) {
		term += *j;
		++j;
	    }
	    if (j != s_end && isalnum(*j)) {
		term.resize(len);
	    } else {
		last = j;
	    }
	}
	if (term.length() <= MAX_PROB_TERM_LENGTH) {
	    lowercase_term(term);
	    doc.add_term( term );
 
	}
    }

} // END index_text

string get_document( string & text, int & curpos, int uncolen ) {

  string enddoc = "</DOC>";
  char *end = strstr( &chamber[curpos], enddoc.c_str()  );
  string document;
  for (int i=0; i < uncolen && &chamber[i] != end; i++)
    document += chamber[i];
  document += "</DOC>";
  curpos += document.length();
  
  return document;

} // get_document

Xapian::Document  remove_stopwords( Xapian::Document doc, SW_STORE & sw_store ) {
// take a list of keywords and remove 

  Xapian::Document wordlist;
  char word[100];
	
  for( TermIterator t = doc.termlist_begin(); t != doc.termlist_end();  t++ ) {
    for( int i=0; i < (*t).size(); i++ ) word[i] = (*t)[i];
    if(!IsStopWord( sw_store, word )) wordlist.add_term( *t );
    
  } // END for

  return wordlist;

} // END remove_stopwords

Xapian::Document stem_document( Xapian::Document & doc ) {

  Stem stemmer("english");
  Xapian::Document wordlist;

  for( TermIterator t = doc.termlist_begin(); t != doc.termlist_end();  t++ ) {
    wordlist.add_term(stemmer.stem_word(*t) );
    
  } // END for

  return wordlist;


} // END stem_document 


/**********************************************************************/
/*                Process one compressed bundle                       */
/**********************************************************************/

static int processfile(string fp, Xapian::WritableDatabase &db, SW_STORE & sw_store ) { 
  /* A file believed to be a compressed doc bundle has been encountered.  It's 
     full path is fp. It is to be decompressed and processed as requested. */
  
  int uncolen; 
  int pointer=0;
  u_char filen[100];

  // uncompress the file 
  cout << "Processing" << fp <<  "\n"; 
  for( int i=0; i < fp.size(); i++ ) filen[i] = fp[i];
  uncolen = decompress_bundle(filen, (u_char *) chamber, CHAMBER_SIZE);

  // index the file in Xapian 
  do {
    
    Xapian::Document newdoc;

    // parse the document 
    string text = get_document( text, pointer, uncolen );
    TrecHtmlParser p;
    p.parse_html(text);
 
    // index the document 
    index_text(p.keywords, newdoc );
    Xapian::Document doc_stopsremoved = remove_stopwords( newdoc, sw_store );
    Xapian::Document stemdoc = stem_document( doc_stopsremoved ); 
    stemdoc.set_data(p.docno);  // set the data 
    db.add_document(stemdoc);

  } while( pointer < uncolen );


} /* END processfile */

static void index_directory( const string &dir, Xapian::WritableDatabase & db,
			     SW_STORE sw_store )
{
    DIR *d;
    struct dirent *ent;
    string path = dir;

    cout << "[Entering directory " << dir << "]" << endl;

    d = opendir(path.c_str());
    if (d == NULL) {
	cout << "Can't open directory \"" << path << "\" - skipping\n";
	return;
    }
    while ((ent = readdir(d)) != NULL) {
	struct stat statbuf;
	// ".", "..", and other hidden files
	if (ent->d_name[0] == '.') continue;
	string file = dir;
	if (!file.empty() && file[file.size() - 1] != '/') file += '/';
	file += ent->d_name;
	if (stat(file.c_str(), &statbuf) == -1) {
	    cout << "Can't stat \"" << file << "\" - skipping\n";
	    continue;
	} // END if

	if (S_ISDIR(statbuf.st_mode)) {
	    // file is a directory
	    try {
		index_directory( file,  db, sw_store );
	    }
	    catch (...) {
		cout << "Caught unknown exception in index_directory, rethrowing" << endl;
		throw;
	    }
	    continue;
	} // END if

	if (S_ISREG(statbuf.st_mode)) {
	    // file is a regular indexable text file
	    string ext;
	    string::size_type dot = file.find_last_of('.');
	    if (dot != string::npos) ext = file.substr(dot + 1);

	    processfile( file, db, sw_store );
	    continue;
	} // END if

	cout << "Not a regular file \"" << file << "\" - skipping\n";
    }
    closedir(d);

} // END index_directory


int main (int argc, char *argv[]) {

  // There must be four command line arguments passed in the order:
  // database_name, query file name, results file name, db type, run type,
  if (argc < 2) {
    cerr << "ERROR: Insufficient arguments passed to program\n";
    cerr << "USAGE: index-xapian-trec <database> \n";
    exit(0);
  } // END if
 
  // Catch any Error exceptions thrown
  try {

    /* load the stopword list */
    SW_STORE sw_store;
    Read_SW_File( SW_FILE, &sw_store );

    /* set up xapian indexing */
    Xapian::WritableDatabase db(Xapian::Flint::open(argv[1], Xapian::DB_CREATE_OR_OPEN));

    /* scan the directories/files and put them in an index */
    index_directory( argv[1],  db, sw_store );

  } catch (const Error &error) {
    cout << "Exception: "  << error.get_msg() << endl;
    exit(1);
  } // END try/catch

} // END main

