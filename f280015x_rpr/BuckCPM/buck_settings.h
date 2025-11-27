#ifndef _BUCK_PROJSETTINGS_H
#define _BUCK_PROJSETTINGS_H

#ifdef __cplusplus

extern "C" {
#endif

//
//*****************************************************************************
//defines
//*****************************************************************************
//

//
// Lab numbers defined in buck_user_settings.h
//
#define BUCK_LAB_NUMBER                    4

//
// PWM Switching frequency and control update ratio
//
#define BUCK_DRV_EPWM_SWITCHING_FREQUENCY  ((float32_t)50 * 1000)
#define BUCK_ISR_CTRL_LOOP_RATIO           1

//
// Treat BUCK_LAUNCHPAD_SITE = 0 as CUSTOM hardware
//
#define BUCK_LAUNCHPAD_SITE                2

//
// Board characteristics and calculations
//
#define BUCK_VIN_NOM_V                     ((float32_t) 9)
#define BUCK_VIN_MAX_SENSE_V               ((float32_t)30.3) //((float32_t)13.3)
#define BUCK_VOUT_NOM_V                    ((float32_t) 2)
#define BUCK_VOUT_MAX_SENSE_V                ((float32_t)  13.1951)                  // ((float32_t) 6.7)
#define BUCK_IL_MAX_SENSE_A                ((float32_t) 0.080566406250000*4096)                 //7.52)
#define BUCK_ILOAD_NOM_A                   ((float32_t) 50)

//FLOAT32 factor_i_L1=0.080566406250000;//0.161050545;             for B0.1


//
// CMPSS RAMPMAXREF register value (only used for PCMC)
//
#define BUCK_PCMC_CMPSS_SLOPE              ((uint16_t)7)

//
// DCL DF22 coefficients from Compensation Designer
//
#define BUCK_DCL_COMPDES_KDC               ((float32_t)38904)
#define BUCK_DCL_COMPDES_Z0                ((float32_t)6.100)
#define BUCK_DCL_COMPDES_Z1                ((float32_t)6.100)
#define BUCK_DCL_COMPDES_P1                ((float32_t)11.000)

#define BUCK_DCL_COMPDES_B0                ((float32_t) 1.8741543518)
#define BUCK_DCL_COMPDES_B1                ((float32_t)-3.0928031117)
#define BUCK_DCL_COMPDES_B2                ((float32_t) 1.2759662882)
#define BUCK_DCL_COMPDES_A1                ((float32_t)-1.7053386367)
#define BUCK_DCL_COMPDES_A2                ((float32_t) 0.7053386367)

//
// User code settings file
//
#include "buck_user_settings.h"

#ifdef __cplusplus
}
#endif                                  /* extern "C" */

#endif //_PROJSETTINGS_H
