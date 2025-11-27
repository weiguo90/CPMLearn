//#############################################################################
//
// FILE:  buck_hal.h
//
// TITLE: Solution hardware abstraction layer
//        Low level functions and resources with hardware dependencies
//        Example: Functions to read current and voltage signals
//        Example: Functions to configure device peripherals
//
//#############################################################################
// $TI Release: TIDM_DC_DC_BUCK v2.01.05.00 $
// $Release Date: Fri Aug 22 11:50:37 CDT 2025 $
// $Copyright:
// Copyright (C) 2025 Texas Instruments Incorporated - http://www.ti.com/
//
// ALL RIGHTS RESERVED
// $
//#############################################################################

#ifndef BUCK_HAL_H
#define BUCK_HAL_H

#ifdef __cplusplus

extern "C" {
#endif

//
//=============================================================================
// includes
//=============================================================================
//
#include "driverlib.h"
#include "device.h"
#include "buck_settings.h"

//
//=============================================================================
// variables
//=============================================================================
//
#if(BUCK_DRV_EPWM_HR_ENABLED == true)
    extern volatile uint32_t ePWM[9];
#endif

//
//=============================================================================
// Function prototypes from HAL source
//=============================================================================
//
__attribute__((interrupt))  void Cla1Task1();
__attribute__((interrupt))  void Cla1BackgroundTask();
void BUCK_HAL_setupDevice(void);
void BUCK_HAL_setupAdc(void);
void BUCK_HAL_setupAdcTrigger(void);
void BUCK_HAL_setupRunLed(void);
void BUCK_HAL_toggleRunLed(void);

void BUCK_HAL_setupSyncBuckPinsGpio(void);
void BUCK_HAL_setupSyncBuckPinsEpwm(void);
void BUCK_HAL_setupSyncBuckPwm(void);
void BUCK_HAL_setupSyncBuckTripAction(void);
void BUCK_HAL_setupSyncBuckBlankingWindow(void);
void BUCK_HAL_setupSyncBuckOverCurrentTripAction(void);
void BUCK_HAL_setupSyncBuckPcmcTripAction(void);

void BUCK_HAL_setupActiveLoadPinGPIO(void);
void BUCK_HAL_setupActiveLoadPinEPWM(void);
void BUCK_HAL_setupActiveLoadPWM(void);

uint16_t BUCK_HAL_setOverCurrentTripLimit(float32_t tripLimit_A);
void BUCK_HAL_setupOverCurrentCmpssTrip(void);
void BUCK_HAL_setupPcmcCmpssTrip(void);

void BUCK_HAL_disableEpwmCounting(void);
void BUCK_HAL_enableEpwmCounting(void);

void BUCK_HAL_setupInterrupt(void);

void BUCK_HAL_setupCLA(void);
#if(BUCK_CONTROL_RUNNING_ON == C28X_CORE)
interrupt void ISR1(void); // from buck_main.c
#endif

//
//=============================================================================
// static inline functions
//=============================================================================
//
#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_getLowSpeedClock)
static inline uint32_t BUCK_HAL_getLowSpeedClock(void)
{
    #ifdef USE_INTOSC
        return( SysCtl_getLowSpeedClock((uint32_t)BUCK_INTOSC_HZ) );
    #else
        return( SysCtl_getLowSpeedClock(DEVICE_OSCSRC_FREQ) );
    #endif
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_disableActiveLoad);
static inline void BUCK_HAL_disableActiveLoad(void)
{
    EPWM_setCounterCompareValue(BUCK_LOAD_EPWM_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                BUCK_LOAD_EPWM_CMPA_DISABLED);
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_enableDiscontinuousActiveLoad);
static inline void BUCK_HAL_enableDiscontinuousActiveLoad(void)
{
    EPWM_setCounterCompareValue(BUCK_LOAD_EPWM_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                BUCK_LOAD_EPWM_CMPA_DISCONTINUOUS);
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_enableContinuousActiveLoad);
static inline void BUCK_HAL_enableContinuousActiveLoad(void)
{
    EPWM_setCounterCompareValue(BUCK_LOAD_EPWM_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                BUCK_LOAD_EPWM_CMPA_CONTINUOUS);
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_readVin_V);
static inline float32_t BUCK_HAL_readVin_V(void)
{
    return(BUCK_VIN_ADC_FACTOR_V *
           ADC_readResult(BUCK_VIN_ADCRESULTREGBASE,
                          BUCK_VIN_ADC_SOC_NO));
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_readVout_pu);
static inline float32_t BUCK_HAL_readVout_pu(void)
{
    return(BUCK_VOUT_ADC_FACTOR_PU *
           ADC_readResult(BUCK_VOUT_ADCRESULTREGBASE,
                          BUCK_VOUT_ADC_SOC_NO));
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_readVout_V);
static inline float32_t BUCK_HAL_readVout_V(void)
{
    return(BUCK_VOUT_ADC_FACTOR_V *
           ADC_readResult(BUCK_VOUT_ADCRESULTREGBASE,
                          BUCK_VOUT_ADC_SOC_NO));
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_readFilteredIl_A);
static inline float32_t BUCK_HAL_readFilteredIl_A(void)
{
    return(BUCK_IL_ADC_FACTOR_A *
           ADC_readResult(BUCK_IL_FILT_ADCRESULTREGBASE,
                          BUCK_IL_FILT_ADC_SOC_NO));
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_readOverCurrentTripFlag);
static inline uint16_t BUCK_HAL_readOverCurrentTripFlag(void)
{
    if((EPWM_TZ_FLAG_DCAEVT1 | EPWM_TZ_FLAG_OST) &
       EPWM_getTripZoneFlagStatus(BUCK_DRV_EPWM_BASE))
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_clearOverCurrentTripFlag);
static inline void BUCK_HAL_clearOverCurrentTripFlag(void)
{
    EPWM_clearTripZoneFlag(BUCK_DRV_EPWM_BASE,
                           EPWM_TZ_FLAG_DCAEVT1 |
                           EPWM_TZ_FLAG_OST);
}

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_updateSyncBuckDuty);
static inline void BUCK_HAL_updateSyncBuckDuty(float32_t dutySet_pu)
{
    #if(BUCK_DRV_EPWM_HR_ENABLED == true)
#ifndef __TMS320C28XX_CLA__
        HRPWM_setCounterCompareValue(BUCK_DRV_EPWM_BASE,
                                     HRPWM_COUNTER_COMPARE_A,
                                     dutySet_pu *
                                     BUCK_DRV_EPWM_CMPAHR_SCALE *
                                     BUCK_DRV_EPWM_TBPRD);
#endif
    #else
        EPWM_setCounterCompareValue(BUCK_DRV_EPWM_BASE,
                                    EPWM_COUNTER_COMPARE_A,
                                    dutySet_pu *
                                    BUCK_DRV_EPWM_TBPRD);
    #endif
}

#if(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
    #pragma FUNC_ALWAYS_INLINE(BUCK_HAL_updateSyncBuckDac);
    static inline void BUCK_HAL_updateSyncBuckDac(float32_t dacSet_pu)
    {
        CMPSS_setMaxRampValue(BUCK_PCMC_CMPSS_BASE,
                              dacSet_pu * BUCK_PCMC_CMPSS_RAMPMAX_MAX);
    }
#endif

#pragma FUNC_ALWAYS_INLINE(BUCK_HAL_clearInterruptFlags);
static inline void BUCK_HAL_clearInterruptFlags(void)
{
    ADC_clearInterruptStatus(BUCK_VOUT_ADC_MODULE, BUCK_VOUT_ADC_INT_NUMBER);
#ifndef __TMS320C28XX_CLA__
#if(BUCK_CONTROL_RUNNING_ON == C28X_CORE)
    Interrupt_clearACKGroup(BUCK_VOUT_ADC_ISR_PIE_GROUP);
#endif
#endif
}


#ifdef __cplusplus
}
#endif                                  /* extern "C" */

#endif
