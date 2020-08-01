/* File: timer.c
   Date: 9 December 1996
   Description: timing routines
   Copyright: A.MacFarlane
	 Altered 27/2/2007 for terabyte web efficiency experiments

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

#include <sys/time.h>

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
