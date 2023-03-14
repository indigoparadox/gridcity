
#ifndef GRIDGEN_H
#define GRIDGEN_H

#include <mtypes.h>

/**
 * \brief Parameter data structure for retrogam_generate_diamond_square_iter().
 */
struct GRIDGEN_DATA_DSQUARE {
   int16_t sect_x;
   int16_t sect_y;
   int16_t sect_w;
   int16_t sect_h;
};

struct GRIDGEN_DATA_VORONOI {
   int16_t spb;
   int16_t drift;
};

/*! \brief Default sector per blocks if not specified.  */
#ifndef GRIDGEN_VORONOI_DEFAULT_SPB
#  define GRIDGEN_VORONOI_DEFAULT_SPB 8
#endif /* !GRIDGEN_VORONOI_DEFAULT_SPB */

#ifndef GRIDGEN_VORONOI_DEFAULT_DRIFT
#  define GRIDGEN_VORONOI_DEFAULT_DRIFT 4
#endif /* !GRIDGEN_VORONOI_DEFAULT_DRIFT */

typedef void (*gridgen_generator)(
   int8_t* map, int8_t min_z, int8_t max_z, int16_t map_w, int16_t map_h,
   void* data );

#define gridgen_generate_diamond_square( city, min_z, max_z ) \
   gridgen_generate_diamond_square_iter( city, min_z, max_z, NULL );

#define gridgen_generate_voronoi( map, min_z, max_z, map_w, map_h ) \
   gridgen_generate_voronoi_iter( \
      map, min_z, max_z, map_w, map_h, NULL );

/**
 * \brief Diamond-square terrain generator.
 *
 * This generator creates random numbers in the corners of progressively
 * smaller map sectors while averaging between them for continuity.
 */
MERROR_RETVAL gridgen_generate_diamond_square_iter(
   struct GRIDCITY* city, int8_t min_z, int8_t max_z, void* data );

void gridgen_generate_voronoi_iter(
   struct GRIDCITY* city, int8_t min_z, int8_t max_z, void* data );

#endif /* !GRIDGEN_H */

