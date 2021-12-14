//
// Created by kevin on 2021/10/6.
//

#include "align.h"

/**
 * Calculate offset to next 32-aligned position.
 * @param ptr
 * @return The offset to next 32-aligned position.
 */
int mem_aligned32(void* ptr) {
    char offset = 32 - (size_t)ptr % 32;
    if(offset == 32) return 0;
    return offset;
}