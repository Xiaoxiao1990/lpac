//#include <azure_c_shared_utility/refcount.h>

/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 8/30/18 5:54 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#include "utils.h"
#include "types.h"
#include "macros.h"
#include "logs.h"

#include <memory.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <stdio.h>

char big_endian_test(void)
{
	/*定义一个2个字节长度的数据，并赋值为1,则n的16进制表示为0x0001
	如果系统以“大端”存放数据，也即是以MSB方式存放，那么低字节存放的必定是0x00，高字节存放的必定是0x01
	如果系统以“小端”存放数据，也即是以LSB方式存放，那么低字节存放的必定是0x01，高字节存放的必定是0x00
	所谓MSB，就是将最重要的位存入低位，而LSB则是将最不重要的位存入低位
	我们可以通过检测低位的数值就可以知道系统的字节序
	*/
	const s16 n = 1;
	if (*(char *)&n) {
		return LittleEndian;
	}
	return BigEndian;
}

int str2hex(const char *str, unsigned char *hex, int hex_len)
{
	int i = 0;
	unsigned char ch;

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
			hex[i / 2] = (unsigned char)((ch << 4) & 0xF0);
		else {
			hex[i / 2] |= (ch & 0x0F);
			if (i / 2 >= hex_len)
				return hex_len;
		}
		i++;
		str++;
	}

	return i >> 1;
}

int hex2str(const unsigned char *hex, const int hex_len, char *str, int str_len)
{
	char ch[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	int i = 0, j = 0;

	if ((hex == NULL) || (str == NULL) || hex_len <= 0 || str_len <= 0)
		return -1;

	while ((i + 2) < str_len) {
		str[i++] = ch[((hex[j] >> 4) & 0x0F)];
		str[i++] = ch[(hex[j++] & 0x0F)];
		str[i] = 0;
		if (j >= hex_len)
			break;
	}

	return i;
}

/* naive function to check whether char *s is an ip address */
int is_ip_address(const char *s)
{
	u32 n1, n2, n3, n4;

	if (sscanf(s, "%u.%u.%u.%u", &n1, &n2, &n3, &n4) != 4)
		return 0;

	if ((n1 <= 255) && (n2 <= 255) && (n3 <= 255) && (n4 <= 255))
		return 1;

	return 0;
}

bool is_process_running(const char *process)
{
	char cmd[MAX_COMMAND_STRING_LENGTH];
	int num = -1;
	FILE *pF = NULL;

	snprintf(cmd, MAX_COMMAND_STRING_LENGTH, "ps | grep \"%s$\" | grep -v grep | wc -l", process);
	if ((pF = popen(cmd, "r")) == NULL) {
		return false;
	}

	if (1 != fscanf(pF, "%d", &num)) {
		pclose(pF);
		return false;
	}

	pclose(pF);

	if (num == 0)
		return false;
	else
		return true;
}

int get_pid_by_name(const char *process)
{
	char cmd[MAX_COMMAND_STRING_LENGTH];
	int num = -1;
	FILE *pF = NULL;

	snprintf(cmd, MAX_COMMAND_STRING_LENGTH, "ps |grep %s |grep -v grep|awk '{print $1}'", process);
	if ((pF = popen(cmd, "r")) == NULL) {
		return -1;
	}

	if (1 != fscanf(pF, "%d", &num)) {
		pclose(pF);
		return -1;
	}

	pclose(pF);

	return num;
}

int my_kill(const char *process)
{
	char cmd[MAX_COMMAND_STRING_LENGTH];
	FILE *pF = NULL;
	int pid = get_pid_by_name(process);

	if (pid < 0)
		return -1;

	snprintf(cmd, MAX_COMMAND_STRING_LENGTH, "kill -9 %d", pid);
	LOGI("{Kill process[%s]}[%s]", process, cmd);
	if ((pF = popen(cmd, "r")) == NULL) {
		return -1;
	}
	pclose(pF);

	return 0;
}


int my_pkill(const char *process)
{
	int pid;

	do {
		pid = get_pid_by_name(process);
		if (pid < 0)
			break;

		LOGI("Kill pid:%d", pid);
		my_kill(process);
	} while (1);

	return 0;
}

static char time_stamp_string[TIME2STR_LEN];

const char *get_time_stamp_string(void)
{
	struct timeval us;

	gettimeofday(&us, NULL);
	snprintf(time_stamp_string, TIME2STR_LEN, "%lld", (long long)us.tv_sec * 1000 + us.tv_usec / 1000);
	return time_stamp_string;
}

long long get_time_stamp_value(void)
{
	struct timeval us;

	gettimeofday(&us, NULL);
	return (long long)us.tv_sec * 1000 + us.tv_usec / 1000;
}

int get_current_time_hour(void)
{
	time_t seconds;
	struct tm now;

	seconds = time(0);
	localtime_r(&seconds, &now);

//	printf("%d-%d-%d ", (1900 + now.tm_year), (1 + now.tm_mon), now.tm_mday);
//	printf("%d:%d:%d\n", now.tm_hour, now.tm_min, now.tm_sec);

	return now.tm_hour;
}

int get_current_time_minute(void)
{
	time_t seconds;
	struct tm now;

	seconds = time(0);
	localtime_r(&seconds, &now);

//	printf("%d-%d-%d ", (1900 + now.tm_year), (1 + now.tm_mon), now.tm_mday);
//	printf("%d:%d:%d\n", now.tm_hour, now.tm_min, now.tm_sec);

	return now.tm_min;
}

int get_current_time_second(void)
{
	time_t seconds;
	struct tm now;

	seconds = time(0);
	localtime_r(&seconds, &now);

//	printf("%d-%d-%d ", (1900 + now.tm_year), (1 + now.tm_mon), now.tm_mday);
//	printf("%d:%d:%d\n", now.tm_hour, now.tm_min, now.tm_sec);

	return now.tm_sec;
}

static char file_md5_checksum[MD5_DIGEST_LENGTH * 2 + 1];
char *get_md5_checksum_from_buffer(const unsigned char *data, size_t len)
{
	char md[MD5_DIGEST_LENGTH] = {0};

	MD5(data, len, md);
	memset(file_md5_checksum, 0, sizeof(file_md5_checksum));
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(file_md5_checksum + i * 2, "%02x", md[i]);
	}
	file_md5_checksum[MD5_DIGEST_LENGTH * 2] = '\0';
	return file_md5_checksum;
}

char *get_md5_checksum_from_file(const char *file_name)
{
	FILE* fp = NULL;
	unsigned char md5_hex[MD5_DIGEST_LENGTH];
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	MD5_CTX ctx;

	fp = fopen(file_name, "rb");
	if (NULL == fp) {
		return NULL;
	}

	MD5_Init(&ctx);

	while ((nread = getline(&line, &len, fp)) != -1) {
		MD5_Update(&ctx, line, nread);
	}
	free(line);
	MD5_Final(md5_hex, &ctx);
	fclose(fp);

	memset(file_md5_checksum, 0, sizeof(file_md5_checksum));

	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(file_md5_checksum + i * 2, "%02x", md5_hex[i]);
	}
	file_md5_checksum[MD5_DIGEST_LENGTH * 2] = '\0';

	return file_md5_checksum;
}

int my_system(const char *fmt, ...) /* with appropriate signal handling */
{
	va_list list;
	char cmdstring[MAX_COMMAND_STRING_LENGTH] = {0};
	pid_t pid;
	int status;
	struct sigaction ignore, saveintr, savequit;
	sigset_t chldmask, savemask;

	va_start(list, fmt);
	vsnprintf(cmdstring, MAX_COMMAND_STRING_LENGTH, fmt, list);
	va_end(list);

	if (cmdstring == NULL)
		return (1); /* always a command processor with UNIX */

	ignore.sa_handler = SIG_IGN; /* ignore SIGINT and SIGQUIT */
	sigemptyset(&ignore.sa_mask);
	ignore.sa_flags = 0;
	if (sigaction(SIGINT, &ignore, &saveintr) < 0)
		return (-1);
	if (sigaction(SIGQUIT, &ignore, &savequit) < 0)
		return (-1);
	sigemptyset(&chldmask); /* now block SIGCHLD */
	sigaddset(&chldmask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
		return (-1);

	if ((pid = fork()) < 0) {
		status = -1; /* probably out of processes */
	} else if (pid == 0) { /* child */
		/* restore previous signal actions & reset signal mask */
		sigaction(SIGINT, &saveintr, NULL);
		sigaction(SIGQUIT, &savequit, NULL);
		sigprocmask(SIG_SETMASK, &savemask, NULL);
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127); /* exec error */
	} else { /* parent */
		while (waitpid(pid, &status, 0) < 0)
			if (errno != EINTR) {
				status = -1; /* error other than EINTR from waitpid() */
				break;
			}
	}

	/* restore previous signal actions & reset signal mask */
	if (sigaction(SIGINT, &saveintr, NULL) < 0)
		return (-1);
	if (sigaction(SIGQUIT, &savequit, NULL) < 0)
		return (-1);
	if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
		return (-1);

	return (status);
}

size_t count_ch(const char *src, char c)
{
    size_t cnt = 0;
    size_t i = 0;
    while (src[i]) {
        if (src[i++] == c)
            cnt++;
    }

    return cnt;
}
