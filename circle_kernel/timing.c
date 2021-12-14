//
// Created by Chang Liu on 9/27/21.
//

/**
 * Function: rdtsc
 * @return Number of cycles at nominal frequency
 */
unsigned long long rdtsc() {
    unsigned int a, d;

    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));

    return ((unsigned long long)a) | (((unsigned long long)d) << 32);
}