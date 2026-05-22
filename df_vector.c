#include "df_vector.h"

static const size_t DF_VEC_INIT_SIZE = 1;

static const size_t DF_VEC_RESIZE_FACTOR = 2;

struct DF_CVECTOR
{
	/*
	 *	data		content of vector
	 *	vec_size	number of elements stored
	 *	capacity	current number of elements allocated to memory
	 *	elem_size	element size in bytes
	 *
	 *	construct			Default constructor
	 *	construct_copy		Copy constructor
	 *	construct_move		Move constructor
	 *	destruct			Destructor
	 */

	void* data;
	size_t vec_size;
	size_t capacity;
	size_t elem_size;

	void (*construct)(void *elem);
	void (*construct_copy) (void *elem, const void* src);
	void (*construct_move) (void *dest, void *src);
	void (*destruct) (void *elem);
};

/* Destructor. Used to safely set up the vector object for deallocation */
static void df_vector_destruct(void *ptr)
{
	if (!ptr)
		return;
	
	struct DF_CVECTOR *vec = (struct DF_CVECTOR*)ptr;
	
	if (vec->destruct)
	{
		for (size_t i = 0; i < vec -> vec_size; i++)
		{
			void *el = (char*)vec->data+i*vec->elem_size;
			vec->destruct(el);
		}
	}
	
	free(vec->data);
	
	vec->data = NULL;
	vec->vec_size = 0;
	vec->capacity = 0;
	vec->elem_size = 0; 
}

/* Vector constructor for the special of a vector-in-vector scenario */
static void df_vector_construct_default(void *ptr)
{
	struct DF_CVECTOR *vec = (struct DF_CVECTOR*)ptr;
	
	vec->elem_size = 0;
	vec->vec_size = 0;
	vec->capacity = 0;
	vec->data = NULL;
	
	
	/* WARNING: DO NOT CALL vec->construct WITHIN THIS FUNCTION TO AVOID RECURSION */
	vec->construct = df_vector_construct_default; 
	
	
	vec->construct_copy = df_vector_copy;
	vec->construct_move = df_vector_move;
	vec->destruct = df_vector_destruct;
}

DF_CVECTOR* df_create_vector(const size_t in_elemsize, const size_t in_capacity,
							void (*in_construct)(void *in_elem),
							void (*in_construct_copy)(void *in_elem, const void* src),
							void (*in_construct_move)(void *dest, void *src),
							void (*in_destruct)(void *in_elem))
{
	if (in_elemsize == 0)
		return NULL;
	
	if ((in_construct || in_construct_copy || in_construct_move || in_destruct) && 
		!(in_construct && in_construct_copy && in_construct_move && in_destruct))
		return NULL;
	
	DF_CVECTOR* newvec = malloc(sizeof(DF_CVECTOR));
	
	if (!newvec)
		return NULL;

	
	newvec->construct = in_construct;
	newvec->construct_copy = in_construct_copy;
	newvec->construct_move = in_construct_move;
	newvec->destruct = in_destruct;
	
	// Vector allocation successful
	newvec->elem_size = in_elemsize;
	newvec->vec_size = 0;
	

	
	if (in_capacity > 0)
		newvec->capacity = in_capacity;
	else
		newvec->capacity = DF_VEC_INIT_SIZE;
	
	if (newvec->capacity > SIZE_MAX / newvec->elem_size)
	{
		free (newvec);
		return NULL;
	}
	
	newvec->data = malloc(newvec->elem_size * newvec->capacity);
	
	if (!newvec->data)
	{
		// Data allocation unsuccessful
		free(newvec);
		return NULL;
	}
	
	
	
	return newvec;
}

DF_CVECTOR* df_create_vec_in_vec(const size_t in_capacity)
{
	return df_create_vector(sizeof(DF_CVECTOR), in_capacity,
							df_vector_construct_default,
							df_vector_copy,
							df_vector_move,
							df_vector_destruct);
}

void df_init_inner_vector(DF_CVECTOR *in_vec, const size_t in_elemsize, const size_t in_capacity,
							void (*in_construct)(void *in_elem),
							void (*in_construct_copy)(void *in_elem, const void* src),
							void (*in_construct_move)(void *dest, void *src),
							void (*in_destruct)(void *in_elem))
{
	if (!in_vec || in_elemsize == 0)
		return;
	
	in_vec->construct = in_construct;
	in_vec->construct_copy = in_construct_copy;
	in_vec->construct_move = in_construct_move;
	in_vec->destruct = in_destruct;
	
	in_vec->elem_size = in_elemsize;
	in_vec->vec_size = 0;
	
	if (in_capacity > 0)
		in_vec->capacity = in_capacity;
	else
		in_vec->capacity = DF_VEC_INIT_SIZE;
	
	
	if (in_vec->capacity > SIZE_MAX / in_vec->elem_size)
	{
		in_vec->data = NULL;
		return;
	}
	
	in_vec->data = malloc(in_vec->elem_size * in_vec->capacity);
	
	if (!in_vec->data)
		in_vec->capacity = 0;
	
}

void df_delete_vector(DF_CVECTOR **in_vec)
{
	if (!in_vec || !*in_vec)
		return;
	
	df_vector_destruct((void*)*in_vec);
	
	free(*in_vec);
	
	*in_vec=NULL;

}

void df_vector_copy(void *dst, const void *src)
{
	if (!dst || !src)
		return ;

	DF_CVECTOR *dest = (DF_CVECTOR*)dst;
	DF_CVECTOR *source = (DF_CVECTOR*)src;
	
	dest->elem_size = source->elem_size;
	
	dest->capacity = source->capacity;
	
	dest->vec_size = source->vec_size;
	
	dest->data = NULL;
	
	if (source->data)
	{
		dest->data = malloc(dest->elem_size * dest->capacity);
		if (!dest->data)
		{
			dest->vec_size = 0;
			dest->elem_size = 0;
			dest->capacity = 0;
			return;
		}
	}
	
	dest->construct = source->construct;
	dest->construct_copy = source->construct_copy;
	dest->construct_move = source->construct_move;
	dest->destruct = source->destruct;
	
	if (source->construct_copy)
	{
		if (dest->data)
		{
			void *op = source->data;
			void *np = dest->data;
			for (size_t i = 0; i < dest->vec_size; i++)
			{
				dest->construct_copy(np, op);
				
				op = (char*)op + source->elem_size;
				np = (char*)np + dest->elem_size;
			}
		}
	}
	else
	{
		if (dest->data && dest->vec_size > 0)
			memcpy(dest->data, source->data, dest->elem_size * dest->vec_size);
	}
}

void df_vector_move(void *dst, void *src)
{
	if (!dst || !src)
		return;
	
	DF_CVECTOR *dest = (DF_CVECTOR*)dst;
	DF_CVECTOR *source = (DF_CVECTOR*)src;

	*dest = *source;
	
	source->elem_size = 0;
	source->vec_size = 0;
	source->capacity = 0;
	source->data = NULL;
	
	source->construct = NULL;
	source->construct_copy = NULL;
	source->construct_move = NULL;
	source->destruct = NULL;
}

size_t df_vector_size(const DF_CVECTOR *in_vec)
{
	if (in_vec)
		return in_vec->vec_size;
	else
		return 0;
}

size_t df_vector_capacity(const DF_CVECTOR *in_vec)
{
	if (in_vec)
		return in_vec->capacity;
	else
		return 0;
}

void* df_vector_at(const DF_CVECTOR *in_vec, const size_t pos)
{
	
	if ((in_vec) && (in_vec->vec_size > 0) && (pos < in_vec->vec_size))
		return (void*)((char*)in_vec->data+pos*in_vec->elem_size);
	else
		return NULL;
}

void* df_vector_front(const DF_CVECTOR *in_vec)
{
	if ((in_vec) && (in_vec -> vec_size > 0))
		return in_vec->data;
	else 
		return NULL;
}

void* df_vector_back(const DF_CVECTOR *in_vec)
{
	if ((in_vec) && (in_vec -> vec_size > 0))
		return (void*)((char*)in_vec->data + in_vec->elem_size * (in_vec->vec_size-1));
	else
		return NULL;
}

bool df_vector_empty(const DF_CVECTOR *in_vec)
{
	if ((in_vec) && (in_vec->vec_size > 0))
		return false;
	
	return true;
}

int df_vector_resize(DF_CVECTOR *in_vec, const size_t new_size)
{
	if (!in_vec || new_size > SIZE_MAX / in_vec->elem_size)
		return 0;
	
	// Needed variables that will be changed within the vector.
	const size_t old_size = in_vec->vec_size;
	
	
	if (new_size > in_vec->capacity)
	{
		if (new_size > SIZE_MAX / DF_VEC_RESIZE_FACTOR)
			return 0;
		
		// Vector has to reallocate.
		if (!df_vector_reserve(in_vec, DF_VEC_RESIZE_FACTOR * new_size))
			return 0; // exit ealier if reallocation fails.
		

		for(size_t i = old_size; i < new_size; i++)
		{
			void *elem = (char*)in_vec->data + i * in_vec->elem_size;
			if (in_vec->construct)
				in_vec->construct(elem);
			else
				memset(elem, 0, in_vec->elem_size);
		}
		
		in_vec->vec_size = new_size;
	}
	else if (new_size > in_vec->vec_size && new_size <= in_vec->capacity) 
	{		
		for(size_t i = old_size; i < new_size; i++)
		{
			void *elem = (char*)in_vec->data + in_vec->elem_size * i;
			if (in_vec->construct)
				in_vec->construct(elem);
			else
				memset(elem, 0, in_vec->elem_size);
		}
		
		in_vec->vec_size = new_size;
	}
	else if (new_size < old_size)
	{
		// Vector has to run destructors.
		
		// Run destructor on all elements out of bounds.
		for (size_t i = new_size; i < old_size; i++)
		{
			void *elem = (char*)in_vec->data + in_vec->elem_size * i;
			if (in_vec->destruct)
				in_vec->destruct(elem);
			else
				memset(elem,0,in_vec->elem_size);
		}
		
		in_vec->vec_size = new_size;
		
	}
	
	return 1;

}

int df_vector_reserve(DF_CVECTOR *in_vec, const size_t new_cap)
{
	if (!in_vec || new_cap > SIZE_MAX / in_vec->elem_size)
		return 0;
	
	if (new_cap <= in_vec->capacity)
		return 1;

	/* Only reserve new space if the requested capacity is larger
		than currently capacity */
	
	if (in_vec->construct_move)
	{
		/* malloc */
		
		void *temp = malloc(new_cap * in_vec->elem_size);
		
		if (!temp)
			return 0; /* Allocation failure */
		
		for (size_t i = 0; i < in_vec->vec_size; i++)
		{
			void *src = (char*)in_vec->data + in_vec->elem_size * i;
			void *dst = (char*)temp + in_vec->elem_size * i;
			in_vec->construct_move(dst, src);
			in_vec->destruct(src);
		}
		
		free(in_vec->data);
		in_vec->data = temp;
		
	}
	else
	{
		void *newdata = realloc(in_vec->data, new_cap * in_vec->elem_size);
		if (!newdata)
			return 0; /* If realloc fails the original block remains unchanged. */
		
		
		in_vec->data = newdata;
	}
	
	in_vec->capacity = new_cap;
	
	return 1;
}

int df_vector_push_back(DF_CVECTOR *in_vec, const void *src)
{
	if (!in_vec)
		return 0;

	if (in_vec->vec_size >= in_vec->capacity)
	{
		if (in_vec->capacity > SIZE_MAX/DF_VEC_RESIZE_FACTOR)
			return 0;
		if (!df_vector_reserve(in_vec, in_vec->capacity * DF_VEC_RESIZE_FACTOR))
			return 0;
	}
	
	if (in_vec->vec_size > SIZE_MAX / in_vec->elem_size)
		return 0;
	
	void *dst = (char*)in_vec->data + in_vec->vec_size * in_vec->elem_size;
	
	if (in_vec->construct_copy)
		in_vec->construct_copy(dst, src);
	else
		memcpy(dst, src, in_vec->elem_size);
	
	in_vec->vec_size++;
	
	return 1;
}

void df_vector_pop_back(DF_CVECTOR *in_vec)
{
	if (!in_vec)
		return;
	
	if (in_vec->vec_size > 0)
	{
		void *elem = df_vector_back(in_vec);
		
		if (in_vec->destruct)
			in_vec->destruct(elem);
		else
			memset(elem,0,in_vec->elem_size);
		
		in_vec->vec_size--;
		
	}
}

int df_vector_insert(DF_CVECTOR *in_vec, const size_t pos, const void *src)
{
	if (!in_vec || pos > in_vec->vec_size)
		return 0;
	
	// Protect against self insertion 
	char tmp[in_vec->elem_size];
	memcpy(tmp,src, in_vec->elem_size);
	src = tmp;
	
	// Grow as needed.
	if (in_vec->vec_size==in_vec->capacity)
	{
		if (in_vec->capacity > SIZE_MAX/DF_VEC_RESIZE_FACTOR)
			return 0;
		
		if (!df_vector_reserve(in_vec, in_vec->capacity == 0 ? 1 : in_vec->capacity * DF_VEC_RESIZE_FACTOR))
			return 0; // reservation failed. Do not continue executing function.
	}
	
	
	if (in_vec->construct_move)
	{
		for (size_t i = in_vec->vec_size; i > pos; i--)
		{
			// op	:	old position
			// np	:	new position
			void *np = (char*)in_vec->data + in_vec->elem_size * i;
			void *op = (char*)in_vec->data + in_vec->elem_size * (i-1);
			
			in_vec->construct_move(np, op);
			in_vec->destruct(op);
			
		}
		
		void *op = (char*)in_vec->data + in_vec->elem_size * pos;
		in_vec->construct_copy(op,src);
	}
	else
	{
		// op : old position
		// np : new position
		void *op = (char*)in_vec->data + pos * in_vec->elem_size;
		void *np = (char*)in_vec->data + (pos + 1) * in_vec->elem_size;
		
		memmove(np, op,(in_vec->vec_size-pos)*in_vec->elem_size);
		memcpy(op, src, in_vec->elem_size);
	}
	
	in_vec->vec_size++;
	return 1;
	
}

int df_vector_shrink_to_fit(DF_CVECTOR *in_vec)
{
	if (!in_vec)
		return 0;

	size_t new_cap = (in_vec->vec_size == 0) ? 1 : in_vec->vec_size;

	if (in_vec->capacity == new_cap)
		return 1;

	if (in_vec->construct_move)
	{
		void *temp = malloc(new_cap * in_vec->elem_size);

		if (!temp)
			return 0;

		for (size_t i = 0; i < in_vec->vec_size; i++)
		{
			void *src = (char*)in_vec->data + i * in_vec->elem_size;
			void *dst = (char*)temp + i * in_vec->elem_size;
			in_vec->construct_move(dst, src);
			in_vec->destruct(src);
		}

		free(in_vec->data);
		in_vec->data = temp;
	}
	else
	{
		void *newdata = realloc(in_vec->data, new_cap * in_vec->elem_size);

		if (!newdata)
			return 0;

		in_vec->data = newdata;
	}

	in_vec->capacity = new_cap;

	return 1;
}


int df_vector_clear(DF_CVECTOR *in_vec)
{
	if (!in_vec)
		return 0;
	
	if (in_vec->vec_size == 0)
		return 1;
	
	
	if (!in_vec->destruct)
	{
		memset(in_vec->data,0,in_vec->elem_size * in_vec->vec_size);
	}
	else
	{
		for (size_t i = 0; i < in_vec->vec_size; i++)
			in_vec->destruct((void*)((char*)in_vec->data + i * in_vec->elem_size));
	}
	
	in_vec->vec_size = 0;
	
	return 1;
}

int df_vector_erase(DF_CVECTOR *in_vec, size_t pos)
{
	if (!in_vec || pos >= in_vec->vec_size)
		return 0;
	
	
	if (in_vec->construct_move)
	{
		/* Destroy element being erased. */
		in_vec->destruct((char*)in_vec->data + pos * in_vec->elem_size);
		
		/* Shift elements one to the left */
		for (size_t i = pos; i < in_vec->vec_size - 1; i++)
		{
			void *src = (char*)in_vec->data + (i+1) * in_vec->elem_size;
			void *dst = (char*)in_vec->data + i * in_vec->elem_size;
			
			in_vec->construct_move(dst, src);
			in_vec->destruct(src);
		}
	}
	else
	{
		void *np = (char*)in_vec->data + in_vec->elem_size * pos;
		void *op = (char*)in_vec->data + in_vec->elem_size * (pos+1);
		
		memmove(np,op,in_vec->elem_size * (in_vec->vec_size - pos - 1));
	}
	
	in_vec->vec_size--;
	
	return 1;
}

int df_vector_erase_range(DF_CVECTOR *in_vec, const size_t pos_start, const size_t pos_end)
{
	if (!in_vec || pos_start >= pos_end || pos_end > in_vec->vec_size)
		return 0;
	
	size_t delta = pos_end - pos_start;
	
	if (in_vec->destruct)
	{
		void *elem;
		/* */
		for (size_t i = pos_start; i <pos_end; i++)
		{
			elem = (void*)((char*)in_vec->data + i * in_vec->elem_size);
			in_vec->destruct(elem);
		}


		for (size_t i = pos_end; i < in_vec->vec_size; i++)
		{
			void *op = (void*)((char*)in_vec->data + i * in_vec->elem_size);
			void *np = (void*)((char*)in_vec->data + (i-delta) * in_vec->elem_size);

			in_vec->construct_move(np,op);
			in_vec->destruct(op);
		}


	}
	else 
	{
		size_t num_bytes = in_vec->elem_size * (in_vec->vec_size - pos_end);
		void *op = (void*)((char*)in_vec->data + in_vec->elem_size * pos_end);
		void *np = (void*)((char*)in_vec->data + in_vec->elem_size * pos_start);
		memmove(np, op, num_bytes);


	}

	in_vec->vec_size = in_vec->vec_size - delta;
	
	return 1;
}

/* End df_vector.c */
