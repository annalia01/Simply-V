// Copyright 2020 ETH Zurich and University of Bologna.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Author: Matteo Perotti <mperotti@iis.ee.ethz.ch>

#include <stdint.h>
#include <string.h>

#include "runtime.h"

#include "dotproduct.h"

#include "util.h"
#define MSTATUS_FS (0x000006000U)
#define MSTATUS_XS (0x000018000U)
#define MSTATUS_VS (0x000000600U)
#define NR_LANES 2
#include "uninasoc.h"

// Run also the scalar benchmark
#define SCALAR 0

// Check the vector results against golden vectors
#define CHECK 0

static inline int64_t read_minstret(void) {
    int64_t value;
    asm volatile ("csrr %0, instret"
                  : "=r"(value));
    return value;
}

// Vector size (Byte)
extern uint64_t vsize;
// Vectors for benchmarks
extern int32_t v32a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int32_t v32b[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

// Output vectors

extern int32_t res32_v, res32_s;


int main() {
  uninasoc_init();
  printf("\n");
  printf("==========\n");
  printf("=  DOTP  =\n");
  printf("==========\n");
  printf("\n");
  printf("\n");

  int64_t runtime_s, runtime_v;
  asm volatile (" li      t0, %0     " :: "i"(MSTATUS_FS | MSTATUS_XS | MSTATUS_VS));
  asm volatile (" csrs    mstatus, t0" );

  for (uint64_t avl = 8; avl <= (vsize >> 2); avl *= 8) {
    // Dotp
    printf("Calulating 32b dotp with vectors with length = %lu\n", avl);

    int64_t start_minstret = read_minstret();
  
    start_timer();
    res32_v = dotp_v32b(v32a, v32b, avl);
    stop_timer();

    int64_t end_minstret = read_minstret();
    uint64_t delta_minstret = end_minstret - start_minstret;

    runtime_v = get_timer();
    printf("Vector runtime: %ld\n", runtime_v);

    printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);


    if (SCALAR) {
      start_timer();
      res32_s = dotp_s32b(v32a, v32b, avl);
      stop_timer();
      runtime_s = get_timer();
      printf("Scalar runtime: %ld\n", runtime_s);

      if (CHECK) {
        if (res32_v != res32_s) {
          printf("Error: v = %ld, g = %ld\n", res32_v, res32_s);
          return -1;
        }
      }
    }
  }

  printf("SUCCESS.\n");

  return 0;
}
