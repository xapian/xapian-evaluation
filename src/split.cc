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

#include <algorithm>
#include <string>
#include "split.h"

using namespace std;

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
	       if (s[j] != '"' && s[j] != '\n')
		 tmps += tolower(s[j]);
             }
           res.push_back(tmps);
           pos=i+1;
         } // END if
     } // END for

 } // END split
