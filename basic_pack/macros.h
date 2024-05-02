#ifndef __MACROS_H__
#define __MACROS_H__

#include "types.h"
#define ON                                      1
#define OFF                                     0

#define ENABLE                                  ON
#define DISABLE                                 OFF

#define DEBUG                                   ON
#define RELEASE                                 OFF

#define BUILD_VERSION                           RELEASE

//#define LOGS_BUFFER_LENGTH                      (u16)1024
//#define PRINT_BUFFER_LENGTH                     (u16)1024
#define DATE_LEN                                (u8)30
#define TIME2STR_LEN                            (u8)40

#define MAX_EPOLL_SIZE                          (u8)10

#define COMMAND_RETRY_TIMES                     (u8)3

/* device definition */
#define UART1_PATH                              "/dev/ttyO1"
#define UART2_PATH                              "/dev/ttyO2"
#define UART3_PATH                              "/dev/ttyO3"
#define UART4_PATH                              "/dev/ttyO4"

#define UART_BUFFER_SIZE                        (u32)1024

#define LTE_GSM_PORT                            "/dev/ttyUSB3"

#define GPIO_DEVICE_PATH                        "/dev/am335x_gpio"

/* function selection */
#define LOG_TO_UART                             OFF
#define LOG_TO_FILE                             ON
#define PLC_ASYNC_MODE                          ENABLE
#define MODBUS_ASYNC_MODE                       ENABLE
#define MPPT_ASYNC_MODE                         ENABLE
#define PLC_ASYNC_TIMEOUT_VALUE                 2
#define MPPT_ASYNC_TIMEOUT_VALUE                4
#define SENSOR_ASYNC_MODE                       ENABLE
#define SENSOR_ASYNC_TIMEOUT_VALUE              4
#define LTE_ASYNC_MODE                          ENABLE
#define LTE_ASYNC_TIMEOUT_VALUE                 5

/* log config */
#define LOG_UART_PATH                           UART1_PATH
#define DATA_LOG_PATH_LENGTH                    256

/* PLC config */
//#define PLC_PATH                                UART3_PATH
#define PLC_MAX_RETRY_TIMES                     20

/* sensor config */
//#define SENSOR_PATH                             UART4_PATH

/* LTE config */
#define LTE_AT_PATH                             LTE_GSM_PORT

/* Solar cells */
#define SOLAR_CELLS_PORT                        UART3_PATH

/* Solar cells */
#define MODBUS_DEVICE_PORT                      UART4_PATH

/* DNS */
#define DNS_CONFIG_FILE_PATH                    "/etc/resolv.conf"
#define IPv4_STRING_LENGTH                      16  // xxx.xxx.xxx.xxx\0
#define DNS_QUERY_TIMEOUT_VALUE                 5
#define MAX_LINE_LENGTH                         256

/* SIM */
#define IMSI_STRING_LENGTH                      16
#define ICCID_STRING_LENGTH                     21
#define MOBILE_NETWORK_ACT_STRING_LENGTH        20
#define PLMN_STRING_LENGTH                      7
#define MAX_COMMAND_STRING_LENGTH               512

/* Platform */
#define CONFIG_FILE_PATH                        "/home/root/security_iot/config.json"
#define CLIENT_ENDPOINT_ENCRYPT_FILE_PATH       "/home/root/security_iot/.endpoint.bin"
#define MESSAGE_TEMP_FILE                       "/home/root/security_iot/message.db"

#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MACROS_H__ */
