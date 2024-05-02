/*********************************************************************
 * Author: Zhou Linlin
 * E-mail: 461146760@qq.com
 * Date  : 2022-06-10 10:08:21.
 * Copyright (c) 2022 all rights reserved.
*********************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#ifdef __cplusplus
extern "C"{
#endif

#define IMEI_LEN			15
#define ICCID_LEN			10
#define ICCID_STR_LEN		20
#define IMSI_LEN			9	// start with: 08 x9
#define IMSI_STR_LEN		15
#define KI_OPC_LEN			16
#define MAX_APN_NAME_LEN	32

typedef char 			    s8;
typedef unsigned char 	    u8;
typedef short 			    s16;
typedef unsigned short 	    u16;
typedef int 			    s32;
typedef unsigned int 	    u32;
typedef long long 			s64;
typedef unsigned long long 	u64;

typedef enum {
    DISABLE,
    ENABLE
} functional_state_t;

#ifndef bool
typedef enum {false, true} boolean;
#else
typedef bool boolean;
#endif

#define size_array(a) sizeof(a)/sizeof(a[0])

#ifdef __cplusplus
}
#endif
#endif
