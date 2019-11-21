//
// Created by antonrampage on 18.11.2019.
//

#ifndef PLAYER_LED_H
#define PLAYER_LED_H


#include "ws2811.h"

#define TARGET_FREQ                             WS2811_TARGET_FREQ
#define GPIO_PIN                                18
#define DMA                                     5
#define STRIP_TYPE                              WS2811_STRIP_RGB
#define LED_COUNT                               256 // TODO: AS PARAM
#define MAX_BRGHT                               40

#define UPDATE_LEDS                             8

typedef struct RGB {
    uint32_t r;
    uint32_t g;
    uint32_t b;
} RGB;

class LED {
public:
    LED();
    ~LED();
    void show_led_on_pi(RGB &led_rgb);
private:
    ws2811_t ledstring;
};



#endif //PLAYER_LED_H
