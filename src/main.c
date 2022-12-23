
#include "gridcity.h"

#define RETROFLT_C
#include <retroflt.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BLOCKS_C
#include "blocks.h"
#include "draw.h"

#define GRIDCITY_MAP_W 17
#define GRIDCITY_MAP_H 17

#define ERROR_ALLOC 0x100

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
   unsigned char running = 1;
   char* gridcity_map = NULL;
   int retval = 0,
      i = 0,
      view_x = 0,
      view_y = 100;
   struct RETROFLAT_BITMAP* blocks = NULL;
   struct RETROFLAT_INPUT input_evt;

   /* === Setup === */

   srand( time( NULL ) );

   retval = retroflat_init( 320, 240 );

   retroflat_set_assets_path( "blocks" );

   /* === Allocation and Loading === */

   blocks = calloc( sizeof( struct RETROFLAT_BITMAP ), BLOCK_MAX );
   if( NULL == blocks ) {
      allegro_message( "unable to allocate blocks!" );
      retval = ERROR_ALLOC;
      goto cleanup;
   }
   for( i = 0 ; BLOCK_MAX > i ; i++ ) {
      retval = retroflat_load_bitmap( gc_block_filenames[i], &(blocks[i]) );
      if( RETROFLAT_ERROR_BITMAP == retval ) {
         goto cleanup;
      }
   }

   gridcity_map = calloc( GRIDCITY_MAP_W * GRIDCITY_MAP_H, 1 );
   if( NULL == gridcity_map ) {
      allegro_message( "unable to allocate map!" );
      retval = ERROR_ALLOC;
      goto cleanup;
   }
   memset( gridcity_map, -1, GRIDCITY_MAP_W * GRIDCITY_MAP_H );

   gridcity_generate_terrain(
      gridcity_map, 100, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

   gridcity_dump_terrain( gridcity_map, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

   /* === Main Loop === */

   while( running ) {

      switch( retroflat_poll_input( &input_evt ) ) {
      case KEY_Q:
         running = 0;
         break;
      }

      retroflat_draw_lock();
      retroflat_rect(
         NULL, RETROFLAT_COLOR_GRAY, 0, 0, SCREEN_W, SCREEN_H,
         RETROFLAT_FLAGS_FILL );

      draw_city( view_x, view_y, gridcity_map,
         GRIDCITY_MAP_W, GRIDCITY_MAP_H, blocks );

      retroflat_draw_flip();
   }

cleanup:

   if( NULL != blocks ) {
      for( i = 0 ; BLOCK_MAX > i ; i++ ) {
         if( retroflat_bitmap_ok( &(blocks[i]) ) ) {
            retroflat_destroy_bitmap( &(blocks[i]) );
         }
      }
      free( blocks );
   }

   if( NULL != gridcity_map ) {
      free( gridcity_map );
   }

   retroflat_shutdown( retval );

   return retval;
}
END_OF_MAIN()

