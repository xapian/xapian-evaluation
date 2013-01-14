/* @file qrelinmemory.cc
 * @abstract Implementation file of class which store Qrel file In memory 
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

#include "qrelinmemory.h"
#include <iostream>
#include "config_file.h"

using namespace std;

bool
QRelInMemory::isRelevant(const string & docno) const {
	set<int>::iterator grades;
	
	for (grades=relGrade.begin() ;grades != relGrade.end() ;grades++ ) {
		if ( relGradeDocMap.find(*grades) != relGradeDocMap.end()) {
			if(relGradeDocMap.find(*grades)->second.find(docno) != relGradeDocMap.find(*grades)->second.end())
			return true;
		}
	}
return false;
}

int 
QRelInMemory::getGrade(const string & docno) { 
	set<int>::iterator grades;
	
	for (grades=relGrade.begin() ;grades != relGrade.end() ;grades++ ) {
	    docs = relGradeDocMap.find(*grades);
		if (docs != relGradeDocMap.end()) {
			if(docs->second.find(docno) != docs->second.end()) {
			return *grades;
			}
		}
	}
return 0;
}

set<string> 
QRelInMemory::getAllRelevantDocument() const {
	set<int>::iterator grades;
	set<string> reldocs;	
	for (grades=relGrade.begin() ;grades != relGrade.end() ;grades++ ) {	
			set<string> docs = relGradeDocMap.find(*grades)->second;
			reldocs.insert(docs.begin(),docs.end());
	}
	return reldocs;
}

set<string> 
QRelInMemory::getRelevantDocument(const int & grade) const {
	return relGradeDocMap.find(grade)->second;
}

bool 
QRelInMemory::insertRelDocument(const string & docno,const int & grade) {
	if (relGrade.find(grade) != relGrade.end()) {
	map<int,set<string> >::iterator docs =	relGradeDocMap.find(grade);
	docs->second.insert(docno);
	} else {
	relGrade.insert(grade);
    set<string> reldocs;
	reldocs.insert(docno);
	relGradeDocMap[grade] = reldocs;
	}
	return true;
}

bool 
QRelInMemory::insertNonRelDocument(const string & docno) {
	nonRelDocuments.insert(docno);
    return true;
}
/*
int main() {
QRelInMemory qrel = QRelInMemory("1");
qrel.insertRelDocument("mydoc",1);
qrel.insertRelDocument("mydoc3",1);
qrel.insertRelDocument("mydoc2",5);
set<string> rel = qrel.getRelevantDocument(1);
set<string>::iterator  it;
for(it = rel.begin();it != rel.end();it++)
{
	cout<<"RelAssessment\t"<<*it<<endl;
}
cout<<"mydoc grade:\t"<< qrel.getGrade("mydoc")<<endl;
cout<<"mydoc grade:\t"<< qrel.getGrade("mydoc2")<<endl;
cout<<"mydoc grade:\t"<< qrel.getGrade("mydoc3")<<endl;
cout<<"mydoc relevance:\t"<< qrel.isRelevant("mydoc3")<<endl;
cout<<"mydoc relevance:\t"<< qrel.isRelevant("mydoc6")<<endl;
}
*/
