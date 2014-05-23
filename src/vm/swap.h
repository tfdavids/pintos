#ifndef VM_SWAP_H
#define VM_SWAP_H

void swap_init (void);

size_t swap_write_page (void *kpage);

bool swap_load_page (size_t swap_index, void *kpage);

#endif /* VM_SWAP_H */


