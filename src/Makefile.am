# Makefile for Ad-hoc batch TREC index and search

noinst_HEADERS = config_file.h htmlparse.h indextext.h P98_gzip.h qrelinmemory.h split.h timer.h timerstruct.h trec_adhoceval.h trec_eval.h trec_qrel.h
common_sources = config_file.cc htmlparse.cc qrelinmemory.cc split.cc trec_qrel.cc timer.c

bin_PROGRAMS = trec_adhoceval trec_index trec_query trec_search
LDADD = $(XAPIAN_LIBS)
AM_CXXFLAGS = $(XAPIAN_CXXFLAGS)

trec_adhoceval_SOURCES = trec_adhoceval.cc $(common_sources)

trec_index_SOURCES = trec_index.cc $(common_sources)

trec_index_LDADD = $(LDADD) -lz

trec_query_SOURCES = trec_query.cc $(common_sources)

trec_search_SOURCES = trec_search.cc $(common_sources)
