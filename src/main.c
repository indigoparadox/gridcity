
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

struct GRIDCITY_DATA {
   char* map;
   struct RETROFLAT_BITMAP* blocks;
   int view_x;
   int view_y;
};

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

void gridcity_loop( struct GRIDCITY_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;

   switch( retroflat_poll_input( &input_evt ) ) {
   case RETROFLAT_KEY_Q:
      retroflat_quit( 0 );
      break;
   }

   retroflat_draw_lock( NULL );

   retroflat_rect(
      NULL, RETROFLAT_COLOR_GRAY, 0, 0, SCREEN_W, SCREEN_H,
      RETROFLAT_FLAGS_FILL );

   draw_city( data->view_x, data->view_y, data->map,
      GRIDCITY_MAP_W, GRIDCITY_MAP_H, data->blocks );

   retroflat_draw_release( NULL );
}

int main( int argc, char* argv[] ) {
   struct GRIDCITY_DATA data;
   struct RETROFLAT_ARGS args;
   int retval = 0,
      i = 0;

   /* === Setup === */

   srand( time( NULL ) );

   args.title = "GridCity";
   args.screen_w = 320;
   args.screen_h = 200;
   args.assets_path = "blocks";

   retval = retroflat_init( argc, argv, &args );

   /* === Allocation and Loading === */

   memset( &data, '\0', sizeof( struct GRIDCITY_DATA ) );
   data.view_y = 100;

   data.blocks = calloc( sizeof( struct RETROFLAT_BITMAP ), BLOCK_MAX );
   if( NULL == data.blocks ) {
      retroflat_message( "GridCity Error", "Unable to allocate blocks!" );
      retval = ERROR_ALLOC;
      goto cleanup;
   }
   for( i = 0 ; BLOCK_MAX > i ; i++ ) {
      retval = retroflat_load_bitmap(
         gc_block_filenames[i], &(data.blocks[i]) );
      if( RETROFLAT_ERROR_BITMAP == retval ) {
         goto cleanup;
      }
   }

   data.map = calloc( GRIDCITY_MAP_W * GRIDCITY_MAP_H, 1 );
   if( NULL == data.map ) {
      retroflat_message( "GridCity Error", "Unable to allocate map!" );
      retval = ERROR_ALLOC;
      goto cleanup;
   }
   memset( data.map, -1, GRIDCITY_MAP_W * GRIDCITY_MAP_H );

   gridcity_generate_terrain(
      data.map, 100, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

   gridcity_dump_terrain( data.map, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)gridcity_loop, &data );

cleanup:

   if( NULL != data.blocks ) {
      for( i = 0 ; BLOCK_MAX > i ; i++ ) {
         if( retroflat_bitmap_ok( &(data.blocks[i]) ) ) {
            retroflat_destroy_bitmap( &(data.blocks[i]) );
         }
      }
      free( data.blocks );
   }

   if( NULL != data.map ) {
      free( data.map );
   }

   retroflat_shutdown( retval );

   return retval;
}
END_OF_MAIN()

