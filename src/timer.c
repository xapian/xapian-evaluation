/* File: timer.c
   Date: 9 December 1996
   Description: timing routines 
   Copyright: A.MacFarlane
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

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "timerstruct.h"        /* timing data structures */

#define MICROS 1000000       /* microsecs */
#define MICROS_REAL 1000000.0       /* microsecs in float */

float time_real( struct timeval t ) {
  /* convert struct timeval into a real time in one figure */

  float real_t = 0.0;   /* the final real time */

  real_t = (float) t.tv_sec + ((float) t.tv_usec/MICROS_REAL);

  return real_t;

} /* END time_real */

void diff_time( struct timeval finish, 
				struct timeval start, 
				struct timeval *cost ){

  /* calculate the difference in time between start and finish (in whole and 
     decimal parts) */
  if( finish.tv_usec < start.tv_usec )
    {
      cost->tv_sec = (finish.tv_sec - start.tv_sec) - 1;
      cost->tv_usec = (finish.tv_usec + MICROS) - start.tv_usec;
    }
  else
    {
      cost->tv_sec = finish.tv_sec - start.tv_sec;
      cost->tv_usec = finish.tv_usec - start.tv_usec;
    } /* END if */

} /* END diff_time */

void Disp_Time( struct timeval timebefore, struct timeval timeafter ) {
  /* display the time lapse between before and after */
  struct timeval result;

  diff_time( timebefore, timeafter, &result );
  fprintf( stderr, "Time taken for computation is %d.%d secs\n",
	   (int)result.tv_sec, (int)result.tv_usec );

} /* END Disp_Time */

void accumtime( struct timeval *current,  struct timeval newv ) {

  /* accumulate time from new into current */
  int offset = 0;

  current->tv_usec = current->tv_usec + newv.tv_usec;
  if(current->tv_usec > MICROS)
    {
      offset = 1;
      current->tv_usec = current->tv_usec - MICROS;
    } /* END if */

  current->tv_sec = current->tv_sec + newv.tv_sec + offset;

} /* END accumtime */

void accumcost( COST *cur_cost, COST new_cost ) {
  /* accumulate costs form new in cur */

  accumtime( &cur_cost->io,    new_cost.io );
  accumtime( &cur_cost->cpu,   new_cost.cpu );
  accumtime( &cur_cost->mar,   new_cost.mar );
  accumtime( &cur_cost->comms, new_cost.comms );
  accumtime( &cur_cost->wait,  new_cost.wait );
 
} /* END accumcost */

void Print_Costs( COST cost ) {
  /* print cost information to standard error */

  fprintf( stderr, "I/O time is %d.%d secs\n",           (int)cost.io.tv_sec,    (int)cost.io.tv_usec );
  fprintf( stderr, "CPU time is %d.%d secs\n",           (int)cost.cpu.tv_sec,   (int)cost.cpu.tv_usec );
  fprintf( stderr, "Marshalling time is %d.%d secs\n",   (int)cost.mar.tv_sec,   (int)cost.mar.tv_usec );
  fprintf( stderr, "Communication time is %d.%d secs\n", (int)cost.comms.tv_sec, (int)cost.comms.tv_usec );
  fprintf( stderr, "Waiting time is %d.%d secs\n",       (int)cost.wait.tv_sec,  (int)cost.wait.tv_usec );

} /* END Print_Costs */

void Init_Cost( COST *cost ) {
  /* initialise cost structure */

  cost->io.tv_sec = 0;
  cost->cpu.tv_sec = 0;
  cost->mar.tv_sec = 0;
  cost->comms.tv_sec = 0;
  cost->wait.tv_sec = 0;
  cost->io.tv_usec = 0;
  cost->cpu.tv_usec = 0;
  cost->mar.tv_usec = 0;
  cost->comms.tv_usec = 0;
  cost->wait.tv_usec = 0;

} /* END Init_Cost */
