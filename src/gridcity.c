
#include "gridcity.h"
#include "blocks.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <uprintf.h>

MERROR_RETVAL gridcity_draw_iso( struct GRIDCITY_DATA* data ) {
   int x = -1,
      y = 2,
      px_x = 0,
      px_y = 0,
      block_id = 0,
      offset_y = 100;
   struct GRIDCITY_TILE* tiles = NULL;
   struct GRIDCITY_BLOCK* blocks = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t tile_idx = 0;

   tiles = maug_mlock( data->city.tiles );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles );

   blocks = maug_mlock( data->city.blocks );
   maug_cleanup_if_null_alloc( struct GRIDCITY_BLOCK*, blocks );

   for( y = 0 ; data->city.tiles_h > y ; y++ ) {

      /* Draw X coordinates backwards to fix overlapping. */
      for( x = data->city.tiles_w - 1 ; 0 <= x ; x-- ) {

         tile_idx = gridcity_idx( x, y, data->city.tiles_w );

         /* TODO: Optimize drawing off-screen out. */
         draw_grid_to_screen_coords( &px_x, &px_y, x, y,
            data->view_x, data->view_y );

         /* If the block is water, it's 0, if it's grass, it's 1, otherwise
          * it's a building.
          */
         if( BLOCK_Z_WATER >= tiles[tile_idx].z ) {
            block_id = 0;
         } else {
            if( 0 == tiles[tile_idx].building ) {
               block_id = 1;
            } else {
               block_id = tiles[tile_idx].building;
            }
         }

         retroflat_blit_bitmap(
            NULL,
            &(blocks[block_id].bmp),
            0, 0,
            px_x,
            offset_y + (BLOCK_Z_WATER >= tiles[tile_idx].z ?
               px_y - BLOCK_Z_WATER : px_y - tiles[tile_idx].z),
            BLOCK_PX_W, BLOCK_PX_H );
      }
   }

cleanup:

   if( NULL != blocks ) {
      maug_munlock( data->city.blocks );
   }

   if( NULL != tiles ) {
      maug_munlock( data->city.tiles );
   }

   return retval;
}

MERROR_RETVAL gridcity_grow( struct GRIDCITY* city ) {
   int x = 0,
      y =0;
   unsigned char building_id = 0;
   MAUG_MHANDLE tiles_new_h = NULL;
   struct GRIDCITY_TILE* tiles_new = NULL;
   struct GRIDCITY_TILE* tiles = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   tiles_new_h = maug_malloc(
      sizeof( struct GRIDCITY_TILE ), city->tiles_h * city->tiles_w );

   tiles_new = maug_mlock( tiles_new_h );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles_new );
   maug_mzero( tiles_new, city->tiles_h * city->tiles_w );

   tiles = maug_mlock( city->tiles );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles );

   for( y = 0 ; city->tiles_h > y ; y++ ) {
      for( x = 0 ; city->tiles_w > x ; x++ ) {
         tiles_new[(y * city->tiles_w) + x].terrain =
            tiles[(y * city->tiles_w) + x].terrain;
         if( BLOCK_Z_WATER == tiles[(y * city->tiles_w) + x].z ) {
            /* Don't grow tiles on water. */
            tiles_new[(y * city->tiles_w) + x].building =
               tiles[(y * city->tiles_w) + x].building;
            continue;
         }
         building_id = tiles[(y * city->tiles_w) + x].building;
         if( BLOCK_MAX - 1 <= building_id ) {
            /* Don't grow tiles grown to the max already. */
            tiles_new[(y * city->tiles_w) + x].building = 
               tiles[(y * city->tiles_w) + x].building;
            continue;
         }
         if(
            (city->tiles_h - 1 == y || \
               0 == tiles[((y + 1) * city->tiles_w) + x].building) &&
            (0 == y || \
               0 == tiles[((y - 1) * city->tiles_w) + x].building) &&
            (city->tiles_w - 1 == x || \
               0 == tiles[(y * city->tiles_w) + (x + 1)].building) &&
            (0 == x || 0 == tiles[(y * city->tiles_w) + (x - 1)].building)
         ) {
            /* Don't grow tiles unless they border another building or
             * the edge of the map.
             */
            tiles_new[(y * city->tiles_w) + x].building = 
               tiles[(y * city->tiles_w) + x].building;
            continue;
         }

         tiles_new[(y * city->tiles_w) + x].building = 
            tiles[(y * city->tiles_w) + x].building + 1;
         debug_printf( 1, "growing %d, %d (%d) to %d", x, y, building_id,
            tiles[(y * city->tiles_w) + x].building );
      }
   }

   memcpy( tiles, tiles_new, city->tiles_w * city->tiles_h );

cleanup:

   if( NULL != tiles ) {
      maug_mfree( city->tiles );
   }

   if( NULL != tiles_new_h ) {
      maug_munlock( tiles_new_h );
      maug_mfree( tiles_new_h );
   }

   return retval;
}

MERROR_RETVAL gridcity_dump_terrain( struct GRIDCITY* city ) {
   int x = 0,
      y = 0;
   struct GRIDCITY_TILE* tiles = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   tiles = maug_mlock( city->tiles );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles );

   printf( "\n" );
   printf( "      " );
   for( x = 0 ; city->tiles_w > x ; x++ ) {
      printf( "%03d ", x );
   }
   printf( "\n" );
   for( x = -2 ; city->tiles_h > x ; x++ ) {
      printf( "----" );
   }
   printf( "\n" );
   for( y = 0 ; city->tiles_h > y ; y++ ) {
      printf( "%03d | ", y );
      for( x = 0 ; city->tiles_h > x ; x++ ) {
         printf( "%03d ", tiles[gridcity_idx( x, y, city->tiles_w )].z );
      }
      printf( "\n" );
   }
   printf( "\n" );

cleanup:

   if( NULL != tiles ) {
      maug_munlock( city->tiles );
   }

   return retval;
}

MERROR_RETVAL gridcity_init( struct GRIDCITY* city ) {
   struct GRIDCITY_TILE* tiles = NULL;
   struct GRIDCITY_BLOCK* blocks = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   city->blocks =
      maug_malloc( sizeof( struct RETROFLAT_BITMAP ), BLOCK_MAX );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, city->blocks );

   blocks = maug_mlock( city->blocks );
   maug_cleanup_if_null_alloc( struct GRIDCITY_BLOCK*, blocks );

   for( i = 0 ; BLOCK_MAX > i ; i++ ) {
      retval = retroflat_load_bitmap(
         gc_block_filenames[i], &(blocks[i].bmp) );
      maug_cleanup_if_not_ok();
   }

   city->tiles = maug_malloc(
      sizeof( struct GRIDCITY_TILE ),
      city->tiles_w * city->tiles_h );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, city->tiles );

   tiles = maug_mlock( city->tiles );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles );

   memset( tiles, -1, city->tiles_w * city->tiles_h );

cleanup:
   
   if( NULL != blocks ) {
      maug_munlock( city->blocks );
   }

   if( NULL != tiles ) {
      maug_munlock( city->tiles );
   }

   return retval;
}

void gridcity_free( struct GRIDCITY* city ) {
   struct GRIDCITY_BLOCK* blocks;
   size_t i = 0;

   if( NULL != city->tiles ) {
      maug_mfree( city->tiles );
   }

   if( NULL == city->blocks ) {
      goto cleanup;
   }
   blocks = maug_mlock( city->blocks );
   if( NULL == blocks ) {
      goto cleanup;
   }

   for( i = 0 ; city->blocks_sz > i ; i++ ) {
      if( retroflat_bitmap_ok( &(blocks[i].bmp) ) ) {
         retroflat_destroy_bitmap( &(blocks[i].bmp) );
      }
   }

   maug_munlock( city->blocks );
   maug_mfree( city->blocks );

cleanup:
   return;
}

MERROR_RETVAL gridcity_build_seed( struct GRIDCITY* city ) {
   int16_t gridcity_start_x = -1;
   int16_t gridcity_start_y = -1;
   size_t tile_idx = 0;
   struct GRIDCITY_TILE* tiles;
   MERROR_RETVAL retval = MERROR_OK;

   tiles = maug_mlock( city->tiles );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles );

   while(
      (0 > gridcity_start_x && 0 > gridcity_start_y) ||
      2 > tiles[tile_idx].building
   ) {
      gridcity_start_x = rand() % city->tiles_w;
      gridcity_start_y = rand() % city->tiles_h;
      tile_idx = gridcity_idx(
         gridcity_start_x, gridcity_start_y, city->tiles_w );

      if( BLOCK_Z_WATER < tiles[tile_idx].z ) {
         debug_printf( 1, "starting at %d, %d (z: %d)",
            gridcity_start_x, gridcity_start_y, tiles[tile_idx].z );
         tiles[tile_idx].building = 2;
      }
   }

cleanup:

   if( NULL != tiles ) {
      maug_mfree( city->tiles );
   }

   return retval;
}

