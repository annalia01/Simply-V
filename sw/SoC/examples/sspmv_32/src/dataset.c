#include <stdint.h>
#define NR_LANES 2


int32_t M = 4;
int32_t N = 2;
int32_t rows = 8;
int32_t cols = 8;

float VALUES[32] __attribute__((aligned(32 * NR_LANES))) = {
    1, 2, 3, 4,
    9, 10, 11, 12,
    17, 18, 19, 20,
    25, 26, 27, 28,
    33, 34, 35, 36,
    41, 42, 43, 44,
    49, 50, 51, 52,
    57, 58, 59, 60
};

int32_t col_idx[32] __attribute__((aligned(32 * NR_LANES))) = {
    0, 2, 1, 3,
    2, 3, 0, 1,
    0, 1, 1, 2,
    0, 3, 0, 2,
    1, 2, 1, 2,
    0, 3, 0, 3,
    1, 2, 2, 3
};

float IN_VEC[8] __attribute__((aligned(32 * NR_LANES))) = {
    1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0
};

float OUT_VEC[8] __attribute__((aligned(32 * NR_LANES)));
