//#############################################################################
//
// FILE:  buck.c
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

//
//=============================================================================
// includes
//=============================================================================
//
#include <buck.h>

//
//=============================================================================
// Global variables
//=============================================================================
//

//
//=====================================
// Variables used by the program to run the BUCK solution.
//
// This subset of variables can be used interactively to monitor and
// control the execution of the BUCK solution.
//
// See buck.h for a description of the variables.
//
volatile float32_t            BUCK_dutySetRef_pu;
float32_t                     BUCK_dutySet_pu;
volatile float32_t            BUCK_dacSetRef_pu;
float32_t                     BUCK_dacSet_pu;
float32_t                     BUCK_vInSensed_Volts;
volatile float32_t            BUCK_vOutSetRef_Volts;
float32_t                     BUCK_vOutSensed_Volts;
float32_t                     BUCK_iOutSensed_Amps;
volatile float32_t            BUCK_iOutTripSetRef_Amps;
uint16_t                      BUCK_iOutTripFlag;
volatile uint16_t             BUCK_iOutTripFlagClear;
volatile uint16_t             BUCK_activeLoadEnable;
volatile uint16_t             BUCK_activeLoadContEnable;
volatile BUCK_CtrlUpdate_Type BUCK_ctrlRef;
volatile uint16_t             BUCK_isrDutyUpdateMarginClear;
volatile BUCK_TcmRef_Type     BUCK_vOutLogRef;

//
//=====================================
// Variables used by the program to run the BUCK solution.
//
// See buck.h for a description of the variables.
//
float32_t            BUCK_dutyTarget_pu;
float32_t            BUCK_dutySlewed_pu;
float32_t            BUCK_dacTarget_pu;
float32_t            BUCK_dacSlewed_pu;
float32_t            BUCK_vOutSet_pu;
float32_t            BUCK_vOutTarget_pu;
float32_t            BUCK_vOutSlewed_pu;
float32_t            BUCK_vOutSensed_pu;
float32_t            BUCK_kp;
float32_t            BUCK_ki;
float32_t            BUCK_kd;
float32_t            BUCK_uk_pu;
float32_t            BUCK_ek_pu;
DCL_ZPK3             BUCK_zpk3;
uint16_t             BUCK_adcSampleIndex;
float32_t            BUCK_vInSamples_Volts[BUCK_AVG_ADC_SAMPLES];
float32_t            BUCK_vOutSamples_Volts[BUCK_AVG_ADC_SAMPLES];
float32_t            BUCK_iLSamples_Amps[BUCK_AVG_ADC_SAMPLES];

uint32_t             MEP_ScaleFactor;
uint16_t             BUCK_sfoStatus;

//
//=====================================
// Variables that are useful for debug of the BUCK solution.
//
// See buck.h for a description of the variables.
//
BUCK_Lab_EnumType    BUCK_labNumber = (BUCK_Lab_EnumType)BUCK_LAB_NUMBER;
uint16_t             BUCK_siteNumber = BUCK_LAUNCHPAD_SITE;
uint16_t             BUCK_iLTripDacVal;
uint16_t             BUCK_rampDecVal;
uint16_t             BUCK_rampMaxRef;
uint16_t             BUCK_isrEnter_ticks;
uint16_t             BUCK_isrExit_ticks;
uint16_t             BUCK_isrDutyUpdate_ticks;
int16_t              BUCK_isrDutyUpdateMargin_ticks;
int16_t              BUCK_isrDutyUpdateMinMargin_ticks;

//
//=====================================
// DCL control variables
//
BUCK_DCL_CTRL_TYPE   BUCK_ctrl    = BUCK_DCL_CTRL_DEFAULTS;
#ifndef __TMS320C28XX_CLA__
//
//#####BEGIN_INTERNAL#####
//todo Figure out how to replace the FPU32 equivalents of _SPS and _CSS
//#####END_INTERNAL#####
// NOTE: these are still used in the CLA case. Just that they get defined
// out when compiling the .cla file since they are not visible to it.
//
BUCK_DCL_SPS_TYPE    BUCK_ctrlSps = BUCK_DCL_SPS_DEFAULTS;
DCL_CSS              BUCK_ctrlCss = DCL_CSS_DEFAULTS;
#endif
//
//=====================================
// DCL TCM datalog variables
//
TCM                  BUCK_vOutLogTcm;
FDLOG                BUCK_vOutLogFdlog;
float32_t            BUCK_vOutLog_Volts[BUCK_VOUT_LOG_SIZE];

//
//=====================================
// SFRA variables
//
#if(BUCK_SFRA_ENABLED == true)
SFRA_F32             BUCK_sfra;
BUCK_SfraDataType    BUCK_sfraData;
#endif

//
//=============================================================================
// solution functions
//=============================================================================
//

void BUCK_initUserVariables(void)
{
    BUCK_dutySetRef_pu             = 0.0f;
    BUCK_dutySet_pu                = 0.0f;
    BUCK_dacSetRef_pu              = 0.0f;
    BUCK_dacSet_pu                 = 0.0f;
    BUCK_vInSensed_Volts           = 0.0f;
    BUCK_vOutSetRef_Volts          = 0.0f;
    BUCK_vOutSensed_Volts          = 0.0f;
    BUCK_iOutSensed_Amps           = 0.0f;
    BUCK_iOutTripSetRef_Amps       = BUCK_USER_DEFAULT_TRIP_A;
    BUCK_iOutTripFlag              = 0;
    BUCK_iOutTripFlagClear         = 0;
    BUCK_activeLoadEnable          = BUCK_USER_DEFAULT_LOAD_ENABLE;
    BUCK_activeLoadContEnable      = BUCK_USER_DEFAULT_LOAD_CONT;
    BUCK_ctrlRef.ctrlUpdate        = 0;
    BUCK_ctrlRef.ctrlFormat        = BUCK_DCL_DEFAULT_FORMAT;
    BUCK_ctrlRef.ctrlPid.Scale     = BUCK_DCL_DEFAULT_PID_SCALE;
    BUCK_ctrlRef.ctrlPid.Kp        = BUCK_DCL_DEFAULT_KP;
    BUCK_ctrlRef.ctrlPid.Ki        = BUCK_DCL_DEFAULT_KI;
    BUCK_ctrlRef.ctrlPid.Kd        = BUCK_DCL_DEFAULT_KD;
    BUCK_ctrlRef.ctrlPid.fc_Hz     = BUCK_DCL_DEFAULT_BW_HZ;
    BUCK_ctrlRef.ctrlZpk.Kdc       = BUCK_DCL_DEFAULT_KDC;
    BUCK_ctrlRef.ctrlZpk.Z0_kHz    = BUCK_DCL_DEFAULT_Z0;
    BUCK_ctrlRef.ctrlZpk.Z1_kHz    = BUCK_DCL_DEFAULT_Z1;
    BUCK_ctrlRef.ctrlZpk.P1_kHz    = BUCK_DCL_DEFAULT_P1;
    BUCK_ctrlRef.ctrlCoeff.B0      = BUCK_DCL_DEFAULT_B0;
    BUCK_ctrlRef.ctrlCoeff.B1      = BUCK_DCL_DEFAULT_B1;
    BUCK_ctrlRef.ctrlCoeff.B2      = BUCK_DCL_DEFAULT_B2;
    BUCK_ctrlRef.ctrlCoeff.A1      = BUCK_DCL_DEFAULT_A1;
    BUCK_ctrlRef.ctrlCoeff.A2      = BUCK_DCL_DEFAULT_A2;

    #if(BUCK_CTL_LOOP == BUCK_CTL_LOOP_CLOSED)
        BUCK_vOutSetRef_Volts      = BUCK_VOUT_NOM_V;
    #endif

    BUCK_iLTripDacVal                 = 0;
    BUCK_isrEnter_ticks               = 0;
    BUCK_isrDutyUpdate_ticks          = 0;
    BUCK_isrExit_ticks                = 0;
    BUCK_isrDutyUpdateMargin_ticks    = 0;
    BUCK_isrDutyUpdateMinMargin_ticks = BUCK_DRV_EPWM_PERIOD_TICKS;
    BUCK_isrDutyUpdateMarginClear     = 0;
}

void BUCK_initProgramVariables(void)
{
    BUCK_dutyTarget_pu = 0.0f;
    BUCK_dutySlewed_pu = 0.0f;
    BUCK_dacTarget_pu  = 0.0f;
    BUCK_dacSlewed_pu  = 0.0f;
    BUCK_vOutSet_pu    = 0.0f;
    BUCK_vOutTarget_pu = 0.0f;
    BUCK_vOutSlewed_pu = 0.0f;
    BUCK_vOutSensed_pu = 0.0f;
    BUCK_kp            = 0.0f;
    BUCK_ki            = 0.0f;
    BUCK_kd            = 0.0f;
    BUCK_uk_pu         = 0.0f;
    BUCK_ek_pu         = 0.0f;

    BUCK_adcSampleIndex = BUCK_AVG_ADC_SAMPLES;
    do {
        BUCK_adcSampleIndex--;
        BUCK_vInSamples_Volts[BUCK_adcSampleIndex]  = 0.0f;
        BUCK_vOutSamples_Volts[BUCK_adcSampleIndex] = 0.0f;
        BUCK_iLSamples_Amps[BUCK_adcSampleIndex]    = 0.0f;
    } while(BUCK_adcSampleIndex > 0);

    DCL_initLog(&BUCK_vOutLogFdlog, BUCK_vOutLog_Volts, BUCK_VOUT_LOG_SIZE);
    DCL_initTCM(&BUCK_vOutLogTcm,
                BUCK_vOutLogFdlog.fptr,
                BUCK_VOUT_LOG_SIZE,
                BUCK_VOUT_LOG_LEAD_SIZE,
                BUCK_vOutSetRef_Volts - BUCK_VOUT_LOG_UNDER_V,
                BUCK_vOutSetRef_Volts + BUCK_VOUT_LOG_OVER_V);

    BUCK_vOutLogRef.forceTrig   = 0;
    BUCK_vOutLogRef.armTrig     = 0;
    BUCK_vOutLogRef.leadSamples = BUCK_vOutLogTcm.lead;
    BUCK_vOutLogRef.trigMin_V   = BUCK_vOutLogTcm.trigMin;
    BUCK_vOutLogRef.trigMax_V   = BUCK_vOutLogTcm.trigMax;
}

void BUCK_setupHrpwmMepScaleFactor(void)
{
    #if(BUCK_DRV_EPWM_HR_ENABLED == true)
        //
        // Calibrate MEP_ScaleFactor
        //
        do {
            BUCK_sfoStatus = SFO();
        } while(BUCK_sfoStatus == SFO_INCOMPLETE);
    #endif
}

void BUCK_setupDcl(void)
{
    BUCK_DCL_RESET(&BUCK_ctrl);
    BUCK_ctrl.sps = &BUCK_ctrlSps;
    BUCK_ctrl.css = &BUCK_ctrlCss;

    BUCK_ctrl.css->T = (float32_t)1.0f / BUCK_DRV_EPWM_SWITCHING_FREQUENCY;

    BUCK_ctrl.sps->b0 = BUCK_ctrlRef.ctrlCoeff.B0;
    BUCK_ctrl.sps->b1 = BUCK_ctrlRef.ctrlCoeff.B1;
    BUCK_ctrl.sps->b2 = BUCK_ctrlRef.ctrlCoeff.B2;
    BUCK_ctrl.sps->a1 = BUCK_ctrlRef.ctrlCoeff.A1;
    BUCK_ctrl.sps->a2 = BUCK_ctrlRef.ctrlCoeff.A2;

    DCL_REQUEST_UPDATE(&BUCK_ctrl);
    BUCK_DCL_UPDATE(&BUCK_ctrl);
}

void BUCK_setupSfra(void)
{
    #if(BUCK_SFRA_ENABLED == true)
        SFRA_F32_reset(&BUCK_sfra);
        SFRA_F32_config(&BUCK_sfra,
                        BUCK_SFRA_ISR_FREQ,
                        BUCK_SFRA_AMPLITUDE,
                        BUCK_SFRA_FREQ_LENGTH,
                        BUCK_SFRA_FREQ_START,
                        BUCK_SFRA_FREQ_STEP_MULTIPLY,
                        BUCK_sfraData.plantMagVect,
                        BUCK_sfraData.plantPhaseVect,
                        BUCK_sfraData.olMagVect,
                        BUCK_sfraData.olPhaseVect,
                        NULL,
                        NULL,
                        BUCK_sfraData.freqVect,
                        BUCK_SFRA_SWEEP_SPEED);
        SFRA_F32_resetFreqRespArray(&BUCK_sfra);
        SFRA_F32_initFreqArrayWithLogSteps(&BUCK_sfra,
                                           BUCK_SFRA_FREQ_START,
                                           BUCK_SFRA_FREQ_STEP_MULTIPLY);
    #endif
}

void BUCK_setupSfraGui(void)
{
    #if(BUCK_SFRA_ENABLED == true)
        SFRA_GUI_config(BUCK_SFRA_GUI_SCI_BASE,
                        BUCK_HAL_getLowSpeedClock(),
                        BUCK_SFRA_GUI_SCI_BAUDRATE,
                        BUCK_SFRA_GUI_SCIRX_GPIO,
                        BUCK_SFRA_GUI_SCIRX_PIN_CONFIG,
                        BUCK_SFRA_GUI_SCITX_GPIO,
                        BUCK_SFRA_GUI_SCITX_PIN_CONFIG,
                        BUCK_SFRA_GUI_LED_ENABLE,
                        BUCK_SFRA_GUI_LED_GPIO,
                        BUCK_SFRA_GUI_LED_PIN_CONFIG,
                        &BUCK_sfra,
                        BUCK_SFRA_GUI_PLOT_OPTION);
    #endif
}

//
// BUCK_updateControllerCoefficients - Convert the user-selected input format
// into DCL coefficients and update the controller when the update flag is set.
//
void BUCK_updateControllerCoefficients(void)
{
    //
    // Update the active DCL coefficients if requested by the user
    //
    if(BUCK_ctrlRef.ctrlUpdate != 0)
    {
        if(BUCK_ctrlRef.ctrlFormat == Format_PID)
        {
            //
            // Store the scaled values into DCL PID format
            //
            BUCK_kp = BUCK_ctrlRef.ctrlPid.Kp / BUCK_ctrlRef.ctrlPid.Scale;
            BUCK_ki = BUCK_ctrlRef.ctrlPid.Ki / BUCK_ctrlRef.ctrlPid.Scale;
            BUCK_kd = BUCK_ctrlRef.ctrlPid.Kd / BUCK_ctrlRef.ctrlPid.Scale;

            BUCK_DCL_LOAD_PID(&BUCK_ctrl,
                              BUCK_kp,
                              BUCK_ki,
                              BUCK_kd,
                              BUCK_ctrlRef.ctrlPid.fc_Hz);
        }
        else if(BUCK_ctrlRef.ctrlFormat == Format_ZPK)
        {
            //
            // Store the calculated values into the DCL ZPK3 format
            // Z-P frequency values are converted to radians in Left Half-Plane
            //
            BUCK_zpk3.z1 = (-2 * BUCK_DCL_PI * 1000) *
                           BUCK_ctrlRef.ctrlZpk.Z0_kHz;
            BUCK_zpk3.z2 = (-2 * BUCK_DCL_PI * 1000) *
                           BUCK_ctrlRef.ctrlZpk.Z1_kHz;
            BUCK_zpk3.p1 = 0;
            BUCK_zpk3.p2 = (-2 * BUCK_DCL_PI * 1000) *
                           BUCK_ctrlRef.ctrlZpk.P1_kHz;

            //
            // Normalize Gain
            //
            BUCK_zpk3.K = BUCK_ctrlRef.ctrlZpk.Kdc *
                          BUCK_ctrlRef.ctrlZpk.P1_kHz /
                          BUCK_ctrlRef.ctrlZpk.Z0_kHz /
                          BUCK_ctrlRef.ctrlZpk.Z1_kHz /
                          (2 * BUCK_DCL_PI * 1000);

            BUCK_DCL_LOAD_ZPK(&BUCK_ctrl,
                              &BUCK_zpk3);
        }
        else if(BUCK_ctrlRef.ctrlFormat == Format_Coeff)
        {
            //
            // Copy coefficients directly into DCL SPS structure
            //
            #if(BUCK_DCL_CTRL_TYPE == DCL_DF22)
                BUCK_ctrl.sps->b0 = BUCK_ctrlRef.ctrlCoeff.B0;
                BUCK_ctrl.sps->b1 = BUCK_ctrlRef.ctrlCoeff.B1;
                BUCK_ctrl.sps->b2 = BUCK_ctrlRef.ctrlCoeff.B2;
                BUCK_ctrl.sps->a1 = BUCK_ctrlRef.ctrlCoeff.A1;
                BUCK_ctrl.sps->a2 = BUCK_ctrlRef.ctrlCoeff.A2;
            #else
                asm(" ESTOP0");
            #endif
        }

        DCL_REQUEST_UPDATE(&BUCK_ctrl);
        BUCK_DCL_UPDATE(&BUCK_ctrl);
        BUCK_DCL_RESET(&BUCK_ctrl);
        BUCK_ctrlRef.ctrlUpdate = 0;
        BUCK_isrDutyUpdateMarginClear = 1;
    }

    //
    // Store the active DCL coefficients for user observation
    //
    BUCK_ctrlRef.ctrlActiveCoeff.B0 = BUCK_ctrl.b0;
    BUCK_ctrlRef.ctrlActiveCoeff.B1 = BUCK_ctrl.b1;
    BUCK_ctrlRef.ctrlActiveCoeff.B2 = BUCK_ctrl.b2;
    BUCK_ctrlRef.ctrlActiveCoeff.A1 = BUCK_ctrl.a1;
    BUCK_ctrlRef.ctrlActiveCoeff.A2 = BUCK_ctrl.a2;
}

//
// BUCK_updateDutyTarget - Store a snapshot of the desired duty cycle as
// provided by the user. Range check and correct the snapshot value.
//
#pragma CODE_SECTION(BUCK_updateDutyTarget,"ramfuncs");
void BUCK_updateDutyTarget(void)
{
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
        BUCK_dutyTarget_pu = BUCK_dutySetRef_pu;

        if(BUCK_dutyTarget_pu > BUCK_DUTY_SET_MAX_PU)
        {
            BUCK_dutyTarget_pu = BUCK_DUTY_SET_MAX_PU;
        }
        else if(BUCK_dutyTarget_pu < BUCK_DUTY_SET_MIN_PU)
        {
            BUCK_dutyTarget_pu = BUCK_DUTY_SET_MIN_PU;
        }
    #endif
}

//
// BUCK_updateDutySlewed - Generate a slewed (dampened) value of the
// target duty cycle to avoid disruptions from abrupt system changes.
//
#pragma CODE_SECTION(BUCK_updateDutySlewed,"ramfuncs");
void BUCK_updateDutySlewed(void)
{
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
        float32_t dutySlew;

        dutySlew = BUCK_dutyTarget_pu - BUCK_dutySet_pu;

        if(dutySlew > BUCK_DUTY_SLEW_MAX_PU)
        {
            BUCK_dutySlewed_pu = BUCK_dutySet_pu + BUCK_DUTY_SLEW_MAX_PU;
        }
        else if(dutySlew < (0 - BUCK_DUTY_SLEW_MAX_PU))
        {
            BUCK_dutySlewed_pu = BUCK_dutySet_pu - BUCK_DUTY_SLEW_MAX_PU;
        }
        else
        {
            BUCK_dutySlewed_pu = BUCK_dutyTarget_pu;
        }
    #endif
}

//
// BUCK_updateDacTarget - Store a snapshot of the desired MAXREF DAC code
// provided by the user. Range check and correct the snapshot value.
//
#pragma CODE_SECTION(BUCK_updateDacTarget,"ramfuncs");
void BUCK_updateDacTarget(void)
{
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        BUCK_dacTarget_pu = BUCK_dacSetRef_pu+2025.0/4096 ;

        if(BUCK_dacTarget_pu > BUCK_DAC_SET_MAX_PU)
        {
            BUCK_dacTarget_pu = BUCK_DAC_SET_MAX_PU;
        }
        else if(BUCK_dacTarget_pu < BUCK_DAC_SET_MIN_PU)
        {
            BUCK_dacTarget_pu = BUCK_DAC_SET_MIN_PU;
        }
    #endif
}

//
// BUCK_updateDacSlewed - Generate a slewed (dampened) value of the
// target DAC code to avoid disruptions from abrupt system changes.
//
#pragma CODE_SECTION(BUCK_updateDacSlewed,"ramfuncs");
void BUCK_updateDacSlewed(void)
{
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        float32_t dacSlew;

        dacSlew = BUCK_dacTarget_pu - BUCK_dacSet_pu;

        if(dacSlew > BUCK_DAC_SLEW_MAX_PU)
        {
            BUCK_dacSlewed_pu = BUCK_dacSet_pu + BUCK_DAC_SLEW_MAX_PU;
        }
        else if(dacSlew < (0 - BUCK_DAC_SLEW_MAX_PU))
        {
            BUCK_dacSlewed_pu = BUCK_dacSet_pu - BUCK_DAC_SLEW_MAX_PU;
        }
        else
        {
            BUCK_dacSlewed_pu = BUCK_dacTarget_pu;
        }
    #endif
}

//
// BUCK_updateVoutTarget - Store a snapshot of the desired Vout as
// provided by the user. Range check and correct the snapshot value.
//
#pragma CODE_SECTION(BUCK_updateVoutTarget,"ramfuncs");
void BUCK_updateVoutTarget(void)
{
    if( (BUCK_HAL_readOverCurrentTripFlag() == 0) &&
        (BUCK_vInSensed_Volts > BUCK_VOUT_ENABLE_VIN_THRESH) )
    {
        //
        // If system is not tripped and Vin is on, store the snapshot as PU
        //
        BUCK_vOutTarget_pu = BUCK_vOutSetRef_Volts * BUCK_VOUT_V_FACTOR_PU;
    }
    else
    {
        //
        // If system is tripped or Vin is off, ignore user input and
        // set target to 0V to avoid control effort saturation at MAX
        //
        BUCK_vOutTarget_pu = 0;
    }
}

//
// BUCK_updateVoutSlewed - Generate a slewed (dampened) value of the
// target Vout to avoid disruptions from abrupt system changes.
//
#pragma CODE_SECTION(BUCK_updateVoutSlewed,"ramfuncs");
void BUCK_updateVoutSlewed(void)
{
    float32_t voutSlew;

    voutSlew = BUCK_vOutTarget_pu - BUCK_vOutSet_pu;

    if(voutSlew > BUCK_VOUT_SLEW_MAX_PU)
    {
        BUCK_vOutSlewed_pu += BUCK_VOUT_SLEW_MAX_PU;
    }
    else if(voutSlew < (0 - BUCK_VOUT_SLEW_MAX_PU))
    {
        BUCK_vOutSlewed_pu -= BUCK_VOUT_SLEW_MAX_PU;
    }
    else
    {
        BUCK_vOutSlewed_pu = BUCK_vOutTarget_pu;
    }
}

void BUCK_updateActiveLoad(void)
{
    if(BUCK_activeLoadEnable == 1)
    {
        if(BUCK_activeLoadContEnable == 1)
        {
            BUCK_HAL_enableContinuousActiveLoad();
        }
        else
        {
            BUCK_HAL_enableDiscontinuousActiveLoad();
        }
    }
    else
    {
        BUCK_HAL_disableActiveLoad();
    }
}

//
// BUCK_updateSensedValues - Calculate averages from a recent series of
// ADC conversions for user consumption.
//
void BUCK_updateSensedValues(void)
{
    uint16_t j;
    float32_t sumVin;
    float32_t sumVout;
    float32_t sumIl;

    sumVin  = 0;
    sumVout = 0;
    sumIl   = 0;

    for(j=0;j<BUCK_AVG_ADC_SAMPLES;j++)
    {
        sumVin  += BUCK_vInSamples_Volts[j];
        sumVout += BUCK_vOutSamples_Volts[j];
        sumIl   += BUCK_iLSamples_Amps[j];
    }

    BUCK_vInSensed_Volts  = sumVin  / BUCK_AVG_ADC_SAMPLES;
    BUCK_vOutSensed_Volts = sumVout / BUCK_AVG_ADC_SAMPLES;
    BUCK_iOutSensed_Amps  = sumIl   / BUCK_AVG_ADC_SAMPLES;
}

void BUCK_updateOverCurrentStatus(void)
{
    if(BUCK_iOutTripFlagClear == 1)
    {
        #if(BUCK_CTL_LOOP == BUCK_CTL_LOOP_CLOSED)
            BUCK_DCL_RESET(&BUCK_ctrl);
        #endif
        BUCK_HAL_clearOverCurrentTripFlag();
        BUCK_iOutTripFlagClear = 0;
    }
    BUCK_iOutTripFlag = BUCK_HAL_readOverCurrentTripFlag();
}

void BUCK_processIsrDutyUpdateMarginStats(void)
{
    if(BUCK_isrDutyUpdateMarginClear == 1)
    {
        BUCK_isrDutyUpdateMinMargin_ticks = BUCK_DRV_EPWM_PERIOD_TICKS;
        BUCK_isrDutyUpdateMarginClear = 0;
    }
}

//
// BUCK_updateVoutLog - Arm the TCM datalogger when the forceTrig or armTrig
// bits are set.
//
void BUCK_updateVoutLog(void)
{
    //
    // Immediately collect a frame of data on forceTrig == 1
    // This action requires two iterations of the BUCK_updateVoutLog:
    //     1) Arm the TCM using triggers that cannot be qualified in order to
    //        populate the Lead Frame
    //     2) Intrusively update the TCM with triggers that will be qualified
    //        in order to populate the Capture Frame
    //
    if(BUCK_vOutLogRef.forceTrig == 1)
    {
        if(BUCK_vOutLogTcm.mode != TCM_armed)
        {
            DCL_initTCM(&BUCK_vOutLogTcm,
                        BUCK_vOutLogFdlog.fptr,
                        BUCK_VOUT_LOG_SIZE,
                        BUCK_vOutLogRef.leadSamples,
                        -1.0f,
                        BUCK_VOUT_MAX_SENSE_V + 1.0f);
            DCL_armTCM(&BUCK_vOutLogTcm);
        }
        else
        {
            BUCK_vOutLogTcm.trigMin   =  1.0f;
            BUCK_vOutLogTcm.trigMax   = -1.0f;
            BUCK_vOutLogRef.forceTrig =  0;
        }
    }

    if(BUCK_vOutLogRef.armTrig == 1)
    {
        DCL_initTCM(&BUCK_vOutLogTcm,
                    BUCK_vOutLogFdlog.fptr,
                    BUCK_VOUT_LOG_SIZE,
                    BUCK_vOutLogRef.leadSamples,
                    BUCK_vOutLogRef.trigMin_V,
                    BUCK_vOutLogRef.trigMax_V);
        DCL_armTCM(&BUCK_vOutLogTcm);
        BUCK_vOutLogRef.armTrig = 0;
    }

    BUCK_vOutLogRef.tcmMode = (TCM_states)BUCK_vOutLogTcm.mode;

    if(BUCK_vOutLogRef.tcmMode == TCM_complete)
    {
        BUCK_vOutLogRef.triggered_V =
                BUCK_vOutLog_Volts[BUCK_vOutLogRef.leadSamples];
    }
}

//
// End of buck.c
//
