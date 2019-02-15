#ifndef _INC_STDINTVC
#define _INC_STDINTVC

      /* For some reason,  Microsoft Visual C/C++ doesn't have a 'stdint.h' */
      /* file to define the following 'intxx_t' and 'uintxx_t' types.  This */
      /* is by no means a complete 'stdint.h' replacement.  I'll add things */
      /* in as I need them. */

typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
