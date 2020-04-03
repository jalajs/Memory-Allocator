#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>
//======================================================================
// Global Variables
//======================================================================

int sched_getcpu();

#define BLOCK_SIZE sizeof(block_t)

#define THREAD_NUM 48

//======================================================================
// Struct Definitions
//======================================================================

typedef struct block{
	size_t size; // How many bytes beyond this block have been allocated in the heap
	struct block* next; // Where is the next block in your linked list
	int free; // Is this memory free?
}block_t;

//======================================================================
// Initializing Free List and Lock List
//======================================================================

block_t* free_list[THREAD_NUM];

pthread_mutex_t lock_list[THREAD_NUM] = {PTHREAD_MUTEX_INITIALIZER};

//======================================================================
// Helper Functions
//======================================================================

// Find a free block of memory in our current heap
struct block *find_free_block(struct block **l, size_t size) {
	struct block *curr = free_list[sched_getcpu() % THREAD_NUM];
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
	if (size < 4096) {
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
	else {
		b = mmap(NULL, size, PROT_WRITE, MAP_ANONYMOUS, 0, 0);
		if (b == (void*) -1) {
			return NULL;
		}
		int leftover = size % sysconf(_SC_PAGE_SIZE);
		if (leftover == 0) {
			if (l) {
				l->next = b;
			}
			b->size = size;
			b->next = NULL;
			b->free = 0;
			return b;
		}
		else {
			int free_space = sysconf(_SC_PAGE_SIZE) - leftover;
			struct block *free_block;
			if (l) {
				l->next = b;
			}
			b->size = size;
			free_block->size = free_space;
			b->next = free_block;
			free_block->next = NULL;
			b->free = 0;
			free_block->free = 1;
			return free_block;
		}
	}	
}

//======================================================================

// Gets the pointer for the block
struct block *get_ptr(void *ptr) {
	return (struct block*)ptr - 1;
}

//======================================================================
//======================================================================

void* mymalloc(size_t s){
	struct block *base = free_list[sched_getcpu() % THREAD_NUM];
	pthread_mutex_t mutex = lock_list[sched_getcpu() % THREAD_NUM];
	pthread_mutex_lock(&mutex);
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
	pthread_mutex_unlock(&mutex);	
	// Returns the pointer to the actual memory
	return (b + 1);
}

//======================================================================

void* mycalloc(size_t nmemb, size_t s){
	pthread_mutex_t mutex = lock_list[sched_getcpu() % THREAD_NUM];
	pthread_mutex_lock(&mutex);
	// Calculate the new size
	size_t size = nmemb * s;
	// Get the pointer to the array
	void *ptr = mymalloc(size);
	// Fill the allocated memory with 0 values as a place holder
	memset(ptr, 0, size);
	printf("calloc %zu bytes\n", size);
	pthread_mutex_unlock(&mutex);
	return ptr;
}
	
//======================================================================

void myfree(void *ptr){
	pthread_mutex_t mutex = lock_list[sched_getcpu() % THREAD_NUM];
	pthread_mutex_lock(&mutex);
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
	pthread_mutex_unlock(&mutex);
}
	
//======================================================================
