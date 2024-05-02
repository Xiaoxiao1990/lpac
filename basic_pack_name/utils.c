//#include <azure_c_shared_utility/refcount.h>

/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 8/30/18 5:54 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#include "utils.h"
#include "types.h"
#include "logs.h"

#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char big_endian_test(void)
{
    /*定义一个2个字节长度的数据，并赋值为1,则n的16进制表示为0x0001
    如果系统以“大端”存放数据，也即是以MSB方式存放，那么低字节存放的必定是0x00，高字节存放的必定是0x01
    如果系统以“小端”存放数据，也即是以LSB方式存放，那么低字节存放的必定是0x01，高字节存放的必定是0x00
    所谓MSB，就是将最重要的位存入低位，而LSB则是将最不重要的位存入低位
    我们可以通过检测低位的数值就可以知道系统的字节序
    */
    const s16 n = 1;
    if (*(char*)&n) {
        return LittleEndian;
    }
    return BigEndian;
}

/**
 * hex string transfer to hex array
 * @param str: input hex string
 * @param hex: output hex array
 * @param hex_len: output hex array storage buffer size
 * @return -1: error occur, others transformed hex array length.
 */
int str2hex(const char* str, u8* hex, u32 hex_len)
{
    int i = 0;
    u8 ch;

    if (str == NULL || hex == NULL || hex_len <= 0)
        return -1;

    while (*str) {
        if (*str >= '0' && *str <= '9') {
            ch = *str - '0';
        } else if (*str >= 'a' && *str <= 'f') {
            ch = *str - 'a' + 10;
        } else if (*str >= 'A' && *str <= 'F') {
            ch = *str - 'A' + 10;
        } else {
            return -1;
        }

        if ((i % 2) == 0)
            hex[i / 2] = (u8)((ch << 4) & 0xF0);
        else {
            hex[i / 2] |= (ch & 0x0F);
            if (i / 2 >= hex_len)
                return (int)hex_len;
        }
        i++;
        str++;
    }

    return i >> 1;
}

int hex2str(const u8* hex, u32 hex_len, char* str, u32 str_len)
{
    char ch[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    int i = 0, j = 0;

    if ((hex == NULL) || (str == NULL) || hex_len <= 0 || str_len <= 0)
        return -1;

    while ((i + 2) <= str_len) {
        str[i++] = ch[((hex[j] >> 4) & 0x0F)];
        str[i++] = ch[(hex[j++] & 0x0F)];
        str[i] = 0;
        if (j >= hex_len)
            break;
    }

    return i;
}

/* naive function to check whether char *s is an ip address */
int is_ip_address(const char* s)
{
    u32 n1, n2, n3, n4;

    if (sscanf(s, "%u.%u.%u.%u", &n1, &n2, &n3, &n4) != 4)
        return 0;

    if ((n1 <= 255) && (n2 <= 255) && (n3 <= 255) && (n4 <= 255))
        return 1;

    return 0;
}

int mk_path(const char* path)
{
    char command[MAX_PATH_LEN];

    if (path == NULL)return -1;

    if (snprintf(command, MAX_PATH_LEN, "mkdir -p %s", path) == 0)return -1;

    return system(command);
}

int __execute_command(const char* file, const char* func, u32 line, char* resp, size_t resp_len, const char* fmt, ...)
{
    va_list list;
    FILE* pF = NULL;
    int res;
    static char cmd[MAX_COMMAND_STRING_LENGTH] = { 0 };

    if (resp_len < 0 || resp == NULL) {
        LOGE("No space for command response!");
        return -1;
    }

    memset(cmd, 0, MAX_COMMAND_STRING_LENGTH);

    va_start(list, fmt);
    vsnprintf(cmd, MAX_COMMAND_STRING_LENGTH, fmt, list);
    va_end(list);

    //    LOGI("command: %s", cmd);
    logs(file, "", line, LOG_COLOR_I, "%s call execute_command: %s", func, cmd);

    if ((pF = popen(cmd, "r")) == NULL) {
        return -1;
    }

    if (fread(resp, 1, resp_len, pF) < 0) {
        LOGE("Get response error!");
        res = -1;
    } else
        res = 0;

    pclose(pF);

    return res;
}

int hex_char_to_int(char hex)
{
    int outHex;

    if (isdigit(hex)) {
        outHex = hex - '0';
    } else if (isupper(hex)) {
        outHex = hex - 'A' + 10;
    } else {
        outHex = hex - 'a' + 10;
    }

    return outHex;
}

int __execute_shell_command(const char* file, const char* func, u32 line, const char* fmt, ...)
{
    va_list list;
    char cmd[MAX_PATH_LEN] = { 0 };

    va_start(list, fmt);
    vsnprintf(cmd, LOGS_BUFFER_LENGTH, fmt, list);
    va_end(list);

    logs(file, "", line, LOG_COLOR_I, "%s: %s", func, cmd);

    return system(cmd);
}

int copy(const char* src, const char* dest)
{
    return execute_shell_command("cp %s %s", src, dest);
}

int mv(const char* src, const char* dest)
{
    return execute_shell_command("mv %s %s", src, dest);
}

int string_set(const char** target, const char* value)
{
    if (target == NULL)
        return -1;

    if (*target)
        free((void*)*target);

    *target = strdup(value);

    return 0;
}

void string_free(const char* string)
{
    if (string)
        free((void*)string);
}

/*****************************************************************************
 函 数 名  : strtrim
 功能描述  : 删除行首、行尾空白符号
 输入参数  : char *is
 输出参数  : 无
 返 回 值  : char *
*****************************************************************************/
char* strtrim(char* s)
{
    char* p = s;
    char* q = s;

    //去掉行首的空格
    while (*p == ' ' || *p == '\t')
        ++p;
    //赋值
    while ((p != NULL) && (q != NULL) && (*p != '\0') && (*p != '\0')) {
        *q++ = *p++;
    };

    //删除'\0'字符 注意是 -2 上面q++是先操作再自加
    q -= 2;
    //去掉行末的空格
    while (*q == ' ' || *q == '\t')
        --q;
    //给字符串添加字符结束标志
    *(q + 1) = '\0';
    //这里的return s要注意看好
    //因为p q经过一系列操作后，已经不是原来的位置，越界了 ，s还在原来位置，所以return s才是正确的。
    return s;
}

/*****************************************************************************
 函 数 名  : strtrimc
 功能描述  : 删除字符串空白符,包括行首和行尾
 输入参数  : char * s
 输出参数  : 无
 返 回 值  : char *
*****************************************************************************/
char* strtrimc(char* s)
{
    char* p1 = s;
    char* p2 = s;

    while (*p1 != '\0') {
        while (*p1 == ' ' || *p1 == '\t') {
            p1++;
        }
        *p2++ = *p1++;
    }

    *p2 = '\0';

    return (s);
}

size_t count_ch(const char* src, char c)
{
    size_t cnt = 0;
    size_t i = 0;
    while (src[i]) {
        if (src[i++] == c)
            cnt++;
    }

    return cnt;
}

int hex_str_to_hex_data(const char* hex_str, u8* hex_data)
{
    const char* p = hex_str;
    u8 data;
    int len = 0;

    if (!p)
        return -1;

    if (strlen(p) < 1)
        return -1;

    while (*p) {
        if (*p >= '0' && *p <= '9') {
            data = *p - '0';
        } else if (*p >= 'a' && *p <= 'f') {
            data = *p - 'a' + 0x0A;
        } else if (*p >= 'A' && *p <= 'F') {
            data = *p - 'A' + 0x0A;
        } else {
            p++;
            continue;
        }
        data <<= 4;
        p++;

        if (!(*p)) {
            *hex_data = data;
            break;
        }

        if (*p >= '0' && *p <= '9') {
            data |= *p - '0';
        } else if (*p >= 'a' && *p <= 'f') {
            data |= *p - 'a' + 0x0A;
        } else if (*p >= 'A' && *p <= 'F') {
            data |= *p - 'A' + 0x0A;
        } else {
            p++;
            continue;
        }
        *hex_data = data;
        hex_data++;
        len++;
        p++;
    }

    return len;
}

void half_byte_swap_buffer(u8* buf, u16 buf_len)
{
    for (int i = 0; i < buf_len; ++i) {
        buf[i] = swap8(buf[i]);
    }
}
