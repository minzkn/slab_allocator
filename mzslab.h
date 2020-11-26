/*
 Copyright (C) JAEHYUK CHO
 All rights reserved.
 Code by JaeHyuk Cho <mailto:minzkn@minzkn.com>
*/

#if !defined(__def_mzapi_header_mzslab_h__)
#define __def_mzapi_header_mzslab_h__ "mzslab.h"

#define def_mzslab_debug (1)

typedef unsigned short int __mzslab_index_t;
#define mzslab_index_t __mzslab_index_t

#pragma pack(push,8)
typedef struct mzslab_ts {
    size_t object_size;
    size_t objects;
    unsigned char *entry;
    mzslab_index_t f; /* first free block index */
}__mzslab_t;
#define mzslab_t __mzslab_t
#pragma pack(pop)

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(__def_mzapi_source_mzslab_c__)
extern mzslab_t * mzslab_init(void *s_page, size_t s_page_size, size_t s_object_size);
extern void * mzslab_alloc(mzslab_t *s_slab);
extern void * mzslab_free(mzslab_t *s_slab, void *s_ptr);
#endif

#if defined(__cplusplus)
}
#endif

#endif

/* vim: set expandtab: */
/* End of source */
