/* @file trec_qrel.cc
 * @abstract Implementation for TREC Relevance Judgement InMemory.
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

#include "trec_qrel.h"
#include "split.h"
#include "qrelinmemory.h"

using namespace std;

set<string> 
TrecQrel::getQueryIds() {
	set<string> queryids;
	for ( int i = 0;i < getNumberofQueries();i++) {
		queryids.insert(qrelPerQuery[i].queryid);
	}
	return queryids;
}


set<string> 
TrecQrel::getRelevantDocument(int grade,string queryid) {
	for (int i = 0; i < getNumberofQueries(); i++) {
		if ( qrelPerQuery[i].queryid.compare(queryid) == 0) {
		return qrelPerQuery[i].getRelevantDocument(grade);
		}
	}
}


set<string> 
TrecQrel::getAllRelevantDocument() {
	set<string> reldocuments;
	for ( int i = 0 ;i <  getNumberofQueries(); i++) {
		set<string> relcurrquery = qrelPerQuery[i].getAllRelevantDocument();
		reldocuments.insert(relcurrquery.begin(),relcurrquery.end());
	}
	return reldocuments;
}


set<string> 
TrecQrel::getRelevantDocument(string queryid) {
	for ( int i = 0 ;i <  getNumberofQueries(); i++) {
		if ( qrelPerQuery[i].queryid.compare(queryid) == 0 ) {
		return qrelPerQuery[i].getAllRelevantDocument();
		}
	}
	set<string> rel ;
	return rel;
}

int 
TrecQrel::getNumberofRelevant(string queryid) {
	for ( int i = 0 ;i <  getNumberofQueries(); i++) {
		if ( qrelPerQuery[i].queryid.compare(queryid) == 0) {
		return qrelPerQuery[i].getAllRelevantDocument().size();
		}
	}
	return 0;

}

int 
TrecQrel::getNumberofQueries() {
	return qrelPerQuery.size();
}

void 
TrecQrel::loadQRelFile() {
	string line;
	if ( !fqrels.eof())
	getline(fqrels,line);
	vector<string> data;
	split(line,' ',data);
	int currentquery = atoi(data[0].c_str());
    std::stringstream inttostring;
	inttostring << currentquery;
	QRelInMemory * qrelquery = new QRelInMemory(inttostring.str());
	while ( !fqrels.eof() ) {
	if( currentquery != atoi(data[0].c_str()) ) {
		qrelPerQuery.push_back(*qrelquery);
//		cout<< "New Query Found in Qrel FIle :\t"<<data[0]<<endl;
		currentquery = atoi(data[0].c_str());
    	std::stringstream itostring;
	    itostring << currentquery;
		qrelquery = NULL;
		qrelquery = new QRelInMemory(itostring.str());
	}
//	cout<<"Document and status :\t"<<data[2] <<"\tRelevance Status:\t"<<data[3]<<endl;
	int grade = atoi(data[3].c_str());
	if ( grade != 0) {
		qrelquery->insertRelDocument(data[2],grade);
	}
	else {
		qrelquery->insertNonRelDocument(data[2]);
	}
	getline(fqrels,line);
	split(line,' ',data);
	}
	qrelPerQuery.push_back(*qrelquery);
}

bool
TrecQrel::existInQrel(string queryid) {
	for (int i = 0;i < getNumberofQueries();i++) {
		if (qrelPerQuery[i].queryid.compare(queryid) == 0) {
		return true;
		}
	}
	return false;
}

bool
TrecQrel::isRelevantDoc(string docno,string queryid) {
	for (int i = 0;i < getNumberofQueries();i++) {
		if (qrelPerQuery[i].queryid.compare(queryid) == 0) {
		//cout<<"query in relevant:\t"<<qrelPerQuery[i].queryid<<endl;
		return qrelPerQuery[i].isRelevant(docno);
		}
	}
	return false;
}

int 
TrecQrel::getGrade(string docno,string queryid) {
	for (int i = 0;i < getNumberofQueries();i++) {
		if (qrelPerQuery[i].queryid.compare(queryid) == 0) {
		return qrelPerQuery[i].getGrade(docno);
		}
	}
	return 0;
}
/*
int main(int argc,char **argv)
{
CONFIG_TREC config;
config.setup_config(string(argv[1]));
TrecQrel *qrl =new  TrecQrel(config);
set<string> rel = qrl->getRelevantDocument(string(argv[2]));
cout<< "Queryid"<<string(argv[2])<<endl;
for ( set<string>::iterator docs = rel.begin() ;docs != rel.end() ; docs++) {
	cout<<"Relevant Document:\t"<<*docs<<endl;
}
//cout<<"IS Relevan"<<qrl->isRelevantDoc(argv[2],argv[3]) <<"Grade"<<qrl->getGrade(argv[2],argv[3])<<endl;
}
*/
