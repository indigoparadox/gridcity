
#include "gridcity.h"
#include "blocks.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <uprintf.h>

void gridcity_grow(
   signed char* map, signed char* buildings, int map_w, int map_h
) {
   int x = 0,
      y =0;
   unsigned char building_id = 0;
   unsigned char* buildings_new = NULL;

   buildings_new = calloc( map_h, map_w );

   for( y = 0 ; map_h > y ; y++ ) {
      for( x = 0 ; map_w > x ; x++ ) {
         if( BLOCK_Z_WATER == buildings[(y * map_w) + x] ) {
            /* Don't grow buildings on water. */
            buildings_new[(y * map_w) + x] = buildings[(y * map_w) + x];
            continue;
         }
         building_id = buildings[(y * map_w) + x];
         if( BLOCK_MAX - 1 <= building_id ) {
            /* Don't grow buildings grown to the max already. */
            buildings_new[(y * map_w) + x] = buildings[(y * map_w) + x];
            continue;
         }
         if(
            (map_h - 1 == y || 0 == buildings[((y + 1) * map_w) + x]) &&
            (0 == y || 0 == buildings[((y - 1) * map_w) + x]) &&
            (map_w - 1 == x || 0 == buildings[(y * map_w) + (x + 1)]) &&
            (0 == x || 0 == buildings[(y * map_w) + (x - 1)])
         ) {
            /* Don't grow buildings unless they border another building or
             * the edge of the map.
             */
            buildings_new[(y * map_w) + x] = buildings[(y * map_w) + x];
            continue;
         }

         buildings_new[(y * map_w) + x] = buildings[(y * map_w) + x] + 1;
         debug_printf( 1, "growing %d, %d (%d) to %d", x, y, building_id,
            buildings[(y * map_w) + x] );
      }
   }

   memcpy( buildings, buildings_new, map_w * map_h );

   free( buildings_new );
}

