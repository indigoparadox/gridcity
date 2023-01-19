
#include "gridcity.h"

#define MAUG_C
#include <maug.h>

#define MARGE_C
#include <marge.h>

#define RETROFLT_C
#include <retroflt.h>

#define RETROGAM_C
#include <retrogam.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BLOCKS_C
#include "blocks.h"
#include "draw.h"

#define GRIDCITY_MAP_W 40
#define GRIDCITY_MAP_H 40

#define ERROR_ALLOC 0x100

struct GRIDCITY_DATA {
   signed char* map;
   signed char* buildings;
   struct RETROFLAT_BITMAP* blocks;
   int view_x;
   int view_y;
   int next_ms;
};

void gridcity_dump_terrain( signed char* map, int map_w, int map_h ) {
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
   static int init = 0;
   int gridcity_start_x = -1;
   int gridcity_start_y = -1;
   int block_z = 0;

   if( !init ) {
      /* Show "Generating Terrain..." */

      retroflat_draw_lock( NULL );

      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 0, 0,
         retroflat_screen_w(), retroflat_screen_h(),
         RETROFLAT_FLAGS_FILL );

      retroflat_string(
         NULL, RETROFLAT_COLOR_RED,
         "Generating Terrain", 19, NULL, 10, 10, 0 );

      retroflat_draw_release( NULL );

      /* Generate terrain. */
      retrogam_generate_diamond_square(
         data->map, 0, BLOCK_MAX_Z, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

      gridcity_dump_terrain( data->map, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

      /* Pick random starting plot. */
      while(
         (0 > gridcity_start_x && 0 > gridcity_start_y) ||
         2 > data->buildings[gridcity_idx(
            gridcity_start_x, gridcity_start_y, GRIDCITY_MAP_W)]
      ) {
         gridcity_start_x = rand() % GRIDCITY_MAP_W;
         gridcity_start_y = rand() % GRIDCITY_MAP_H;

         block_z = block_get_z(
               gridcity_start_x, gridcity_start_y, data->map, GRIDCITY_MAP_W );

         if( BLOCK_Z_WATER < block_z ) {
            debug_printf( 1, "starting at %d, %d (z: %d)",
               gridcity_start_x, gridcity_start_y, block_z );
            data->buildings[gridcity_idx( gridcity_start_x, gridcity_start_y,
               GRIDCITY_MAP_W )] = 2;
         }
      }
      gridcity_dump_terrain( data->map, GRIDCITY_MAP_W, GRIDCITY_MAP_H );

      init = 1;
   }

   /* === Check Input === */

   switch( retroflat_poll_input( &input_evt ) ) {
   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* === Simulate === */

   if( data->next_ms < retroflat_get_ms() ) {

      gridcity_grow( data->map, data->buildings,
         GRIDCITY_MAP_W, GRIDCITY_MAP_H );
         /* data->map_w, data->map_h ); */

      /* Timer has expired! */
      data->next_ms = retroflat_get_ms() + 2000;
   }

   /* === Draw === */

   retroflat_draw_lock( NULL );

   retroflat_rect(
      NULL, RETROFLAT_COLOR_GRAY, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );

   draw_city( data->view_x, data->view_y, data->map, data->buildings,
      GRIDCITY_MAP_W, GRIDCITY_MAP_H, data->blocks );

   retroflat_draw_release( NULL );
}

int main( int argc, char* argv[] ) {
   struct GRIDCITY_DATA data;
   struct RETROFLAT_ARGS args;
   int retval = 0,
      i = 0;

   /* === Setup === */

   logging_init();

   debug_printf( 1, "test" );

   srand( time( NULL ) );

   args.title = "GridCity";
   args.screen_w = 640;
   args.screen_h = 480;
   args.assets_path = "blocks";

   memset( &data, '\0', sizeof( struct GRIDCITY_DATA ) );

   retval = retroflat_init( argc, argv, &args );
   if( MERROR_OK != retval ) {
      goto cleanup;
   }

   /* === Allocation and Loading === */

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

   data.buildings = calloc( GRIDCITY_MAP_W * GRIDCITY_MAP_H, 1 );
   if( NULL == data.buildings ) {
      retroflat_message( "GridCity Error", "Unable to allocate buildings!" );
      retval = ERROR_ALLOC;
      goto cleanup;
   }

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

   logging_shutdown();

   return retval;
}
END_OF_MAIN()

