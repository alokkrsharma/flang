/*
 * Copyright (c) 2014-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "mthdecls.h"

/*
 * libm's round() could also be used if compiled with:
 *      _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
 */

#if     defined(TARGET_LINUX_POWER)
double
__mth_i_dnint(double f)
{
  double x;
  asm("frin %0, %1"
     : "=d"(x)
     : "d"(f)
     :
     );
  return x;
}

#elif   defined(__aarch64__)
double
__mth_i_dnint(double f)
{
  double r;
  asm("frinta   %d0, %d1"
    : "=w"(r)
    : "w"(f)
    :);
  return r;
}

#else   /* #if     defined(TARGET_LINUX_POWER) */
#include <math.h>
#include <ieee754.h>

double
__mth_i_dnint(double f)
{
  double x = f;     /* Cases where f == 0.0, or f == NaN */
  union ieee754_double *u = (union ieee754_double *)&x;

  /*
   * Depending on the default rounding mode of the processor, the logic
   * below with modf(f + 0.5) can result in a bogus rounding when 0.5
   * is normalized such that it falls within the guard or round bits.
   *
   * Fast return if the exponent guarantees that the floating point number
   * is a whole integer.
   *
   * This quick exit also catches infinities and NaNs.
   */

  if (u->ieee.exponent >= IEEE754_DOUBLE_BIAS+52) return x;

  if (f > 0.0)
    (void)modf(f + 0.5, &x);
  else if (f < 0.0)
    (void)modf(f - 0.5, &x);
  return x;
}
#endif  /* #if     defined(TARGET_LINUX_POWER) */
