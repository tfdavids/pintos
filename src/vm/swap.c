#include <bitmap.h>
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

#include "devices/block.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

static struct block *swap_device;
static struct bitmap *swap_slots;
static struct lock swap_slots_lock;
static size_t sectors_per_page = PGSIZE / BLOCK_SECTOR_SIZE;
size_t num_swap_slots;

void swap_init (void)
{
  lock_init (&swap_slots_lock);
  swap_device = block_get_role (BLOCK_SWAP);
  if (swap_device == NULL)
    PANIC ("No swap device found, can't initialize swap");

  num_swap_slots = block_size (swap_device) / sectors_per_page;
  swap_slots = bitmap_create (num_swap_slots);
  if (swap_slots == NULL)
    PANIC ("bitmap creation failed--swap device is too large");
  lock_acquire (&swap_slots_lock);
  bitmap_set_all (swap_slots, false);
  lock_release (&swap_slots_lock);

}



size_t swap_write_page (void *upage)
{
  lock_acquire (&swap_slots_lock);
  size_t free_slot_index = bitmap_scan_and_flip (swap_slots, 0, 1, false);
  lock_release (&swap_slots_lock);

  if (free_slot_index == BITMAP_ERROR)
    PANIC ("swap is full");
  printf("free_slot_index %d\n", free_slot_index);
  size_t i;
  for (i = 0; i < sectors_per_page; i++)
    {
      //printf("swap is %d, upage = %p, i = %d\n", free_slot_index, upage, i);
      block_write (swap_device,
                   free_slot_index * sectors_per_page + i,
                   upage + i * BLOCK_SECTOR_SIZE);
    }
  
  return free_slot_index;
}


bool swap_load_page (size_t slot_index, void *upage)
{
  if (slot_index > num_swap_slots ||
      !bitmap_test(swap_slots, slot_index))
    {
      return false;
    }
  size_t i;
  for (i = 0; i < sectors_per_page; i++)
    {
      block_read (swap_device,
                  slot_index * sectors_per_page + i,
                  upage + i * BLOCK_SECTOR_SIZE);
    }
  lock_acquire (&swap_slots_lock);
  bitmap_set (swap_slots, slot_index, false);
  lock_release (&swap_slots_lock);

  return true;
}
