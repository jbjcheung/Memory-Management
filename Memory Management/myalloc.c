#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "myalloc.h"
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct nodeStruct* List_createNode(void* ptr)
{
	nodeStruct* newnode = (nodeStruct*)malloc(sizeof(nodeStruct));
	newnode -> ptr = ptr;
	newnode -> next = NULL;
	return newnode;
}

void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node){
	if(*headRef == NULL){
		*headRef = node;
		return;
	}
	node -> next = *headRef;
	*headRef = node;
	
}

void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node){
	if(*headRef == NULL){
		*headRef = node;
		return;
	}
	nodeStruct* display = *headRef;
	while(display -> next != NULL){
		display = display -> next;
	}
	display -> next = node;
}

void List_sort (struct nodeStruct **headRef)
{
	while (doSinglePassOnSort(headRef)) {
		// Do nothing: work done in loop condition.
	}
}
static _Bool doSinglePassOnSort(struct nodeStruct **headRef)
{
	_Bool didSwap = false;
	while (*headRef != NULL) {
		struct nodeStruct *nodeA = *headRef;
		// If we don't have 2 remaining elements, nothing to swap.
		if (nodeA->next == NULL) {
			break;
		}
		struct nodeStruct *nodeB = nodeA->next;

		// Swap needed?
		if ((char*)nodeA->ptr > (char*)nodeB->ptr){
			swapElements(headRef, nodeA, nodeB);
			didSwap = true;
		}

		// Advance to next elements
		headRef = &((*headRef)->next);
	}
	return didSwap;
}
static void swapElements(struct nodeStruct **previous,
		struct nodeStruct *nodeA,
		struct nodeStruct *nodeB)
{
	*previous = nodeB;
	nodeA->next = nodeB->next;
	nodeB->next = nodeA;
}

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
	struct nodeStruct* allocatedlist;
	struct nodeStruct* freelist;
	
    // Some other data members you want, 
    // such as lists to record allocated/free memory
};


struct Myalloc myalloc;

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    myalloc.aalgorithm = _aalgorithm;
    myalloc.size = _size;
    myalloc.memory = malloc((size_t)myalloc.size);
	memset(myalloc.memory, 0, (size_t)myalloc.size);
	
	myalloc.allocatedlist = NULL;
	myalloc.freelist = NULL;
	
	
	
	myalloc.memory = myalloc.memory;
	nodeStruct *node1 = List_createNode(myalloc.memory);
	List_insertTail(&myalloc.freelist, node1);
	*(int *)node1 -> ptr = _size;
	
	
	
    // Add some other initialization 
}

void destroy_allocator() {
	
	
	
	// Frees the allocated list
	nodeStruct* allocated = myalloc.allocatedlist;
	nodeStruct* temp = NULL;
	while(allocated != NULL){
		if(allocated == NULL){
			continue;
		}
		temp = allocated;
		allocated = allocated -> next;
		free(temp);
	}
	
	// Frees the free list
	nodeStruct* freelist = myalloc.freelist;
	temp = NULL;
	while(freelist != NULL){
		if(freelist == NULL){
			continue;
		}
		temp = freelist;
		freelist = freelist -> next;
		free(temp);
		
	}
	free(myalloc.memory);
	
	
    // free other dynamic allocated memory to avoid memory leak
}

void* allocate(int _size) {
	
	pthread_mutex_lock(&mutex);
    void* ptr = NULL;
	void* sizeptr = NULL;
	
	
	if(myalloc.aalgorithm == FIRST_FIT){
		nodeStruct *display = myalloc.freelist;
		
	
		
		while(display != NULL){
			if(display == NULL){
				return NULL;
			}
			int freesize = *(int *)display->ptr;
			if(freesize >= _size + 8){
				//Free size is equal to allocated size
				if(*(int *)display->ptr == _size + 8){
					//First free node is big enough
					if(myalloc.freelist == display){
						nodeStruct *temp = myalloc.freelist;
						myalloc.freelist = myalloc.freelist -> next;
					
						// Saves the size into memory
						
						
						*(int *)temp -> ptr = _size + 8;
						ptr = temp -> ptr;
						
						// Creates an allocated node
						nodeStruct *node1 = List_createNode(ptr);
						List_insertTail (&myalloc.allocatedlist, node1);
					
						// Prepares the return ptr
						ptr = ptr + 8;
						free(temp);
					
						return ptr;
					}
					nodeStruct *display2 = myalloc.freelist;
					while(display2 -> next != display){
						display2 = display2 -> next;
					}
					nodeStruct *temp = display2 -> next;
					display2 -> next = display2 -> next -> next;
					
					// Saves the size into memory
					*(int *)temp -> ptr = _size + 8;
					ptr = temp -> ptr;
						
					// Creates an allocated node
					nodeStruct *node1 = List_createNode(ptr);
					List_insertTail (&myalloc.allocatedlist, node1);
					
					// Prepares the return ptr
					ptr = ptr + 8;
					free(temp);
					
					return ptr;
				
				
				}	
				// Saves the size into memory
				*(int *)display -> ptr = _size + 8;
				ptr = display -> ptr;
						
				// Creates an allocated node
				nodeStruct *node1 = List_createNode(ptr);
				List_insertTail (&myalloc.allocatedlist, node1);
					
				// Prepares the return ptr
				ptr = ptr + 8;
				 
				
				// Shrinks free node
				display -> ptr = display -> ptr + _size + 8;
				*(int *)display -> ptr = freesize - _size - 8;
				
				
			
				return ptr;
				
			}
			
			display = display -> next;
		}
		return ptr;
	}
	
	if(myalloc.aalgorithm == WORST_FIT){
		nodeStruct *display = myalloc.freelist;
		nodeStruct *largestfree = NULL;
		while(display != NULL){
			if(display == NULL){
				continue;
			}
			if(largestfree == NULL){
				if(*(int *)display -> ptr >= _size + 8){
					largestfree = display;
				}
			}
			if(largestfree != NULL){
				if(*(int *)display -> ptr > *(int *)largestfree -> ptr){
					largestfree = display;
				}
			}
			display = display -> next;
			
		}
		
		if(largestfree == NULL){
			return NULL;
		}
		int freesize = *(int *)largestfree->ptr;
		if(freesize >= _size + 8){
			//Free size is equal to allocated size
			if(*(int *)largestfree->ptr == _size + 8){
				//First free node is big enough
				if(myalloc.freelist == largestfree){
					nodeStruct *temp = myalloc.freelist;
					myalloc.freelist = myalloc.freelist -> next;
					
					// Saves the size into memory
					*(int *)temp -> ptr = _size + 8;
					ptr = temp -> ptr;
						
					// Creates an allocated node
					nodeStruct *node1 = List_createNode(ptr);
					List_insertTail (&myalloc.allocatedlist, node1);
					
					// Prepares the return ptr
					ptr = ptr + 8;
					free(temp);
					
					return ptr;
				}
				nodeStruct *display2 = myalloc.freelist;
				while(display2 -> next != largestfree){
					display2 = display2 -> next;
				}
				nodeStruct *temp = display2 -> next;
				display2 -> next = display2 -> next -> next;
			
				// Saves the size into memory
				*(int *)temp -> ptr = _size + 8;
				ptr = temp -> ptr;
						
				// Creates an allocated node
				nodeStruct *node1 = List_createNode(ptr);
				List_insertTail (&myalloc.allocatedlist, node1);
					
				// Prepares the return ptr
				ptr = ptr + 8;
				free(temp);
					
				return ptr;
			
				
			}	
			// Saves the size into memory
			*(int *)largestfree -> ptr = _size + 8;
			ptr = largestfree -> ptr;
			
			// Creates an allocated node
			nodeStruct *node1 = List_createNode(ptr);
			List_insertTail (&myalloc.allocatedlist, node1);
			
			// Sets up the ptr where the thing is gonna be saved
			ptr = ptr + 8;

			// Shrinks free node
			largestfree -> ptr = largestfree -> ptr + _size + 8;
			*(int *)largestfree -> ptr = freesize - _size - 8;
			
			
			return ptr;
				
		}
		
		
	return ptr;
	}
	
	if(myalloc.aalgorithm == BEST_FIT){
		nodeStruct *display = myalloc.freelist;
		nodeStruct *smallestfree = NULL;
		while(display != NULL){
			if(display == NULL){
				continue;
			}
			if(smallestfree == NULL){
				if(*(int *)display -> ptr >= _size + 8){
					smallestfree = display;
				}
			}
			if(smallestfree != NULL && *(int *)display -> ptr >= _size + 8){
				if(*(int *)display -> ptr < *(int *)smallestfree -> ptr){
					smallestfree = display;
				}
			}
			display = display -> next;
			
		}
		
		if(smallestfree == NULL){
			return NULL;
		}
		int freesize = *(int *)smallestfree->ptr;
		if(freesize >= _size + 8){
			//Free size is equal to allocated size
			if(*(int *)smallestfree->ptr == _size + 8){
				//First free node is big enough
				if(myalloc.freelist == smallestfree){
					nodeStruct *temp = myalloc.freelist;
					myalloc.freelist = myalloc.freelist -> next;
					
					// Saves the size into memory
					*(int *)temp -> ptr = _size + 8;
					ptr = temp -> ptr;
						
					// Creates an allocated node
					nodeStruct *node1 = List_createNode(ptr);
					List_insertTail (&myalloc.allocatedlist, node1);
					
					// Prepares the return ptr
					ptr = ptr + 8;
					free(temp);
					
					return ptr;
				}
				nodeStruct *display2 = myalloc.freelist;
				while(display2 -> next != smallestfree){
					display2 = display2 -> next;
				}
				nodeStruct *temp = display2 -> next;
				display2 -> next = display2 -> next -> next;
			
				// Saves the size into memory
				*(int *)temp -> ptr = _size + 8;
				ptr = temp -> ptr;
						
				// Creates an allocated node
				nodeStruct *node1 = List_createNode(ptr);
				List_insertTail (&myalloc.allocatedlist, node1);
					
				// Prepares the return ptr
				ptr = ptr + 8;
				free(temp);
					
				return ptr;
			
				
			}	
			// Saves the size into memory
			
			*(int *)smallestfree -> ptr = _size + 8;
			ptr = smallestfree -> ptr;
			
			// Creates an allocated node
			nodeStruct *node1 = List_createNode(ptr);
			List_insertTail (&myalloc.allocatedlist, node1);
			
			// Sets up the ptr where the thing is gonna be saved
			ptr = smallestfree -> ptr + 8;

			// Shrinks free node
			smallestfree -> ptr = smallestfree -> ptr + _size + 8;
			*(int *)smallestfree -> ptr = freesize - _size - 8;
			
			
			return ptr;
				
		}
		
		
	return ptr;
	}
	pthread_mutex_unlock(&mutex);
	
}

void deallocate(void* _ptr) {
	
	pthread_mutex_lock(&mutex);
    assert(_ptr != NULL);
	
	char size = (char*)myalloc.allocatedlist -> ptr;
	myalloc.memory = myalloc.allocatedlist -> ptr;
	int i = *(int *)myalloc.memory;

	nodeStruct *display = myalloc.allocatedlist;
	while(display != NULL){
		
		if(display -> ptr + 8 == _ptr){
			nodeStruct *node1 = List_createNode(display -> ptr);
			List_insertTail(&myalloc.freelist, node1);
			
			
			//If the first allocated node was freed
			if(myalloc.allocatedlist -> ptr + 8 == _ptr){
				myalloc.allocatedlist = myalloc.allocatedlist -> next;
				// Coalesce free blocks that are adjacent
				nodeStruct *coalesce1 = myalloc.freelist;
				while(coalesce1 != NULL){
					
					if(coalesce1 == NULL){
						continue;
					}
					
					char d = (char*)node1 -> ptr;
					int coalsize1 = *(int *)node1 -> ptr;
					char c = (char*)node1 -> ptr - 8;
					
					if(c + coalsize1 <= myalloc.size){
					
					if((node1 -> ptr + coalsize1) == coalesce1 -> ptr){
						int i = (*(int *)coalesce1 -> ptr);
						int j = (*(int *)node1 -> ptr);
						int k = i + j;
						*(int *)node1 -> ptr = k;
						coalesce1 -> ptr = NULL;
						nodeStruct *removenode = myalloc.freelist;
						while(removenode -> next != coalesce1){
							removenode = removenode -> next;
						}
						nodeStruct *temp = coalesce1;
						removenode -> next = removenode -> next -> next;
						free(temp);
						return;
					}
					coalesce1 = coalesce1 -> next;
					}
				}
				// Coalesce free blocks that are adjacent
				nodeStruct *coalesce2 = myalloc.freelist;
				while(coalesce2 != NULL){
					if(coalesce2 == NULL){
						continue;
					}
					char d = (char*)coalesce2 -> ptr;
					int coalsize2 = *(int *)coalesce2 -> ptr;
					char c = (char*)coalesce2 -> ptr - 8;
					
					if(c + coalsize2 <= myalloc.size){
				
						if((coalesce2 -> ptr + coalsize2) == node1 -> ptr){
							int i = (*(int *)coalesce2 -> ptr);
							int j = (*(int *)node1 -> ptr);
							int k = i + j;
							*(int *)coalesce2 -> ptr = k;
							node1 -> ptr = NULL;
							nodeStruct *removenode = myalloc.freelist;
							
							while(removenode -> next != node1){
								removenode = removenode -> next;	
							}
							nodeStruct *temp = node1;
							removenode -> next = removenode -> next -> next;
							free(temp);
							return;
						}
						
					}
					
					coalesce2 = coalesce2 -> next;
					
				}
				
				return;
			}
			nodeStruct *display2 = myalloc.allocatedlist;
			while(display2 -> next -> ptr + 8 != _ptr){
				display2 = display2 -> next;
			}
			display2 -> next = display2 -> next -> next;
			// Coalesce free blocks that are adjacent
			nodeStruct *coalesce1 = myalloc.freelist;
				while(coalesce1 != NULL){
				
					if(coalesce1 == NULL){
						continue;
					}
					
					char d = (char*)node1 -> ptr;
					int coalsize1 = *(int *)node1 -> ptr;
					char c = (char*)node1 -> ptr - 8;
					
					if(c + coalsize1 <= myalloc.size){
					
					if((node1 -> ptr + coalsize1) == coalesce1 -> ptr){
						int i = (*(int *)coalesce1 -> ptr);
						int j = (*(int *)node1 -> ptr);
						int k = i + j;
						*(int *)node1 -> ptr = k;
						coalesce1 -> ptr = NULL;
						nodeStruct *removenode = myalloc.freelist;
						while(removenode -> next != coalesce1){
							removenode = removenode -> next;
						}
						nodeStruct *temp = coalesce1;
						removenode -> next = removenode -> next -> next;
						free(temp);
						return;
					}
					coalesce1 = coalesce1 -> next;
					}
				}
				// Coalesce free blocks that are adjacent
			nodeStruct *coalesce2 = myalloc.freelist;
				while(coalesce2 != NULL){
					if(coalesce2 == NULL){
						continue;
					}
					char d = (char*)coalesce2 -> ptr;
					int coalsize2 = *(int *)coalesce2 -> ptr;
					char c = (char*)coalesce2 -> ptr - 8;
					
					if(c + coalsize2 <= myalloc.size){
				
						if((coalesce2 -> ptr + coalsize2) == node1 -> ptr){
							int i = (*(int *)coalesce2 -> ptr);
							int j = (*(int *)node1 -> ptr);
							int k = i + j;
							*(int *)coalesce2 -> ptr = k;
						
							node1 -> ptr = NULL;
							nodeStruct *removenode = myalloc.freelist;
							
							while(removenode -> next != node1){
								removenode = removenode -> next;	
							}
							nodeStruct *temp = node1;
							removenode -> next = removenode -> next -> next;
							free(temp);
							return;
						}
						
					}
					
					coalesce2 = coalesce2 -> next;
					
			}
			
			return;
		}
		display = display -> next;
	}
	
	pthread_mutex_unlock(&mutex);
}

int compact_allocation(void** _before, void** _after) {
	pthread_mutex_lock(&mutex);
    int compacted_size = 0;

    // compact allocated memory
    // update _before, _after and compacted_size
	
	nodeStruct *beforeArray = myalloc.allocatedlist;
	int numofindex = 0;
	while(beforeArray != NULL){
		if(beforeArray == NULL){
			continue;
		}
		*(_before + numofindex) = *(int*)(beforeArray -> ptr + 8);
		beforeArray = beforeArray -> next;
		numofindex = numofindex + 1;
	}
	
	
	nodeStruct *display = myalloc.freelist -> next;
	nodeStruct *temp = NULL;
	myalloc.freelist -> next = NULL;
	//Makes one free node
	while(display != NULL){
		temp = display -> next;
		*(int *)myalloc.freelist -> ptr = (*(int *)myalloc.freelist -> ptr) + (*(int *)display -> ptr);
		display -> ptr = NULL;
		free(display);
		display = temp;
	}
	int freesize = *(int *)myalloc.freelist -> ptr;
	*(int *)myalloc.freelist -> ptr = 0;
	
	// Sort list
	List_sort(&myalloc.allocatedlist);
	char c = (char*)myalloc.memory;
	

	// Saves values
	nodeStruct *display2 = myalloc.allocatedlist;
	int tempval = *(int *)display2 -> ptr;

	
	*(int *)display2 -> ptr = 0;
	display2 -> ptr = myalloc.memory;
	*(int *)display2 -> ptr = tempval;

	// Rearrange allocated blocks
	while(display2 -> next != NULL){
		
		tempval = *(int *)display2 -> next -> ptr;
		*(int *)display2 -> next -> ptr = 0;
		
		display2 -> next -> ptr = display2 -> ptr + *(int *)display2 -> ptr;
		
		
		*(int *)display2 -> next -> ptr = tempval;
		
		display2 = display2 -> next;
		
	}
	
	myalloc.freelist -> ptr = display2 -> ptr + *(int *)display2 -> ptr;
	*(int *)myalloc.freelist -> ptr = freesize;
	
	
	// Creates after array
	nodeStruct *afterArray = myalloc.allocatedlist;
	int numofindex1 = 0;
	while(afterArray != NULL){
		if(afterArray == NULL){
			continue;
		}
		*(_after + numofindex1) = afterArray -> ptr + 8;
		afterArray = afterArray -> next;
		numofindex = numofindex + 1;
	}
	compacted_size = numofindex1;
	pthread_mutex_unlock(&mutex);
    return compacted_size;
}

int available_memory() {
	pthread_mutex_lock(&mutex);
    int available_memory_size = 0;
    // Calculate available memory size
	int currentfreesize;
	nodeStruct* display2 = myalloc.freelist;
	while(display2 != NULL){
		currentfreesize = currentfreesize + *(int *)display2->ptr;
		if(*(int *)display2->ptr > 8){
			currentfreesize = currentfreesize - 8;
		}
		display2 = display2 -> next;
	}
	if(currentfreesize < 0){
		currentfreesize = 0;
	}
	available_memory_size = currentfreesize;
	pthread_mutex_unlock(&mutex);
    return available_memory_size;
}

void print_statistics() {
	
	pthread_mutex_lock(&mutex);
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = myalloc.size;
    int largest_free_chunk_size = 0;

    // Calculate the statistics
	// Calculate statistics in allocatedlist
	int currentsize = 0;
	int currentchunks = 0;
	nodeStruct *display = myalloc.allocatedlist;
	while(display != NULL){
	
		currentsize = currentsize + *(int *)display->ptr;
		currentsize = currentsize - 8;
		display = display -> next;
		currentchunks = currentchunks + 1;
	}
	allocated_size = currentsize;
	allocated_chunks = currentchunks;
	
	int currentfreesize = 0;
	int currentfreechunks = 0;
	int largestfree = 0;
	int smallestfree = myalloc.size;
	
	// Calculate statistics in freelist
	nodeStruct *display2 = myalloc.freelist;
	while(display2 != NULL){
		
		currentfreesize = currentfreesize + *(int *)display2->ptr;
	
		if(*(int *)display2->ptr > 8){
			currentfreesize = currentfreesize - 8;
		}
		
		if(largestfree < *(int *)display2->ptr){
			largestfree = *(int *)display2->ptr;
		}
		if(smallestfree > *(int *)display2->ptr){
			smallestfree = *(int *)display2->ptr;
		}
		display2 = display2 -> next;
		currentfreechunks = currentfreechunks + 1;
	}
	smallestfree = smallestfree - 8;
	largestfree = largestfree - 8;
	if(currentfreesize < 0){
		currentfreesize = currentfreesize + 8;
	}
	if(smallestfree < 0){
		smallestfree = smallestfree + 8;
	}
	if(largestfree < 0){
		largestfree = largestfree + 8;
	}
	
	free_size = currentfreesize;
	free_chunks = currentfreechunks;
	largest_free_chunk_size = largestfree;
	smallest_free_chunk_size = smallestfree;
	

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
	pthread_mutex_unlock(&mutex);
}



