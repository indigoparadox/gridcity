
#ifndef DRAW_H
#define DRAW_H

#include <retroflt.h>

void draw_grid_from_screen_coords(
   int* tile_x, int* tile_y, int mouse_c_x, int mouse_c_y,
   int view_x, int view_y );

void draw_grid_to_screen_coords(
   int* screen_px_x, int* screen_px_y, int grid_px_x, int grid_px_y,
   int view_x, int view_y );

void draw_city(
   int view_x, int view_y,
   signed char* map, int map_w, int map_h, struct RETROFLAT_BITMAP* blocks );

#endif /* !DRAW_H */

