//#############################################################################
//
// FILE:  buck.h
//
// TITLE: Solution functions and resources
//        High level components that apply universally across hardware variants
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

#ifndef BUCK_H
#define BUCK_H

#ifdef __cplusplus

extern "C" {
#endif

//
//=============================================================================
// includes and defines
//=============================================================================
//

#ifndef __TMS320C28XX_CLA__
    #include <math.h>
#else
    #include <CLAmath.h>
#endif
#include "buck_settings.h"
#include "buck_hal.h"
#include "DCL_fdlog.h"
#include "DCL_TCM.h"

#if(BUCK_DRV_EPWM_HR_ENABLED == true)
    #include "SFO_V8.h"
#endif

#ifndef __TMS320C28XX_CLA__
    #include "DCLF32.h"
#else
   #include "DCLCLA.h"
#endif

#if(BUCK_SFRA_ENABLED == true)
#ifndef __TMS320C28XX_CLA__
    //
    // Only pull in SFRA includes if SFRA is enabled
    //
    #include <stddef.h>
    #include "sfra_f32.h"
    #include "sfra_gui_scicomms_driverlib.h"

    //
    // Use macros to call the SFRA functions
    // The input parameters are passed through to the underlying SFRA functions
    //
    #define BUCK_SFRA_INJECT(x) SFRA_F32_inject(x)
    #define BUCK_SFRA_COLLECT(x, y) SFRA_F32_collect(x, y)
    #define BUCK_SFRA_GUI_RUN_COMMS(x) SFRA_GUI_runSerialHostComms(x)
    #define BUCK_SFRA_RUN_BACKGROUND(x) SFRA_F32_runBackgroundTask(x)
#endif
#else
    //
    // The macros will eliminate the SFRA function calls when not enabled
    //   BUCK_SFRA_INJECT(x) will be replaced inline with just the (x) value
    //   The other macros will be replaced inline with empty code blocks {}
    //
    #define BUCK_SFRA_INJECT(x) (x)
    #define BUCK_SFRA_COLLECT(x, y) {}
    #define BUCK_SFRA_GUI_RUN_COMMS(x) {}
    #define BUCK_SFRA_RUN_BACKGROUND(x) {}
#endif

//
//=============================================================================
// typdefs and externs
//=============================================================================
//
typedef enum {
    Lab_1_OpenLoopVmc    = 1,
    Lab_2_ClosedLoopVmc  = 2,
    Lab_3_OpenLoopPcmc   = 3,
    Lab_4_ClosedLoopPcmc = 4
} BUCK_Lab_EnumType;

typedef enum {
    Format_PID   = 1,
    Format_ZPK   = 2,
    Format_Coeff = 3
} BUCK_CtrlFormat_EnumType;

typedef struct {
    float32_t Scale;
    float32_t Kp;
    float32_t Ki;
    float32_t Kd;
    float32_t fc_Hz;
} BUCK_CtrlPid_Type;

typedef struct {
    float32_t Kdc;
    float32_t Z0_kHz;
    float32_t Z1_kHz;
    float32_t P1_kHz;
} BUCK_Zpk_Type;

typedef struct {
    float32_t B0;
    float32_t B1;
    float32_t B2;
    float32_t A1;
    float32_t A2;
} BUCK_CtrlCoeff_Type;

typedef struct {
    BUCK_CtrlFormat_EnumType ctrlFormat;
    uint16_t                 ctrlUpdate;
    BUCK_CtrlPid_Type        ctrlPid;
    BUCK_Zpk_Type            ctrlZpk;
    BUCK_CtrlCoeff_Type      ctrlCoeff;
    BUCK_CtrlCoeff_Type      ctrlActiveCoeff;
} BUCK_CtrlUpdate_Type;

typedef struct {
    uint16_t                 forceTrig;
    uint16_t                 armTrig;
    float32_t                trigMin_V;
    float32_t                trigMax_V;
    uint16_t                 leadSamples;
    TCM_states               tcmMode;
    float32_t                triggered_V;
} BUCK_TcmRef_Type;

//
//=====================================
// Variables used by the program to run the BUCK solution.
//
// This subset of variables can be used interactively in the watch window
// to monitor and control the execution of the BUCK solution.
//
//     dutySetRef_pu: Desired duty cycle; Input from user for Open Loop VMC
//     dutySet_pu: Effective duty cycle setting from ISR for VMC
//     dacSetRef_pu: Desired MAXREF DAC value; Input from user for OL PCMC
//     dacSet_pu: Effective MAXREF DAC setting from ISR for VMC
//     vInSensed_Volts: Vin measured by ADC
//     vOutSetRef_Volts: Desired Vout; Input from user for Closed Loop
//     vOutSensed_Volts: Vout measured by ADC
//     iOutSensed_Amps: ILFB measured by ADC
//     iOutTripSetRef_Amps: Desired over-current trip limit; Input from user
//     iOutTripFlag: 0 = No trip detected; 1 = Trip detected
//     iOutTripFlagClear: Write 1 to clear trip; Input from user
//     activeLoadEnable: 0 = Load off; 1 = Load on; Input from user
//     activeLoadContEnable: 0 = Periodic load; 1 = Constant load
//     ctrlRef: DCL update structure; Input from user for Closed Loop
//     isrDutyUpdateMarginClear: Reset BUCK_isrDutyUpdateMinMargin_ticks; Input
//     vOutLogRef: Structure for controlling the vOutSensed datalog; Input
//=====================================
//
extern volatile float32_t            BUCK_dutySetRef_pu;
extern float32_t                     BUCK_dutySet_pu;
extern volatile float32_t            BUCK_dacSetRef_pu;
extern float32_t                     BUCK_dacSet_pu;
extern float32_t                     BUCK_vInSensed_Volts;
extern volatile float32_t            BUCK_vOutSetRef_Volts;
extern float32_t                     BUCK_vOutSensed_Volts;
extern float32_t                     BUCK_iOutSensed_Amps;
extern volatile float32_t            BUCK_iOutTripSetRef_Amps;
extern uint16_t                      BUCK_iOutTripFlag;
extern volatile uint16_t             BUCK_iOutTripFlagClear;
extern volatile uint16_t             BUCK_activeLoadEnable;
extern volatile uint16_t             BUCK_activeLoadContEnable;
extern volatile BUCK_CtrlUpdate_Type BUCK_ctrlRef;
extern volatile uint16_t             BUCK_isrDutyUpdateMarginClear;
extern volatile BUCK_TcmRef_Type     BUCK_vOutLogRef;

//
//=====================================
// Variables used by the program to run the BUCK solution.
//
//     dutyTarget_pu: Error-checked duty cycle target from user input
//     dutySlewed_pu: Incrementally slewed duty cycle to reach dutyTarget_pu
//     dacTarget_pu: Error-checked MAXREF DAC target from user input
//     dacSlewed_pu: Incrementally slewed MAXREF DAC to reach dacTarget_pu
//     vOutSet_pu: Effective Vout setting from ISR
//     vOutTarget_pu: Error-checked Vout target calculated from user input
//     vOutSlewed_pu: Incrementally slewed Vout to reach vOutTarget_pu
//     vOutSensed_pu: ADC reading of Vout
//     kp: kp from user input
//     ki: ki from user input
//     kd: Kd from user input
//     uk_pu: Control effort
//     ek_pu: Control error
//     iLTripDacVal: Effective comparator trip setting
//     isrEnter_ticks: TBCTR at beginning of ISR
//     isrDutyUpdate_ticks: TBCTR after CMPA shadow register is written
//     isrDutyUpdateMargin_ticks: Margin before CMPA is shadow loaded
//     isrDutyUpdateMinMargin_ticks: MIN(isrDutyUpdateMargin_ticks)
//     buildLevel: Effective build level for program
//     adcSampleIndex: Array index pointing to oldest value
//     vInSamples_Volts: Array of ADC conversions of Vin
//     vOutSamples_Volts: Array of ADC conversions of Vout
//     iLSamples_Amps: Array of ADC conversions of ILFB
//     MEP_ScaleFactor: Required variable for HRPWM SFO library
//     sfoStatus: Status from HRPWM SFO library
//=====================================
//
extern float32_t  BUCK_dutyTarget_pu;
extern float32_t  BUCK_dutySlewed_pu;
extern float32_t  BUCK_dacTarget_pu;
extern float32_t  BUCK_dacSlewed_pu;
extern float32_t  BUCK_vOutSet_pu;
extern float32_t  BUCK_vOutTarget_pu;
extern float32_t  BUCK_vOutSlewed_pu;
extern float32_t  BUCK_vOutSensed_pu;
extern float32_t  BUCK_kp;
extern float32_t  BUCK_ki;
extern float32_t  BUCK_kd;
extern float32_t  BUCK_uk_pu;
extern float32_t  BUCK_ek_pu;
#ifndef __TMS320C28XX_CLA__
extern DCL_ZPK3   BUCK_zpk3;
#endif
extern uint16_t   BUCK_adcSampleIndex;
extern float32_t  BUCK_vInSamples_Volts[BUCK_AVG_ADC_SAMPLES];
extern float32_t  BUCK_vOutSamples_Volts[BUCK_AVG_ADC_SAMPLES];
extern float32_t  BUCK_iLSamples_Amps[BUCK_AVG_ADC_SAMPLES];
extern uint32_t   MEP_ScaleFactor;
extern uint16_t   BUCK_sfoStatus;

//
//=====================================
// Variables that are useful for debug of the BUCK solution.
//
//     labNumber: Effective build level for program
//     siteNumber: Launchpad site (headers) used for Booster Pack
//     iLTripDacVal: Effective comparator trip setting
//     rampDecVal: Effective CMPSS RAMPDECVAL
//     isrEnter_ticks: TBCTR at beginning of ISR
//     isrExit_ticks: TBCTR at end of ISR
//     isrDutyUpdate_ticks: TBCTR after CMPA shadow register is written
//     isrDutyUpdateMargin_ticks: Margin before CMPA is shadow loaded
//     isrDutyUpdateMinMargin_ticks: MIN(isrDutyUpdateMargin_ticks)
//
//=====================================
//
extern BUCK_Lab_EnumType  BUCK_labNumber;
extern uint16_t           BUCK_siteNumber;
extern uint16_t           BUCK_iLTripDacVal;
extern uint16_t           BUCK_rampDecVal;
extern uint16_t           BUCK_isrEnter_ticks;
extern uint16_t           BUCK_isrExit_ticks;
extern uint16_t           BUCK_isrDutyUpdate_ticks;
extern int16_t            BUCK_isrDutyUpdateMargin_ticks;
extern int16_t            BUCK_isrDutyUpdateMinMargin_ticks;

//
//=====================================
// DCL control variables
//
extern BUCK_DCL_CTRL_TYPE BUCK_ctrl;
#ifndef __TMS320C28XX_CLA__
extern BUCK_DCL_SPS_TYPE BUCK_ctrlSps;
extern DCL_CSS BUCK_ctrlCss;
#endif
//
//=====================================
// DCL TCM datalog variables
//
extern TCM                BUCK_vOutLogTcm;
extern FDLOG              BUCK_vOutLogFdlog;
extern float32_t          BUCK_vOutLog_Volts[BUCK_VOUT_LOG_SIZE];

//
//=====================================
// SFRA variables
//
#if(BUCK_SFRA_ENABLED == true)
#ifndef __TMS320C28XX_CLA__
    extern SFRA_F32 BUCK_sfra;

    //
    //=====================================
    // BUCK_SfraDataType - SFRA sweep data
    //
    typedef struct {
        float32_t  plantMagVect[BUCK_SFRA_FREQ_LENGTH];
        float32_t  plantPhaseVect[BUCK_SFRA_FREQ_LENGTH];
        float32_t  olMagVect[BUCK_SFRA_FREQ_LENGTH];
        float32_t  olPhaseVect[BUCK_SFRA_FREQ_LENGTH];
        float32_t  freqVect[BUCK_SFRA_FREQ_LENGTH];
    } BUCK_SfraDataType;
    extern BUCK_SfraDataType BUCK_sfraData;
#endif
#endif


//
//=============================================================================
// Function prototypes from solution source
//=============================================================================
//
void BUCK_initUserVariables(void);
void BUCK_initProgramVariables(void);
void BUCK_setupHrpwmMepScaleFactor(void);
void BUCK_setupDcl(void);
void BUCK_setupSfra(void);
void BUCK_setupSfraGui(void);
void BUCK_updateControllerCoefficients(void);
void BUCK_updateDutyTarget(void);
void BUCK_updateDutySlewed(void);
void BUCK_updateVoutTarget(void);
void BUCK_updateVoutSlewed(void);
void BUCK_updateDacTarget(void);
void BUCK_updateDacSlewed(void);
void BUCK_updateActiveLoad(void);
void BUCK_updateSensedValues(void);
void BUCK_updateOverCurrentStatus(void);
void BUCK_processIsrDutyUpdateMarginStats(void);
void BUCK_updateVoutLog(void);

//
//=============================================================================
// static inline functions
//=============================================================================
//
#pragma FUNC_ALWAYS_INLINE(BUCK_updateOverCurrentLimit)
static inline void BUCK_updateOverCurrentLimit(void)
{
    //
    // Update effective over-current trip limit based on user input
    //
    BUCK_iLTripDacVal = BUCK_HAL_setOverCurrentTripLimit(
                                        BUCK_iOutTripSetRef_Amps);
}

#pragma FUNC_ALWAYS_INLINE(BUCK_updateHrpwmMepScaleFactor)
static inline void BUCK_updateHrpwmMepScaleFactor(void)
{
    #if(BUCK_DRV_EPWM_HR_ENABLED == true)
        BUCK_sfoStatus = SFO();
    #endif
}

//
// When executed immediately after the ePWM CMPA register is written, this
// function will calculate how many ePWM ticks remain (margin) before the
// CMPA shadow register is loaded to active.
// The intended CMPA update may be delayed by one ePWM cycle when there is
// negative margin.
//
#pragma FUNC_ALWAYS_INLINE(BUCK_logDutyUpdateMargin)
static inline void BUCK_logDutyUpdateMargin(void)
{
    //
    // Record the current value of the BUCK ePWM TBCTR
    //
    BUCK_isrDutyUpdate_ticks =
            EPWM_getTimeBaseCounterValue(BUCK_DRV_EPWM_BASE);

    //
    // Calculate the update margin with consideration for TBCTR overflow.
    // It is assumed that the ISR execution time will not exceed TBPRD.
    //
    if( BUCK_isrDutyUpdate_ticks > BUCK_DRV_EPWM_ADC_TRIGGER_TBCTR)
    {
        //
        // Positive margin
        //
        BUCK_isrDutyUpdateMargin_ticks =
                BUCK_DRV_EPWM_PERIOD_TICKS -
                BUCK_isrDutyUpdate_ticks;
    }
    else
    {
        //
        // Negative margin
        //
        BUCK_isrDutyUpdateMargin_ticks =
                (0 - BUCK_isrDutyUpdate_ticks);
    }

    //
    // Record MIN(margin)
    //
    if( BUCK_isrDutyUpdateMargin_ticks <
        BUCK_isrDutyUpdateMinMargin_ticks )
    {
        BUCK_isrDutyUpdateMinMargin_ticks =
                BUCK_isrDutyUpdateMargin_ticks;
    }
}


#pragma FUNC_ALWAYS_INLINE(BUCK_runIsr)
static inline void BUCK_runIsr(void)
{
    //
    // Record the ePWM TBCTR value for informational purposes.
    // The value is helpful for measuring the sum of ADC ACQPS + ISR latency.
    // The value can also help to confirm that enough time has elapsed for
    //   the Vout ADC conversion to complete and arrive in ADCxRESULT
    //
    BUCK_isrEnter_ticks = EPWM_getTimeBaseCounterValue(BUCK_DRV_EPWM_BASE);

    //
    // Record the most recent ADC conversion of Vout
    //
    BUCK_vOutSensed_pu = BUCK_HAL_readVout_pu();

    #if(BUCK_CTL_LOOP == BUCK_CTL_LOOP_OPEN)
        #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
            //
            // Determine the desired duty cycle setting and update CMPA
            //
            BUCK_dutySet_pu = BUCK_SFRA_INJECT(BUCK_dutySlewed_pu);
            BUCK_HAL_updateSyncBuckDuty(BUCK_dutySet_pu);

            //
            // For informational purposes, calculate and record the amount of
            // time remaining before the CMPA shadow load takes place.
            //
            BUCK_logDutyUpdateMargin();
        #elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
            BUCK_dacSet_pu = BUCK_SFRA_INJECT(BUCK_dacSlewed_pu);
            BUCK_HAL_updateSyncBuckDac(BUCK_dacSet_pu);

            //
            // For informational purposes, calculate and record the amount of
            // time remaining before the CMPA shadow load takes place.
            //
            BUCK_logDutyUpdateMargin();
        #endif
    #elif(BUCK_CTL_LOOP == BUCK_CTL_LOOP_CLOSED)
        //
        // Determine the desired Vout setting and its associated error
        //
        BUCK_vOutSet_pu =
            BUCK_SFRA_INJECT(BUCK_vOutSlewed_pu);

        BUCK_ek_pu = BUCK_vOutSet_pu -
                                 BUCK_vOutSensed_pu;

        //
        // Calculate the required control effort (duty cycle)
        //
        BUCK_uk_pu =
                BUCK_DCL_RUN_IMMEDIATE(&BUCK_ctrl,
                                       BUCK_ek_pu);

        //
        // Only update the duty cycle if the control effort is within range
        //
        if(BUCK_DCL_RUN_CLAMP(&BUCK_uk_pu,
                              BUCK_UK_MAX_PU,
                              BUCK_UK_MIN_PU) == 0)
        {
            #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
                //
                // Update CMPA with the calculated duty cycle
                //
                BUCK_HAL_updateSyncBuckDuty(BUCK_uk_pu);
            #elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
                //
                // Update CMPSS DAC MAXREF with the calculated level
                //
                BUCK_HAL_updateSyncBuckDac(BUCK_uk_pu);
            #endif

            //
            // For informational purposes, calculate and record the amount of
            // time remaining before the CMPA shadow load takes place.
            //
            BUCK_logDutyUpdateMargin();

            //
            // Record the duty cycle and pre-compute the partial control
            // effort for the next ISR execution
            //
            #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
                BUCK_dutySet_pu = BUCK_uk_pu;
            #elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
                BUCK_dacSet_pu = BUCK_uk_pu;
            #endif
            BUCK_DCL_RUN_PARTIAL(&BUCK_ctrl,
                                 BUCK_ek_pu,
                                 BUCK_uk_pu);
        }
        else {
            #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
                if(BUCK_uk_pu <= BUCK_UK_MIN_PU)
                {
                    BUCK_HAL_updateSyncBuckDuty(BUCK_UK_MIN_PU);
                    BUCK_dutySet_pu = BUCK_UK_MIN_PU;
                }
                if(BUCK_uk_pu >= BUCK_UK_MAX_PU)
                {
                    BUCK_HAL_updateSyncBuckDuty(BUCK_UK_MAX_PU);
                    BUCK_dutySet_pu = BUCK_UK_MAX_PU;
                }
            #elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
                if(BUCK_uk_pu <= BUCK_UK_MIN_PU)
                {
                    BUCK_HAL_updateSyncBuckDac(BUCK_UK_MIN_PU);
                    BUCK_dacSet_pu = BUCK_UK_MIN_PU;
                }
                if(BUCK_uk_pu >= BUCK_UK_MAX_PU)
                {
                    BUCK_HAL_updateSyncBuckDac(BUCK_UK_MAX_PU);
                    BUCK_dacSet_pu = BUCK_UK_MAX_PU;
                }
            #endif
        }
    #endif

    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
        BUCK_SFRA_COLLECT(&BUCK_dutySet_pu,
                          &BUCK_vOutSensed_pu);
    #elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        BUCK_SFRA_COLLECT(&BUCK_dacSet_pu,
                          &BUCK_vOutSensed_pu);
    #endif

    BUCK_vInSamples_Volts[BUCK_adcSampleIndex]  = BUCK_HAL_readVin_V();
    BUCK_vOutSamples_Volts[BUCK_adcSampleIndex] = BUCK_HAL_readVout_V();
    BUCK_iLSamples_Amps[BUCK_adcSampleIndex]    = BUCK_HAL_readFilteredIl_A();

    BUCK_adcSampleIndex++;
    if( BUCK_adcSampleIndex >= BUCK_AVG_ADC_SAMPLES) {
        BUCK_adcSampleIndex = 0;
    }

    DCL_runTCM(&BUCK_vOutLogTcm, BUCK_HAL_readVout_V());

    BUCK_HAL_clearInterruptFlags();
    BUCK_isrExit_ticks = EPWM_getTimeBaseCounterValue(BUCK_DRV_EPWM_BASE);
}

#ifdef __cplusplus
}
#endif                                  /* extern "C" */

#endif
