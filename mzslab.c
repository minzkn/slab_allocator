/*
 Copyright (C) JAEHYUK CHO
 All rights reserved.
 Code by JaeHyuk Cho <mailto:minzkn@minzkn.com>
*/

#if !defined(__def_mzapi_source_mzslab_c__)
#define __def_mzapi_source_mzslab_c__ "mzslab.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "mzslab.h"

mzslab_t * mzslab_init(void *s_page, size_t s_page_size, size_t s_object_size);
void * mzslab_alloc(mzslab_t *s_slab);
void * mzslab_free(mzslab_t *s_slab, void *s_ptr);

mzslab_t * mzslab_init(void *s_page, size_t s_page_size, size_t s_object_size)
{
    size_t s_index;

    mzslab_t *s_slab;
    mzslab_index_t *s_nft; /* next free table */
    
    s_slab = (mzslab_t *)s_page;
    
    s_slab->object_size = s_object_size;

    s_slab->objects = (s_page_size - sizeof(mzslab_t)) / s_object_size;
    while((s_slab->objects > ((size_t)0)) && ((s_page_size - sizeof(mzslab_t)) < ((s_slab->objects * s_object_size) + (s_slab->objects * sizeof(mzslab_index_t))))) {
        s_slab->objects--;
    }
    if(s_slab->objects <= ((size_t)0)) {
#if def_mzslab_debug != (0)
        (void)fprintf(stderr, "not enough page size !\n");
#endif
        
        return((mzslab_t *)0);
    }

    s_slab->entry = ((unsigned char *)(&s_slab[1])) + (s_slab->objects * sizeof(mzslab_index_t));

    s_slab->f = (mzslab_index_t)0u;
    
    s_nft = (mzslab_index_t *)(&s_slab[1]);
    for(s_index = ((size_t)0u);s_index < s_slab->objects;s_index++) {
        s_nft[s_index] = (mzslab_index_t)(s_index + ((size_t)1u));
    }

#if def_mzslab_debug != (0)
    (void)fprintf(stdout,
        "mzslab_init: page_size=%lu, object_size=%lu, objects=%lu, entry=%p\n",
        (unsigned long)s_page_size,
        (unsigned long)s_slab->object_size,
        (unsigned long)s_slab->objects,
        (void *)s_slab->entry
    );
    (void)fprintf(stdout,
        "  - slab size=%lu\n"
        "  - next_free_table[%lu] size=%lu\n"
        "  - objects size=%lu\n"
        "  - can not use size=%lu\n",
        (unsigned long)sizeof(mzslab_t),
        (unsigned long)s_slab->objects,
        (unsigned long)(s_slab->objects * sizeof(mzslab_index_t)),
        (unsigned long)(s_slab->objects * s_slab->object_size),
        (unsigned long)(s_page_size - ((s_slab->objects * sizeof(mzslab_index_t)) + (s_slab->objects * s_slab->object_size)))
    );
#endif

    return(s_slab);
}

void * mzslab_alloc(mzslab_t *s_slab)
{
    mzslab_index_t *s_nft = (mzslab_index_t *)(&s_slab[1]);
    void *s_result;

    if(s_slab->f >= s_slab->objects) { /* not enough object */
        s_result = (void *)0;
    }
    else { /* avail */
        s_result = (void *)(&s_slab->entry[s_slab->f * s_slab->object_size]);
        s_slab->f = s_nft[s_slab->f];
    }

#if def_mzslab_debug != (0)
    (void)fprintf(stdout,
        "mzslab_alloc: result=%p, f=%lu\n",
        s_result,
        (unsigned long)s_slab->f
    );
#endif

    return(s_result);
}

void * mzslab_free(mzslab_t *s_slab, void *s_ptr)
{
    mzslab_index_t *s_nft = (mzslab_index_t *)(&s_slab[1]);
    size_t s_index;

    s_index = (size_t)(((unsigned char *)s_ptr) - s_slab->entry) / s_slab->object_size;

    s_nft[s_index] = s_slab->f;
    s_slab->f = (mzslab_index_t)s_index;

#if def_mzslab_debug != (0)
    (void)fprintf(stdout,
        "mzslab_free: ptr=%p, f=%lu\n",
        s_ptr,
        (unsigned long)s_slab->f
    );
#endif

    return((void *)0);
}

#endif

/* vim: set expandtab: */
/* End of source */
