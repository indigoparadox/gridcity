
#ifndef BLOCK_H
#define BLOCK_H

#ifdef DOS
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

#define PATH_BLOCKS "blocks"

#define BLOCK_LIST( f ) f( 0, "water" ) f( 1, "grass" )

#define BLOCK_MAX 2

#define BLOCK_EXT "bmp"

#define BLOCK_PX_W 16
#define BLOCK_PX_H 16
#define BLOCK_PX_OFFSET 8

#define BLOCK_LIST_FILENAMES( idx, filename ) filename,

#ifdef BLOCKS_C
const char* gc_block_filenames[] = {
   BLOCK_LIST( BLOCK_LIST_FILENAMES )
};
#else
extern const char* gc_block_filenames[];
#endif /* BLOCKS_C */

#endif /* !BLOCK_H */

