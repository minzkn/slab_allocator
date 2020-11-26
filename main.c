/*
 Copyright (C) JAEHYUK CHO
 All rights reserved.
 Code by JaeHyuk Cho <mailto:minzkn@minzkn.com>
*/

#if !defined(__def_mzapi_source_main_c__)
#define __def_mzapi_source_main_c__ "main.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "mzslab.h"

int main(int s_argc, char **s_argv);

int main(int s_argc, char **s_argv)
{
    unsigned char s_my_local_page[ 128 ];
    mzslab_t *s_slab;

    void *s_ptr[ 16 ];

    (void)s_argc;
    (void)s_argv;

    s_slab = mzslab_init((void *)(&s_my_local_page[0]), sizeof(s_my_local_page), (size_t)16u);

    s_ptr[0] = mzslab_alloc(s_slab);
    s_ptr[1] = mzslab_alloc(s_slab);
    s_ptr[2] = mzslab_alloc(s_slab);
    s_ptr[3] = mzslab_alloc(s_slab);

    s_ptr[1] = mzslab_free(s_slab, s_ptr[1]);
    s_ptr[2] = mzslab_free(s_slab, s_ptr[2]);

    s_ptr[4] = mzslab_alloc(s_slab);
    s_ptr[5] = mzslab_alloc(s_slab);
    s_ptr[6] = mzslab_alloc(s_slab);

    s_ptr[5] = mzslab_free(s_slab, s_ptr[5]);

    for(;;) {
        s_ptr[8] = mzslab_alloc(s_slab);
        s_ptr[8] = mzslab_free(s_slab, s_ptr[8]);
    }

    return(0);
}

#endif

/* vim: set expandtab: */
/* End of source */
