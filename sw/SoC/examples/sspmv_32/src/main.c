#include <stdint.h>
#include <string.h>

#include "sspmv.h"
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

extern float VALUES[] __attribute__((aligned(32 * NR_LANES)));
extern int32_t col_idx[] __attribute__((aligned(32 * NR_LANES)));
extern float IN_VEC[] __attribute__((aligned(32 * NR_LANES)));
extern float OUT_VEC[] __attribute__((aligned(32 * NR_LANES)));

static inline uint64_t read_minstret(void) {
    uint64_t value;
    asm volatile ("csrr %0, instret" : "=r"(value));
    return value;
}

int main() {
  uninasoc_init();
  printf("\n");
  printf("==========\n");
  printf("=  SpMV  =\n");
  printf("==========\n");
  printf("\n");
  printf("\n");
  asm volatile (" li      t0, %0     " :: "i"(MSTATUS_FS | MSTATUS_XS | MSTATUS_VS));
  asm volatile (" csrs    mstatus, t0" );

  printf("\n");
  printf(
      "-------------------------------------------------------------------\n");
  printf(
      "Calculating a (%d x %d) x %d sparse matrix vector multiplication...\n",
      rows, cols);
 
  printf(
      "-------------------------------------------------------------------\n");
  printf("\n");

  printf("calculating ... \n");
  
  uint64_t start_minstret = read_minstret();
  
  start_timer();
  sspmv_32(M, N, rows, cols, VALUES, col_idx, IN_VEC, OUT_VEC);
  stop_timer();
 
  uint64_t end_minstret = read_minstret();
  uint64_t delta_minstret = end_minstret - start_minstret;

  // Metrics
  int64_t runtime = get_timer();
    

  printf("Instructions retired (CSR minstret): %lu\n", delta_minstret);

  printf("The execution took %d cycles.\n", runtime);


  printf("Verifying ...\n");
  if (sspmv_verify(M, N, rows, cols, VALUES, col_idx, IN_VEC, OUT_VEC)) {
    return 1;
  } else {
    printf("Passed.\n");
  }
  return 0;
}
