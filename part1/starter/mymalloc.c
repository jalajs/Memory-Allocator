#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

//======================================================================
// Global Variables
//======================================================================

#define BLOCK_SIZE sizeof(block_t)
void *base = NULL;

//======================================================================
// Struct Definitions
//======================================================================

typedef struct block{
	size_t size; // How many bytes beyond this block have been allocated in the heap
	struct block* next; // Where is the next block in your linked list
	int free; // Is this memory free?
}block_t;

//======================================================================
// Helper Functions
//======================================================================

// Find a free block of memory in our current heap
struct block *find_free_block(struct block **l, size_t size) {
	struct block *curr = base;
	while (curr && !(curr->free && curr->size >= size && curr->size - size <= 4)) {
		*l = curr;
		curr = curr->next;
	}
	return curr;
}

//======================================================================

// Request more free space to be put onto the heap
struct block *request_free_space(struct block* l, size_t size) {
	struct block *b;
	b = sbrk(0);
	void *request = sbrk(size + BLOCK_SIZE);
	if (request == (void*) -1) {
		return NULL;
	}
	if (l) {
		l->next = b;
	}
	b->size = size;
	b->next = NULL;
	b->free = 0;
	return b;	
}

//======================================================================

// Gets the pointer for the block
struct block *get_ptr(void *ptr) {
	return (struct block*)ptr - 1;
}

//======================================================================
//======================================================================

void* mymalloc(size_t s){
	struct block *b;
	// If invalid size, malloc returns NULL
	if (s <= 0) {
		return NULL;
	}
	
	// If it is the first malloc
	if (!base) {
		b = request_free_space(NULL, s);
		// If no more space, malloc returns NULL
		if (!b) {
			return NULL;
		}
		base = b;
	}
	// Else try to find free space
	else {
		// Keep track of the last element
		struct block *l = base;
		// Find the next free block
		b = find_free_block(&l, s);
		// If there was no free blocks
		if (!b) {
			// Request more memory from the OS to the heap
			b = request_free_space(l, s);
			// If no more memory to give, malloc returns NULL
			if (!b) {
				return NULL;
			}
		}
		// The block was a free block, so now it isn't
		else {
			b->free = 0;
		}
	}

	printf("malloc %zu bytes\n",s);
	
	// Returns the pointer to the actual memory
	return (b + 1);
}

//======================================================================

void* mycalloc(size_t nmemb, size_t s){
	// Calculate the new size
	size_t size = nmemb * s;
	// Get the pointer to the array
	void *ptr = mymalloc(size);
	// Fill the allocated memory with 0 values as a place holder
	memset(ptr, 0, size);
	printf("calloc %zu bytes\n", size);
	return ptr;
}
	
//======================================================================

void myfree(void *ptr){
	// If ptr is NULL
	if (!ptr) {
		return;
	}
	// Get the pointer to that block
	struct block* b_ptr = get_ptr(ptr);
	// Make sure the block is not free
	assert(b_ptr->free == 0);
	// Free the block
	b_ptr->free = 1; 
	printf("Freed %zu bytes\n", b_ptr->size);
}
	
//======================================================================
