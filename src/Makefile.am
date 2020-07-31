# Makefile for Ad-hoc batch TREC index and search

HELPERFILES = config_file.cc htmlparse.cc split.cc qrelinmemory.cc trec_qrel.cc timer.c
HELPERHEADERS = config_file.h htmlparse.h indextext.h split.h symboltab.h timer.h timerstruct.h P98_gzip.h qrelinmemory.h trec_qrel.h trec_eval.h trec_adhoceval.h

bin_PROGRAMS = $(top_builddir)/bin/index-xapian-trec $(top_builddir)/bin/trec_index $(top_builddir)/bin/trec_search $(top_builddir)/bin/trec_query $(top_builddir)/bin/trec_adhoceval
LDADD = $(XAPIAN_LIBS)
AM_CXXFLAGS = $(XAPIAN_CXXFLAGS)
include_HEADERS = $(HELPERHEADERS)

__top_builddir__bin_index_xapian_trec_SOURCES = index-xapian-trec.cc $(HELPERFILES)

__top_builddir__bin_index_xapian_trec_LDADD = $(LDADD) -lz

__top_builddir__bin_trec_search_SOURCES = 	trec_search.cc $(HELPERFILES)

__top_builddir__bin_trec_index_SOURCES = trec_index.cc $(HELPERFILES)

__top_builddir__bin_trec_index_LDADD = $(LDADD) -lz

__top_builddir__bin_trec_query_SOURCES = trec_query.cc $(HELPERFILES)

__top_builddir__bin_trec_adhoceval_SOURCES = trec_adhoceval.cc $(HELPERFILES)

clean :
	rm -rf *.o  xapian-trec index-xapian-trec trec_index trec_search trec_query trec_adhoceval
