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
#include <stdio.h>  /* for printf()/fprintf() */
#include <stdlib.h> /* for exit() */
#include <stdbool.h>

#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */
#include "bsp_led.h"
#include "bsp_motors.h"
#include "bsp_buzzer.h"

#ifdef Q_SPY

#include "qs_defines.h"
static QSpyId const l_clock_tick = { QS_AP_ID };

#endif

int sensor_active = 0;
int radio_x = 0;
int radio_y = 0;

void BSP_init(void)   {
    printf("SumoHSM\n"
           "QP/C version: %s\n"
           "Press Ctrl-C to quit...\n",
           QP_VERSION_STR);

    BSP_ledInit();
    BSP_motorsInit();
    BSP_buzzerInit();

    #ifdef Q_SPY

    QS_INIT((void *)0);
    
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_OBJ_DICTIONARY(&l_clock_tick);
    QS_USR_DICTIONARY(SIMULATOR);

    /* setup the QS filters... */
    QS_GLB_FILTER(QS_ALL_RECORDS);
    QS_GLB_FILTER(-QS_QF_TICK);

    #endif


    
}


int BSP_Check_Dist(void) {
    return sensor_active;
}

int BSP_Get_Radio_X(){
    return radio_x;
}

int BSP_Get_Radio_Y(){
    return radio_y;

}


/* callback functions needed by the framework ------------------------------*/
void QF_onStartup(void) {
    QF_setTickRate(BSP_TICKS_PER_SEC, 50); /* desired tick rate/ticker-prio */
}
void QF_onCleanup(void) {}
void QF_onClockTick(void) {
    QF_TICK_X(0U, (void *)0); /* QF clock tick processing for rate 0 */

    #ifdef Q_SPY
    QS_RX_INPUT(); /* handle the QS-RX input */
    QS_OUTPUT();   /* handle the QS output */
    #endif

}
void Q_onAssert(char const * const module, int loc) {
    fprintf(stderr, "Assertion failed in %s:%d", module, loc);
    exit(-1);
}

#ifdef Q_SPY
/*..........................................................................*/
/*! callback function to execute user commands */
void QS_onCommand(uint8_t cmdId,
                  uint32_t param1, uint32_t param2, uint32_t param3)
{
    typedef struct {
    /* protected: */
        QActive super;

    /* private: */
        QTimeEvt timeEvt;
        QTimeEvt buzzerTimeEvt;
        uint8_t buzzerCount;
        uint8_t strategy;
        QTimeEvt timeEvt_2;
    } SumoHSM;

    switch (cmdId) {
       case 0: { 
            QEvt evt = {.sig = START_RC_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }
        case 1: { 
            QEvt evt = {.sig = START_AUTO_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }

        case 2: { 
            QEvt evt = {.sig = START_CALIB_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }

        case 3: { 

            SumoHSM *me = (SumoHSM *)AO_SumoHSM;
            me->strategy = param1;
            printf("Strategy = %d\r\n", param1);
            break;
        }

        case 4: { 
            QEvt evt = {.sig = LINE_DETECTED_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }

        case 5: { 
            sensor_active = param1;
            QEvt evt = {.sig = DIST_SENSOR_CHANGE_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            printf("Sensor Seeing = %d\r\n", param1);
            break;
        }

        case 6: { 
            QEvt evt = {.sig = GO_TO_IDLE_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }

        case 7: { 
            QEvt evt = {.sig = STOP_AUTO_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            break;
        }

        case 8: { 
            QEvt evt = {.sig = RADIO_DATA_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
            radio_x = param1 - 127;
            radio_y = param2 - 127;
            break;
        }



       default:
           break;
    }

    /* unused parameters */
    (void)param1;
    (void)param2;
    (void)param3;
}
#endif
