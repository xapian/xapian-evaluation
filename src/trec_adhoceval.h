/* @file trec_adhoceval.h
 * @abstract Implements the Evaluation of Trec Adhoc Evaluation which read result file and evaluate for adhoc evaluation.
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

#include "trec_eval.h"
#include "config_file.h"
#include <map>
#include <vector>

using namespace std;

class AdhocEvaluation : public TrecEval {

protected:

/** Maximum number of document retrieved for a query.*/

int maxNumberRetrieved;

/** The number of Effective Queries.
 *  An effective query that has corresponding relevant
 *  document in qrel file.
 */

int numberofEffQuery;

/* Total number of document retrieved .*/

int totalNumberofRetrieved;

/** Total number of relevant document in the qrel file*/

int totalNumberofRelevant;

/** The total number of relevant document retrieved in the task*/

int totalNumberofRelevantRetrieved;

map<int,double> precisionAtRank;

map<int,double> precisionAtRecall;

public:

AdhocEvaluation(CONFIG_TREC & config) : TrecEval(config) {
}

protected:
/* Average Precision*/
double meanAveragePrecision;
/** Relevant Precision. */
double meanRelevantPrecision;
/** The average precision of each query*/
double * averagePrecisionofEachQuery;
/**The Query number of each query*/
vector<string> queryNo;



public:
/** Initialise the statistics for evaluating the result*/
void initialise();

/** Evaluate the given result file.*/
void evaluate();

void writeEvaluationResult();

};
