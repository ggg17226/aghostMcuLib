/*
 *  Created by aghost on 2019/7/10.
 *  last edit on 2019/7/10
 *
 *  dynamic control for stm32 ccmram
 *
 *  please edit these macro to fit your program and hardware
 *    AGHOST_CCMRAM_CTRL_SIZE
 *    AGHOST_CCMRAM_CTRL_MIN_ALLOC_SIZE
 *    AGHOST_CCMRAM_CTRL_ALLOC_ALIGN - 2/4/8/16/32
 *    CCMRAM - this macro must be the same as ccmram section name in ld script
 *    AGHOST_CCMRAM_CTRL_USE_SHORT_FUNC_NAME
 */

#ifndef AGHOST_CCMRAM_CTRL_H
#define AGHOST_CCMRAM_CTRL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define AGHOST_CCMRAM_CTRL_USE_SHORT_FUNC_NAME

#ifndef AGHOST_CCMRAM_CTRL_VERSION
#define AGHOST_CCMRAM_CTRL_VERSION 1
#endif

#ifdef AGHOST_CCMRAM_CTRL_USE_SHORT_FUNC_NAME
#define ccmInit() aghostCcmramInit()
#define ccmMalloc(size) aghostCcmramMalloc(size)
#define ccmCalloc(size) aghostCcmramCalloc(size)
#define ccmFree(ptr) aghostCcmramFree(ptr)
#define ccmramMalloc(size) aghostCcmramMalloc(size)
#define ccmramCalloc(size) aghostCcmramCalloc(size)
#define ccmramFree(ptr) aghostCcmramFree(ptr)
#endif

#ifndef AGHOST_CCMRAM_CTRL_USE_FREE_SPACE_MERGE
#define AGHOST_CCMRAM_CTRL_USE_FREE_SPACE_MERGE 1
#endif

#ifndef AGHOST_CCMRAM_CTRL_SIZE
/**
 * number of bytes
 * 49152 = 48k
 * 61952 = 60.5k
 */
#define AGHOST_CCMRAM_CTRL_SIZE 49152
#endif

#ifndef AGHOST_CCMRAM_CTRL_MIN_ALLOC_SIZE
#define AGHOST_CCMRAM_CTRL_MIN_ALLOC_SIZE 4
#endif

#ifndef AGHOST_CCMRAM_CTRL_ALLOC_ALIGN
#define AGHOST_CCMRAM_CTRL_ALLOC_ALIGN 4
#endif

#if AGHOST_CCMRAM_CTRL_ALLOC_ALIGN == 2
#define AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT 1
#define AGHOST_CCMRAM_CTRL_ALLOC_ALIGN_NUM 2

#elif AGHOST_CCMRAM_CTRL_ALLOC_ALIGN == 4
#define AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT 2
#define AGHOST_CCMRAM_CTRL_ALLOC_ALIGN_NUM 4

#elif AGHOST_CCMRAM_CTRL_ALLOC_ALIGN == 8
#define AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT 3
#define AGHOST_CCMRAM_CTRL_ALLOC_ALIGN_NUM 8

#elif AGHOST_CCMRAM_CTRL_ALLOC_ALIGN == 16
#define AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT 4
#define AGHOST_CCMRAM_CTRL_ALLOC_ALIGN_NUM 16

#elif AGHOST_CCMRAM_CTRL_ALLOC_ALIGN == 32
#define AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT 5
#define AGHOST_CCMRAM_CTRL_ALLOC_ALIGN_NUM 32

#else
#define AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT 2
#define AGHOST_CCMRAM_CTRL_ALLOC_ALIGN_NUM 4

#endif


#ifndef CCMRAM
#define CCMRAM __attribute__((section(".ccmram")))
#endif


#ifndef NULL
#define NULL ((void *)0)
#endif


#ifndef AGHOST_MEM_BLOCK
#define AGHOST_MEM_BLOCK
typedef struct aghostMemBlock {
    struct aghostMemBlock *before; //4byte
    struct aghostMemBlock *next;   //4byte
    uint8_t attr;                    //1byte
    uint16_t size;                   //2byte
} aghostMemBlock_t;
#endif

#ifndef AGHOST_MEM_BLOCK_ATTRS
#define AGHOST_MEM_BLOCK_ATTRS

#define AGHOST_MEM_BLOCK_ATTR_FREE 0b00000001
#define AGHOST_MEM_BLOCK_ATTR_USED 0b00000000

#endif


#ifndef AGHOST_MEM_BLOCK_SIZE
#define AGHOST_MEM_BLOCK_SIZE sizeof(struct aghostMemBlock)
#endif


void aghostCcmramInit(void);

void *aghostCcmramMalloc(size_t size);

void *aghostCcmramCalloc(size_t size);

void aghostCcmramFree(void *ptr);

#ifdef __cplusplus
}
#endif
#endif //AGHOSTCCMRAMCTRL_H