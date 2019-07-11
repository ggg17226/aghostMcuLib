//
// Created by aghost on 2019/7/10.
//

#include <sched.h>
#include "aghostCcmramCtrl.h"

CCMRAM uint8_t aghostCcmram[AGHOST_CCMRAM_CTRL_SIZE];
CCMRAM void *lastByte;
CCMRAM aghostMemBlock_t *firstBlock;


void mergeFreeSpace() {
    //first block
    aghostMemBlock_t *ptr = (aghostMemBlock_t *) &aghostCcmram[0];
    while (1) {
        // finish merge condition
        if (ptr->next == NULL) {
            return;
        }
        //find and merge free space
        aghostMemBlock_t *nextPtr = ptr->next;
        if (ptr->attr & AGHOST_MEM_BLOCK_ATTR_FREE == AGHOST_MEM_BLOCK_ATTR_FREE &&
            nextPtr->attr & AGHOST_MEM_BLOCK_ATTR_FREE == AGHOST_MEM_BLOCK_ATTR_FREE) {
            ptr->next = nextPtr->next;
            nextPtr->before = NULL;
            ptr->size += nextPtr->size + AGHOST_MEM_BLOCK_SIZE;
        } else {
            ptr = nextPtr;
            continue;
        }
    }
}

void aghostCcmramInit(void) {
    void *pos = &aghostCcmram[0];
    uint16_t count = AGHOST_CCMRAM_CTRL_SIZE;
    while (count--) {
        *(char *) pos = 0;
        pos = (char *) pos + 1;
    }
    lastByte = &aghostCcmram[AGHOST_CCMRAM_CTRL_SIZE];
    firstBlock = (aghostMemBlock_t *) &aghostCcmram[0];
    firstBlock->before = NULL;
    firstBlock->next = NULL;
    firstBlock->attr = AGHOST_MEM_BLOCK_ATTR_FREE;
    firstBlock->size = sizeof(aghostCcmram) - AGHOST_MEM_BLOCK_SIZE;
}


void *aghostCcmramMalloc(size_t size) {
    // check size
    if (size < AGHOST_CCMRAM_CTRL_MIN_ALLOC_SIZE) {
        size = AGHOST_CCMRAM_CTRL_MIN_ALLOC_SIZE;
    } else {
        if (size % AGHOST_CCMRAM_CTRL_ALLOC_ALIGN_NUM != 0) {
            size = (size >> AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT + 1)
                    << AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT;
        }
    }

    //first block
    aghostMemBlock_t *block = firstBlock;
    if (
            (block->attr & AGHOST_MEM_BLOCK_ATTR_FREE == AGHOST_MEM_BLOCK_ATTR_FREE) &&
            (block->size >= (size + AGHOST_MEM_BLOCK_SIZE))
            ) {

        aghostMemBlock_t *newBlock = (aghostMemBlock_t *) ((void *) block + AGHOST_MEM_BLOCK_SIZE + size);
        newBlock->before = block;
        newBlock->next = NULL;
        newBlock->size = block->size - AGHOST_MEM_BLOCK_SIZE - size;
        newBlock->attr = AGHOST_MEM_BLOCK_ATTR_FREE;
        block->attr = AGHOST_MEM_BLOCK_ATTR_USED;
        block->size = size;
        block->next = newBlock;
        return ((void *) block) + AGHOST_MEM_BLOCK_SIZE;
    }
        //find free block
    else {
        while (1) {
            if (block->next == NULL) {
                return NULL;
            }
            block = block->next;
            if (
                    (block->attr & AGHOST_MEM_BLOCK_ATTR_FREE == AGHOST_MEM_BLOCK_ATTR_FREE) &&
                    (block->size >= (size + AGHOST_MEM_BLOCK_SIZE))
                    ) {
                aghostMemBlock_t *newBlock = (aghostMemBlock_t *) ((void *) block + AGHOST_MEM_BLOCK_SIZE + size);
                newBlock->before = block;
                newBlock->next = NULL;
                newBlock->size = block->size - AGHOST_MEM_BLOCK_SIZE - size;
                newBlock->attr = AGHOST_MEM_BLOCK_ATTR_FREE;
                block->attr = AGHOST_MEM_BLOCK_ATTR_USED;
                block->size = size;
                block->next = newBlock;
                return ((void *) block) + AGHOST_MEM_BLOCK_SIZE;
            } else {
                continue;
            }
        }
    }
}

void *aghostCcmramCalloc(size_t size) {
    // check size
    if (size < AGHOST_CCMRAM_CTRL_MIN_ALLOC_SIZE) {
        size = AGHOST_CCMRAM_CTRL_MIN_ALLOC_SIZE;
    } else {
        if (size % AGHOST_CCMRAM_CTRL_ALLOC_ALIGN_NUM != 0) {
            size = (size >> AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT + 1)
                    << AGHOST_CCMRAM_CTRL_ALLOC_MOVEBIT;
        }
    }
    //alloc space
    void *ptr = aghostCcmramMalloc(size);
    if (ptr == NULL)return NULL;
    else {
        // init space
        void *ptrFirstByte = ptr;
        while (size--) {
            *(char *) ptr = 0;
            ptr = (char *) ptr + 1;
        }
        return ptrFirstByte;
    }
}


void aghostCcmramFree(void *ptr) {
    if (ptr <= (void *) &aghostCcmram[0] || ptr >= lastByte) {
        return;
    }
    aghostMemBlock_t *block = (aghostMemBlock_t *) (ptr - AGHOST_MEM_BLOCK_SIZE);
    aghostMemBlock_t *firstPtr = firstBlock;
    //free first block
    if (block == firstPtr) {
        if ((block->attr << 7 >> 7) == AGHOST_MEM_BLOCK_ATTR_USED) {
            block->attr = AGHOST_MEM_BLOCK_ATTR_FREE;
            if (AGHOST_CCMRAM_CTRL_USE_FREE_SPACE_MERGE) mergeFreeSpace();
        }
        return;
    }
    //check block
    if (
            (block->attr << 7 >> 7) != AGHOST_MEM_BLOCK_ATTR_USED ||
            block->before == NULL
            ) {
        return;
    }
    //search first block
    aghostMemBlock_t *searchBlockPtr = firstPtr;
    while (searchBlockPtr->next != NULL) {
        searchBlockPtr = searchBlockPtr->next;
        if (searchBlockPtr == block) {
            block->attr = AGHOST_MEM_BLOCK_ATTR_FREE;
            if (AGHOST_CCMRAM_CTRL_USE_FREE_SPACE_MERGE) mergeFreeSpace();
            return;
        }
    }
}
