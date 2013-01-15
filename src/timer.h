/* File: timer.h
   Date: 9 December 1996
   Description: timing library routines
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
#include "timerstruct.h"       /* timer data structures */

#ifdef __cplusplus
extern "C" {
#endif

extern void diff_time( struct timeval finish, 
		       struct timeval start,
		       struct timeval *cost );
/* calculate the difference in time between start and finish (in whole and 
   decimal parts) */

extern void accumtime( struct timeval *current, 
		       struct timeval newv );
/* accumulate time from new into current */

extern void accumcost( COST *cur_cost, COST new_cost );
/* accumulate costs form new in cur */

extern void Init_Cost( COST *cost );
/* initialise cost structure */

extern void Disp_Time( struct timeval timebefore, struct timeval timeafter );
/* display the time lapse between before and after */

extern void Print_Costs( COST costs );
  /* print cost information to standard error */

extern float time_real( struct timeval t );
/* convert struct timeval into a real time in one figure */

#ifdef __cplusplus
}
#endif
