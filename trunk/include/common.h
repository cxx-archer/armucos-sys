#ifndef _H_STD_H
#define _H_STD_H

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef signed int sint32_t;
typedef signed short sint16_t;
typedef signed char sint8_t;

typedef sint32_t bool;

#ifndef NULL
#define NULL	((void*)0)
#endif

#ifdef false
#undef false
#endif

#ifdef true
#undef true
#endif

#define false		(0)
#define true		(1)
#define BIT(n)		(0x1 << (n))

#define SECTION(section) 	__attribute__((__section__(#section)))
#define __BOOTSTONE 		SECTION(.bootstone.text)
#define __BOOTSTONE_DATA	SECTION(.bootstone.data)

#define INLINE			inline

#define offset_of(f,s)		((uint32_t)(&((s*)0)->f))

#define container_of(ptr,type,member) \
	((type *)((uint32_t)(ptr) - offset_of(member,type)))

#define MIN(a,b)		((a) < (b) ? (a) : (b))
#define MAX(a,b)		((a) > (b) ? (a) : (b))

INLINE static void set_bit(void *p, int nr)
{
    uint8_t *o = (uint8_t*)p + nr / 8;
    *o |= (1 << (nr % 8));
}

INLINE static void clear_bit(void *p, int nr)
{
    uint8_t *o = (uint8_t*)p + nr / 8;
    *o &= ~(1 << (nr % 8));
}

INLINE static bool xor_bit(void *p, int nr)
{
    uint8_t *o = (uint8_t*)p + nr / 8;
    *o ^= (1 << (nr % 8));
    return !!(*o & (1 << (nr % 8)));
}

INLINE static bool get_bit(void *p, int nr)
{
    uint8_t *o = (uint8_t*)p + nr / 8;
    return !!(*o & (1 << (nr % 8)));
}



#endif
