# Makefile for Ad-hoc batch TREC index and search

HELPERFILES = config_file.cc htmlparse.cc stopword.cc split.cc qrelinmemory.cc trec_qrel.cc timer.c
HELPERHEADERS = config_file.h htmlparse.h stopword.h split.h timer.h timerstruct.h qrelinmemory.h trec_qrel.h trec_eval.h trec_adhoceval.h
GUNZIPPERFILES = crypt.c inflate.c unlzw.c unzip.c gzip.c unlzh.c unpack.c util.c

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = trec_index trec_search trec_query trec_adhoceval
AM_LDFLAGS = $(XAPIAN_LIBS)
AM_CPPFLAGS = -I$(srcdir)/../lib
AM_CXXFLAGS = $(XAPIAN_CXXFLAGS)
include_HEADERS = $(HELPERHEADERS)

LIBTOOL = @LIBTOOL@ $(QUIET:y=--quiet)

trec_search_SOURCES = 	trec_search.cc $(HELPERFILES)
			
trec_index_SOURCES = trec_index.cc $(HELPERFILES) $(GUNZIPPERFILES)


trec_query_SOURCES = trec_query.cc $(HELPERFILES)

trec_adhoceval_SOURCES = trec_adhoceval.cc $(HELPERFILES)

clean :
	rm -rf *.o  xapian-trec index-xapian-trec trec_index trec_search trec_query trec_adhoceval
