/**
 * @file types.h
 * @author Linlin Zhou
 * @e-mail 461146760@qq.com
 * @abstract This file defines the basic data types.
 * @date 2018-08-14
 *
 * */

#ifndef __TYPES_H__
#define __TYPES_H__

typedef char 			s8;
typedef unsigned char 	u8;
typedef short 			s16;
typedef unsigned short 	u16;
typedef int 			s32;
typedef unsigned int 	u32;
typedef long 			s64;
typedef unsigned long 	u64;

#include <stdbool.h>
#include <glob.h>
#include <stdint.h>

#define size_array(a) sizeof(a)/sizeof(a[0])
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#define get_file_name(s) (strrchr(s, '/') ? (strrchr(s, '/') + 1):(s))

typedef enum {
    CES_SUCCESS                     = 0x00000000,
    CES_RESPONSE_TIMEDOUT           = 0x00000001,
    CES_LOCK_MUTEX_FAILED           = 0x00000002,
    CES_KERNEL_EVENT_FAILED         = 0x00000003,
    CES_PLC_HMI_ERROR               = 0x00000004,
    PLC_MAX_RETRY_ERRORS            = 0xFFFFFFFE,
    CES_FAILURE                     = -1,
    CES_UNDEFINE                    = 0xFFFFFFFF
}Command_Execute_Status_TypeDef;

#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
}
#endif
#endif
