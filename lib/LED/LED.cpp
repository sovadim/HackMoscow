//
// Created by antonrampage on 18.11.2019.
//

#include <iostream>
#include <rpi_ws281x/ws2811.h>
#include <vector>
#include <cmath>

#include "LED.h"

static inline int32_t rgb_to_hex(RGB &led_rgb) {
    return ((led_rgb.g & 0xff) << 16) + ((led_rgb.r & 0xff) << 8) + (led_rgb.b & 0xff);
}

LED::LED(int32_t width, int32_t length) :
    width(width),
    length(length) {
    ledstring = {0, 0, 0, TARGET_FREQ, DMA, {GPIO_PIN, 0, width * length, STRIP_TYPE, 0, MAX_BRGHT}};
    try {
        ws2811_return_t ret;
        if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS) {
            throw ret;
        }
    }
    catch (ws2811_return_t ret) {
        std::cout << "ws2811_init failed: " << ws2811_get_return_t_str(ret) << std::endl;
    }

    for (int i = 0; i < width * length; ++i) {
        ledstring.channel[0].leds[i] = 0x000000;
    }
}

LED::~LED() {
    ws2811_fini(&ledstring);
}

void LED::show_led_on_pi(RGB &led_rgb) {
    // left
//	std::cout << led_rgb.r << ' ' << led_rgb.g << ' ' << led_rgb.b << std::endl;
    for (int i = width * length - 1; i >= width + width * length / 2; --i) {
        ledstring.channel[0].leds[i] = ledstring.channel[0].leds[i - width];
    }
    // right
    for (int i = 0; i < width * length / 2 - width; ++i) {
        ledstring.channel[0].leds[i] = ledstring.channel[0].leds[i + width];
    }
    // Set the left most updateLEDs with the new color
    for (int i = width * length / 2 - width; i < width * length / 2 + width; ++i) {
        ledstring.channel[0].leds[i] = rgb_to_hex(led_rgb);
    }
}

int32_t LED::transform_coord(int32_t x, int32_t y) {
    int centre = width * length / 2 - width / 2 - 1;
    int res = 0;
    if (x % 2 != 0) {
        res = (x == 0) ? centre : (centre + ((x * width) - y + 1));
    } else {
        res = centre + ((x * width) + y);
    }
    return res;
}

bool LED::check_coord(int32_t x, int32_t y) {
    return ((y <= width / 2) && (y >= -width / 2 + 1)) &&
           ((x <= length / 2) && (x >= -length / 2 + 1));
}

void LED::draw_line(Point &a_real, Point &b_real, RGB &led_rgb) {
    Point a = {a_real.x + 0.5f, a_real.y + 0.5f};
    Point b = {b_real.x + 0.5f, b_real.y + 0.5f};
    bool steep = (std::fabs(a.y - b.y) > std::fabs(a.x - b.x));
    if (steep) {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
    }
    if (a.x > b.x) {
        std::swap(a.x, b.x);
        std::swap(a.y, b.y);
    }

    float dx = b.x - a.x;
    float dy = std::fabs(b.y - a.y);

    float error = dx / 2.0f;
    int y_step = (a.y < b.y) ? 1 : -1;
    int y = static_cast<int>(a.y);

    const int max_x = static_cast<int>(b.x);

    for (int x = static_cast<int>(a.x); x <= max_x; x++) {
        if (steep) {
            if (check_coord(y, x)) {
                ledstring.channel[0].leds[transform_coord(y, x)] = rgb_to_hex(led_rgb);
            }
        } else {
            if (check_coord(x, y)) {
                ledstring.channel[0].leds[transform_coord(x, y)] = rgb_to_hex(led_rgb);
            }
        }
        error -= dy;
        if (error < 0) {
            y += y_step;
            error += dx;
        }
    }
}

void LED::show_figure_on_led(Polygon *polygon) {
    for (int i = 0; i < polygon->verteces; ++i) {
        if (i == polygon->verteces - 1) {
            draw_line(polygon->vectors[i], polygon->vectors[0], polygon->color);
        } else {
            draw_line(polygon->vectors[i], polygon->vectors[i + 1], polygon->color);
	    }
    }
}

void LED::show_circle_on_led(Polygon *polygon) {
    float_t x0 = polygon->vectors[0].x + 0.5f;
    float_t y0 = polygon->vectors[0].x + 0.5f;
    double_t ir_x = 0.f;
    double_t ir_y = 0.f;
    modf(std::fabs(x0), &ir_x);
    modf(std::fabs(y0), &ir_y);
    int mode = 0;
    if (((ir_x - 0.5 < 0.25) && (ir_x - 0.5 >= 0)) &&
        ((ir_y - 0.5 < 0.25) && (ir_y - 0.5 >= 0))) {
        x0 = std::ceil(x0) + 0.5f;
        y0 = std::ceil(y0) + 0.5f;
        mode = 1;
    }

    if (!mode) {
        int x = 0;
        int y = static_cast<int>(polygon->radius);
        int delta = 1 - 2 * y;
        int error = 0;
        auto x_i = static_cast<int>(x0);
        auto y_i = static_cast<int>(y0);
        while (y >= 0) {
            if (check_coord(x_i + x, y_i + y)) {
                ledstring.channel[0].leds[transform_coord(x_i + x, y_i + y)] =
                        rgb_to_hex(polygon->color);
            }
            if (check_coord(x_i + x, y_i - y)) {
                ledstring.channel[0].leds[transform_coord(x_i + x, y_i - y)] =
                        rgb_to_hex(polygon->color);
            }
            if (check_coord(x_i - x, x_i + y)) {
                ledstring.channel[0].leds[transform_coord(x_i - x, y_i + y)] =
                        rgb_to_hex(polygon->color);
            }
            if (check_coord(x_i - x, y_i - y)) {
                ledstring.channel[0].leds[transform_coord(x_i - x, y_i - y)] =
                        rgb_to_hex(polygon->color);
            }
            error = 2 * (delta + y) - 1;
            if (delta < 0 && error <= 0) {
                ++x;
                delta += 2 * x + 1;
                continue;
            }
            error = 2 * (delta - x) - 1;
            if (delta > 0 && error > 0) {
                --y;
                delta += 1 - 2 * y;
                continue;
            }
            ++x;
            delta += 2 * (x - y);
            --y;
        }
    } else {
        float_t x = 0.5f;
        float_t y = std::ceil(polygon->radius) + 0.5f;
        float_t rad = y;
        float_t d = 0;
        float_t v = 0;
        float_t g = 0;
        while (y >= 0.5) {
            if (check_coord(std::floor(x0 + x), std::floor(y0 + y))) {
                ledstring.channel[0].leds[transform_coord(std::floor(x0 + x), std::floor(y0 + y))] =
                        rgb_to_hex(polygon->color);
            }
            if (check_coord(std::floor(x0 + x), std::floor(y0 - y))) {
                ledstring.channel[0].leds[transform_coord(std::floor(x0 + x), std::floor(y0 - y))] =
                        rgb_to_hex(polygon->color);
            }
            if (check_coord(std::floor(x0 - x), std::floor(y0 + y))) {
                ledstring.channel[0].leds[transform_coord(std::floor(x0 - x), std::floor(y0 + y))] =
                        rgb_to_hex(polygon->color);
            }
            if (check_coord(std::floor(x0 - x), std::floor(y0 - y))) {
                ledstring.channel[0].leds[transform_coord(std::floor(x0 - x), std::floor(y0 - y))] =
                        rgb_to_hex(polygon->color);
            }

            v = (x + 0.5f) * (x + 0.5f) + y * y - rad * rad;
            g = x * x + (y - 0.5f) * (y - 0.5f) - rad;
            d = (x + 0.5f) * (x + 0.5f) + (y - 0.5f) * (y - 0.5f) - rad * rad;
            if (d < 0) {
                if (std::fabs(g) - std::fabs(d) <= 0) {
                    x++;
                } else {
                    x++;
                    y--;
                }
            } else if (d > 0) {
                if (std::fabs(d) - std::fabs(v) <= 0) {
                    x++;
                    y--;
                } else {
                    y--;
                }
            } else {
                x++;
                y--;
            }
        }
    }
}

void LED::change_settings(int32_t width, int32_t length) {
    this->width = width;
    this->length = length;
    ws2811_fini(&ledstring);
    ledstring = {0, 0, 0, TARGET_FREQ, DMA, {GPIO_PIN, 0, width * length, STRIP_TYPE, 0, MAX_BRGHT}};
    try {
        ws2811_return_t ret;
        if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS) {
            throw ret;
        }
    }
    catch (ws2811_return_t ret) {
        std::cout << "ws2811_init failed: " << ws2811_get_return_t_str(ret) << std::endl;
    }

    for (int i = 0; i < width * length; ++i) {
        ledstring.channel[0].leds[i] = 0x000000;
    }
}

void LED::render() {
    ws2811_render(&ledstring);
}

int32_t LED::get_width() const {
    return width;
}
