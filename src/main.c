#include "esp_log.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "event_queue.h"
#include "fsm.h"
#include "bsp.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "producer-consumer-demo";
static volatile bool button_pressed_flag = false;

ButtonQueue_t queue;
uint8_t event_id = 0;

void on_user_button_press(BSP_Button_TypeDef button) {
    button_pressed_flag = true;
}

void app_main(void) {
    vTaskDelay(50 / portTICK_PERIOD_MS);

    Queue_Init(&queue);
    ButtonEvent_t event_out = { 0 };

    BSP_Init();
    BSP_Button_RegisterCallback(BSP_BUTTON_USER, on_user_button_press);

    ESP_LOGI(TAG, "--- BSP ready ---");

    vTaskDelay(50 / portTICK_PERIOD_MS);

    // start processing events
    while (1) {
        BSP_Process();
        
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
