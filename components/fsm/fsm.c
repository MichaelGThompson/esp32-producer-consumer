#include "include/fsm.h"
#include "bsp.h"

void display_all_off(void) {
    ESP_LOGI(FSM_TAG, "[Action] Clearing outputs -> ALL OFF\n");
    BSP_LED_Off(BSP_LED_GREEN);
    BSP_LED_Off(BSP_LED_RED);
}

void display_red_on(void) {
    ESP_LOGI(FSM_TAG, "[Action] Modifying pins -> RED ONLY\n");
    BSP_LED_Off(BSP_LED_GREEN);
    BSP_LED_On(BSP_LED_RED);
}

void display_green_on(void) {
    ESP_LOGI(FSM_TAG, "[Action] Modifying pins -> GREEN ONLY\n");
    BSP_LED_On(BSP_LED_GREEN);
    BSP_LED_Off(BSP_LED_RED);
}

void display_all_on(void) {
    ESP_LOGI(FSM_TAG, "[Action] Driving outputs -> ALL ON\n");
    BSP_LED_On(BSP_LED_GREEN);
    BSP_LED_On(BSP_LED_RED);
}