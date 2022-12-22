
#include "gridcity.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define GRIDCITY_MAP_W 17
#define GRIDCITY_MAP_H 17

void gridcity_dump_terrain( char* map, int map_w, int map_h ) {
   int x = 0,
   y = 0;

   printf( "\n" );
   printf( "      " );
   for( x = 0 ; map_w > x ; x++ ) {
      printf( "%03d ", x );
   }
   printf( "\n" );
   for( x = -2 ; map_h > x ; x++ ) {
      printf( "----" );
   }
   printf( "\n" );
   for( y = 0 ; map_h > y ; y++ ) {
      printf( "%03d | ", y );
      for( x = 0 ; map_h > x ; x++ ) {
         printf( "%03d ", map[(y * map_w) + x] );
      }
      printf( "\n" );
   }
   printf( "\n" );
}

int main( int argc, char* argv[] ) {
   char* gridcity_map = NULL;

   srand( time( NULL ) );

   gridcity_map = calloc( GRIDCITY_MAP_W * GRIDCITY_MAP_H, 1 );
   if( NULL == gridcity_map ) {
      goto cleanup;
   }
   memset( gridcity_map, -1, GRIDCITY_MAP_W * GRIDCITY_MAP_H );

   gridcity_generate_terrain( gridcity_map, 3, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

   gridcity_dump_terrain( gridcity_map, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

cleanup:
   
   if( NULL != gridcity_map ) {
      free( gridcity_map );
   }

   return 0;
}

