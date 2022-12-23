
#include "gridcity.h"

#include <allegro.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BLOCKS_C
#include "blocks.h"
#include "draw.h"

#define GRIDCITY_MAP_W 17
#define GRIDCITY_MAP_H 17

#define ERROR_ALLEGRO 1
#define ERROR_ALLEGRO_GFX 2
#define ERROR_ALLEGRO_MOUSE 4
#define ERROR_ALLOC 8
#define ERROR_BITMAP 0xf

BITMAP* load_block_bitmap( const char* filename ) {
   BITMAP* bmp_out = NULL;
   char filename_path[PATH_MAX + 1];

   /* Build the path to the bitmap. */
   memset( filename_path, '\0', PATH_MAX + 1 );
   snprintf(
      filename_path, PATH_MAX, "%s%c%s.%s",
      PATH_BLOCKS, PATH_SEP, filename, BLOCK_EXT );

   printf( "%s\n", filename_path );

   /* Load and adjust the bitmap. */
   bmp_out = load_bmp( filename_path, NULL );

   return bmp_out;
}

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
      key_val = 0,
      view_x = 0,
      view_y = 100;
   BITMAP** blocks = NULL;
   BITMAP* buffer = NULL;

   /* === Setup === */

   srand( time( NULL ) );

   if( allegro_init() ) {
      allegro_message( "could not setup allegro!" );
      retval = ERROR_ALLEGRO;
      goto cleanup;
   }

   install_keyboard();

#ifdef DOS
   if( set_gfx_mode( GFX_AUTODETECT, 320, 200, 0, 0 ) ) {
#else
   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0 ) ) {
#endif
      allegro_message( "could not setup graphics!" );
      retval = ERROR_ALLEGRO_GFX;
      goto cleanup;
   }

   /* === Allocation and Loading === */

   buffer = create_bitmap( SCREEN_W, SCREEN_H );
   if( NULL == buffer ) {
      allegro_message( "unable to allocate screen buffer!" );
      goto cleanup;
   }

   blocks = calloc( sizeof( BITMAP* ), BLOCK_MAX );
   if( NULL == blocks ) {
      allegro_message( "unable to allocate blocks!" );
      retval = ERROR_ALLOC;
      goto cleanup;
   }
   for( i = 0 ; BLOCK_MAX > i ; i++ ) {
      blocks[i] = load_block_bitmap( gc_block_filenames[i] );
      if( NULL == blocks[i] ) {
         allegro_message( "unable to load bitmap: %s", gc_block_filenames[i] );
         retval = ERROR_BITMAP;
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

      if( keypressed() ) {
         key_val = readkey();
         switch( (key_val >> 8) ) {
         case KEY_Q:
            running = 0;
            break;
         }
      }

      acquire_screen();
      clear_to_color( buffer, makecol( 128, 128, 128 ) );

      draw_city( buffer, view_x, view_y, gridcity_map,
         GRIDCITY_MAP_W, GRIDCITY_MAP_H, blocks );

      blit( buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

      release_screen();
      vsync();
   }

cleanup:
   
   if( ERROR_ALLEGRO != retval ) {
      clear_keybuf();
   }

   if( NULL != blocks ) {
      for( i = 0 ; BLOCK_MAX > i ; i++ ) {
         if( NULL != blocks[i] ) {
            destroy_bitmap( blocks[i] );
         }
      }
      free( blocks );
   }

   if( NULL != gridcity_map ) {
      free( gridcity_map );
   }

   if( NULL != buffer ) {
      destroy_bitmap( buffer );
   }

   return retval;
}
END_OF_MAIN()

