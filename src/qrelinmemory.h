/* @file qrelinmemory.h
 * @abstract Header file of class which store Qrel file In memory 
 *	         for access during evaluation of resultfile.
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
 */

#ifndef _QRELINMEMORY_H_
#define _QRELINMEMORY_H_

#include <set>
#include <map>
#include <string>
using namespace std;

class QRelInMemory {

public:

	//Query identifier for which this class contains relevance judgement.
	string queryid;
	
	//All the relevant grade available for this query in assessment pool.
	set<int> relGrade;
	
	//Document identifier of all the document which are Non-relevant for the Query
	set<string> nonRelDocuments;

	//Document identifier of all the relevant document categorised by the grade.
	map<int,set<string> > relGradeDocMap;
    
    //Iterator for the docs.
    map<int,set<string> >::iterator docs; 

	//Constructor of class.
	QRelInMemory(const string &queryid_) :queryid(queryid_) {
	}

	/**
	 *	Check if the document is relevant for the associated
	 *  Query.
	 *  @param docno Document identifier to be checked for relevance
	 * @return true if document is relevant,otherwise false.
	*/
	bool isRelevant(const string & docno) const;

	/**
	 * Get the grades for the document.
	 * @param docno Document identifier to return grade for.
	 * @return int return grade if grade available in assessment or default grade 0.
	 */
	int getGrade(const string & docno);
	
	/** 
	 * Get all the relevant document for this query from relevance assessment
	 * @return Set of Document ids of all the relevant docids for the query.
	*/
	
	set<string> getAllRelevantDocument() const;
	
	/** 
	 * Get all the relevant document for this query from relevance assessment with a particular grade
	 * @param grade grade for which relevance assessment to return
	 * @return Set of Document ids of all the relevant docids for the query for a particular grade.
	*/

	set<string> getRelevantDocument(const int & grade) const;

	/** 
	 * Add identifier of the relevant document to the set
	 * @param docno Document identifier to be added to relevant set.
	 * @param grade grade of the document to be added.
	 * @return true if document is added.
	 */
	
	bool insertRelDocument(const string &docno,const int & grade);

	/** 
	 * Add identifier of the non relevant document to the set
	 * @param docno Document identifier to be added to non relevant set.
	 * @return true if document is added.
	 */
	
	bool insertNonRelDocument(const string &docno);
};

#endif

