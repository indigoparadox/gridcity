
#ifndef GRIDCITY_H
#define GRIDCITY_H

#include <maug.h>

struct GRIDCITY_BLOCK {
   struct RETROFLAT_BITMAP bmp;
};

struct GRIDCITY_TILE {
   int8_t terrain;
   int8_t z;
   int8_t building;
};

struct GRIDCITY {
   MAUG_MHANDLE tiles;
   size_t tiles_w;
   size_t tiles_h;
   MAUG_MHANDLE blocks;
   size_t blocks_sz;
};

struct GRIDCITY_DATA {
   struct GRIDCITY city;
   int view_x;
   int view_y;
   int next_ms;
};

MERROR_RETVAL gridcity_draw_iso( struct GRIDCITY_DATA* data );
MERROR_RETVAL gridcity_grow( struct GRIDCITY* city );
MERROR_RETVAL gridcity_dump_terrain( struct GRIDCITY* city );
MERROR_RETVAL gridcity_init( struct GRIDCITY* city );
void gridcity_free( struct GRIDCITY* city );
MERROR_RETVAL gridcity_build_seed( struct GRIDCITY* city );

#define gridcity_idx( x, y, w ) (((y) * (w)) + (x))


#endif /* !GRIDCITY_H */

