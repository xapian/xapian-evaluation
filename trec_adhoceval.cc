/* @file trec_adhoceval.cc
 * @abstract Implementation file for TREC Evaluation which read result file and evaluate for Adhoc Retrieval.
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
#include "trec_adhoceval.h"
#include <iostream>
#include <fstream>
#include "split.h"

using namespace std;

static const int PRECISION_RANK[] = {1,2,3,4,5,10,15,20,30,50,100,200,500,1000};
static const int PRECISION_PERCENTAGES[] = {0,10,20,30,40,50,60,70,80,90,100};

void 
AdhocEvaluation::intialise() {

	maxNumberRetrieved = configobj.get_noresults();
	precisionAtRank.clear();
	precisionAtRecall.clear();
	numberofEffQuery = 0;
	totalNumberofRetrieved = 0;
	totalNumberofRelevant = 0;
	totalNumberofRelevantRetrieved = 0;
	meanAveragePrecision = 0; 
}


void
AdhocEvaluation::evaluate() {
intialise();
int effQueryCounter = 0;
set<int>  numberofRelevantRetrived ;
set<int> numberofRelevant  ;
set<int>  numberofRetrived ;

vector<vector<Record> > listofRetrieved;
vector<vector<Record> > listofRelevantRetrieved;
vector<int> vecNumberofRelevant;
vector<int> vecNumberofRetrieved;
vector<int> vecNumberofRelevantRetrieved;
vector<string> vecQueryNo;
ifstream resultfile;
string resultfilename = configobj.get_resultsfile();
//string resultfilename = "/home/gaurav/Work/xapiantrec/testresult1";
//cout<<resultfilename<<endl;
resultfile.open(resultfilename.c_str(),ifstream::in);
if(!resultfile.is_open()) {
	cout<<"ERROR - can't open the result file"<<configobj.get_resultsfile().c_str() << "for reading"<<endl;
}
string str;
string previous = "";
int numberofRetrievedCounter = 0;
int numberofRelevantRetrievedCounter = 0;
vector<Record> *relevantRetrieved = new vector<Record>();
vector<Record> *retrieved = new vector<Record>();
while( resultfile.good() && !resultfile.eof() ) {
getline(resultfile,str);
vector<string> data;
split(str,' ',data);
if (data.size() == 7) {
	//cout<<" data0 "<<data[0]<<" data1 "<<data[1]<<" data2 "<<data[2]<<" data3 "<<data[3]<<" data4 "<<data[4]<<" data5 "<<data[5]<<" data6 "<<data[6]<<" data7 "<<data.size()<<endl;
	//Query id for the Query.
	int query = atoi(data[0].c_str());
	std::stringstream inttostring;
	inttostring << query;
	string queryid = inttostring.str();
	
	if(!qrel->existInQrel(queryid))
		continue;
	string docId = data[2];
	int rank = atoi(data[4].c_str());
//	cout<<"Rank"<<rank<<data[3]<<endl;
	/** If Queryid have changed save stats to the final vectors
	 * which store statistics of all read queries*/
	if(previous.compare(queryid) != 0) {
		if(effQueryCounter != 0) {
			vecNumberofRetrieved.push_back(numberofRetrievedCounter);
			vecNumberofRelevantRetrieved.push_back(numberofRelevantRetrievedCounter);
		listofRetrieved.push_back(*retrieved);
		listofRelevantRetrieved.push_back(*relevantRetrieved);
		numberofRetrievedCounter = 0;
		numberofRelevantRetrievedCounter = 0;
	cout<<"Insert QueryID:\t"<<queryid<<"\twith number of Relevant Docs:\t"<<qrel->getNumberofRelevant(queryid)<<"\tTotalRetreived\t"<<retrieved->size()<<"\tTotal relevant retreived\t"<<relevantRetrieved->size()<<endl;
		retrieved = new vector<Record>();
		relevantRetrieved = new vector<Record>();
		}
	effQueryCounter++;
	vecQueryNo.push_back(queryid);
	vecNumberofRelevant.push_back(qrel->getNumberofRelevant(queryid));
	}		
	previous = queryid;
	numberofRetrievedCounter++;
	totalNumberofRetrieved++;
	Record *currrec = new Record(queryid,docId,rank);
	retrieved->push_back(*currrec);	
	//adding relevant document to the relevant retreived set
	if (qrel->isRelevantDoc(docId,queryid)) {
		relevantRetrieved->push_back(*currrec);
	numberofRelevantRetrievedCounter++;
	}

	} //end of data checking if condition	
}
	vecNumberofRetrieved.push_back(numberofRetrievedCounter);
	vecNumberofRelevantRetrieved.push_back(numberofRelevantRetrievedCounter);
	listofRetrieved.push_back(*retrieved);
	listofRelevantRetrieved.push_back(*relevantRetrieved);
	resultfile.close();
	queryNo.swap(vecQueryNo);
	numberofEffQuery = effQueryCounter;
	vector<vector<Record> >::iterator it ;
	vector<Record>::iterator recorditr;
	int totalQuery = listofRelevantRetrieved.size();
	//calculating statistics of number of relevent ,retreived 
	for (int itr = 0;itr < numberofEffQuery ;itr++) {
		totalNumberofRetrieved += vecNumberofRetrieved[itr];
		totalNumberofRelevant  += vecNumberofRelevant[itr];
		totalNumberofRelevantRetrieved += vecNumberofRelevantRetrieved[itr];
	}
	//Variable to store precision by rank and recall.
	map<int,double> *precisionAtRankByQuery;
	map<int,double> *precisionAtRecallByQuery;
		precisionAtRankByQuery = new map<int,double>[numberofEffQuery];
		precisionAtRecallByQuery = new map<int,double>[numberofEffQuery];

	//Store the exact precision for all queries.
	double * ExactPrecision = new double[totalQuery];
	// Store the R(Relevance precision for all queries.
	double * RPrecision = new double[totalQuery];
	int currentQuery = 0;
	//This forloop is equivalent to iterating Queries.
	for (it = listofRelevantRetrieved.begin();it != listofRelevantRetrieved.end();it++) {
		vector<Record> recordvec = *it;
	    int docrank = 0;
		ExactPrecision[currentQuery] = 0;
		RPrecision[currentQuery] = 0;
		int sizeofvector  = recordvec.size();
		// Iterating the ranklist(Record List of the Queries.
		for (recorditr = recordvec.begin();recorditr != recordvec.end();recorditr++) {
			Record rec = *recorditr;
			/** Incrementing Relevance Precision if relevant document 
			is found at rank smaller than number of relevant document.
			*/
			if (rec.getRank() < vecNumberofRelevant[currentQuery]) {
				RPrecision[currentQuery] += 1;
			}
			//Adding to the sum of precision value if a relevant document is found for query.
			ExactPrecision[currentQuery]  += (double)(docrank+1)/(double)(rec.getRank()+1);
			rec.precision  += (double)(docrank+1)/(double)(rec.getRank()+1);
			rec.recall += (double) (docrank+1) / vecNumberofRelevant[currentQuery];
		
			for ( int precisionRank = 0;precisionRank < 14;precisionRank++) {
				if (rec.getRank()  < PRECISION_RANK[precisionRank]) {
					map<int,double> precisioncurrent = precisionAtRankByQuery[currentQuery];
					map<int,double>::iterator precisionrankitr = precisioncurrent.find(PRECISION_RANK[precisionRank]);
					if ( precisionrankitr == precisioncurrent.end()) {
						precisioncurrent.insert( pair<int,double>(PRECISION_RANK[precisionRank],1.0));
					}
					else {
						double precisionhere = precisionrankitr->second;
						precisioncurrent.erase(PRECISION_RANK[precisionRank]);
						precisioncurrent.insert( pair<int,double>(PRECISION_RANK[precisionRank],precisionhere+1.0));
					}
					precisionAtRankByQuery[currentQuery] = precisioncurrent;
				}
			}
			//Calculating the Precision at particular recall values for each Query.
			for ( int precisionPercentage = 0;precisionPercentage < 11;precisionPercentage++) {
			const double fraction = ((double) PRECISION_PERCENTAGES[precisionPercentage])/100.0;
			if ( rec.recall >= fraction && rec.precision >= (precisionAtRecallByQuery[currentQuery])[PRECISION_PERCENTAGES[precisionPercentage]]) {
					map<int,double> precisioncurrent = precisionAtRecallByQuery[currentQuery];
					map<int,double>::iterator precisionrankitr = precisioncurrent.find(PRECISION_PERCENTAGES[precisionPercentage]);
					if ( precisionrankitr == precisioncurrent.end()) {
						precisioncurrent.insert( pair<int,double>(PRECISION_PERCENTAGES[precisionPercentage],rec.precision));
					}
					else {
						double precisionhere = precisionrankitr->second;
						precisioncurrent.erase(PRECISION_PERCENTAGES[precisionPercentage]);
						precisioncurrent.insert( pair<int,double>(PRECISION_PERCENTAGES[precisionPercentage],precisionhere+rec.precision));
					}
					precisionAtRecallByQuery[currentQuery] = precisioncurrent;
				
			}
			}
			docrank++;
		}
	// Dividing by number of relevant for average of particular Query.
	ExactPrecision[currentQuery] /= (double)(vecNumberofRelevant[currentQuery]+1);	
	RPrecision[currentQuery] /= (double)(vecNumberofRelevant[currentQuery]+1);	
	
	
	// Summing Precision of all queries for final Average Precisions.
	meanAveragePrecision +=  ExactPrecision[currentQuery];
	meanRelevantPrecision +=  RPrecision[currentQuery];
	currentQuery++;
	}

	// Merge the precision at rank for the all Queries to get accumlated results.
	for ( int precisionRank = 0;precisionRank < 14;precisionRank++) {
		double rankPrecision = 0.0;
		for ( int queryitr = 0; queryitr < numberofEffQuery;queryitr++ ) {
			rankPrecision += (precisionAtRankByQuery[queryitr])[PRECISION_RANK[precisionRank]]/PRECISION_RANK[precisionRank];
		}
		rankPrecision /= numberofEffQuery;
		precisionAtRank.insert( pair<int,double>(PRECISION_RANK[precisionRank],rankPrecision));
	}
	
	// Merge the precision at recall values for all the queries to get accumlate results.	
	for ( int precisionPercentage = 0;precisionPercentage < 11;precisionPercentage++) {
		double recallPrecision = 0.0;
		for ( int queryitr = 0; queryitr < numberofEffQuery;queryitr++ ) {
			recallPrecision += (precisionAtRecallByQuery[queryitr])[PRECISION_PERCENTAGES[precisionPercentage]];
		}
		recallPrecision /= numberofEffQuery;
		precisionAtRecall.insert( pair<int,double>(PRECISION_PERCENTAGES[precisionPercentage],recallPrecision));
	}
	// Taking average of all Queries for final precision value/
	averagePrecisionofEachQuery = ExactPrecision;
	meanAveragePrecision /= (currentQuery+1);
	meanRelevantPrecision /= (currentQuery+1);
}


void
AdhocEvaluation::writeEvaluationResult() {
cout<<"_______________________________________________________"<<endl;
cout<<"Result for the run:\t"<<configobj.get_runname()<<endl;
cout<<"-----------------------------------------------------"<<endl;
cout<<"Query wise Precision:\t"<<endl;
cout<<"_______________________________________________________"<<endl;
cout<<numberofEffQuery;
for(int i = 0;i<numberofEffQuery;i++) {
	cout<<"Precision of Query:\t"<<queryNo[i]<<"\tis : "<<averagePrecisionofEachQuery[i]<<endl;
cout<<"-----------------------------------------------------"<<endl;

}
cout<<"_______________________________________________________"<<endl;
cout<<"Evaluation:\t"<<endl;
cout<<"_______________________________________________________"<<endl;
cout<<"Number of Queries:\t"<<numberofEffQuery<<endl;
cout<<"-----------------------------------------------------"<<endl;
cout<<"Retrieved:\t"<<totalNumberofRetrieved<<endl;
cout<<"-----------------------------------------------------"<<endl;
cout<<"Relevant Retrieved:\t"<<totalNumberofRelevantRetrieved<<endl;
cout<<"-----------------------------------------------------"<<endl;
cout<<"Relevant in Qrel:\t"<<totalNumberofRelevant<<endl;
cout<<"_______________________________________________________"<<endl;
cout<<"Mean Evaluation:\t"<<endl;
cout<<"_______________________________________________________"<<endl;
cout<<"Mean Average Precision :\t"<<meanAveragePrecision<<endl;
cout<<"-----------------------------------------------------"<<endl;
cout<<"Mean Relevance Precision :\t"<<meanRelevantPrecision<<endl;
cout<<"_______________________________________________________"<<endl;
cout<<"Precision At Rank:"<<endl;
cout<<"_______________________________________________________"<<endl;
for ( int precisionRank = 0;precisionRank < 14;precisionRank++) {
	cout<<"Precision at Rank:\t"<<PRECISION_RANK[precisionRank]<<"\tis:\t"<<precisionAtRank[PRECISION_RANK[precisionRank]]<<endl;
	cout<<"-----------------------------------------------------"<<endl;
}
cout<<"_______________________________________________________"<<endl;
cout<<"Precision At Recall:"<<endl;
cout<<"_______________________________________________________"<<endl;
for ( int precisionPercentage = 0;precisionPercentage < 11;precisionPercentage++) {
	cout<<"Precision at Recall:\t"<<PRECISION_PERCENTAGES[precisionPercentage]<<"\tis:\t"<<precisionAtRecall[PRECISION_PERCENTAGES[precisionPercentage]]<<endl;
	cout<<"-----------------------------------------------------"<<endl;
}
cout<<"_______________________________________________________"<<endl;
}

int main(int argc,char **argv) {
CONFIG_TREC config;
config.setup_config(string(argv[1]));
AdhocEvaluation *eval = new AdhocEvaluation(config);
eval->evaluate();
eval->writeEvaluationResult();
}
