/*********************************************************************
 * Author: Zhou Linlin
 * E-mail: 461146760@qq.com
 * Date  : 2022-08-12 19:42:39.
 * Copyright (c) 2022 all rights reserved.
*********************************************************************/

#include "hal_uart.h"
#include "security_memory.h"
#include "logs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>


static int speed_arr[] = {
    B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200
};

static int name_arr[] = {
    115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200
};

static int set_uart_baud_rate(uart_t* uart)
{
    unsigned int i;
    int status;
    struct termios Opt;

    tcgetattr(uart->fd, &Opt);

    for (i = 0; i < sizeof(speed_arr) / sizeof(speed_arr[0]); i++) {
        if (uart->baud_rate == name_arr[i]) {
            tcflush(uart->fd, TCIOFLUSH);
            cfsetispeed(&Opt, (speed_t)speed_arr[i]);
            cfsetospeed(&Opt, (speed_t)speed_arr[i]);
            status = tcsetattr(uart->fd, TCSANOW, &Opt);
            if (status != 0)
                return -1;
            return 0;
        }
        tcflush(uart->fd, TCIOFLUSH);
    }

    return -1;
}

static int set_uart_options(uart_t* uart)
{
    struct termios options;
    if (tcgetattr(uart->fd, &options) != 0) {
        LOGI("Get [%s] options failed:%s", uart->dev_path, strerror(errno));
        return -1;
    }
    options.c_cflag &= ~CSIZE;
    switch (uart->data_bits) {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            LOGI("Setup [%s] data bits failed: Unsupported data size.", uart->dev_path);
            return -1;
    }

    switch (uart->parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;
        case 'S':
        case 's':
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            LOGI("Setup [%s] parity failed: Unsupported parity type.", uart->dev_path);
            return -1;
    }

    switch (uart->stop_bits) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            LOGI("Setup [%s] stop bits failed: Unsupported data size.", uart->dev_path);
            return -1;
    }

    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    /* Set input parity option */
    if (uart->parity != 'n')
        options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 100; // Max wait time 10 seconds
    options.c_cc[VMIN] = 0;    // Min read words return

    tcflush(uart->fd, TCIFLUSH); /* Update the options and do it NOW */

    if (tcsetattr(uart->fd, TCSANOW, &options) != 0) {
        LOGI("Setup [%s] failed.", uart->dev_path);
        return -1;
    }

    return 0;
}

int uart_buff_init(struct Buffer* buf, u32 size)
{
    buf->bgn = (char*)security_zerolloc(size);
    if (buf->bgn == NULL) {
        LOGI("No space for UART buffer allocate now.");
        return -1;
    }

    buf->pos = buf->bgn;
    buf->end = buf->bgn + size;

    buf->bytes = 0;

    return 0;
}

int uart_init(uart_t* uart, int has_buffer)
{
    uart->fd = open(uart->dev_path, O_RDWR);
    if (uart->fd < 0) {
        LOGI("Open [%s] failed.", uart->dev_path);
        return -1;
    }

    if (0 != set_uart_baud_rate(uart)) {
        LOGI("Set [%s] baud rate failed.", uart->dev_path);
        return -1;
    }

    if (0 != set_uart_options(uart)) {
        LOGI("Set [%s] options failed.", uart->dev_path);
        return -1;
    }

    if (has_buffer) {
        uart->buffer = (struct Buffer*)security_zerolloc(sizeof(struct Buffer));

        if (uart->buffer == NULL) {
            LOGI("No memory");
            return -1;
        }

        if (0 != uart_buff_init(uart->buffer, UART_BUFFER_SIZE)) {
            LOGI("UART buffer initial failed.");
            return -1;
        }
    }

    return 0;
}

void uart_deinit(uart_t* uart, int has_buffer)
{
    if (has_buffer) {
        if (uart->buffer == NULL)
            return;
        security_free(uart->buffer->bgn);//LOGI("OK>>>>");
        uart->buffer->bgn = NULL;
        uart->buffer->pos = NULL;
        uart->buffer->end = NULL;
        uart->buffer->bytes = 0;
        security_free(uart->buffer);
        uart->buffer = NULL;
    }

    if (uart->fd < 0)
        return;
    
    close(uart->fd);
    uart->fd = -1;
}