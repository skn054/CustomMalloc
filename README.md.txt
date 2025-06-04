# Custom sbrk-based Memory Allocator in C

This project is a simple implementation of a custom memory allocator in C. It mimics the basic functionality of `malloc`, `free`, `calloc`, and `realloc` by directly managing memory requested from the operating system using the `sbrk()` system call.

This allocator is primarily an educational exercise to understand the fundamentals of dynamic memory management, including heap organization, block metadata, free list management, and basic thread safety.

## Features

*   **`custom_malloc(size_t size)`**: Allocates a block of memory of the specified size.
*   **`custom_free(void *ptr)`**: Frees a previously allocated block of memory.
*   **`custom_calloc(size_t n_num, size_t size)`**: Allocates memory for an array of `n_num` elements of `size` bytes each and initializes all bytes to zero.
*   **`custom_realloc(void *old_block, size_t new_size)`**: Changes the size of the memory block pointed to by `old_block`.
*   **Heap Management**: Uses `sbrk()` to request memory from the OS and extend the program's data segment (heap).
*   **Block Metadata**: Each memory block is preceded by a header (`header_t`) containing:
    *   `size`: The size of the usable payload.
    *   `is_free`: A flag indicating if the block is free (1) or in use (0).
    *   `next`: A pointer to the next block in a singly-linked list.
*   **First-Fit Allocation**: When `custom_malloc` is called, it searches the linked list of blocks for the first free block that is large enough to satisfy the request.
*   **Heap Shrinking**: `custom_free` attempts to return memory to the OS via `sbrk()` if the block being freed is the last one on the heap.
*   **Thread Safety**: Basic thread safety is implemented using a global `pthread_mutex_t` (`global_malloc_lock`) to protect critical sections in allocation and deallocation routines.