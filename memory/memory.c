#include "common.h"
#include "list.h"
#include "memory.h"
#include "utils.h"
#include "includes.h"

#define NR_FREE_LIST (15)
#define ALIGN(x, align) (((x) + (align) - 1) & ~((align) - 1))

//static struct vos_mutex  memory_lock;
static OS_EVENT *memory_lock = NULL;
static struct page      *mem_map = NULL;

#define vos_mutex_lock
#define vos_mutex_unlock

static struct free_area
{
    uint32_t *map;
    int count;
    struct list_head  page_list;
}free_area_list[NR_FREE_LIST];

uint32_t page_address(struct page *page)
{
    uint32_t map_nr = page - mem_map;
    return map_nr << PAGE_SHIFT;
}

struct page* address_page(uint32_t addr)
{
    uint32_t map_nr = MAP_NR(addr);
    return mem_map + map_nr;
}

static uint32_t free_area_init(uint32_t start_mem, uint32_t end_mem)
{
    uint32_t               addr;
    sint32_t               map_nr, order;
    struct page      *p;
    struct free_area *area;

    start_mem = PAGE_ALIGN(start_mem);
    mem_map   = (struct page*)start_mem;
    map_nr    = MAP_NR(end_mem);
    p         = mem_map + map_nr;
    memset(mem_map, 0, (uint32_t)p - (uint32_t)mem_map);
    start_mem = ALIGN((uint32_t)p, sizeof(uint32_t));

    /* start_mem is point to the bitmap */
    for(area = free_area_list, order = 0;
	area < free_area_list + NR_FREE_LIST;
	area++, order++ ) {
	int bitlength;

	bitlength  = end_mem >> (PAGE_SHIFT + order + 1);
	bitlength  = (bitlength + 7) >> 3;
	area->map  = (int*)start_mem;
	INIT_LIST_HEAD(&area->page_list);
	memset(area->map, 0, bitlength);
	area->count = 0;
	start_mem += bitlength;
	start_mem  = ALIGN(start_mem, sizeof(uint32_t));
    }

    return start_mem;
}

static struct page* __alloc_page(int order)
{
    uint32_t               size;
    int               map_nr, index;
    int               new_order;
    struct page*      page = NULL;
    struct free_area *area;

    if(order >= NR_FREE_LIST) {
	printk("alloc_page FAULT!\n");
	return NULL;
    }
    area      = free_area_list + order;
    new_order = order;

    do{
	if(!list_empty(&area->page_list)) {
	    page = list_entry(area->page_list.next, struct page, list);
	    break;
	}
	area++;
	new_order++;
    }while(new_order < NR_FREE_LIST);

    if(!page) {
	return NULL;
    }
    size   = 1 << new_order;
    list_del(&page->list);
    area->count--;
    map_nr = page - mem_map;
    index  = map_nr >> (new_order + 1);

    xor_bit(area->map, index);

    while(new_order > order) {
	area--;
	new_order--;
	size >>= 1;

	list_add_tail(&page->list, &area->page_list);
	area->count++;

	map_nr  = page - mem_map;
	index   = map_nr >> (new_order + 1);
	xor_bit(area->map, index);
	page   += size;
	map_nr += size;
    }

    return mem_map + map_nr;
}

struct page* alloc_page(int order)
{
    INT8U err;
    struct page *pg = NULL;

    OSMutexPend(memory_lock, 0, &err);
    if(err != OS_NO_ERR){
	panic("OSMutexAccept Failed!\n");
    }
    pg = __alloc_page(order);
    err = OSMutexPost(memory_lock);
    if(err != OS_NO_ERR){
	panic("OSMutexPost Failed!\n");
    }
    
    return pg;
}

static void __free_page(struct page *page, int order)
{
    uint32_t               mask;
    int               map_nr, index;
    struct page      *buddy;
    struct free_area *area;

    map_nr  = page - mem_map;
    mask    = ~((1 << order) - 1);
    map_nr &= mask;
    area    = free_area_list + order;
    index   = map_nr >> (order + 1);

    while(mask + (1 << (NR_FREE_LIST - 1))) {
	if(xor_bit(area->map, index))
	    break;

	buddy = mem_map + (map_nr ^ -mask);
	list_del(&buddy->list);
	area->count--;

	mask   <<= 1;
	map_nr  &= mask;
	index  >>= 1;
	area++;
    }

    list_add_tail(&mem_map[map_nr].list, &area->page_list);
    area->count++;
}

void free_page(struct page *page, int order)
{
    INT8U err;

    OSMutexPend(memory_lock, 0, &err);
    if(err != OS_NO_ERR){
	panic("OSMutexAccept Failed!\n");
    }
    __free_page(page, order);
    err = OSMutexPost(memory_lock);
    if(err != OS_NO_ERR){
	panic("OSMutexPost Failed!\n");
    }
}

void mem_init(uint32_t start_mem, uint32_t end_mem)
{
    INT8U err;
    uint32_t          free;
    struct page *page;

    free = free_area_init(start_mem, end_mem);
    free = PAGE_ALIGN(free);

    page = mem_map + MAP_NR(free);
    while(page < mem_map + MAP_NR(end_mem)) {
	__free_page(page, 0);
	page++;
    }
    kmalloc_init();
    memory_lock = OSMutexCreate(1, &err);
    if(err != OS_NO_ERR){
	panic("OSMutexCreate Failed!\n");
    }
}

void dump_mem_info(void)
{
    int               order;
    struct free_area *area;
    
    for(area = free_area_list, order = 0; order < NR_FREE_LIST; area++, order++) {
	printk("free_area_list[%d], count=%d\n", order, area->count);
    }
}


/* copy from cnix system */
typedef struct list_head  list_t;
struct page_descriptor {
    list_t               list;
    struct block_header *pd_firstfree;
    int                  pd_order;
    int                  pd_nfrees;
};

#define PAGE_DESC_SIZE sizeof(struct page_descriptor)

struct block_header {
    unsigned long        bh_flags;
    struct block_header *bh_next;
};

struct sizes_descriptor {
    list_t                  list;
    struct page_descriptor *sd_dmafree;
    int                     sd_nblocks;
    int                     sd_npages; /* how many pages dose this bucket has */
    int                     sd_nfrees; /* how many free blocks in this bucket */
    unsigned long           sd_gfporder;
};

/* the blocksize = (PAGE_SIZE - sizeof(page_descriptor))/number */
static const unsigned int blocksize[] = {
    32,
    64,
    128,
    256,
    512,
    1024,
    2048,
    4096 - PAGE_DESC_SIZE,
    8192 - PAGE_DESC_SIZE,
    16384 - PAGE_DESC_SIZE,
    32768 - PAGE_DESC_SIZE,
    65536 - PAGE_DESC_SIZE,
    131072 - PAGE_DESC_SIZE,
    262144 - PAGE_DESC_SIZE,
    524288 - PAGE_DESC_SIZE,
    0
};

static struct sizes_descriptor sizes[] = {
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
    {{NULL, NULL}, NULL, 0, 0, 0, 1},
    {{NULL, NULL}, NULL, 0, 0, 0, 2},
    {{NULL, NULL}, NULL, 0, 0, 0, 3},
    {{NULL, NULL}, NULL, 0, 0, 0, 4},
    {{NULL, NULL}, NULL, 0, 0, 0, 5},
    {{NULL, NULL}, NULL, 0, 0, 0, 6},
    {{NULL, NULL}, NULL, 0, 0, 0, 7},
    {{NULL, NULL}, NULL, 0, 0, 0, 0},
};

#define BLOCKSIZE(order) (blocksize[order])
#define MFREE            0xAA55AA55UL

static void* __kmalloc(int size)
{
    char                    *p;
    int                      order;
    struct page*             page;
    struct block_header     *bh;
    struct sizes_descriptor *bucket;
    struct page_descriptor  *sd_firstfree, *pg;

    bucket = sizes;
    order  = 0;
    for(;;){
	if((size <= BLOCKSIZE(order)) || !BLOCKSIZE(order))
	    break;
	bucket++;
	order++;
    }

    if(!BLOCKSIZE(order)){
	printk("The size you request is too large for cnix\n");
	return NULL;
    }

    sd_firstfree = NULL;
    if(!list_empty(&bucket->list)) {
	list_t *tmp;

	list_for_each(tmp,  &bucket->list) {
	    sd_firstfree = list_entry(tmp, struct page_descriptor, list);
	    if (sd_firstfree->pd_firstfree) // has free block, use this page {
		if (sd_firstfree->pd_nfrees == 0) {
		    panic("Impossible, give up....");
		}
	    break;
	}
    }

/* has no free blocks, allocate */
    if((sd_firstfree == NULL) || (sd_firstfree->pd_nfrees == 0)){
	unsigned long         addr;
	struct block_header * tmp;
	int                   i;

	page = __alloc_page(bucket->sd_gfporder);
	if(!page){
	    printk("Not enough %d continous pages\n",
		   bucket->sd_gfporder);
	    return NULL;
	}
	addr = page_address(page);

	/* initial the bucket and the page_descriptor */
	sd_firstfree = pg = (struct page_descriptor *)addr;

	list_add(&pg->list,&bucket->list);

	bucket->sd_npages++;
	pg->pd_nfrees      = bucket->sd_nblocks;
	bucket->sd_nfrees += pg->pd_nfrees;
	pg->pd_order       = order;

	/* PAGE_DESC_SIZE = sizeof(struct page_descriptor) */
	pg->pd_firstfree
	    = (struct block_header *)(addr + PAGE_DESC_SIZE);

	addr += PAGE_DESC_SIZE;
	tmp   = (struct block_header *)addr;

	/* add the free blocks to the list */
	for(i = 0;i < (bucket->sd_nblocks - 1);i++){
	    addr += BLOCKSIZE(order); /* move to next block */

	    tmp->bh_flags = MFREE;
	    tmp->bh_next  = (struct block_header *)(addr);
	    tmp           = tmp->bh_next;
	}

	/* the last one */
	tmp->bh_flags = MFREE;
	tmp->bh_next  = NULL;
    }

    bh = sd_firstfree->pd_firstfree;
    if(bh->bh_flags != MFREE){
	printk("Error, block_header list in page is not free!\n");
	printk("this page pd_nfrees: %d \n", sd_firstfree->pd_nfrees);
	panic("");

	return NULL;
    }

    sd_firstfree->pd_firstfree = bh->bh_next;
    sd_firstfree->pd_nfrees--;

/* update the sizes_descriptor */
    bucket->sd_nfrees--;

    p = (char *)bh;
    return (void*)p;
}

void* kmalloc(int size)
{
    void *p = NULL;
    INT8U err;

    OSMutexPend(memory_lock, 0, &err);
    if(err != OS_NO_ERR){
	panic("OSMutexAccept Failed!\n");
    }
    p = __kmalloc(size);
    err = OSMutexPost(memory_lock);
    if(err != OS_NO_ERR){
	panic("OSMutexPost Failed!\n");
    }

    return p;
}
#define PAGE_DESC(x) ((struct page_descriptor *)((unsigned long)x & PAGE_MASK))
static void *__krealloc(void *p, int size)
{
    struct page_descriptor *pg;
    int                     order;
    void                   *nptr;

    pg    = PAGE_DESC(p);
    order = pg->pd_order;
    if (size <= BLOCKSIZE(order)) {                          // the same region
	return p;
    }

    // now, we should allocate the new size memory and copy
    // the old content to the new region, and free the old
    // memory.
    nptr = __kmalloc(size);
    if (!nptr) {
	return NULL;
    }

    memcpy(nptr, p, BLOCKSIZE(order));

    kfree(p);

    return nptr;
}

void *krealloc(void *p, int size)
{
    INT8U err;
    void *ptr = NULL;

    OSMutexPend(memory_lock, 0, &err);
    if(err != OS_NO_ERR){
	panic("OSMutexAccept Failed!\n");
    }
    ptr = __krealloc(p, size);
    err = OSMutexPost(memory_lock);
    if(err != OS_NO_ERR){
	panic("OSMutexPost Failed!\n");
    }
    return ptr;
}

#define SIZES_NUM (sizeof(sizes) / sizeof(struct sizes_descriptor))
static void __kfree(void *p)
{
    struct page_descriptor  *pg;
    struct block_header     *bh;
    struct sizes_descriptor *bucket;
    int                      order;

    pg = PAGE_DESC(p);
    bh = (struct block_header *)p;

    /* add the free block to the free list in page */
    bh->bh_next      = pg->pd_firstfree;
    pg->pd_firstfree = bh;

    bh->bh_flags = MFREE;      /* mark it unused */

    order  = pg->pd_order;
    bucket = &(sizes[order]);
    if(order >= SIZES_NUM - 1)
	panic("mem bug!\n");

    /* upate the page_desc and sizes_desc */
    pg->pd_nfrees++;
    bucket->sd_nfrees++;

    if(pg->pd_nfrees == bucket->sd_nblocks){ /* all free */
	list_del(&pg->list);

	/* update the size_desc */
	bucket->sd_npages--;
	bucket->sd_nfrees -= bucket->sd_nblocks;

	/* free the 2^sd_gfporder pages */
	__free_page(address_page((uint32_t)pg),bucket->sd_gfporder);
    }else if(&pg->list != bucket->list.next){ // not the first one
	list_del(&pg->list);
	list_add(&pg->list,&bucket->list);
    }
}

void kfree(void *p)
{
    INT8U err;

    OSMutexPend(memory_lock, 0, &err);
    if(err != OS_NO_ERR){
	panic("OSMutexAccept Failed!\n");
    }
    __kfree(p);
    err = OSMutexPost(memory_lock);
    if(err != OS_NO_ERR){
	panic("OSMutexPost Failed!\n");
    }
}

void kmalloc_init(void)
{
    int i;

    for(i          = 0; i < SIZES_NUM - 1; i++){
	int order = sizes[i].sd_gfporder;

	INIT_LIST_HEAD(&sizes[i].list);
	sizes[i].sd_nblocks = ((PAGE_SIZE << order) - PAGE_DESC_SIZE) / blocksize[i];
    }
}

