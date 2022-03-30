// The MIT License (MIT)
// 
// Copyright (c) 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdlib.h>
#include "mavalloc.h"

// Pointer to the start of memoryarena
void * memory_arena;

// Varible that contains the current algorithm used
enum ALGORITHM heap_algo;

// Enum that specifies hole or process for the node structure
enum ALLOCATE {
    HOLE = 0,
    PROCESS = 1
};

// Node structure for linked list
// Each node specifies hole or process, the address where it starts, 
// the length, and a pointer to the next item.
struct Node {
    enum ALLOCATE type;
    void * start;
    size_t length;
    struct Node * next;
};

// Pointer to the first node in the linked list
struct Node * head = NULL;

// Due to only using malloc() once, the size of the node itself 
// will have to be taken into account when determining the address and length,
// as the size of the node structure itself will take up space in the 
// memory arena
size_t node_size = sizeof( struct Node );

/**
 * @brief Initialize the allocation arena and set the algorithm type
 *
 * This function takes the provided size and calls malloc to allocate a 
 * memory arena. The size must be aligned to a word boundary.
 * 
 * If the allocation succeeds it returns 0. If the allocation fails or the 
 * size is less than 0 the function returns -1
 *
 * \param size The size of the pool to allocate in bytes
 * \param algorithm The heap algorithm to implement
 * \return 0 on success. -1 on failure
 **/
int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
    if ( size < 0 ) return -1;
    
    // 4 byte word align size
    size_t requested_size = ALIGN4( size );

    // If malloc() succeeds, malloc() returns a void pointer pointing 
    // to the memory allocated
    memory_arena = malloc( requested_size );

    // If malloc() fails, malloc() returns a NULL pointer
    if ( memory_arena == NULL ) return -1;

    // Sets current algorithm
    heap_algo = algorithm;

    // Initiate first node in linked list, type hole
    head = (struct Node *)memory_arena;
    head->next = NULL;
    new->type = HOLE;
    new->start = memory_arena;
    new->length = requested_size; 

    return 0;
}


/**
 * @brief Destroy the arena
 *
 * This function releases the arena
 *
 * \return None 
 **/
void mavalloc_destroy( )
{
    return;
}


/**
 * @brief Allocate memory from the arena 
 *
 * This function allocated memory from the arena.  The parameter size 
 * specifies the number of bytes to allocates.  This _must_ be 4 byte aligned using the 
 * ALIGN4 macro. 
 * 
 * The function searches the arena for a free block using the heap allocation algorithm 
 * specified when the arena was allocated.
 *
 * If there is no available block of memory the function returns NULL
 *
 * \return A pointer to the available memory or NULL if no free block is found 
 **/
void * mavalloc_alloc( size_t size )
{
    // only return NULL on failure
    return NULL;
}


/*
 * \brief free the pointer
 *
 * frees the memory block pointed to by pointer. if the block is adjacent
 * to another block then coalesce (combine) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void mavalloc_free( void * ptr )
{
    return;
}


/*
 * \brief Allocator size
 *
 * Return the number of nodes in the allocators linked list 
 *
 * \return The size of the allocator linked list 
 */
int mavalloc_size( )
{
    int number_of_nodes = 0;

    return number_of_nodes;
}



