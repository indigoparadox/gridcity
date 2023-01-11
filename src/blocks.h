
#ifndef BLOCK_H
#define BLOCK_H

#ifdef DOS
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

#define PATH_BLOCKS "blocks"

#define BLOCK_LIST( f ) f( 0, "water" ) f( 1, "grass" ) f( 2, "house" ) f( 3, "wrhouse" )

#define BLOCK_MAX 4

#define BLOCK_EXT "bmp"

#define BLOCK_PX_W 16
#define BLOCK_PX_H 64
#define BLOCK_PX_OFFSET 8

#define BLOCK_MAX_Z 100
#define BLOCK_Z_DIVISOR 5
#define BLOCK_Z_WATER 8

#define block_get_z( x, y, map, map_w ) \
   (((map[((y) * (map_w)) + (x)])) / BLOCK_Z_DIVISOR)

#define block_get_id( b )

#define BLOCK_LIST_FILENAMES( idx, filename ) filename,

#ifdef BLOCKS_C
const char* gc_block_filenames[] = {
   BLOCK_LIST( BLOCK_LIST_FILENAMES )
};
#else
extern const char* gc_block_filenames[];
#endif /* BLOCKS_C */

#endif /* !BLOCK_H */

