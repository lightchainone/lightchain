
#ifndef __DF_BITOPS_H__
# define __DF_BITOPS_H__
# if defined __x86_64__
#  include "asm-x86_64/df_bitops.h"
# else
#  if defined __i386__
#   include "asm-i386/df_bitops.h"
#  else
#   error "MUST COMPILED ON i386 or x86_64"
#  endif
# endif
#endif

