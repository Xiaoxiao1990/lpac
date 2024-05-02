/***********************************************************************
 * @file uart.c
 * @brief 
 * Author: Linlin Zhou
 * E-mail: 461146760@qq.com
 * Date: 2024-05-02
 * @Copyright (C) 2024 all right reserved
***********************************************************************/
#include "uart.h"
#include "hal_uart.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <euicc/hexutil.h>
#include <euicc/interface.h>
#include <logs.h>

#define EUICC_INTERFACE_BUFSZ 264

// #define APDU_ST33_MAGIC "\x90\xBD\x36\xBB\x00"
#define APDU_TERMINAL_CAPABILITIES "\x80\xAA\x00\x00\x0A\xA9\x08\x81\x00\x82\x01\x01\x83\x01\x07"
#define APDU_OPENLOGICCHANNEL "\x00\x70\x00\x00\x01"
#define APDU_CLOSELOGICCHANNEL "\x00\x70\x80\xFF\x00"
#define APDU_SELECT_HEADER "\x00\xA4\x04\x00\xFF"

static uart_t uart_device = {
    .dev_path = "/dev/ttyUSB1",
    .baud_rate = 115200,
    .data_bits = 8,
    .stop_bits = 1,
    .parity = 'N',
    .fd = -1,
    .buffer = NULL,
};

static int uart_device_open(const char* device)
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

static int uart_device_close()
{
    uart_t* uart = &uart_device;

    uart_deinit(uart, 0);

    return 0;
}

static int uart_device_write(const char* data, int len)
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

static int uart_device_read(char* data, int len)
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
// static int uart_device_read_timeout(char* data, int len, int timeout)
// {
//     uart_t* uart = &uart_device;
//     fd_set rfds;
//     struct timeval tv;
//     int ret;

//     if (uart->fd < 0) {
//         LOGE("uart device is not open");
//         return -1;
//     }

//     FD_ZERO(&rfds);
//     FD_SET(uart->fd, &rfds);

//     tv.tv_sec = timeout;
//     tv.tv_usec = 0;

//     ret = select(uart->fd + 1, &rfds, NULL, NULL, &tv);
//     if (ret < 0) {
//         LOGE("select error");
//         return -1;
//     } else if (ret == 0) {
//         LOGE("select timeout");
//         return -1;
//     }

//     ret = read(uart->fd, data, len);
//     if (ret < 0) {
//         LOGE("read data failed");
//         return -1;
//     }

//     return ret;
// }



static int uart_transmit_lowlevel(uint8_t* rx, uint32_t* rx_len, const uint8_t* tx, const uint8_t tx_len)
{
    int ret;
    uint32_t n = 0;

    LOG_HEX(tx, tx_len, "TX[%2d]", tx_len);

    ret = uart_device_write(tx, tx_len);
    if (ret != tx_len) {
        LOGE("uart device write failed!");
        return -1;
    }

    // ret = uart_device_read_timeout(rx_data, UART_RX_BUFFER_SIZE, 2);
    uint32_t expected_len = 0;
    if (tx[1] == 0x70 || tx[1] == 0xC0 || tx[1] == 0xB0) {
        expected_len = tx[4] + 2;
    } else {
        expected_len = 2;
    }

    LOGI("expected_len: %d", expected_len);
    do {
        ret = uart_device_read(rx + n, expected_len - n);
        if (ret < 0) {
            LOGE("uart device read failed!");
            return -1;
        }

        LOG_HEX(rx + n, ret, "Received[%2d]", ret);
        n += ret;
    } while (n < expected_len);

    *rx_len = n;

    LOG_HEX(rx, *rx_len, "RX[%2d]", *rx_len);

    return 0;
}

static void uart_logic_channel_close(uint8_t channel)
{
    uint8_t tx[sizeof(APDU_CLOSELOGICCHANNEL) - 1];
    uint8_t rx[EUICC_INTERFACE_BUFSZ];
    uint32_t rx_len;

    memcpy(tx, APDU_CLOSELOGICCHANNEL, sizeof(tx));
    tx[3] = channel;

    rx_len = sizeof(rx);

    uart_transmit_lowlevel(rx, &rx_len, tx, sizeof(tx));
}

static int uart_logic_channel_open(const uint8_t* aid, uint8_t aid_len)
{
    int channel = 0;
    uint8_t tx[EUICC_INTERFACE_BUFSZ];
    uint8_t* tx_wptr;
    uint8_t rx[EUICC_INTERFACE_BUFSZ];
    uint32_t rx_len;

    if (aid_len > 32) {
        goto err;
    }

    rx_len = sizeof(rx);
    if (uart_transmit_lowlevel(rx, &rx_len, (const uint8_t*)APDU_OPENLOGICCHANNEL, sizeof(APDU_OPENLOGICCHANNEL) - 1) < 0) {
        goto err;
    }

    if (rx_len != 3) {
        goto err;
    }

    if ((rx[1] & 0xF0) != 0x90) {
        goto err;
    }

    channel = rx[0];

    tx_wptr = tx;
    memcpy(tx_wptr, APDU_SELECT_HEADER, sizeof(APDU_SELECT_HEADER) - 1);
    tx_wptr += sizeof(APDU_SELECT_HEADER) - 1;
    memcpy(tx_wptr, aid, aid_len);
    tx_wptr += aid_len;

    tx[0] = (tx[0] & 0xF0) | channel;
    tx[4] = aid_len;

    rx_len = sizeof(rx);
    if (uart_transmit_lowlevel(rx, &rx_len, tx, tx_wptr - tx) < 0) {
        goto err;
    }

    if (rx_len < 2) {
        goto err;
    }

    switch (rx[rx_len - 2]) {
        case 0x90:
        case 0x61:
            return channel;
        default:
            goto err;
    }

    err:
    if (channel) {
        uart_logic_channel_close(channel);
    }

    return -1;
}

static int apdu_interface_connect(struct euicc_ctx* ctx)
{
    uint8_t rx[EUICC_INTERFACE_BUFSZ];
    uint32_t rx_len;

    const char* device;

    if (!(device = getenv("AT_DEVICE"))) {
        device = "/dev/ttyUSB0";
    }

    if (uart_device_open(device) < 0) {
        LOGE("Open device: %s failed!", device);
        return -1;
    }

    LOGI("Opened device: %s success!", device);

    rx_len = sizeof(rx);
    uart_transmit_lowlevel(rx, &rx_len, (const uint8_t*)APDU_TERMINAL_CAPABILITIES, sizeof(APDU_TERMINAL_CAPABILITIES) - 1);

    return 0;
}

static void apdu_interface_disconnect(struct euicc_ctx* ctx)
{
    uart_device_close();
}

static int apdu_interface_transmit(struct euicc_ctx* ctx, uint8_t** rx, uint32_t* rx_len, const uint8_t* tx, uint32_t tx_len)
{
    *rx = malloc(EUICC_INTERFACE_BUFSZ);
    if (!*rx) {
        LOGE("SCardTransmit() RX buffer alloc failed");
        return -1;
    }
    *rx_len = EUICC_INTERFACE_BUFSZ;

    if (uart_transmit_lowlevel(*rx, rx_len, tx, tx_len) < 0) {
        free(*rx);
        *rx_len = 0;
        return -1;
    }

    return 0;
}

static int apdu_interface_logic_channel_open(struct euicc_ctx* ctx, const uint8_t* aid, uint8_t aid_len)
{
    return uart_logic_channel_open(aid, aid_len);
}

static void apdu_interface_logic_channel_close(struct euicc_ctx* ctx, uint8_t channel)
{
    uart_logic_channel_close(channel);
}

static int libapduinterface_init(struct euicc_apdu_interface* ifstruct)
{
    memset(ifstruct, 0, sizeof(struct euicc_apdu_interface));

    ifstruct->connect = apdu_interface_connect;
    ifstruct->disconnect = apdu_interface_disconnect;
    ifstruct->logic_channel_open = apdu_interface_logic_channel_open;
    ifstruct->logic_channel_close = apdu_interface_logic_channel_close;
    ifstruct->transmit = apdu_interface_transmit;

    return 0;
}

static int libapduinterface_main(int argc, char** argv)
{
    LOGI("driver UART in use!");
    return 0;
}

static void libapduinterface_fini(void)
{
    LOGI("driver UART fini");
}

const struct euicc_driver driver_apdu_uart = {
    .type = DRIVER_APDU,
    .name = "uart",
    .init = (int (*)(void*))libapduinterface_init,
    .main = libapduinterface_main,
    .fini = libapduinterface_fini,
};
