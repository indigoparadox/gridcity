
#ifndef GRIDCITY_H
#define GRIDCITY_H

#include <maug.h>
#include <retrotil.h>

#include "blocks.h"
#include "draw.h"

#define GRIDCITY_LAYER_IDX_TERRAIN 0
#define GRIDCITY_LAYER_IDX_BUILDINGS 1

struct GRIDCITY_DATA {
   MAUG_MHANDLE city_h;
   MAUG_MHANDLE blocks_h;
   size_t blocks_sz;
   int view_x;
   int view_y;
   retroflat_ms_t next_ms;
   uint8_t dirty;
};

MERROR_RETVAL gridcity_grow( struct RETROTILE* city );
MERROR_RETVAL gridcity_dump_terrain( struct RETROTILE* city );
void gridcity_free_blocks( struct GRIDCITY_DATA* data );
MERROR_RETVAL gridcity_build_seed( struct RETROTILE* city );

#define gridcity_idx( x, y, w ) (((y) * (w)) + (x))


#endif /* !GRIDCITY_H */

