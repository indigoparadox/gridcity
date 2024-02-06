
#include <stdio.h>

#include <maug.h>

#include "gridcity.h"

void draw_grid_from_screen_coords(
   int* tile_x, int* tile_y, int mouse_c_x, int mouse_c_y,
   int view_x, int view_y
) {
   /* Not 100% confident in these, kind of arrived by trial-and-error. */
   mouse_c_x = (mouse_c_x - view_x) / (BLOCK_PX_W / 2);
   mouse_c_y = (mouse_c_y - view_y) / (-1 * (BLOCK_PX_H / 4));
   *tile_x = ((mouse_c_x + mouse_c_y) / 2) + 1;
   *tile_y = ((mouse_c_x - mouse_c_y) / 2) - 1;
}

void draw_grid_to_screen_coords(
   int* screen_px_x, int* screen_px_y, int grid_px_x, int grid_px_y,
   int view_x, int view_y
) {
   /* Not 100% confident in these, kind of arrived by trial-and-error. */
   *screen_px_x =
      view_x + ((grid_px_x * BLOCK_PX_W / 2) + (grid_px_y * BLOCK_PX_W / 2));
   *screen_px_y = 
      view_y + ((grid_px_y * BLOCK_PX_OFFSET / 2) -
      (grid_px_x * BLOCK_PX_OFFSET / 2) );
}

MERROR_RETVAL draw_init_blocks(
   MAUG_MHANDLE* p_blocks_h, size_t* p_blocks_sz
) {
   struct RETROFLAT_BITMAP* blocks = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   /* Allocate and load blocks. */
   *p_blocks_sz = BLOCK_MAX;
   *p_blocks_h =
      maug_malloc( sizeof( struct RETROFLAT_BITMAP ), *p_blocks_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_blocks_h );

   maug_mlock( *p_blocks_h, blocks );
   maug_cleanup_if_null_alloc( struct RETROFLAT_BITMAP*, blocks );

   for( i = 0 ; *p_blocks_sz > i ; i++ ) {
      retval = retroflat_load_bitmap( gc_block_filenames[i], &(blocks[i]) );
      maug_cleanup_if_not_ok();
   }

cleanup:
   
   if( NULL != blocks ) {
      maug_munlock( *p_blocks_h, blocks );
   }

   return retval;
}

MERROR_RETVAL draw_city_iso(
   struct RETROTILE* city, int view_x, int view_y,
   struct RETROFLAT_BITMAP* blocks, size_t blocks_sz, int offset_y
) {
   int x = -1,
      y = 2,
      px_x = 0,
      px_y = 0,
      block_id = 0;
   struct RETROTILE_LAYER* layer_terrain = NULL;
   struct RETROTILE_LAYER* layer_build = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   retrotile_tile_t tile_build_idx = 0;
   retrotile_tile_t tile_terrain_idx = 0;

   layer_terrain = retrotile_get_layer_p(
      city, GRIDCITY_LAYER_IDX_TERRAIN );
   layer_build = retrotile_get_layer_p(
      city, GRIDCITY_LAYER_IDX_BUILDINGS );

   for( y = 0 ; city->tiles_h > y ; y++ ) {

      /* Draw X coordinates backwards to fix overlapping. */
      for( x = city->tiles_w - 1 ; 0 <= x ; x-- ) {

         tile_build_idx = retrotile_get_tile( city, layer_build, x, y );
         tile_terrain_idx = retrotile_get_tile( city, layer_terrain, x, y );

         /* TODO: Optimize drawing off-screen out. */
         draw_grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );

         /* If the block is water, it's 0, if it's grass, it's 1, otherwise
          * it's a building.
          */
#ifndef GRIDCITY_NO_WATER
         if( BLOCK_Z_WATER >= tile_terrain_idx ) {
            block_id = 0;
         } else {
#endif /* !GRIDCITY_NO_WATER */
            if( 0 == tile_build_idx ) {
               block_id = 1;
            } else {
               block_id = tile_build_idx;
            }
#ifndef GRIDCITY_NO_WATER
         }
#endif /* !GRIDCITY_NO_WATER */

         retroflat_blit_bitmap(
            NULL,
            &(blocks[block_id]),
            0, 0,
            px_x,
#ifndef GRIDCITY_NO_WATER
            BLOCK_Z_WATER >= tile_terrain_idx ?
               offset_y + px_y - BLOCK_Z_WATER :
#endif /* !GRIDCITY_NO_WATER */
               offset_y + px_y - tile_terrain_idx,
            /* offset_y + px_y - tile_terrain_idx, */
            /* offset_y + BLOCK_Z_WATER >= tile_terrain_idx ?
               px_y - BLOCK_Z_WATER : px_y - tile_terrain_idx, */
            BLOCK_PX_W, BLOCK_PX_H );
      }
   }

   return retval;
}



