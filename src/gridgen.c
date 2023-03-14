
#include <maug.h>

#include "gridcity.h"

#include "gridgen.h"

#define GRIDGEN_TRACE_LVL 1

MERROR_RETVAL gridgen_generate_diamond_square_iter(
   struct GRIDCITY* city, int8_t min_z, int8_t max_z, void* data
) {
   int16_t iter_x = 0,
      iter_y = 0,
      iter_depth = 0;
   int16_t corners_x[2][2];
   int16_t corners_y[2][2];
   int32_t avg = 0;
   size_t tile_idx = 0;
   struct GRIDGEN_DATA_DSQUARE data_ds_sub;
   struct GRIDGEN_DATA_DSQUARE* data_ds = NULL;
   struct GRIDCITY_TILE* tiles = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   tiles = maug_mlock( city->tiles );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles );

   if( NULL == data ) {
      data_ds = calloc( sizeof( struct GRIDGEN_DATA_DSQUARE ), 1 );
      data_ds->sect_w = city->tiles_w;
      data_ds->sect_h = city->tiles_h;
   } else {
      data_ds = (struct GRIDGEN_DATA_DSQUARE*)data;
   }
   assert( NULL != data_ds );

   /* Trivial case; end recursion. */
   if( 0 == data_ds->sect_w ) {
      debug_printf(
         GRIDGEN_TRACE_LVL, "%d return: null sector", iter_depth );
      goto cleanup;
   }

   if(
      data_ds->sect_x + data_ds->sect_w > city->tiles_w ||
      data_ds->sect_y + data_ds->sect_h > city->tiles_h
   ) {
      debug_printf(
         GRIDGEN_TRACE_LVL, "%d return: overflow sector", iter_depth );
      goto cleanup;
   }

   iter_depth = city->tiles_w / data_ds->sect_w;

   /* Generate missing corner data. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         /* Make sure corner X is in bounds. */
         corners_x[iter_x][iter_y] =
            (data_ds->sect_x - 1) + (iter_x * data_ds->sect_w);
         if( 0 > corners_x[iter_x][iter_y] ) {
            corners_x[iter_x][iter_y] += 1;
         }

         /* Make sure corner Y is in bounds. */
         corners_y[iter_x][iter_y] =
            (data_ds->sect_y - 1) + (iter_y * data_ds->sect_h);
         if( 0 > corners_y[iter_x][iter_y] ) {
            corners_y[iter_x][iter_y] += 1;
         }

         tile_idx = gridcity_idx(
            corners_x[iter_x][iter_y],
            corners_y[iter_x][iter_y],
            city->tiles_w );

         if( -1 != tiles[tile_idx].z ) {
            debug_printf(
               GRIDGEN_TRACE_LVL, "corner coord %d x %d present: %d",
               corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
               tiles[tile_idx].z );
            continue;
         }

         /* Fill in missing corner. */
         avg = min_z + (rand() % (max_z - min_z));
         debug_printf( GRIDGEN_TRACE_LVL, "missing corner coord %d x %d: rand: %d",
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y], avg );
         
         assert( min_z <= avg );

         tiles[tile_idx].z = avg;
      }
   }

   if( 2 == data_ds->sect_w && 2 == data_ds->sect_h ) {
      /* Nothing to average, this sector is just corners! */
      debug_printf( GRIDGEN_TRACE_LVL, "%d return: reached innermost point", iter_depth );
      goto cleanup;
   }
   
   /* Average corner data. */
   for( iter_y = 0 ; 2 > iter_y ; iter_y++ ) {
      for( iter_x = 0 ; 2 > iter_x ; iter_x++ ) {

         tile_idx = gridcity_idx( 
            corners_x[iter_x][iter_y],
            corners_y[iter_x][iter_y],
            city->tiles_w );
         debug_printf( GRIDGEN_TRACE_LVL, "%d: adding from coords %d x %d: %d",
            iter_depth,
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
            tiles[tile_idx].z );
         avg += tiles[tile_idx].z;
      }
   }

   avg /= 4;
   debug_printf( GRIDGEN_TRACE_LVL, "%d: avg: %d", iter_depth, avg );

   tile_idx = gridcity_idx( 
      data_ds->sect_x + (data_ds->sect_w / 2),
      data_ds->sect_y + (data_ds->sect_h / 2),
      city->tiles_w );
   if( -1 != tiles[tile_idx].z ) {
      debug_printf( GRIDGEN_TRACE_LVL, "avg already present at %d x %d!",
         data_ds->sect_x + (data_ds->sect_w / 2),
         data_ds->sect_y + (data_ds->sect_h / 2) );
   }
   tiles[tile_idx].z = avg;

   assert( 0 <= tiles[tile_idx].z );

   maug_munlock( city->tiles );

   /* Recurse into subsectors. */
   for(
      iter_y = data_ds->sect_y ;
      iter_y < (data_ds->sect_y + data_ds->sect_h) ;
      iter_y++
   ) {
      for(
         iter_x = data_ds->sect_x ;
         iter_x < (data_ds->sect_x + data_ds->sect_w) ;
         iter_x++
      ) {
         data_ds_sub.sect_x = data_ds->sect_x + iter_x;

         data_ds_sub.sect_y = data_ds->sect_y + iter_y;

         data_ds_sub.sect_w = data_ds->sect_w / 2;
         data_ds_sub.sect_h = data_ds->sect_h / 2;

         debug_printf( GRIDGEN_TRACE_LVL, "%d: child sector at %d x %d, %d wide",
            iter_depth,
            data_ds_sub.sect_x, data_ds_sub.sect_y, data_ds_sub.sect_w );

         gridgen_generate_diamond_square_iter(
            city, min_z, max_z, &data_ds_sub );
      }
   }

   debug_printf( GRIDGEN_TRACE_LVL, "%d return: all sectors complete", iter_depth );

cleanup:

   if( NULL == data && NULL != data_ds ) {
      /* We must've alloced this internally. */
      free( data_ds );
   }

   return retval;
}

#if 0
void gridgen_generate_voronoi_iter(
   struct GRIDCITY* city, int8_t min_z, int8_t max_z, void* data
) {
   int16_t x = 0,
      y = 0,
      offset_x = 0,
      offset_y = 0,
      finished = 0,
      pass_done = 0;
   struct GRIDGEN_DATA_VORONOI* data_v = NULL;

   if( NULL == data ) {
      data_v = calloc( sizeof( struct GRIDGEN_DATA_VORONOI ), 1 );
      data_v->spb = GRIDGEN_VORONOI_DEFAULT_SPB;
      data_v->drift = GRIDGEN_VORONOI_DEFAULT_DRIFT;
   } else {
      data_v = (struct GRIDGEN_DATA_VORONOI*)data;
   }

   /* Generate the initial sector starting points. */
   for( y = 0 ; city->tiles_h > y ; y += data_v->spb ) {
      for( x = 0 ; city->tiles_w > x ; x += data_v->spb ) {
         offset_x = x + ((data_v->drift * -1) + (rand() % data_v->drift));
         offset_y = y + ((data_v->drift * -1) + (rand() % data_v->drift));

         /* Clamp sector offsets onto map borders. */
         if( 0 > offset_x ) {
            offset_x = 0;
         }
         if( offset_x >= city->tiles_w ) {
            offset_x = city->tiles_w - 1;
         }
         if( 0 > offset_y ) {
            offset_y = 0;
         }
         if( offset_y >= city->tiles_h ) {
            offset_y = city->tiles_h - 1;
         }

         tiles[(offset_y * city->tiles_w) + offset_x].z =
            min_z + (rand() % max_z);
      }
   }

   /* Grow the sector starting points. */
   while( !finished ) {
      finished = 1;
      printf( "pass\n" );
      for( y = 0 ; city->tiles_h > y ; y++ ) {
         pass_done = 0;
         for( x = 0 ; city->tiles_w > x && !pass_done ; x++ ) {
            if( -1 != tiles[(y * city->tiles_w) + x] ) {
               continue;
            }

            /* This pass still did work, so not finished yet! */
            finished = 0;
            
            if( /* y + 1 */
               city->tiles_h - 1 > y && -1 != tiles[((y + 1) * city->tiles_w) + x]
            ) {
               tiles[(y * city->tiles_w) + x] = tiles[((y + 1) * city->tiles_w) + x];
            
            } else if( /* x - 1 */
               0 < x && -1 != tiles[(y * city->tiles_w) + (x - 1)]
            ) {
               tiles[(y * city->tiles_w) + x] = tiles[(y * city->tiles_w) + (x - 1)];
            
            } else if( /* x + 1 */
               city->tiles_w - 1 > x && -1 != tiles[(y * city->tiles_w) + (x + 1)]
            ) {
               tiles[(y * city->tiles_w) + x] = tiles[(y * city->tiles_w) + (x + 1)];
            
            } else if( /* y - 1 */
               0 < y && -1 != tiles[((y - 1) * city->tiles_w) + x]
            ) {
               tiles[(y * city->tiles_w) + x] = tiles[((y - 1) * city->tiles_w) + x];

#ifdef GRIDGEN_VORONOI_DIAGONAL
            } else if( /* y + 1, x + 1 */
               city->tiles_w - 1 > x && city->tiles_h - 1 > y &&
               -1 != tiles[((y + 1) * city->tiles_w) + (x + 1)]
            ) {
               tiles[(y * city->tiles_w) + x] = tiles[((y + 1) * city->tiles_w) + (x + 1)];
            
            } else if( /* y + 1, x - 1 */
               0 < x && city->tiles_h - 1 > y &&
               -1 != tiles[((y + 1) * city->tiles_w) + (x - 1)]
            ) {
               tiles[(y * city->tiles_w) + x] = tiles[((y + 1) * city->tiles_w) + (x - 1)];
            
            } else if( /* y - 1, x + 1 */
               city->tiles_w - 1 > x && 0 < y &&
               -1 != tiles[((y - 1) * city->tiles_w) + (x + 1)]
            ) {
               tiles[(y * city->tiles_w) + x] = tiles[((y - 1) * city->tiles_w) + (x + 1)];
            
            } else if( /* y - 1, x - 1 */
               0 < x && 0 < y &&
               -1 != tiles[((y - 1) * city->tiles_w) + (x - 1)]
            ) {
               tiles[(y * city->tiles_w) + x] = tiles[((y - 1) * city->tiles_w) + (x - 1)];
#endif /* GRIDGEN_VORONOI_DIAGONAL */

            } else {
               /* Nothing done, so skip pass_done below. */
               continue;
            }

            pass_done = 1;
         }
      }
   }


   if( NULL == data ) {
      /* We must've alloced this internally. */
      free( data_v );
   }

   return;
}
#endif

