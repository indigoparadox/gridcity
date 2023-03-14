
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

