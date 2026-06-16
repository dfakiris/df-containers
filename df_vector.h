#ifndef DF_CVECTOR_H
#define DF_CVECTOR_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

/*
Implementation of a vector object by Dimos Fakiris.

Current version: v2.2

v1.0 - base version. works aiight.

v1.1 - 
	1. Added error return for 
		df_vector_resize()
		df_vector_reserve()
		df_vector_insert()

	2.	df_vector_resize() checks reallocation result.
	
	3.	df_vector_insert() checks reallocation result.
	
	4.	df_vector_front() returns the data rather than the object.
	
	5.	Fixed df_vector_back() return arithmetic.
		* 	Put in_vec->vec_size-1 in brackets 
			to maintain order of operations.
	
	6.	df_vector_insert() 
		*	fixed np calculation.
		*	memmove now moves the whole tail rather than one element.
		*	added protection against self-insertion. Self-insertion happens when
			an elemest is inserted into the vector, with the source element being
			inside the vector. In that case src would be invalidated when memmove().
			The fix copies src into a buffer and then points src at the buffer. 
	
	7.	df_vector_resize()
		*	call to df_vector_reserve() now provides the number of 
			elements needed. not bytes.
	
	8.	df_vector_push_back()
		*	Added error checking for reallocating the vector.
		
	9.	Replaced all the int i counters to size_t counters in all for loops.

v1.2 - 07/03/2026
	1. Improved safety of vector constructor
		* Constructor now checks in_elemsize > 0.
		* Vector constructor checks that if an element constructor has been defined, 
		  then the copy constructor, and destructor are also defined. If not, the constructor fails.
	2. Added df_vector_clear.
	3. Added df_vector_erase.
	4. Added overflow protection to df_vector_push_back()
		* Checks that size_t won't overflow at reallocation
		* Checks that huge element sizes won't cause an overflow before void *dst....
	5. Added overflow protection to df_vector_insert()
		* Checks that size_t won't overflow at reallocation
		
v2.0 - 10/03/2026
	1. Added move contructor callback for each element. 
	2. df_vector_insert now uses construct_move
	3. df_vector_reserve now uses construct_copy
	4. df_vector_reserve now returns 1 if new_cap is less than existing capacity
	5. df_vector_push_back: src is now const void*. previously src was void*
	6. df_vector_erase now uses construct_move to move elements in object mode.

v2.1 - 
	1. Added copy constructor for the vector
	2. Added move constructor for the vector
	3. Optimised layout of the struct to be cache friendly
		* Meta data grouped at the top
	4. Created a separate destructor function df_vector_destruct to allow vectors within vectors.
		* destructor does not deallocate memory. df_delete_vector still has to be called.
		
	5. Created 
	
	6. const parameters to the following functions:
		* df_vector_size
		* df_vector_capacity
		* df_vector_at
		* df_vector_front
		* df_vector_back
		* df_vector_empty
	
	7. Removed the leading ____ from growth factors.
	
	8. Added overflow check in df_vector_resize prior to calling df_vector_reserve.
	
	9. Fixed df_vector_shrink_to_fit. If vector size was 0, data was set to NULL. 
		However, other functions such as push_back did not account for this state and therefore the code would break.
		A minimum of 1 capacity is used in this case to preserve in_vec -> data in a valid state.

v2.2 -
	1. Added df_vector_erase_range
	2. df_vector_shrink_to_fit now respects object vs pod mode.
	3. df_vector_insert - changed allocation of tmp to heap to prevent VLA issues on the stack.

v2.3 -
	1. default constructor signature has the ability to accept parameters.
	2. default constructor code made into macro to make changes easier as the same code is used multiple times.
	
To do
	1. Add insert_range()
	2. Add a void* param to constructor signature.

*/

typedef struct DF_CVECTOR DF_CVECTOR;

typedef struct DF_CVECTOR_PARAMS DF_CVECTOR_PARAMS;

extern const struct DF_CVECTOR_PARAMS DF_CVECTOR_CONSTRUCTOR_PARAMS;

/* Creates vector. Returns NULL if vector creation fails. 
	This object can operate in two modes. pod and object.
	1. If the constructors & destructor are NULL, then only POD types should be used.
	2. If the constructors & destructor are not NULL, then complex elements may be used.*/
DF_CVECTOR* df_create_vector(const size_t in_elemsize, const size_t in_capacity,
							void (*in_construct)(void *in_elem, void *params),
							void (*in_construct_copy)(void *in_elem, const void* src),
							void (*in_construct_move)(void *dest, void *src),
							void (*in_destruct)(void *in_elem));

/* Deletes vector. Provides a safe delete path */
void df_delete_vector(DF_CVECTOR **in_vec);

/* Vector constructor */
void df_vector_construct(void *ptr, void *params);

/* Copy constructor. dst assumed to be allocated but 
   uninitialised memory. */
void df_vector_copy(void *dst, const void *src);

/* Move constructor. dst assumed to be uninitialised memory. */
void df_vector_move(void *dst, void *src);

/* Returns the number of elements */
size_t df_vector_size(const DF_CVECTOR *in_vec);

/* Returns the number of elements that can be held in currently allocated storage */
size_t df_vector_capacity(const DF_CVECTOR *in_vec);

/* Access specified element with bounds checking */
void* df_vector_at(const DF_CVECTOR *in_vec, const size_t pos);

/* Access the first element */
void* df_vector_front(const DF_CVECTOR *in_ver);

/* Access the last element */
void* df_vector_back(const DF_CVECTOR *in_vec);

/* Checks whether the container is empty 
	Return values:
	true	-	Vector is empty OR in_vec is NULL 
	false	-	Vector is not empty. */
bool df_vector_empty(const DF_CVECTOR *in_vec);

/*	Resizes the container size to 'count' elements. 
	Return values:
		0: Could not resize;
		1: No Fault - Resize works as requrested. */
int df_vector_resize(DF_CVECTOR *in_vec, const size_t new_size, const void* params);

/* Changes capacity to new_cap. If new_cap <= in_vec.capacity
the function does nothing. 
	Return
		0: Could not reserve 
		1. Reserve worked as intended. */
int df_vector_reserve(DF_CVECTOR *in_vec, const size_t new_cap);

/* Adds an element at the end of the vector. In the case of failure
	the vector remains unchanged.
	Return values:
	0 - Insertion failed.
	1 - Insertion sucessful  */
int df_vector_push_back(DF_CVECTOR *in_vec, const void *src);

/* Removes an element from the end of the vector */
void df_vector_pop_back(DF_CVECTOR *in_vec);

/* Inserts elements at pos. 
	Return values
	0 - Insertion failed
	1 - Insertion successful */
int df_vector_insert(DF_CVECTOR *in_vec, const size_t pos, const void *src);

/* Changes the capacity to match the size of the vector.
   In the case where the function is called on a vector with size == 0,
   then it is reallocated to capacity 1.
	Return Values
	0 - Failure
	1 - Success		*/
int df_vector_shrink_to_fit(DF_CVECTOR *in_vec);

/* Clears vector. Does not affect capacity.
	Return values:
	0 - Failure
	1 - Success */
int df_vector_clear(DF_CVECTOR *in_vec);

/* Erases an element at pos 
	Return values
	0 - Failure.
	1 - Success. */
int df_vector_erase(DF_CVECTOR *in_vec, size_t pos);

/* Erases elements between pos_start and pos_end. 
	* Inclusive of pos_start, but not inclusive of pos_end
	* returns 1 if successful, 0 if not successful. */
int df_vector_erase_range(DF_CVECTOR *in_vec, const size_t pos_start, const size_t pos_end);




#endif // DF_CVECTOR_H
