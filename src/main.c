#include "esp_log.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "event_queue.h"
#include "fsm.h"
#include "bsp.h"
#include "freertos/FreeRTOS.h"

#define BUTTON_GPIO     GPIO_NUM_0
#define DEBOUNCE_US     50000  // 50 ms 

static const char *TAG = "producer-consumer-demo";
static volatile int64_t last_isr_time = 0;
static volatile bool button_pressed_flag = false;

ButtonQueue_t queue;
uint8_t event_id = 0;

static void IRAM_ATTR gpio0_isr(void *arg)
{
    (void)arg;
    // added switch debounce
    int64_t now = esp_timer_get_time();
    if ((now - last_isr_time) > DEBOUNCE_US) {
        button_pressed_flag = true;
        last_isr_time = now;
    }
}

static void gpio0_interrupt_init(void)
{
    gpio_config_t config = {
        .pin_bit_mask = 1ULL << GPIO_NUM_0,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    gpio_config(&config);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_0, gpio0_isr, NULL);
    
    ESP_LOGI(TAG, "--- Button is ready to trigger interrupt ---");
}

void app_main(void) {
    vTaskDelay(50 / portTICK_PERIOD_MS);

    Queue_Init(&queue);
    ButtonEvent_t event_out = { 0 };

    // initialize button interrupt on pin 0
    gpio0_interrupt_init();

    BSP_LED_InitAll();
    ESP_LOGI(TAG, "--- LEDs ready ---");

    vTaskDelay(50 / portTICK_PERIOD_MS);

    // start processing events
    while (1) {
        if (button_pressed_flag) {
            button_pressed_flag = false;
            ESP_DRAM_LOGI(DRAM_STR("button interrupt"), "--- external interrupt enqueued event ---");
            Queue_Enqueue(&queue, event_id++);
        }

        if (!Queue_IsEmpty(&queue)) {
            if (Queue_Dequeue(&queue, &event_out)) {
                ESP_LOGI(TAG, "--- dequeued event_id: %d ---", event_out.event_id);

                fsm_state_t next = fsm_table[current_state].next_state;
                current_state = next;
                if (fsm_table[current_state].state_action != NULL) {
                    fsm_table[current_state].state_action();
                }

            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));  
    }
}
