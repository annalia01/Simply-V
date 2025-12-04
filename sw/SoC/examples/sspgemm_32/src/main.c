/*
 * Vector kernel implemented for the thesis work of <Annalia Ruggiero>.
 *
 * The algorithm is based on the S2 scheme described in:
 * <Titopoulos; Alexandridis; Peltekis; Nicopoulos; Dimitrakopoulos>,
 * <Optimizing Structured-Sparse Matrix Multiplication in RISC-V Vector Processors>, <2025>.
 *
 * In this implementation, non-zero values are stored following
 * the layout used in algorithm S3, while the computational flow
 * follows algorithm S2.
 */

#include <stdint.h>
#include <string.h>

#include "sspgemm.h"
#include "runtime.h"
#include "util.h"

#define MSTATUS_FS (0x000006000U)
#define MSTATUS_XS (0x000018000U)
#define MSTATUS_VS (0x000000600U)
#define NR_LANES 2
#include "uninasoc.h"

extern int32_t cols;
extern int32_t rows;
extern int32_t M;
extern int32_t N;
extern int32_t cols_b;

extern float VALUES[] __attribute__((aligned(32 * NR_LANES)));
extern int32_t col_idx[] __attribute__((aligned(32 * NR_LANES)));
extern float B[] __attribute__((aligned(32 * NR_LANES)));
extern float C[] __attribute__((aligned(32 * NR_LANES)));

static inline uint64_t read_minstret(void) {
    uint64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main() {
  uninasoc_init();
  printf("\n");
  printf("==========\n");
  printf("=  SSpGEMM =\n");
  printf("==========\n");
  printf("\n");
  printf("\n");
  asm volatile (" li      t0, %0     " :: "i"(MSTATUS_FS | MSTATUS_XS | MSTATUS_VS));
  asm volatile (" csrs    mstatus, t0" );

  printf("\n");
  printf(
      "-------------------------------------------------------------------\n");
  printf(
      "Calculating");
 
  printf(
      "-------------------------------------------------------------------\n");
  printf("\n");

  
  uint64_t start_minstret = read_minstret();

  start_timer();
  sspgemm_32(M, N, rows, cols, VALUES, col_idx, B, cols_b, C);
  stop_timer();

  uint64_t end_minstret = read_minstret();
  uint64_t delta_minstret = end_minstret - start_minstret;

  // Metrics
  int64_t runtime = get_timer();

  printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);

  printf("The execution took %d cycles.\n", runtime);


  printf("Verifying ...\n");
  if (sspgemm_verify_matrix(M, N, rows, cols, VALUES, col_idx, B, cols_b, C)) {
    return 1;
  } else {
    printf("Passed.\n");
  }
  return 0;
}
