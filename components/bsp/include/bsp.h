#ifndef BSP_H
#define BSP_H

#include <stdint.h>

// using this enum means that adding another LED is as simple
// as adding another enum entry
typedef enum {
    BSP_LED_GREEN = 0,
    BSP_LED_RED,
    BSP_LED_COUNT // Automatically equals the number of LEDs 
} BSP_LED_TypeDef;

// public API 
void BSP_LED_InitAll(void);
void BSP_LED_On(BSP_LED_TypeDef led);
void BSP_LED_Off(BSP_LED_TypeDef led);
void BSP_LED_Toggle(BSP_LED_TypeDef led);

#endif
