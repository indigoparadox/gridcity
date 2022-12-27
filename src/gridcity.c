
#include "gridcity.h"

#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#define debug_printf( ... ) printf( __VA_ARGS__ )
#else
#define debug_printf( ... )
#endif

void gridcity_generate_terrain_iter(
   signed char* map, int max_z, int map_w, int map_h,
   int sect_x, int sect_y, int sect_w, int sect_h
) {
   int iter_x = 0,
      iter_y = 0,
      iter = 0,
      iter_depth = 0;

   /* Trivial case; end recursion. */
   if( 0 == sect_w ) {
      return;
   }

   iter_depth = map_w / sect_w;

#ifdef DEBUG
   for( iter = 0 ; iter_depth > iter ; iter++ ) {
      debug_printf( " " );
   }
   debug_printf( "map_w %d, sector %d wide, 2 child sectors %d wide\n",
      map_w, sect_w, sect_w / 2 );
#endif /* DEBUG */

   /* Generate missing corner data. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         if(
            -1 ==
            map[gridcity_idx(
               sect_x + (iter_x * sect_w),
               sect_y + (iter_y * sect_h), map_w )]
         ) {
            map[gridcity_idx(
               sect_x + (iter_x * sect_w),
               sect_y + (iter_y * sect_h), map_w )] = rand() % max_z;
         }
      }
   }
   
   /* Average corner data. */
   for( iter = 0 ; iter_depth > iter ; iter++ ) {
      debug_printf( " " );
   }
   debug_printf( "avg of %d, %d, %d, %d\n",
      map[gridcity_idx( sect_x, sect_y, map_w )],
      map[gridcity_idx( sect_x, sect_y + (sect_h), map_w )],
      map[gridcity_idx( sect_x + (sect_w), sect_y, map_w )],
      map[gridcity_idx( sect_x + (sect_w), sect_y + (sect_h), map_w )] );

   map[gridcity_idx( sect_x + (sect_w / 2), sect_y + (sect_h) / 2, map_w )] =
      (map[gridcity_idx( sect_x, sect_y, map_w )] +
      map[gridcity_idx( sect_x, sect_y + (sect_h), map_w )] +
      map[gridcity_idx( sect_x + (sect_w), sect_y, map_w )] +
      map[gridcity_idx( sect_x + (sect_w), sect_y + (sect_h), map_w )])
      / 4;

   /* Recurse into subsectors. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         for( iter = 0 ; iter_depth > iter ; iter++ ) {
            debug_printf( " " );
         }
         if( 0 == sect_w / 2 ) {
            debug_printf( "return" );
            return;
         }
         debug_printf( "child sector at %d, %d, %d wide\n",
            sect_x + (iter_x * (sect_w / 2)),
            sect_y + (iter_y * (sect_h / 2)), sect_w / 2 );
         gridcity_generate_terrain_iter(
            map, max_z, map_w, map_h,
            sect_x + (iter_x * (sect_w / 2)),
            sect_y + (iter_y * (sect_h / 2)),
            sect_w / 2, sect_h / 2 );
      }
   }

#ifdef DEBUG
   for( iter = 0 ; iter_depth > iter ; iter++ ) {
      debug_printf( " " );
   }
   debug_printf( "return\n" );
#endif
}

/*
void gridcity_generate_terrain( char* map, int max_z, int map_w, int map_h ) {


   map[gridcity_idx( 0, 0, map_w )] = rand() % max_z;
   map[gridcity_idx( 0, map_h - 1, map_w )] = rand() % max_z;
   map[gridcity_idx( map_w - 1, 0, map_w )] = rand() % max_z;
   map[gridcity_idx( map_w - 1, map_h - 1, map_w )] = rand() % max_z;

   map[gridcity_idx( map_w / 2, map_h / 2, map_w )] =
      (map[gridcity_idx( 0, 0, map_w )] +
      map[gridcity_idx( 0, map_h - 1, map_w )] +
      map[gridcity_idx( map_w - 1, 0, map_w )] +
      map[gridcity_idx( map_w - 1, map_h - 1, map_w )]) / 4;

   gridcity_generate_terrain_iter(
      map, max_z, map_w, map_h, 0, 0, map_w / 2, map_h / 2 );
   gridcity_generate_terrain_iter(
      map, max_z, map_w, map_h, map_w / 2, 0, map_w / 2, map_h / 2 );
   gridcity_generate_terrain_iter(
      map, max_z, map_w, map_h, 0, map_h / 2, map_w / 2, map_h / 2 );
   gridcity_generate_terrain_iter(
      map, max_z, map_w, map_h, map_w / 2, map_h / 2, map_w / 2, map_h / 2 );

}
*/

