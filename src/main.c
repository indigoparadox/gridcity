
#ifdef BLOCKS_XPM
#include "blocks_xpm.h"
#endif /* BLOCKS_XPM */

#define MAUG_C
#include <maug.h>

#define MARGE_C
#include <marge.h>

#define RETROFLT_C
#include <retroflt.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "gridcity.h"
#define BLOCKS_C
#include "blocks.h"
#include "draw.h"
#include "gridgen.h"

#define ERROR_ALLOC 0x100

void gridcity_loop( struct GRIDCITY_DATA* data ) {
   struct RETROFLAT_INPUT input_evt;
   static int init = 0;

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
      gridgen_generate_diamond_square( &(data->city), 0, BLOCK_MAX_Z );

      /* Pick random starting plot. */
      gridcity_build_seed( &(data->city) );

      gridcity_dump_terrain( &(data->city) );

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

      gridcity_grow( &(data->city) );

      /* Timer has expired! */
      data->next_ms = retroflat_get_ms() + 2000;
   }

   /* === Draw === */

   retroflat_draw_lock( NULL );

   retroflat_rect(
      NULL, RETROFLAT_COLOR_GRAY, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );

   gridcity_draw_iso( data );

   retroflat_draw_release( NULL );
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

   data.view_y = 100;
   /* TODO: Configurable in options/CLI. */
   data.city.tiles_w = 40;
   data.city.tiles_h = 40;

   retval = gridcity_init( &(data.city) );
   maug_cleanup_if_not_ok();

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)gridcity_loop, &data );

cleanup:

#ifndef RETROFLAT_OS_WASM

   gridcity_free( &(data.city) );

   retroflat_shutdown( retval );

   logging_shutdown();

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()

