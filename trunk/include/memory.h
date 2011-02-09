#ifndef _H_MEMORY_H
#define _H_MEMORY_H
#include "common.h"
#include "list.h"

#define PAGE_SIZE   (4096)
#define PAGE_MASK   (~(PAGE_SIZE - 1))
#define PAGE_SHIFT  (12)
#define MAP_NR(addr)    ((addr) >> PAGE_SHIFT)
#define PAGE_ALIGN(addr)    ((((addr) + PAGE_SIZE - 1) & PAGE_MASK))

struct page
{
    struct list_head list;
};

#ifdef __cplusplus
extern "C" {
#endif

    void mem_init(uint32_t, uint32_t);
    uint32_t page_address(struct page *page);
    struct page* address_page(uint32_t addr);
    struct page* alloc_page(int order);
    void free_page(struct page*, int order);
    void dump_mem_info(void);

    void kmalloc_init(void);
    void kfree(void *p);
    void *krealloc(void *p, int size);

    /* kmalloc max size is 524268 = (512K - 20) bytes ,
     * if you need large memory , please use alloc_page function
     * */
    void * kmalloc(int size);

#ifdef __cplusplus
}
#endif
#endif
