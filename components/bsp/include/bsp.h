#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include <stdbool.h>

/*
    Implementing Board Support Package - STM32 Nucleo boards use a similar interface (via CubeMX)
*/

// using this enum means that adding another LED is as simple
// as adding another enum entry
typedef enum {
    BSP_LED_GREEN = 0,
    BSP_LED_RED,
    BSP_LED_COUNT // Automatically equals the number of LEDs 
} BSP_LED_TypeDef;

typedef enum {
    BSP_BUTTON_USER = 0,
    BSP_BUTTON_COUNT
} BSP_Button_TypeDef;

// Callback - which button triggered the event?
typedef void (*BSP_Button_Callback_t)(BSP_Button_TypeDef button);

// public API 
void BSP_Init(void);
void BSP_Process(void);

void BSP_LED_On(BSP_LED_TypeDef led);
void BSP_LED_Off(BSP_LED_TypeDef led);
void BSP_LED_Toggle(BSP_LED_TypeDef led);

bool BSP_Button_IsPressed(BSP_Button_TypeDef button);
void BSP_Button_RegisterCallback(BSP_Button_TypeDef button, BSP_Button_Callback_t callback);

#endif
