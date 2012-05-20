/* File: timerstruct.h
   Date: 9 December 1996
   Description: structure for timing routines 
   Copyright: A. MacFarlane (1996)
	 Altered 27/2/2007 for terabtye web efficiency experiments

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  

*/

#include <sys/types.h>
#include <sys/time.h>

#ifndef _TIMER_H_
#define _TIMER_H_

typedef struct cost {
  /* break down of costs for a timed entity */

  struct timeval io;       /* input/output */
  struct timeval cpu;      /* cpu cost */
  struct timeval mar;      /* cost of marshalling */
  struct timeval comms;    /* comunication costs */
  struct timeval wait;     /* delay costs */
  struct timeval prev;     /* total time at previous node - 
								 					 		for calc comms time */

} COST;

#endif
