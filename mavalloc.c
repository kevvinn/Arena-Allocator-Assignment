/*
   CSE 3320 - Operating Systems
   Professor Trevor Bakker
   Programming Assignment 3: Arena Allocator 
   
   Name:   Ariel Rico
   UTA ID: 1001872003

   Name:   Kevin Vu
   UTA ID: 1001795250
*/
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

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>

// Total number of nodes allocated for the stack
#define NODE_AMOUNT 200

// Pointer to the start of memoryarena
void * memory_arena;


// Size of total memory_arena space allocated (prevent seg faults)
size_t memory_arena_size;


// Varible that contains the current algorithm used
enum ALGORITHM heap_algo;


// Enum that specifies hole or process for the node structure
enum ALLOCATE 
{
    HOLE = 0,
    PROCESS = 1
};


// Node structure for linked list
// Each node specifies hole or process, the address where it starts, 
// the size, a pointer to the previous item, and a pointer to the next item.
struct Node 
{
    enum ALLOCATE type;
    size_t address;
    size_t size;
    struct Node * next;
};


// Pointer to node that points to the head of the linked list (first node)
// Necessary for the triple reference technique for linked lists
struct Node * head_pointer = NULL;

// Points to the previous node, used for next fit algorithm
struct Node * previous_node;

// Memory allocated for the reserve node stack from malloc()
struct Node * node_stack;

// Points to the head of the stack
struct Node * stack_head;

// Initilizes the reserve node stack
void node_stack_init( int node_amount )
{
    int i;

    // Allocates an array of nodes
    node_stack = (struct Node *)malloc( node_amount * sizeof( struct Node ) );

    // Create linked list out of node array, to be used as a stack
    for( i = 0; i < node_amount - 1; i++ )
    {
        node_stack[ i ].next = &(node_stack[ i + 1 ]);
    }
    node_stack[ node_amount - 1 ].next = NULL;

    // stack_head points to the first node in the stack
    stack_head = node_stack;
}

// Returns a node popped off the stack to be used
struct Node * node_malloc()
{
    // If the stack is empty, node_malloc() fails and returns NULL
    if( stack_head == NULL ) return NULL;

    // Pop a node off the stack
    struct Node * new = stack_head;

    stack_head = stack_head->next;
    
    return new;
}

// Takes a node and pushes it onto the stack
void node_free( struct Node * node )
{
    node->next = stack_head;

    stack_head = node;

    return;
}

// Node constructor
/**
 * @brief Node constructor
 *
 * Allocates memory for new node, assigns data, and returns new node
 *
 * \param type The type of node
 * \param address The starting address in the memory arena
 * \param size The number of bytes occupied in the memory arena
 * \return struct Node * new on success. NULL on failure
 **/
struct Node * new_node( enum ALLOCATE type, size_t address, size_t size ) 
{
    struct Node * new = node_malloc();

    // If malloc() fails, malloc() returns a NULL pointer
    if ( new == NULL ) return new;

    new->next = NULL;
    new->type = type;
    new->address = address;
    new->size = size;

    return new;
}


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

    // Sets size of the memory arena
    memory_arena_size = requested_size;

    // Initialize node stack
    node_stack_init( NODE_AMOUNT );

    // Sets current algorithm
    heap_algo = algorithm;

    // Initiate the head pointer, used in triple reference technique
    head_pointer = new_node( HOLE, 0, 0 );

    // If new_node() fails, new_node() returns a NULL pointer
    if ( head_pointer == NULL ) return -1;

    // Initiate hole type head node (first node in linked list)
    head_pointer->next = new_node( HOLE, 0, requested_size );

    // If new_node() fails, new_node() returns a NULL pointer
    if ( head_pointer->next == NULL ) return -1;

    // Set the initial previous node to the head node
    previous_node = head_pointer;

    return 0;
}


/**
 * @brief Destroy the arena
 *
 * This function releases all allocated memory.
 * First it frees all the nodes in the linked link.
 * Then it frees the memory arena.
 *
 * \return None 
 **/
void mavalloc_destroy( )
{
    // Check if linked list exists
    if( head_pointer == NULL ) return;

    // Starting from the pointer to the head of the linked list, 
    // free all nodes in the linked list
    //struct Node * runner = head_pointer;
    //struct Node * node;

    //while( runner != NULL ) 
    //{
    //    node = runner;
    //    runner = runner->next;
    //    node_free( node );
    //}

    // Free the all the nodes allocated in the node_stack array
    free( node_stack );

    // Free the memory arena
    free( memory_arena );

    // Remove access to linked list address
    head_pointer = NULL;

    // Remove access to previous node pointer
    previous_node = NULL;

    // Remove access to the stack head
    stack_head = NULL;

    return;
}

/**
 * @brief Allocates a node of a specified size at the specified hole
 *
 * \param hole_ptr The node pointing to the hole node containing the space to be allocated
 * \param size The number of bytes that will be allocated from the hole node
 * \return void * of address of the allocated space in memory arena on success. NULL on failure.
 **/
void * allocate_node( struct Node * hole_ptr , size_t size )
{
    // Fails if hole pointer doesn't exist
    if( hole_ptr == NULL ) return NULL;

    // Split the hole node into a process node and a hole node
    // Process node will have the requested size
    // Hole node will contain the remaining space

    // First create the new process node at the hole's address
    struct Node * new = new_node( PROCESS, hole_ptr->next->address, size );

    // If new_node() fails, new_node() returns NULL
    if( new == NULL ) return NULL;

    // If no space left in the memory arena, delete the hole node
    if( hole_ptr->next->address + size >= memory_arena_size ) 
    {
        node_free( hole_ptr->next );
    }
    else
    {
        // Update old hole to remaining space
        hole_ptr->next->address = hole_ptr->next->address + size;
        hole_ptr->next->size = hole_ptr->next->size - size;

        // If old hole has no space left, delete the hole node
        if( hole_ptr->next->size <= 0 )
        {
            new->next = hole_ptr->next->next;
            node_free( hole_ptr->next );
        }
        // Else point to the hole node
        else
        {
            new->next = hole_ptr->next;
        }
    }

    // Point to the new process node
    hole_ptr->next = new;

    //Return allocated memory arena address
    return memory_arena + new->address;
}


/**
 * @brief First fit heap allocation algorithm
 *
 * \param size The size of space being requested to be allocated
 * \return void * of address of the allocated space in memory arena on success. NULL on failure.
 **/
void * alloc_first_fit( size_t size ) 
{
    // Check if linked list exists
    if( head_pointer == NULL ) return NULL;

    // Starting from the head of the linked list, 
    // find the first hole that is large enough for the requested size
    struct Node * runner = head_pointer;  // Head pointer points to head node

    while( runner->next->type == PROCESS || runner->next->size < size )
    {
        runner = runner->next;

        // The end of the linked list has been reached
        // There are no eligible holes left
        if( runner->next == NULL ) return NULL;
    }

    // runner->next is now the hole node with the available space
    // runner points to the hole node
    return allocate_node( runner, size );
}


/**
 * @brief Next fit heap allocation algorithm
 *
 * \param size The size of space being requested to be allocated
 * \return void * of address of the allocated space in memory arena on success. NULL on failure.
 **/
void * alloc_next_fit( size_t size )
{
    // check if the linked list exists
    if ( head_pointer == NULL ) return NULL;

    // starting from the previous node in the linked list,
    // find the next hole that is large enough for the requested size
    struct Node * runner = previous_node; // runner pointer points to node previously left off on

    while ( runner->next->type == PROCESS || runner->next->size < size )
    {
        runner = runner->next;

        // If end of the linked list is hit, loop back to the head
        if( runner->next == NULL ) runner = head_pointer;

        // The linked list has been completely looped and is back to where it started
        // There are no eligible holes left
        if( runner == previous_node ) return NULL;
    }

    // Update the previous node to the hole node with the available space
    previous_node = runner;

    // runner->next is now the hole node with the available space
    // runner points to the hole node
    return allocate_node( runner, size );
}

/**
 * @brief Best fit heap allocation algorithm
 *
 * \param size The size of space being requested to be allocated
 * \return void * of address of the allocated space in memory arena on success. NULL on failure.
 **/
void * alloc_best_fit( size_t size )
{
    // check if the linked list exists
    if ( head_pointer == NULL ) return NULL;

    struct Node * best_hole_ptr = NULL;
    size_t min = memory_arena_size+1;

    // Iterate through the linked list
    // Store the smallest eligible hole into best_hole_ptr
    struct Node * runner = head_pointer;

    while( runner->next != NULL )
    {
        if( runner->next->type == HOLE && runner->next->size >= size && runner->next->size < min )
        {
            min = runner->next->size;
            best_hole_ptr = runner;
        }

        runner = runner->next;
    }

    // best_hole_ptr now points to the hole that will be used to allocate memory in the memory arena
    return allocate_node( best_hole_ptr, size );
}

/**
 * @brief Worst fit heap allocation algorithm
 *
 * \param size The size of space being requested to be allocated
 * \return void * of address of the allocated space in memory arena on success. NULL on failure.
 **/
void * alloc_worst_fit( size_t size )
{
    // Check if the linked list exists
    if( head_pointer == NULL ) return NULL;

    struct Node * worst_hole_ptr = NULL;
    size_t max = 0;

    // Iterate through the linked list
    // Store the largest eligible hole into worst_hole_ptr
    struct Node * runner = head_pointer;

    while( runner->next != NULL )
    {
        if( runner->next->type == HOLE && runner->next->size >= size && runner->next->size > max )
        {
            max = runner->next->size;
            worst_hole_ptr = runner;
        }

        runner = runner->next;
    }

    // worst_hole_ptr now points to the hole that will be used to allocate memory in the memory arena
    return allocate_node( worst_hole_ptr, size );
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
    // 4 byte word align size
    size_t requested_size = ALIGN4( size );

    // Use heap algorithm specified at initialization
    switch( heap_algo ) 
    {
        case FIRST_FIT:
            return alloc_first_fit( requested_size );
            break; 
        case NEXT_FIT:
            return alloc_next_fit( requested_size );
            break;
        case BEST_FIT:
            return alloc_best_fit( requested_size );
            break;
        case WORST_FIT:
            return alloc_worst_fit( requested_size );
            break;
        default:
            break;
    }

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
    // Check if linked list exists
    if( head_pointer == NULL ) return;

    struct Node * runner = head_pointer;
    struct Node * node;

    // Iterate through the linked list until the address is found
    while( runner->next->address != ptr - memory_arena )
    {
        runner = runner->next;

        // The end of the linked list has been reached
        if( runner->next == NULL ) return;
    }

    // runner->next is the node to be freed (x)
    if( runner->type == HOLE && runner != head_pointer ) // Situation c)
    {
        node = runner->next;
        runner->size = runner->size + node->size;
        runner->next = node->next;
        node_free( node );
    }
    else // Situation a)
    {
        runner = runner->next;
        runner->type = HOLE;
    }

    // runner is now the node that has been freed (x or x/a)
    if( runner->next == NULL ) return; // runner is at end of linked list

    if( runner->next->type == HOLE ) // Situation b) and d)
    {
        node = runner->next;
        runner->size = runner->size + node->size;
        runner->next = node->next;
        node_free( node );
    }

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
    // Check if linked list exists
    if( head_pointer == NULL ) return 0;

    int number_of_nodes = 0;

    struct Node * runner = head_pointer;

    while( runner->next != NULL )
    {
        number_of_nodes++;
        runner = runner->next;
    }

    return number_of_nodes;
}



/*
 * \brief Print linked list
 *
 * Print node data for each node in the linked list
 *
 * \return None
 */
void mavalloc_print( )
{
    // Check if linked list exists
    if( head_pointer == NULL ) return;

    int number_of_nodes = 0;

    struct Node * runner = head_pointer;

    while( runner->next != NULL )
    {
        number_of_nodes++;

        printf(" %d) type = %d, address = %ld, size = %ld \n", number_of_nodes, runner->next->type, runner->next->address, runner->next->size);

        runner = runner->next;
    }

    return;
}



