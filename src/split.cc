/* split.cc: split a string routine
 *
 * ----START-LICENCE----
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 * -----END-LICENCE-----
 */

#include <fstream>
#include <xapian.h>
#include <algorithm>
#include <iostream>
#include <string>
#include "split.h"

using namespace Xapian;
using namespace std;

void clear_apos(  string & data ) {
// clear " from data

         string temp;

         temp.clear();
         for( int i=0; i < data.size(); i++ )
                                if( data[i] != '"' && data[i] != '\n')
                                                temp += data[i];

         data = temp;

} // END clear_apos

static inline void
lowercase(string & term )
{
    string::iterator i = term.begin();
    while (i != term.end()) {
        *i = tolower(*i);
        i++;
    }
}

void split( const string &s, char c, vector<string> &res)
 {
   res.clear();
   if(s==""){return;}
   uint i;
   uint pos=0;
   for( i=0; i<=s.size(); i++)
     {
       if( i == s.size() || s[i] == c )
         {
           uint j;
           string tmps;
           for( j=pos; j<i; j++)
             {
               tmps += s[j];
             }
                 clear_apos( tmps );
                 lowercase( tmps );
           res.push_back(tmps);
           pos=i+1;
         } // END if
     } // END for

 } // END split
