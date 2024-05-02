//#define _BSD_SOURCE
/***
* To get NI_MAXHOST and NI_MAXSERV definitions
* from <netdb.h>
****/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "logs.h"
#include "inet_socket.h"

static int set_fd_flag(int fd, int flag)
{
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | flag) < 0) {
        LOGE("Failed to set the fd to nonblock mode(errno:%d).", errno);
        return -1;
    }

    return 0;
}

static int clr_fd_flag(int fd, int flag)
{
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~flag) < 0) {
        LOGE("Failed to clear nonblock mode of this fd(errno:%d).", errno);
        return -1;
    }

    return 0;
}

static int connect_with_time_out(int sock_fd, struct sockaddr *__addr, socklen_t __len, unsigned int sec)
{
    set_fd_flag(sock_fd, O_NONBLOCK);

    int connected = connect(sock_fd, __addr, __len);

    int ret = -1;
    if (connected != 0) {
        if(errno != EINPROGRESS)
            LOGE("connect error :%s",strerror(errno));
        else {
            struct timeval tm = {sec, 0};
            fd_set wset,rset;
            FD_ZERO(&wset);
            FD_ZERO(&rset);
            FD_SET(sock_fd, &wset);
            FD_SET(sock_fd, &rset);

            int res = select(sock_fd + 1, &rset, &wset, NULL, &tm);

            if(res < 0) {
                LOGE("network error in connect(%d): %s", errno, strerror(errno));
            } else if(res == 0) {
                LOGI("connect time out.");
            } else if (1 == res) {
                if(FD_ISSET(sock_fd,&wset)) {
                    // logi("connect succeed.");
                    clr_fd_flag(sock_fd, O_NONBLOCK);
                    ret = 0;
                } else {
                    LOGE("others error when select: %s",strerror(errno));
                }
            }
        }
    } else
        return 0;

    return ret;
}

int inetConnect(const char *host, const char *service, int socket_type, unsigned int timeout_sec)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd = -1, s;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;        /* Allows IPv4 or IPv6 */
    hints.ai_socktype = socket_type;

    s = getaddrinfo(host, service, &hints, &result);
    if(s != 0){
        errno = ENOSYS;
        return -1;
    }

    /* Walk through returned list until we find an address structure
       that can be used to successfully connect a socket */

    for(rp = result; rp != NULL; rp = rp->ai_next){
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if(sfd == -1)
            continue;          /* On error, try next address */

        if(connect_with_time_out(sfd, rp->ai_addr, rp->ai_addrlen, timeout_sec) != -1)
            break;   /* Success */
        /* Connect failed: close this socket and try net address */
        close(sfd);
    }

    freeaddrinfo(result);

    //return (rp == NULL)? -1 : sfd;
    return (rp == NULL)? -1 : sfd;
}

static int inetPassiveSocket(const char *service, int type, socklen_t *addrlen, Boolean doListen, int backlog)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, optval, s;

    memset(&hints, 0,sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = type;
    hints.ai_family = AF_UNSPEC;        /* Allows IPv4 or IPv6 */
    hints.ai_flags = AI_PASSIVE;        /* Use wildcard IP address */

    s = getaddrinfo(NULL, service, &hints, &result);
    if(s != 0)return -1;
    /* Walk through returned list until we find an address structure
       that can be used to successflly create and bind a socket */
    optval = 1;

    for(rp = result; rp != NULL; rp = rp->ai_next){
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sfd == -1)continue;          /* On error, try next address */

        if(doListen){/* TCP */
            if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
                close(sfd);
                freeaddrinfo(result);
                return -1;
            }
        } else { /* UDP */
            int tos_local = 0xE0;
            socklen_t local_tos_len = sizeof(tos_local);
            if(setsockopt(sfd, IPPROTO_IP, IP_TOS, &tos_local, local_tos_len)){
                perror("set IP_TOS failed");
                LOGI("set IP_TOS failed");
            } else {
                int tos = 0;
                socklen_t tos_len = sizeof(tos);
                if(getsockopt(sfd, IPPROTO_IP, IP_TOS, &tos, &tos_len) < 0){
                    perror("get IP_TOS failed");
                    LOGI("get IP_TOS failed");
                } else {
                    LOGI("Changing IP_TOS = 0x%02X", tos);
                }
            }
        }

        if(bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)break; /* Success */

        /* bind() failed: close this socket and try next address */
        close(sfd);
    }

    if(rp != NULL && doListen){
        if(listen(sfd, backlog) == -1){
            freeaddrinfo(result);
            return -1;
        }
    }

    if(rp != NULL && addrlen != NULL)*addrlen = rp->ai_addrlen;     /* Return address structure size */
    freeaddrinfo(result);

    return (rp == NULL)? -1 : sfd;
}

/**
 * @brief inetListen : For TCP listen service port
 * @param service    : Service or port
 * @param backlog    : MAX connection
 * @param addrlen    : Address length
 * @return           : If success return sockfd, else return -1
 */
int inetListen(const char *service, int backlog, socklen_t *addrlen)
{
    return inetPassiveSocket(service, SOCK_STREAM, addrlen, TRUE, backlog);
}

int inetBind(const char *service, int type, socklen_t *addrlen)
{
    return inetPassiveSocket(service, type, addrlen, FALSE, 0);
}

void inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrlen)
{
    char host[NI_MAXHOST], service[NI_MAXSERV];

    if(getnameinfo(addr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_DGRAM|NI_NUMERICHOST|NI_NUMERICSERV) == 0){
        snprintf(addrStr, addrStrlen, "%s:%s", host, service);
    } else {
        snprintf(addrStr, addrStrlen, "(?UNKNOW?)");
    }

//    if(addr->sa_family == AF_INET) {
//       // return &(((struct sockaddr_in*)addr)->sin_addr);
//        inet_ntop(addr->sa_family, &((struct sockaddr_in *)addr)->sin_addr, host, addrlen);
//        snprintf(service, NI_MAXSERV, "%d", ((struct sockaddr_in *)addr)->sin_port);
//    } else {
//       inet_ntop(addr->sa_family, &((struct sockaddr_in6 *)addr)->sin6_addr, host, addrlen);
//       snprintf(service, NI_MAXSERV, "%d", ((struct sockaddr_in6 *)addr)->sin6_port);
//    }

//    snprintf(addrStr, IS_ADDR_STR_LEN, "%s:%s", host, service);
    addrStr[addrStrlen - 1] = '\0';     /* Ensure result is null-terminated */
 //   return addrStr;
}
