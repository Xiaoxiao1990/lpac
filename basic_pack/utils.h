/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 8/30/18 5:53 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#ifndef SECURITY_IOT_UTILS_H
#define SECURITY_IOT_UTILS_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>
#include <stdbool.h>

#define swap16(s) (         \
    (((s) >> 8) & 0x00ff) | \
    (((s) << 8) & 0xff00)   \
    )

#define swap32(l) (               \
    (((l) >> 24) & 0x000000ff)  | \
    (((l) >>  8) & 0x0000ff00)  | \
    (((l) <<  8) & 0x00ff0000)  | \
    (((l) << 24) & 0xff000000)    \
    )

#define swap64(ll) (                  \
(((ll) >> 56) & 0x00000000000000ff) | \
(((ll) >> 40) & 0x000000000000ff00) | \
(((ll) >> 24) & 0x0000000000ff0000) | \
(((ll) >>  8) & 0x00000000ff000000) | \
(((ll) <<  8) & 0x000000ff00000000) | \
(((ll) << 24) & 0x0000ff0000000000) | \
(((ll) << 40) & 0x00ff000000000000) | \
(((ll) << 56) & 0xff00000000000000)   \
)

#define BigEndian                   1
#define LittleEndian                0

char big_endian_test(void);

#define BigEndian_16(s)         big_endian_test() ? s : swap16(s)
#define LittleEndian_16(s)      big_endian_test() ? swap16(s) : s
#define BigEndian_32(l)         big_endian_test() ? l : swap32(l)
#define LittleEndian_32(l)      big_endian_test() ? swap32(l) : l
#define BigEndian_64(ll)        big_endian_test() ? ll : swap64(ll)
#define LittleEndian_64(ll)     big_endian_test() ? swap64(ll) : ll

/**
* container_of - cast a member of a structure out to the containing structure
* @ptr:     the pointer to the member.
* @type:     the type of the container struct this is embedded in.
* @member:     the name of the member within the struct.
*
*/
//#define offsetof(s, m)   (size_t)&(((s *)0)->m)

#define container_of(ptr, type, member) ({                  \
    const typeof(((type *)0)->member) *__mptr = (ptr);      \
    (type *)((char *)__mptr - offsetof(type,member));})

int hex2str(const unsigned char *hex, const int hex_len, char *str, int str_len);

int str2hex(const char *str, unsigned char *hex, int hex_len);

int is_ip_address(const char *s);

bool is_process_running(const char *process);

int get_pid_by_name(const char *process);

int my_kill(const char *process);

int my_pkill(const char *process);

const char *get_time_stamp_string(void);

long long get_time_stamp_value(void);

int get_current_time_hour(void);

int get_current_time_minute(void);

int get_current_time_second(void);

char *get_md5_checksum_from_buffer(const unsigned char *data, size_t len);

char *get_md5_checksum_from_file(const char *file_name);

int my_system(const char *fmt, ...);

size_t count_ch(const char *src, char c);

#ifdef __cplusplus
}
#endif
#endif // SECURITY_IOT_UTILS_H
