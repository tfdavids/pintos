#ifndef CACHE_H
#define CACHE_H

#include "devices/block.h"

void cache_init (void);
void cache_read (struct block *block, block_sector_t sector, void *buffer);
void cache_write (struct block *block, block_sector_t sector,
  const void *buffer);
void cache_read_bytes (struct block *block, block_sector_t sector,
  int sector_ofs, int chunk_size, void *buffer);
void cache_write_bytes (struct block *block, block_sector_t sector,
  int sector_ofs, int chunk_size, void *buffer);
void cache_flush (void);
void cache_close (struct block *block, block_sector_t sector);

#endif /* CACHE_H */
