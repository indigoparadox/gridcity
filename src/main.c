
#define MAUG_C
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BLOCKS_C
#include "gridcity.h"

#define ERROR_ALLOC 0x100

MERROR_RETVAL gridcity_gen_ani_cb( void* animation_cb_data, int16_t iter ) {
   static int8_t gen_iter = 0;
   MAUG_CONST char c_gen_strings[][4] = {
      ".",
      "..",
      "...",
      ""
   };
   char gen_str[30];
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROFLAT_INPUT input_evt;
   RETROFLAT_IN_KEY input = 0;

   /* Check for cancel. */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_ESC:
      retroflat_quit( MERROR_GUI );
      retval = MERROR_GUI;
      break;
   }

   /* Draw status update. */

   retroflat_draw_lock( NULL );

   if( 0 == gen_iter ) {
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 0, 0,
         40, 20,
         RETROFLAT_FLAGS_FILL );
   }

   maug_snprintf( gen_str, 30, "Generating Terrain (%d)%s",
      iter, c_gen_strings[gen_iter] );

   retroflat_string(
      NULL, RETROFLAT_COLOR_RED, gen_str, 0, NULL, 10, 10, 0 );

   retroflat_draw_release( NULL );

   gen_iter++;
   if( '\0' == c_gen_strings[gen_iter][0] ) {
      gen_iter = 0;
   }

   return retval;
}

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

#if 0
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 0, 0,
         retroflat_screen_w(), retroflat_screen_h(),
         RETROFLAT_FLAGS_FILL );
#endif

      retroflat_draw_release( NULL );

#ifdef GRIDCITY_VORONOI
      retval = retrotile_gen_voronoi_iter(
         city, 0, BLOCK_MAX_Z, 5, GRIDCITY_LAYER_IDX_TERRAIN, 0, NULL,
         gridcity_gen_ani_cb, NULL );
      maug_cleanup_if_not_ok();
#else
      /* Generate terrain. */
      retval = retrotile_gen_diamond_square_iter(
         city, 0, BLOCK_MAX_Z, 5, GRIDCITY_LAYER_IDX_TERRAIN, 0, NULL,
         gridcity_gen_ani_cb, NULL );
      maug_cleanup_if_not_ok();

      debug_printf( 1, "smoothing terrain..." );

      /* Smooth terrain. */
      retval = retrotile_gen_smooth_iter(
         city, 0, 0, 0, GRIDCITY_LAYER_IDX_TERRAIN, 0, NULL,
         gridcity_gen_ani_cb, NULL );
      maug_cleanup_if_not_ok();
#endif /* GRIDCITY_VORONOI */

      /* Pick random starting plot. */
      /* gridcity_build_seed( city ); */

#ifdef DEBUG_RETROTILE
      gridcity_dump_terrain( city );
#endif /* DEBUG_RETROTILE */

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

      /* gridcity_grow( city ); */

      /* Timer has expired! */
      data->next_ms = retroflat_get_ms() + 2000;
   }

   /* === Draw === */

   retroflat_draw_lock( NULL );

#if 0
   retroflat_rect(
      NULL, RETROFLAT_COLOR_GRAY, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );
#endif

   draw_city_iso(
      city, data->view_x, data->view_y, blocks, data->blocks_sz,
      retroflat_screen_h() >> 1 );

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
   args.assets_path = "blocks";

   memset( &data, '\0', sizeof( struct GRIDCITY_DATA ) );

   retval = retroflat_init( argc, argv, &args );
   if( MERROR_OK != retval ) {
      goto cleanup;
   }

   /* === Allocation and Loading === */

   retval = retrotile_alloc( &(data.city_h), 20, 20, 2 );
   maug_cleanup_if_not_ok();
   
   retval = draw_init_blocks( &(data.blocks_h), &(data.blocks_sz) );
   maug_cleanup_if_not_ok();

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)gridcity_loop, NULL, &data );

cleanup:

#ifndef RETROFLAT_OS_WASM

   maug_mfree( data.city_h );

   gridcity_free_blocks( &data );

   retroflat_shutdown( retval );

   logging_shutdown();

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()

