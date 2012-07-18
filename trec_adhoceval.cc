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
#include <iostream>
#include <fstream>

using namespace std;

void 
AdhocEvaluation::intialise() {

	maxNumberRetrieved = confifobj.get_noresult();
	precisionAtRank.clear();
	precisionAtRecall.clear();
	numberofEffQuery = 0;
	totalNumberofRetrieved = 0;
	totalNumberofRelevant = 0;
	meanAveragePrecision = 0; 
}


void
AdhocEvaluation::evaluate() {
intialise();
int effQueryCounter = 0;
int [] numberofRelevantRetrived = NULL;
int [] numberofRelevant  = NULL;
int [] numberofRetrived = NULL;

vector<vector<Record> > listofRetrieved;
vector<vector<Record> > listofRelevantRetrieved;
vector<int> vecNumberofRelevant;
vector<int> vecNumberofRetrieved;
vector<int> vecNumberofRelevantRetrieved;
vector<string> vecQueryNo;
ifstream resultfile;
string resultfilename = configobj.get_resultfile().c_str();
resultfile.open(resultfilename,ifstream::in);
if(!resultfile.is_open()) {
	cout<<"ERROR - can't open the result file"<<configobj.get_resultfile().c_str() << "for reading"<<endl;
}
string str = NULL;
string previous = "";
int numberofRetrievedCounter = 0;
int numberofRelevantRetrievedCounter = 0;
vector<Record> relevantRetrieved;
vector<Record> retrieved;
while( !resultfile.eof() ) {
getline(resultfile,str);
vector<string> data;
split(line,' ',data);
}
}
void
AdhocEvaluation::writeEvaluationResult() {
}
