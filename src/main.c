
#ifdef BLOCKS_XPM
#include "blocks_xpm.h"
#endif /* BLOCKS_XPM */

#define MAUG_C
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BLOCKS_C
#include "gridcity.h"

#define ERROR_ALLOC 0x100

void gridcity_loop( struct GRIDCITY_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   static int init = 0;
   struct RETROTILE* city = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_BITMAP* blocks = NULL;

   maug_mlock( data->city_h, city );
   maug_cleanup_if_null_alloc( struct RETROTILE*, city );

   maug_mlock( data->blocks_h, blocks );
   maug_cleanup_if_null_alloc( struct RETROFLAT_BITMAP*, blocks );

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
      retrotile_gen_diamond_square_iter(
         city, 0, BLOCK_MAX_Z, GRIDCITY_LAYER_IDX_TERRAIN, NULL );

      /* Pick random starting plot. */
      gridcity_build_seed( city );

      /* gridcity_dump_terrain( city ); */

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

      gridcity_grow( city );

      /* Timer has expired! */
      data->next_ms = retroflat_get_ms() + 2000;
   }

   /* === Draw === */

   retroflat_draw_lock( NULL );

   retroflat_rect(
      NULL, RETROFLAT_COLOR_GRAY, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );

   draw_city_iso(
      city, data->view_x, data->view_y, blocks, data->blocks_sz );

   retroflat_draw_release( NULL );

cleanup:

   if( NULL != blocks ) {
      maug_munlock( data->blocks_h, blocks );
   }

   if( NULL != city ) {
      maug_munlock( data->city_h, city );
   }

   if( MERROR_OK != retval ) {
      retroflat_quit( retval );
   }
}

int main( int argc, char* argv[] ) {
   struct GRIDCITY_DATA data;
   struct RETROFLAT_ARGS args;
   int retval = 0;

   /* === Setup === */

   logging_init();

   debug_printf( 1, "test" );

   srand( time( NULL ) );

   memset( &args, 0, sizeof( struct RETROFLAT_ARGS ) );

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

   retval = retrotile_alloc( &(data.city_h), 40, 40, 2 );
   maug_cleanup_if_not_ok();
   
   retval = draw_init_blocks( &(data.blocks_h), &(data.blocks_sz) );
   maug_cleanup_if_not_ok();

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)gridcity_loop, NULL, &data );

cleanup:

#ifndef RETROFLAT_OS_WASM

   maug_mfree( data.city_h );

   retroflat_shutdown( retval );

   logging_shutdown();

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()

