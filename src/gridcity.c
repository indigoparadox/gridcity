
#include "gridcity.h"

MERROR_RETVAL gridcity_grow( struct RETROTILE* city ) {
   int x = 0,
      y =0;
   unsigned char building_id = 0;
   MAUG_MHANDLE tiles_new_h = NULL;
   retrotile_tile_t* tiles_new = NULL;
   retrotile_tile_t* tiles_build = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_LAYER* layer_terrain = NULL;
   struct RETROTILE_LAYER* layer_build = NULL;

   /* Create a block of memory to hold the after-growth building tiles,
    * so we can maintain a complete atomic image of the old buildings on
    * which to base groth calculations until we're finished.
    */
   tiles_new_h = maug_malloc(
      sizeof( retrotile_tile_t ), city->tiles_h * city->tiles_w );
   maug_mlock( tiles_new_h, tiles_new );
   maug_cleanup_if_null_alloc( retrotile_tile_t*, tiles_new );
   maug_mzero( tiles_new, city->tiles_h * city->tiles_w );

   layer_terrain = retrotile_get_layer_p(
      city, GRIDCITY_LAYER_IDX_TERRAIN );
   layer_build = retrotile_get_layer_p(
      city, GRIDCITY_LAYER_IDX_BUILDINGS );

   for( y = 0 ; city->tiles_h > y ; y++ ) {
      for( x = 0 ; city->tiles_w > x ; x++ ) {
         if(
            BLOCK_Z_WATER ==
            retrotile_get_tile( city, layer_terrain, x, y )
         ) {
            /* Don't grow tiles on water. */
            tiles_new[(y * city->tiles_w) + x] =
               retrotile_get_tile( city, layer_build, x, y );
            continue;
         }
         building_id = retrotile_get_tile( city, layer_build, x, y );
         if( BLOCK_MAX - 1 <= building_id ) {
            /* Don't grow tiles grown to the max already. */
            tiles_new[(y * city->tiles_w) + x] = 
               retrotile_get_tile( city, layer_build, x, y );
            continue;
         }
         if(
            (city->tiles_h - 1 == y || \
               0 == retrotile_get_tile( city, layer_build, x, y + 1 )) &&
            (0 == y || \
               0 == retrotile_get_tile( city, layer_build, x, y - 1 )) &&
            (city->tiles_w - 1 == x || \
               0 == retrotile_get_tile( city, layer_build, x + 1, y )) &&
            (0 == x ||
               0 == retrotile_get_tile( city, layer_build, x - 1, y ))
         ) {
            /* Don't grow tiles unless they border another building or
             * the edge of the map.
             */
            tiles_new[(y * city->tiles_w) + x] = 
               retrotile_get_tile( city, layer_build, x, y );
            continue;
         }

         /* Grow the building by incrementing its tile index. */
         tiles_new[(y * city->tiles_w) + x] = 
            retrotile_get_tile( city, layer_build, x, y ) + 1;
         debug_printf( 1, "growing %d, %d (%d) to %d", x, y, building_id,
            retrotile_get_tile( city, layer_build, x, y ) );
      }
   }

   /* Copy the new buildings all at once. */
   tiles_build = retrotile_get_tiles_p( layer_build );
   memcpy( tiles_build, tiles_new, 
      city->tiles_w * city->tiles_h * sizeof( retrotile_tile_t ) );

cleanup:

   if( NULL != tiles_new ) {
      maug_munlock( tiles_new_h, tiles_new );
   }

   if( NULL != tiles_new_h ) {
      maug_mfree( tiles_new_h );
   }

   return retval;
}

MERROR_RETVAL gridcity_dump_terrain( struct RETROTILE* city ) {
   int x = 0,
      y = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_LAYER* layer_terrain = NULL;
   retrotile_tile_t tile_idx = 0;

   layer_terrain = retrotile_get_layer_p(
      city, GRIDCITY_LAYER_IDX_TERRAIN );

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
         tile_idx = retrotile_get_tile( city, layer_terrain, x, y );
         printf( "%03d ", tile_idx );
      }
      printf( "\n" );
   }
   printf( "\n" );

   return retval;
}

void gridcity_free_blocks( struct GRIDCITY_DATA* data ) {
   struct RETROFLAT_BITMAP* blocks;
   size_t i = 0;

   if( NULL == data->blocks_h ) {
      goto cleanup;
   }
   maug_mlock( data->blocks_h, blocks );
   if( NULL == blocks ) {
      goto cleanup;
   }

   for( i = 0 ; data->blocks_sz > i ; i++ ) {
      if( retroflat_bitmap_ok( &(blocks[i]) ) ) {
         retroflat_destroy_bitmap( &(blocks[i]) );
      }
   }

   maug_munlock( data->blocks_h, blocks );
   maug_mfree( data->blocks_h );

cleanup:
   return;
}

MERROR_RETVAL gridcity_build_seed( struct RETROTILE* city ) {
   int16_t gridcity_start_x = -1;
   int16_t gridcity_start_y = -1;
   retrotile_tile_t tile_build_idx = 0;
   retrotile_tile_t tile_terrain_idx = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_LAYER* layer_build = NULL;
   struct RETROTILE_LAYER* layer_terrain = NULL;

   layer_build = retrotile_get_layer_p(
      city, GRIDCITY_LAYER_IDX_BUILDINGS );
   layer_terrain = retrotile_get_layer_p(
      city, GRIDCITY_LAYER_IDX_TERRAIN );

   while(
      (0 > gridcity_start_x && 0 > gridcity_start_y) ||
      2 > tile_build_idx
   ) {
      gridcity_start_x = rand() % city->tiles_w;
      gridcity_start_y = rand() % city->tiles_h;
      tile_build_idx = retrotile_get_tile( city, layer_build,
         gridcity_start_x, gridcity_start_y );
      tile_terrain_idx = retrotile_get_tile( city, layer_terrain,
         gridcity_start_x, gridcity_start_y );

      if( BLOCK_Z_WATER < tile_terrain_idx ) {
         debug_printf( 1, "starting at %d, %d",
            gridcity_start_x, gridcity_start_y );
         retrotile_get_tile( city, layer_build,
            gridcity_start_x, gridcity_start_y ) = 2;
      }
   }

   return retval;
}

