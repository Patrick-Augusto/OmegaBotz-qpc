/*$file${.::inc::bsp.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: sumo_hsm.qm
* File:  ${.::inc::bsp.h}
*
* This code has been generated by QM 5.2.1 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This code is covered by the following QP license:
* License #    : QM-EVAL-QPC
* Issued to    : Users of QP/C Real-Time Embedded Framework (RTEF)
* Framework(s) : qpc
* Support ends : 2023-12-31
* License scope:
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is free software: you can redistribute it under
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
/*$endhead${.::inc::bsp.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#ifndef BSP_H
#define BSP_H

/* a very simple Board Support Package (BSP) -------------------------------*/
enum {
    BSP_TICKS_PER_SEC = 100
}; /* number of clock ticks in a second */

typedef enum {
    SIMULATOR = 100, //QS_USER0 on qs.h file. only useful on Qspy config
} qs_user_names_t;


#define BSP_TICKS_PER_MILISSEC (BSP_TICKS_PER_SEC/1000.0)

void BSP_init(void);

/* define the event signals used in the application ------------------------*/
enum SumoHSMSignals {
    TIMEOUT_SIG = Q_USER_SIG, /* offset the first signal by Q_USER_SIG */
    TIMEOUT_2_SIG,
    TIMEOUT_3_SIG,
    PLAY_BUZZER_SIG,
    START_SIG,
    STOP_SIG,
    RADIO_EVT_1_SIG,
    RADIO_EVT_2_SIG,
    RADIO_EVT_3_SIG,
    LINE_CHANGED_SIG,
    DIST_SENSOR_CHANGE_SIG,
    RADIO_DATA_SIG,
    BUTTON_SIG,
    MAX_SIG_SUMO_HSM, /* keep last (the number of signals) */
};

/* active object(s) used in this application -------------------------------*/
extern QActive * const AO_SumoHSM; /* opaque pointer to the SumoHSM AO */
/*$declare${AOs::SumoHSM_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::SumoHSM_ctor} .....................................................*/
void SumoHSM_ctor(void);
/*$enddecl${AOs::SumoHSM_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#ifdef Q_SPY
void sumoHSM_update_qs_dict(void);
#endif

#endif /* BSP_H */
