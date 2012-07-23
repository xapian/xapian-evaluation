# Makefile for Ad-hoc batch TREC index and search

OPTIMISE = -O2
DEBUG = -g -Wall -W
GCC = g++

XAPIAN_CONFIG = /usr/local/bin/xapian-config-1.3
XAPIAN_LIB = `$(XAPIAN_CONFIG) --libs`
CXXFLAGS = `$(XAPIAN_CONFIG) --cxxflags`
XAPIAN_INC = /usr/local/include/xapian-1.3/
INC = /usr/local/include/xapian-1.3/
LIBS = config_file.o htmlparse.o stopword.o split.o timer.o qrelinmemory.o trec_qrel.o
HEADERS = config_file.h htmlparse.h stopword.h split.h timer.h timerstruct.h qrelinmemory.h trec_qrel.h trec_eval.h trec_adhoceval.h

all : trec_index trec_search trec_query trec_adhoceval

timer.o : timer.c 
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(INC) timer.c -o timer.o

config_file.o : config_file.cc
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(INC) config_file.cc -o config_file.o

split.o : split.cc
	 $(GCC) -c $(OPTIMISE) $(DEBUG) -I$(INC) split.cc -o split.o

htmlparse.o : htmlparse.cc
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(INC) htmlparse.cc -o htmlparse.o

stopword.o : stopword.cc
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(INC) stopword.cc -o stopword.o

qrelinmemory.o : qrelinmemory.cc
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(INC) qrelinmemory.cc -o qrelinmemory.o

trec_qrel.o : trec_qrel.cc $(HEADERS) 
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(INC) trec_qrel.cc -o qrelinmemory.o

gunzipper.o:
	$(MAKE) -f Makefile.gzip gunzipper

trec_search : trec_search.cc $(LIBS) $(HEADERS)
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(XAPIAN_INC) -I$(INC) trec_search.cc -o trec_search.o
	$(GCC) $(OPTIMISE) $(DEBUG) $(LIBS) trec_search.o $(XAPIAN_LIB) -o trec_search

trec_index : trec_index.cc $(LIBS) $(HEADERS) gunzipper.o
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(XAPIAN_INC) -I$(INC) trec_index.cc -o trec_index.o
	$(GCC) $(OPTIMISE) $(DEBUG) $(LIBS) trec_index.o gunzipper.o $(XAPIAN_LIB) -o trec_index

trec_query : trec_query.cc $(LIBS) $(HEADERS)
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(XAPIAN_INC) -I$(INC) trec_query.cc -o trec_query.o
	$(GCC) $(OPTIMISE) $(DEBUG) $(LIBS) trec_query.o $(XAPIAN_LIB) -o trec_query

trec_adhoceval : trec_adhoceval.cc $(LIBS) $(HEADERS)
	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(XAPIAN_INC) -I$(INC) trec_adhoceval.cc -o trec_adhoceval.o
	$(GCC) $(OPTIMISE) $(DEBUG) $(LIBS) trec_adhoceval.o $(XAPIAN_LIB) -o trec_adhoceval

#index-xapian-trec : index-xapian-trec.cc $(LIBS) $(HEADERS)
#	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(XAPIAN_INC) -I$(INC) index-xapian-trec.cc -o index-xapian-trec.o
#	$(GCC) $(OPTIMISE) $(DEBUG) $(LIBS) index-xapian-trec.o $(XAPIAN_LIB) -o index-xapian-trec

#xapian-trec : xapian-trec.cc $(LIBS) $(HEADERS)
#	$(GCC) -c $(OPTIMISE) $(DEBUG) -I$(XAPIAN_INC) -I$(INC) xapian-trec.cc -o xapian-trec.o
#	$(GCC) $(OPTIMISE) $(DEBUG) $(LIBS) xapian-trec.o $(XAPIAN_LIB) -o xapian-trec

clean :
	rm -rf *.o  xapian-trec index-xapian-trec trec_index trec_search trec_query trec_adhoceval
