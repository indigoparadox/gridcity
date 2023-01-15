
#ifndef GRIDCITY_H
#define GRIDCITY_H

#define gridcity_idx( x, y, w ) (((y) * (w)) + (x))

#define gridcity_generate_terrain( map, max_z, map_w, map_h ) \
   gridcity_generate_terrain_iter( \
      map, max_z, map_w, map_h, 0, 0, map_w - 1, map_h - 1 );

void gridcity_dump_terrain( signed char* map, int map_w, int map_h );
void gridcity_generate_terrain_iter(
   signed char* map, int max_z, int map_w, int map_h,
   int sect_x, int sect_y, int sect_w, int sect_h );
void gridcity_grow(
   signed char* map, signed char* buildings, int map_w, int map_h );

/*
void gridcity_generate_terrain( char* map, int max_z, int map_w, int map_h );
*/

#endif /* !GRIDCITY_H */

