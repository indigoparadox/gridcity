
#include <stdio.h>

#include <maug.h>

#include "draw.h"

#include "blocks.h"

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

void draw_city(
   int view_x, int view_y,
   signed char* map, signed char* buildings,
   int map_w, int map_h, struct RETROFLAT_BITMAP* blocks
) {
   int x = -1,
      y = 2,
      px_x = 0,
      px_y = 0,
      block_z = 0,
      block_id = 0,
      offset_y = 100;

   for( y = 0 ; map_h > y ; y++ ) {

      /* Draw X coordinates backwards to fix overlapping. */
      for( x = map_w - 1 ; 0 <= x ; x-- ) {

         /* TODO: Optimize drawing off-screen out. */
         draw_grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );

         block_z = block_get_z( x, y, map, map_w );

         /* If the block is water, it's 0, if it's grass, it's 1, otherwise it's
          * a building.
          */
         if( BLOCK_Z_WATER >= block_z ) {
            block_id = 0;
         } else {
            if( 0 == buildings[(y * map_w) + x] ) {
               block_id = 1;
            } else {
               block_id = buildings[(y * map_w) + x];
            }
         }

         retroflat_blit_bitmap(
            NULL,
            &(blocks[block_id]),
            0, 0,
            px_x,
            offset_y +
               (BLOCK_Z_WATER >= block_z ?
                  px_y - BLOCK_Z_WATER : px_y - block_z),
            BLOCK_PX_W, BLOCK_PX_H );
      }
   }
}

