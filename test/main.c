/***********************************************************************
 * @file main.c
 * @brief 
 * Author: Linlin Zhou
 * E-mail: 461146760@qq.com
 * Date: 2024-05-01
 * @Copyright (C) 2024 all right reserved
***********************************************************************/

#include "logs.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include "uart.h"


#define COLORFUL_LOG(fmt, ...) \
    do { \
        LOGI(fmt , ##__VA_ARGS__); \
        LOGW(fmt , ##__VA_ARGS__); \
        LOGE(fmt , ##__VA_ARGS__); \
        LOGD(fmt , ##__VA_ARGS__); \
        LOGV(fmt , ##__VA_ARGS__); \
    } while (0)


#define UART_DEVICE             "/dev/ttyUSB0"
#define UART_RX_BUFFER_SIZE     1024

static uart_t uart_device = {
    .dev_path = UART_DEVICE,
    .baud_rate = 115200,
    .data_bits = 8,
    .stop_bits = 1,
    .parity = 'N',
    .fd = -1,
    .buffer = NULL,
};

int uart_device_open(const char* device)
{
    uart_t* uart = &uart_device;
    int ret = uart_init(uart, 0);
    if (ret != 0) {
        LOGE("uart init failed");
        return -1;
    }

    LOGI("uart device open: %s", device);

    return 0;
}

int uart_device_close()
{
    uart_t* uart = &uart_device;

    uart_deinit(uart, 0);

    return 0;
}

int uart_device_write(const char* data, int len)
{
    uart_t* uart = &uart_device;

    if (uart->fd < 0) {
        LOGE("uart device is not open");
        return -1;
    }

    int ret = write(uart->fd, data, len);
    if (ret != len) {
        LOGE("write data failed");
        return -1;
    }

    return ret;
}

int uart_device_read(char* data, int len)
{
    uart_t* uart = &uart_device;

    if (uart->fd < 0) {
        LOGE("uart device is not open");
        return -1;
    }

    int ret = read(uart->fd, data, len);
    if (ret < 0) {
        LOGE("read data failed");
        return -1;
    }

    return ret;
}

// read data from uart device with timeout
int uart_device_read_timeout(char* data, int len, int timeout)
{
    uart_t* uart = &uart_device;
    fd_set rfds;
    struct timeval tv;
    int ret;

    if (uart->fd < 0) {
        LOGE("uart device is not open");
        return -1;
    }

    FD_ZERO(&rfds);
    FD_SET(uart->fd, &rfds);

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    ret = select(uart->fd + 1, &rfds, NULL, NULL, &tv);
    if (ret < 0) {
        LOGE("select error");
        return -1;
    } else if (ret == 0) {
        LOGE("select timeout");
        return -1;
    }

    ret = read(uart->fd, data, len);
    if (ret < 0) {
        LOGE("read data failed");
        return -1;
    }

    return ret;
}

int uart_device_transmit(const char* tx_data, int tx_len, char* rx_data, int *rx_len)
{
    int ret = uart_device_write(tx_data, tx_len);
    if (ret != tx_len) {
        LOGE("uart device write failed!");
        return -1;
    }

    // ret = uart_device_read_timeout(rx_data, UART_RX_BUFFER_SIZE, 2);
    ret = uart_device_read(rx_data, UART_RX_BUFFER_SIZE);
    if (ret < 0) {
        LOGE("uart device read failed!");
        return -1;
    }

    *rx_len = ret;

    return 0;
}

void test(void)
{
    char tx_data[] = { 0x00, 0xA4, 0x00, 0x04, 0x02, 0x2F, 0xE2 };
    int tx_len = sizeof(tx_data);
    char rx_data[256] = { 0 };
    int rx_len;
    int ret = 0;
    
    ret = uart_device_open(UART_DEVICE);
    if (ret != 0) {
        LOGE("open uart device failed!");
        return;
    }

    LOG_HEX(tx_data, tx_len, "Tx[%d] ", tx_len);
    ret = uart_device_transmit(tx_data, tx_len, rx_data, &rx_len);
    if (ret != 0) {
        LOGE("uart device transmit failed!");
        return;
    }
    LOG_HEX(rx_data, rx_len, "Rx[%d] ", rx_len);

    ret = uart_device_close();
    if (ret != 0) {
        LOGE("close uart device failed!");
        return;
    }
}

int main(int argc, char** argv)
{
    test();

    return 0;
}