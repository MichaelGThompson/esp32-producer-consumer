#include "bsp.h"
#include "driver/gpio.h"

// hardware details for one LED
typedef struct {
    gpio_num_t pin;    
    uint8_t active_high; 
} LED_Hardware_t;

// Lookup table indexed by BSP_LED_TypeDef
static const LED_Hardware_t led_config[BSP_LED_COUNT] = {
    [BSP_LED_GREEN] = { GPIO_NUM_8, 8}, 
    [BSP_LED_RED]   = { GPIO_NUM_1, 1 }
};

static uint8_t led_state[BSP_LED_COUNT] = {0}; // to allow toggling

void BSP_LED_InitAll(void) {
    uint64_t pin_mask = 0;

    // Combine all LED pins into a single 64-bit bitmask
    for (uint8_t i = 0; i < BSP_LED_COUNT; i++) {
        pin_mask |= (1ULL << led_config[i].pin);
    }

    // Configure all LED pins at once using gpio_config()
    gpio_config_t io_conf = {
        .pin_bit_mask = pin_mask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // turn off all LEDs for now
    for (uint8_t i = 0; i < BSP_LED_COUNT; i++) {
        BSP_LED_Off((BSP_LED_TypeDef)i);
    }
}

void BSP_LED_On(BSP_LED_TypeDef led) {
    if (led >= BSP_LED_COUNT) { 
        return; 
    }

    uint32_t level = led_config[led].active_high ? 1 : 0;
    gpio_set_level(led_config[led].pin, level);
    led_state[led] = 1;
}

void BSP_LED_Off(BSP_LED_TypeDef led) {
    if (led >= BSP_LED_COUNT) { 
        return; 
    }

    uint32_t level = led_config[led].active_high ? 0 : 1;
    gpio_set_level(led_config[led].pin, level);
    led_state[led] = 0;
}

void BSP_LED_Toggle(BSP_LED_TypeDef led) {
    if (led >= BSP_LED_COUNT) { 
        return; 
    }

    if (led_state[led]) {
        BSP_LED_Off(led);
    } else {
        BSP_LED_On(led);
    }
}
