/* @file trec_qrel.h
 * @abstract Header file for TREC Relevance Judgement InMemory.
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 19  */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include "config_file.h"
#include <set>
#include <vector>
#include "qrelinmemory.h"
#include <sstream>

using namespace std;

class TrecQrel {

protected:
	ifstream fqrels;

public:
	
	vector<QRelInMemory> qrelPerQuery;

	int totalNumberofRelevantDocs;
	
	TrecQrel(CONFIG_TREC & config) {
	string reljudgement = config.get_relfile().c_str();
	fqrels.open(reljudgement.c_str(), ifstream::in);
	if(!fqrels.is_open()) {
		cout<<"ERROR - can't open relevance judgement file" << config.get_relfile().c_str() << "for reading" << endl;
		exit(-1);
	}
	loadQRelFile();
	}
	
	/*  To get Query ids of all the queries present in relevance judgement pool.
 	 *  @return query ids of queries in pool 
	*/
	set<string> getQueryIds();

    /* Get the relevant document ids for a Query  and of particular grade
     *	@param grade - Integer grade for which  relevant document need to be returned.
     *	@param queryid - Queryid of query for which the documents ned to be returned.
	 *  @return - set of the relevant document for query queryid and grade grade
     */

	set<string> getRelevantDocument(int grade,const string & queryid);
	
	/* Get all the relevant document ids 
	 *  @return - set of all the relevant document 
     */

	set<string> getAllRelevantDocument();
    
	/* Get the relevant document ids for a given Query 
     *	@param queryid - Queryid of query for which the documents ned to be returned.
	 *  @return - set of the relevant document for query queryid from all grades
     */
	
	set<string> getRelevantDocument(const string & queryid);

	/* Get number of relevant document for the query.
	 * @param queryid - Queryid of query for which number of relevant documents to be returned.
	 * @return - Number of relevant documents for the query.
	 */

	int getNumberofRelevant(const string & queryid);

	/* Get total number of queries in the pool.
	 * @return Number of  query in the pool.
	 */
	
	int getNumberofQueries();
	
	//Load the QRel file in the InMemory Datastructures.

	void loadQRelFile();	

	/**
	 * Check whether Query with given identifier exist in relvance assessment
	 * @param queryid - identifier of the Query.
	 * @return true if the query exist in assessment pool,false otherwise.
	 */
	
	bool existInQrel(const string & queryid);
	
	/**
	 * Checks if the document is relvant or not for the query given by the query identifier.
	 * @param queryid - Identifier for the query.
	 * @param docno - Identifier for the document to be checked for relevant or not.
	 * @return true if document is relevant for the given query.
	 */
	
	 bool isRelevantDoc(const string & docno,const string & queryid);

	/**
	 * Get grade of the document for the particular query.
	 * @param queryid - Identifier for the query.
	 * @param docno - Identifier for the document to be checked for grade.
	 * @return int grade of the document for query if available in pool,else default grade 0.
	 */

 	int getGrade(const string & docno,const string & queryid);
};
