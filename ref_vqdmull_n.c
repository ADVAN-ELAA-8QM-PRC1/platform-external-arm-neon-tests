/*

Copyright (c) 2009, 2010, 2011, 2012 STMicroelectronics
Written by Christophe Lyon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifdef __arm__
#include <arm_neon.h>
#else
#include "stm-arm-neon.h"
#endif

#include "stm-arm-neon-ref.h"

#define INSN vqdmull
#define TEST_MSG "VQDMULL_N"
#define FNNAME1(NAME) void exec_ ## NAME ## _n (void)
#define FNNAME(NAME) FNNAME1(NAME)

FNNAME (INSN)
{
  int i;

  /* vector_res = vqdmull_n(vector,val), then store the result.  */
#define TEST_VQDMULL_N2(INSN, T1, T2, W, W2, N, L)	\
  Set_Neon_Overflow(0);					\
  VECT_VAR(vector_res, T1, W2, N) =			\
    INSN##_n_##T2##W(VECT_VAR(vector, T1, W, N),	\
		     L);				\
  vst1q_##T2##W2(VECT_VAR(result, T1, W2, N),		\
		 VECT_VAR(vector_res, T1, W2, N));	\
  dump_neon_overflow(TEST_MSG, xSTR(INSN##_n_##T2##W))

  /* Two auxliary macros are necessary to expand INSN */
#define TEST_VQDMULL_N1(INSN, T1, T2, W, W2, N, L)	\
  TEST_VQDMULL_N2(INSN, T1, T2, W, W2, N, L)

#define TEST_VQDMULL_N(T1, T2, W, W2, N, L)	\
  TEST_VQDMULL_N1(INSN, T1, T2, W, W2, N, L)


  /* With ARM RVCT, we need to declare variables before any executable
     statement */
  DECL_VARIABLE(vector, int, 16, 4);
  DECL_VARIABLE(vector, int, 32, 2);
  DECL_VARIABLE(vector2, int, 16, 4);
  DECL_VARIABLE(vector2, int, 32, 2);

  DECL_VARIABLE(vector_res, int, 32, 4);
  DECL_VARIABLE(vector_res, int, 64, 2);

  clean_results ();

  /* Initialize vector */
  TEST_VDUP(vector, , int, s, 16, 4, 0x1000);
  TEST_VDUP(vector, , int, s, 32, 2, 0x1000);

  /* Initialize vector2 */
  TEST_VDUP(vector2, , int, s, 16, 4, 0x4);
  TEST_VDUP(vector2, , int, s, 32, 2, 0x2);

  /* Choose multiplier arbitrarily */
  fprintf(ref_file, "\n%s overflow output:\n", TEST_MSG);
  TEST_VQDMULL_N(int, s, 16, 32, 4, 0x22);
  TEST_VQDMULL_N(int, s, 32, 64, 2, 0x55);

  fprintf(ref_file, "\n%s output:\n", TEST_MSG);
  DUMP(TEST_MSG, int, 32, 4, PRIx32);
  DUMP(TEST_MSG, int, 64, 2, PRIx64);


  TEST_VDUP(vector, , int, s, 16, 4, 0x8000);
  TEST_VDUP(vector, , int, s, 32, 2, 0x80000000);
  fprintf(ref_file, "\n%s overflow output:\n",
	  TEST_MSG " (check mul overflow)");
  TEST_VQDMULL_N(int, s, 16, 32, 4, 0x8000);
  TEST_VQDMULL_N(int, s, 32, 64, 2, 0x80000000);

  fprintf(ref_file, "\n%s output:\n", TEST_MSG " (check mul overflow)");
  DUMP(TEST_MSG, int, 32, 4, PRIx32);
  DUMP(TEST_MSG, int, 64, 2, PRIx64);
}
