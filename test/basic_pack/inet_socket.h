#ifndef INET_SOCKET_H
#define INET_SOCKET_H       /* Prevent accidental double inclusion */

#include <sys/socket.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef enum{
    FALSE = 0,
    TRUE = 1
}Boolean;


// For Client
int inetConnect(const char *host, const char *service, int socket_type, unsigned int timeout_sec);

// For TCP Server
int inetListen(const char *service, int backlog, socklen_t *addrlen);

// For UDP Server
int inetBind(const char *service, int type, socklen_t *addrlen);

// Get Address String
void inetAddressStr(const struct sockaddr *addr, socklen_t addrlen,\
                    char *addrStr, int addrStrlen);

#define IS_ADDR_STR_LEN         4096
/***
 * Suggested length for string buffer that caller should
 * pass to inetAddressStr(). Must be greater than
 * (NI_MAXHOST + NI_MAXSERV + 4)
 ***/

#ifdef __cplusplus
}
#endif

#endif
