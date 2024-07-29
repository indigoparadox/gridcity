
#ifndef DRAW_H
#define DRAW_H

#include <retroflt.h>

void draw_grid_from_screen_coords(
   int* tile_x, int* tile_y, int mouse_c_x, int mouse_c_y,
   int view_x, int view_y );

void draw_grid_to_screen_coords(
   int* screen_px_x, int* screen_px_y, int grid_px_x, int grid_px_y,
   int view_x, int view_y );

MERROR_RETVAL draw_init_blocks(
   MAUG_MHANDLE* p_blocks_h, size_t* p_blocks_sz );

MERROR_RETVAL draw_city_iso(
   struct RETROTILE* city, int view_x, int view_y, int avg,
   struct RETROFLAT_BITMAP* blocks, size_t blocks_sz, int offset_y );

#endif /* !DRAW_H */

