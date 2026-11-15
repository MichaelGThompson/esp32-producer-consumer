#include "bsp.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"

#define DEBOUNCE_TIME_US 50000 // 50 mS

// LED Config
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

// --- Button Config ---
typedef struct {
    gpio_num_t pin;
    uint8_t active_high;
    gpio_pull_mode_t pull_mode;
} Button_Hardware_t;

static const Button_Hardware_t button_config[BSP_BUTTON_COUNT] = {
    [BSP_BUTTON_USER] = { GPIO_NUM_0,  0, GPIO_PULLUP_ONLY },
};

static BSP_Button_Callback_t button_callbacks[BSP_BUTTON_COUNT] = {NULL};
static volatile bool button_irq_flag[BSP_BUTTON_COUNT] = {false};
static volatile int64_t button_irq_time[BSP_BUTTON_COUNT] = {0};
static int64_t last_valid_press[BSP_BUTTON_COUNT] = {0};

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    BSP_Button_TypeDef button = (BSP_Button_TypeDef)(uintptr_t)arg; // uintptr_t is an optional unsigned integer type

    button_irq_time[button] = esp_timer_get_time();
    button_irq_flag[button] = true;
}

/*
    LED functions
*/
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

/*
    Button functions
*/

static void BSP_Button_InitAll(void) {
    gpio_install_isr_service(0);

    for (uint8_t i = 0; i < BSP_BUTTON_COUNT; i++) {
        gpio_int_type_t edge = button_config[i].active_high ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE;

        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << button_config[i].pin),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = (button_config[i].pull_mode == GPIO_PULLUP_ONLY) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
            .pull_down_en = (button_config[i].pull_mode == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
            .intr_type = edge
        };
        gpio_config(&io_conf);

        gpio_isr_handler_add(button_config[i].pin, gpio_isr_handler, (void*)(uintptr_t)i);
    }
}

void BSP_Button_RegisterCallback(BSP_Button_TypeDef button, BSP_Button_Callback_t callback) {
    if (button < BSP_BUTTON_COUNT) {
        button_callbacks[button] = callback;
    }
}

bool BSP_Button_IsPressed(BSP_Button_TypeDef button) {
    if (button >= BSP_BUTTON_COUNT) {
        return false;
    }
    
    int raw_level = gpio_get_level(button_config[button].pin);
    return button_config[button].active_high ? (raw_level == 1) : (raw_level == 0);
}

// --- Public API ---
void BSP_Init(void) {
    BSP_LED_InitAll();
    BSP_Button_InitAll();
}

void BSP_Process(void) {
    for (uint8_t i = 0; i < BSP_BUTTON_COUNT; i++) {
        if (button_irq_flag[i]) {
            // Clear the interrupt flag
            button_irq_flag[i] = false;

            // debouncing
            if ((button_irq_time[i] - last_valid_press[i]) >= DEBOUNCE_TIME_US) {
                last_valid_press[i] = button_irq_time[i];

                // if button is pressed, invoke callback
                if (BSP_Button_IsPressed((BSP_Button_TypeDef)i) && button_callbacks[i] != NULL) {
                    button_callbacks[i]((BSP_Button_TypeDef)i);
                }
            }
        }
    }
}
