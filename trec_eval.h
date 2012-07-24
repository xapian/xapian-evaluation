/* @file trec_eval.h
 * @abstract Abstract Header file for TREC Evaluation which read result file and evaluate.
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

/**
  * An abstract class for evaluating the retrieval results.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "config_file.h"
#include <set>
#include <sstream>
#include "trec_qrel.h"

using namespace std;

/** Record class to store of each tuple in result*/
	 class Record  {
		/** The Topic Number.*/
		string queryNo;

		/** The rank of the document. */
		int rank;

		/* The Document identifier. */
		string docNo;

		public:
		/** The precision at this document.*/
		double precision;

		/** The Recall at this document*/
		double recall;

		/**
		 * create a record of retreived document
		 * @param _queryNo
		 * @param _docNo
		 * @param _rank
		 */
		public:
		 Record(string _queryNo,string _docNo,int _rank) {
			queryNo = _queryNo;
			docNo = _docNo;
			rank = _rank;
		}

		/**
		 * Get rank of the document
		 * @return int
		 */
		int getRank() {
			return rank;
		}
	
		/**
		 * Set rank of the document.
		 */
		void setRank(int _rank) {
			rank = _rank;
		}

		/**
		 * get Document Identifier for the document.
		 * @return string identifier of the document.
		 */
		string getDocNo() {
			return docNo;
		}

		/**
		 * set Query Number.
		 * @param Query Identifier. _queryNo
		 */
		void setQueryNo(string _queryNo) {
			queryNo = _queryNo;
		}

		/**
		 * Get query Number.
		 * @return query number.
		 */
		string getQueryNo() {
			return queryNo;
		}

	};
	
class TrecEval {

	public:
	TrecEval(CONFIG_TREC & config) {
		qrel = new TrecQrel(config);
		configobj = config;
	}
	
	/**
	 * Object to hold the Relevance Assessment.
	 */

	TrecQrel *qrel;
	
	/**
	 * Hold the configuration object.
	 */	
	
	CONFIG_TREC configobj;

	/**
	 * Evaluate the Given result file for the Qrel file.
	 * All Subclasses must implement this method.
 	 */
	virtual void evaluate() = 0;

	/**
	 * Write the evaluation result to the standard output 
	 * And to the file given in configuration.
	 * This function is also needs to be implemented mandatorily.
	 */
	virtual void writeEvaluationResult() = 0;


};

