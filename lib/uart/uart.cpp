#include "uart.h"
#include <unistd.h>
#include <sys/types.h>
#include <cstring>
#include <mutex>
#include <termios.h>

#define CFG_LENGTH 11

std::mutex g_mutex;


static inline bool is_num(char b) {
    return ('0' <= b && b <= '9');
}

static int read_with_markers(int32_t file, char s, char e, char *buf, int32_t length) {
    memset(buf, 0, length + 1);
    char b;
    int32_t i = 0;
    bool in_progress = false;
    while (true) {
        while (read(file, &b, 1) <= 0);

        if (!in_progress) {
            if (b == s) {
                in_progress = true;
            } else {
                continue;
            }
        }

        if (b == e || length < i) {
            break;
        }

        if (is_num(b)) {
            buf[i] = b;
            i++;
        }
    }
    return 0;
}

static void parse_data(const char *buf, RGB &led_rgb, int32_t sum) {
    int32_t r = std::strtol(buf, nullptr, 10);
    int32_t g = std::strtol(&buf[3], nullptr, 10);
    int32_t b = std::strtol(&buf[6], nullptr, 10);
    r -= g;
    r /= 1000000;
    g -= b;
    g /= 1000;

    if (sum == (r + g + b)) {
        g_mutex.lock();
        led_rgb.r = r;
        led_rgb.g = g;
        led_rgb.b = b;
        g_mutex.unlock();
    }
}

void read_serial_port(int32_t file, RGB &led_rgb) {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(file, &rfds);

    char buf[CFG_LENGTH];

    while (select(file + 1, &rfds, NULL, NULL, NULL) > 0) {
        int32_t sum = 0;
        read_with_markers(file, '{', '}', buf, 4);

        // if there is 0 and checksum, then there is an rgb data
        // if there is 1 and checksum, then there is a config data

        sum = std::strtol(buf, nullptr, 10);
        if (sum < 1000) {
            read_with_markers(file, '<', '>', buf, 9);
            parse_data(buf, led_rgb, sum);
        } else {
            read_with_markers(file, '[', ']', buf, CFG_LENGTH);
        }
        tcflush(file, TCIOFLUSH);
    }
}
