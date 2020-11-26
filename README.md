# slab_allocator

https://www.minzkn.com/moniwiki/wiki.php/SlabAllocator

<pre>
=== 유래 및 특징 ===
 Slab Allocator(슬랩 할당자)는 '''1994년 Sun Microsystems의 Solaris 2.4라는 운영체제'''에서 구현된 "Slab allocator"라는 할당전략에서 유래되었습니다.

 이 할당전략의 주요 특징으로는 다음과 같은 점이 있습니다.

  1. 유지에 필요한 자료구조와 함께 생성자(Constructor), 소멸자(Destructor)를 구현함으로써 일종의 객체로서의 접근을 구현합니다. (하지만 일부 운영체제에서는 객체관점으로 바라보기는 하지만 생성자, 소멸자를 사용하지 않는 경우도 많이 있습니다.)
  1. 객체를 불필요하게 반복 초기화 하는 현상을 회피하기 위하여 할당후에 해제되는 객체를 즉각 폐기하지 않고 메모리에 그대로 유지하려는 속성을 가지고 있습니다.
  1. 비슷한 크기의 객체를 캐시하는 유사구조를 사용하기 때문에 일반적으로 발생할수 있는 단편화 문제를 해소합니다.
  1. 리눅스의 경우 커널에서 사용하는 일련의 구조중에 매우 빈도가 높은 반복할당객체에 대해서 Cache로 바라보고 그에 맞도록 Slab을 구현합니다.
  1. 하드웨어 캐시에 정렬될수 있고 컬러링기법이 적용되기 때문에 캐시성능을 높일수 있습니다.

=== 예제를 통한 Slab Allocator의 접근 ===
 * 예제에서는 다음과 같은 구조를 설계안(초기화과정: mzslab_init)으로 설정하고 구현하였습니다. 설계안에서는 생성자(Constructor), 소멸자(Destructor)를 고려하였으나 예제코드 구현에서 이 부분은 적절한 객체의 표면화된 예시까지 제시할 필요가 없기 때문에 생략되었습니다.

 * Slab 자료구조
  {{{#!plain
/*
 Copyright (C) JAEHYUK CHO
 All rights reserved.
 Code by JaeHyuk Cho <mailto:minzkn@minzkn.com>
*/

/* object index의 단위를 나타내는 변수형입니다.
   object수가 65536이상인 경우 이것은 좀 키워야 겠죠. */
typedef unsigned short int __mzslab_index_t;
#define mzslab_index_t __mzslab_index_t

#pragma pack(push,8)
typedef struct mzslab_ts {
    /* object 의 크기를 나타냅니다. */
    size_t object_size;
    /* object 수를 나타냅니다. */
    size_t objects;
    /* 첫 object 의 포인터입니다.
       이 또한 런타임에 계산 가능하지만 미리... */
    unsigned char *entry;
    /* 첫번째 해제된 Object 의 index를 저장합니다.
       만약 이 값이 objects보다 같거나 크게 되면 할당가능한 object가 없게 됩니다. */
    mzslab_index_t f;
}__mzslab_t;
#define mzslab_t __mzslab_t
#pragma pack(pop)
}}}

 * mzslab_t 의 초기화
  {{{#!plain
mzslab_t * mzslab_init(void *s_page, size_t s_page_size, size_t s_object_size)
{
    size_t s_index;

    mzslab_t *s_slab;
    mzslab_index_t *s_nft; /* next free table */

    /* page의 첫 선두부분을 slab구조저장영역으로 사용합니다. */    
    s_slab = (mzslab_t *)s_page;
    
    /* 하나의 object가 가지는 유닛의 크기를 저장해둡니다. */
    s_slab->object_size = s_object_size;

    /* 여러가지 page의 크기에 따른 적절한 object수를 산출하기 위해서 slab index영역을 고려한 산출loop를 사용합니다. */
    s_slab->objects = (s_page_size - sizeof(mzslab_t)) / s_object_size;
    while((s_slab->objects > ((size_t)0)) && ((s_page_size - sizeof(mzslab_t)) < ((s_slab->objects * s_object_size) + (s_slab->objects * sizeof(mzslab_index_t))))) {
        s_slab->objects--;
    }
    if(s_slab->objects <= ((size_t)0)) { /* object를 담기에 불가능한 크기인 경우 slab은 초기화 할수 없습니다. */
        return((mzslab_t *)0);
    }

    /* 첫 object를 가르키는 포인터를 저장해둡니다. */
    s_slab->entry = ((unsigned char *)(&s_slab[1])) + (s_slab->objects * sizeof(mzslab_index_t));

    /* 할당되지 않은 object의 첫번째 index를 저장합니다. */
    s_slab->f = (mzslab_index_t)0u;

    /* 해제된 메모리로 표현하기 위하여 index를 초기화 합니다. */    
    s_nft = (mzslab_index_t *)(&s_slab[1]);
    for(s_index = ((size_t)0u);s_index < s_slab->objects;s_index++) {
        s_nft[s_index] = s_index + ((size_t)1u);
    }

    return(s_slab);
}
}}}
  가장 먼저 Free object block index (s_slab->f)를 0으로 초기화 하고 Slab index table (s_nft[n] = n + 1)을 순차적으로 그 다음값으로 초기화 하여 Slab을 사용하기 위한 초기화 동작을 완료합니다.

 * 할당
  {{{#!plain
void * mzslab_alloc(mzslab_t *s_slab)
{
    mzslab_index_t *s_nft = (mzslab_index_t *)(&s_slab[1]);
    void *s_result;

    if(s_slab->f >= s_slab->objects) {
        /* 이 경우 메모리에 할당가능한 object가 없다고 판단하면 되겠습니다. */
        s_result = (void *)0;
    }
    else { /* 할당가능한 object가 있으므로 이를 반환합니다. */
        s_result = (void *)(&s_slab->entry[s_slab->f * s_slab->object_size]);
        s_slab->f = s_nft[s_slab->f];
    }

    return(s_result);
}
}}}
  할당가능한 Free object block index (s_slab->f)에 대응하는 Free object block의 메모리주소 (&s_slab->entry[...])를 반환후 s_slab->f가 가르키는 Slab index table (s_nft[s_slab->f])에서 가르키는 다음 Free object block index를 s_slab->f로 취하여 할당이 완료되게 됩니다.

 * 해제
  {{{#!plain
/* 원래 s_slab은 전역 cache테이블로 감추고
   s_ptr만으로 추출할수 있도록 구현하는게 바람직하지만
   현재 예제를 들기 위한 부분이기에 그냥 인자로 넘겨 받도록 하여 소개합니다. */
void * mzslab_free(mzslab_t *s_slab, void *s_ptr)
{
    mzslab_index_t *s_nft = (mzslab_index_t *)(&s_slab[1]);
    size_t s_index;

    s_index = (size_t)(((unsigned char *)s_ptr) - s_slab->entry) / s_slab->object_size;

    s_nft[s_index] = s_slab->f;
    s_slab->f = s_index;

    return((void *)0);
}
}}}
  할당된 Object block memory 주소 (s_ptr)로부터 해당하는 Object block index (s_index)를 취하고 해당 Object block index의 Slab index table (s_ntf[s_index])에 현재 할당 가능한 Freee object block index (s_slab->f)를 저장하고 Object block index (s_index)를 Free object block index (s_slab->f)에 저장하여 해제동작이 완료됩니다.
</pre>
