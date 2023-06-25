#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <stdint.h>

/* signed integers */
typedef int8_t B;
typedef int16_t H;
typedef int32_t W;
typedef int64_t D;

/* unsigned integers */
typedef uint8_t UB;
typedef uint16_t UH;
typedef uint32_t UW;
typedef uint64_t UD;

/* volatile signed integers */
typedef volatile int8_t _B;
typedef volatile int16_t _H;
typedef volatile int32_t _W;
typedef volatile int64_t _D;

/* volatole unsigned integers */
typedef volatile uint8_t _UB;
typedef volatile uint16_t _UH;
typedef volatile uint32_t _UW;
typedef volatile uint64_t _UD;

/* unsized integers */
typedef int INT;
typedef unsigned int UINT;

/* special types */
typedef INT ID;    /* ID */
typedef UW ATR;    /* attribute */
typedef INT ER;    /* error code */
typedef INT PRI;   /* priority */
typedef W TMO;     /* timeout */
typedef UW RELTIM; /* relative time */
typedef W SZ;      /* size */

typedef void (*FP)(); /* function pointer */

#define NULL (0)

typedef UINT BOOL;
#define TRUE (1)  /* True */
#define FALSE (0) /* False */

#endif
