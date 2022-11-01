/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdio.h>
#include "bsp_gpio_mapping.h"
#include "bsp_gpio.h"
#include "bsp_ppm.h"

#include "bsp_ppm_fake.h"
#include "utils.h"

/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/


/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
static bsp_ppm_callback_t external_callback;
static bool enabled;
/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/


void fake_ppm_exti_callback(uint8_t ppm_num, uint8_t value) {

    if (!enabled){
        return;
    }

    if (ppm_num < BOARD_NUM_OF_PPMS){
        uint16_t ppm = map(value, 0, 255, PPM_MIN_VALUE_MS, PPM_MAX_VALUE_MS);


        external_callback(ppm_num, ppm);
    }
    
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/


void bsp_ppm_init() {

    printf("BSP PPM INIT\r\n");
    enabled = true;

}

void bsp_ppm_start(){
    printf("BSP PPM START\r\n");
    enabled = true;
   
}

void bsp_ppm_stop(){

    printf("BSP PPM STOP\r\n");
    enabled = false;
    
}

void bsp_ppm_register_callback(bsp_ppm_callback_t callback_function){
    external_callback = callback_function;
}


