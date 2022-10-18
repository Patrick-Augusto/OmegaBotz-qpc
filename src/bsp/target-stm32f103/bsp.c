/*$file${.::bsp.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: blinky_console.qm
* File:  ${.::bsp.c}
*
* This code has been generated by QM 5.2.1 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${.::bsp.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/* Board Support Package implementation for desktop OS (Windows, Linux, MacOS) */
#include <stdlib.h> /* for exit() */

#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */
#include "bsp_led.h"
#include "bsp_motors.h"
#include "bsp_buzzer.h"
#include "bsp_radio.h"
#include "bsp_dist_sensors.h"
#include "main.h"
#include "gpio.h"
#include "dma.h"
#include "tim.h"

__weak void SystemClock_Config(void);



void SysTick_Handler(void) {   /* system clock tick ISR */

    HAL_IncTick();

    QK_ISR_ENTRY();   /* inform QK about entering an ISR */

#ifdef Q_SPY
    {
        tmp = SysTick->CTRL; /* clear CTRL_COUNTFLAG */
        QS_tickTime_ += QS_tickPeriod_; /* account for the clock rollover */
    }
#endif

    QTIMEEVT_TICK_X(0U, &l_SysTick_Handler); /* process time events for rate 0 */

    QK_ISR_EXIT();             /* inform QK about exiting an ISR */
}


void BSP_init(void)   {

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    SystemCoreClockUpdate();

    MX_DMA_Init();

    // MX_USART1_UART_Init();
    // MX_USART3_UART_Init();

    MX_TIM1_Init(); // Motors Timer
    MX_TIM2_Init(); // Led Stripe Timer
    // MX_TIM7_Init();
    // MX_TIM12_Init();

    BSP_ledInit();
    BSP_buzzerInit();
    BSP_radioInit();
    BSP_distSensorsInit();

}


/* callback functions needed by the framework ------------------------------*/
void QF_onStartup(void) {
        /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   QF_AWARE_ISR_CMSIS_PRI + 1);
    /* ... */

    /* enable IRQs... */
    
#ifdef Q_SPY
    NVIC_EnableIRQ(USART2_IRQn); /* UART2 interrupt used for QS-RX */
#endif
}


void QF_onCleanup(void) {

}

// void QF_onClockTick(void) {
//     QF_TICK_X(0U, (void *)0); /* QF clock tick processing for rate 0 */
// }

void QK_onIdle(void) { /* called with interrupts enabled */

    /* toggle an LED on and then off (not enough LEDs, see NOTE01) */
    QF_INT_DISABLE();
    //GPIOA->BSRR |= (LED_LD2);        /* turn LED[n] on  */
    //GPIOA->BSRR |= (LED_LD2 << 16);  /* turn LED[n] off */
    QF_INT_ENABLE();

#ifdef Q_SPY
    QS_rxParse();  /* parse all the received bytes */

    if ((USART2->ISR & (1U << 7)) != 0) {  /* is TXE empty? */
        uint16_t b;

        QF_INT_DISABLE();
        b = QS_getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {  /* not End-Of-Data? */
            USART2->TDR = (b & 0xFFU);  /* put into the DR register */
        }
    }
#elif defined NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    /* !!!CAUTION!!!
    * The WFI instruction stops the CPU clock, which unfortunately disables
    * the JTAG port, so the ST-Link debugger can no longer connect to the
    * board. For that reason, the call to __WFI() has to be used with CAUTION.
    *
    * NOTE: If you find your board "frozen" like this, strap BOOT0 to VDD and
    * reset the board, then connect with ST-Link Utilities and erase the part.
    * The trick with BOOT(0) is it gets the part to run the System Loader
    * instead of your broken code. When done disconnect BOOT0, and start over.
    */
    //__WFI(); /* Wait-For-Interrupt */
#endif
}


Q_NORETURN Q_onAssert(char const * const module, int_t const loc) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)loc;
    QS_ASSERTION(module, loc, 10000U); /* report assertion to QS */

    NVIC_SystemReset();
}
