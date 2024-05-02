/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 8/30/18 5:53 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#ifndef SECURITY_IOT_UTILS_H
#define SECURITY_IOT_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include "types.h"
#include <unistd.h>

#ifdef __cplusplus
extern "C"{
#endif

#define MAX_PATH_LEN    1024

#define swap8(s) (((s) >> 4)|((s) << 4))

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
#define MAX_COMMAND_STRING_LENGTH   1024

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

//#define container_of(ptr, type, member) ({                  
//    const typeof(((type *)0)->member) *__mptr = (ptr);      
//    (type *)((char *)__mptr - offsetof(type,member));})

#define is_digital(c)   (c >= '0' && c <= '9') ? true : false

int hex2str(const u8 *hex, u32 hex_len, char *str, u32 str_len);
int str2hex(const char *str, u8 *hex, u32 hex_len);
int is_ip_address(const char *s);
int mk_dir(const char *dir);
int __execute_command(const char *file, const char *func, u32 line, char *resp, size_t resp_len, const char *fmt, ...);
int hex_char_to_int(char hex);
int __execute_shell_command(const char *file, const char *func, u32 line, const char *fmt, ...);
int copy(const char *src, const char *dest);
int mv(const char *src, const char *dest);
int string_set(const char **target, const char *value);
void string_free(const char *string);
char *strtrim(char *s);
char *strtrimc(char *s);
size_t count_ch(const char *src, char c);
int hex_str_to_hex_data(const char *hex_str, u8 *hex_data);

void half_byte_swap_buffer(u8 *buf, u16 buf_len);

#define execute_shell_command(...)              __execute_shell_command(__FILENAME__, __func__, __LINE__, ##__VA_ARGS__)
#define execute_command(resp, resp_len, ...)    __execute_command(__FILENAME__, __func__, __LINE__, resp, resp_len, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif
#endif // SECURITY_IOT_UTILS_H
