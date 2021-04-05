#include <iostream>
#include "lfsr.h"

void lfsr_calculate(uint16_t *reg) {
    *reg = ((((*reg >> 5) ^ (*reg >> 3) ^ (*reg >> 2) ^ *reg) & 1) << 15) | (*reg >> 1);
}

