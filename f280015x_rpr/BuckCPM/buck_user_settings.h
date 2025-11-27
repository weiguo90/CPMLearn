//#############################################################################
//
// FILE:   buck_user_settings.h
//
// TITLE:  User-customized content for the project
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

#ifndef _BUCK_USERSETTINGS_H
#define _BUCK_USERSETTINGS_H

#ifdef __cplusplus

extern "C" {
#endif

//
//=============================================================================
// defines
//=============================================================================
//
// LAB 1 - Open Loop VMC
// LAB 2 - Closed Loop VMC
// LAB 3 - Open Loop PCMC
// LAB 4 - Closed Loop PCMC
//
#define C28X_CORE 1
#define CLA_CORE  2

#ifdef BUCK_CONTROL_RUNNING_ON_CPU
#define BUCK_CONTROL_RUNNING_ON C28X_CORE
#endif

#ifdef BUCK_CONTROL_RUNNING_ON_CLA
#define BUCK_CONTROL_RUNNING_ON CLA_CORE
#endif


#define BUCK_LAB_OPEN_LOOP_VMC             1
#define BUCK_LAB_CLOSED_LOOP_VMC           2
#define BUCK_LAB_OPEN_LOOP_PCMC            3
#define BUCK_LAB_CLOSED_LOOP_PCMC          4

#define BUCK_CTL_MODE_VMC        1
#define BUCK_CTL_MODE_PCMC       2

#define BUCK_CTL_LOOP_OPEN       1
#define BUCK_CTL_LOOP_CLOSED     2

#if(BUCK_LAB_NUMBER == BUCK_LAB_OPEN_LOOP_VMC)
    #define BUCK_CTL_MODE            BUCK_CTL_MODE_VMC
    #define BUCK_CTL_LOOP            BUCK_CTL_LOOP_OPEN
#elif(BUCK_LAB_NUMBER == BUCK_LAB_CLOSED_LOOP_VMC)
    #define BUCK_CTL_MODE            BUCK_CTL_MODE_VMC
    #define BUCK_CTL_LOOP            BUCK_CTL_LOOP_CLOSED
#elif(BUCK_LAB_NUMBER == BUCK_LAB_OPEN_LOOP_PCMC)
    #define BUCK_CTL_MODE            BUCK_CTL_MODE_PCMC
    #define BUCK_CTL_LOOP            BUCK_CTL_LOOP_OPEN
#elif(BUCK_LAB_NUMBER == BUCK_LAB_CLOSED_LOOP_PCMC)
    #define BUCK_CTL_MODE            BUCK_CTL_MODE_PCMC
    #define BUCK_CTL_LOOP            BUCK_CTL_LOOP_CLOSED
#endif


//
// Device clocking conditions
//
#define BUCK_SYSCLK_HZ        ((float32_t)120 * 1000000)
#define BUCK_SYSCLK_NS        ((float32_t)1000000000 / BUCK_SYSCLK_HZ)
#define BUCK_EPWM_HZ          ((float32_t)120 * 1000000)
#define BUCK_EPWM_NS          ((float32_t)1000000000 / BUCK_EPWM_HZ)
#define BUCK_INTOSC_HZ        ((float32_t) 10 * 1000000)

//
// TASK configuration
//
#define TASKA_CPUTIMER_BASE CPUTIMER0_BASE
#define TASKB_CPUTIMER_BASE CPUTIMER1_BASE
#define TASKC_CPUTIMER_BASE CPUTIMER2_BASE

#define TASKA_FREQ_HZ         ((uint16_t)1000)
#define TASKB_FREQ_HZ         ((uint16_t) 100)
#define TASKC_FREQ_HZ         ((uint16_t)  10)

#define GET_TASKA_TIMER_OVERFLOW_STATUS CPUTimer_getTimerOverflowStatus(TASKA_CPUTIMER_BASE)
#define CLEAR_TASKA_TIMER_OVERFLOW_FLAG CPUTimer_clearOverflowFlag(TASKA_CPUTIMER_BASE)

#define GET_TASKB_TIMER_OVERFLOW_STATUS CPUTimer_getTimerOverflowStatus(TASKB_CPUTIMER_BASE)
#define CLEAR_TASKB_TIMER_OVERFLOW_FLAG CPUTimer_clearOverflowFlag(TASKB_CPUTIMER_BASE)

#define GET_TASKC_TIMER_OVERFLOW_STATUS CPUTimer_getTimerOverflowStatus(TASKC_CPUTIMER_BASE)
#define CLEAR_TASKC_TIMER_OVERFLOW_FLAG CPUTimer_clearOverflowFlag(TASKC_CPUTIMER_BASE)

//
// ADC configuration
//
#define BUCK_ADC_REF_MODE        ADC_REFERENCE_INTERNAL
#define BUCK_ADC_REF_V           ADC_REFERENCE_3_3V
#define BUCK_ADC_FSR_V           ((float32_t)3.3) // ADC full-scale range is 3.3V
#define BUCK_ADC_PRESCALER       ADC_CLK_DIV_2_0
#define BUCK_ADC_POWERUP_US      ((uint16_t)5000)

#define BUCK_ADC_CODES           ((uint16_t)4096) // 12b ADC with 4096 codes 0->4095
#define BUCK_CMPSSDAC_CODES      ((uint16_t)4096) // 12b DAC with 4096 codes 0->4095
#define BUCK_CMPSSDAC_FSR_V      ((float32_t)3.3) // DAC full-scale range is 3.3V

//
// Conversion factors: X_FACTOR_Y
// Multiply X value by conversion factor to arrive at Y units
//
// Example: BUCK_VOUT_V_FACTOR_PU
// Multiply Vout voltage by BUCK_VOUT_V_FACTOR_PU to arrive at per-unit value
//
#define BUCK_VIN_ADC_FACTOR_V    ((float32_t)BUCK_VIN_MAX_SENSE_V  / BUCK_ADC_CODES)
#define BUCK_VOUT_ADC_FACTOR_V   ((float32_t)BUCK_VOUT_MAX_SENSE_V / BUCK_ADC_CODES)
#define BUCK_IL_ADC_FACTOR_A     ((float32_t)BUCK_IL_MAX_SENSE_A   / BUCK_ADC_CODES)
#define BUCK_VOUT_ADC_FACTOR_PU  ((float32_t)1.0 / BUCK_ADC_CODES)
#define BUCK_VOUT_V_FACTOR_PU    ((float32_t)1.0 / BUCK_VOUT_MAX_SENSE_V)

//
// BUCK_AVG_ADC_SAMPLES - Number of ADC samples that are averaged for user
// inspection during program execution
//
#define BUCK_AVG_ADC_SAMPLES               ((uint16_t)  8)

//
// ADC configurations for sampling Vout, Vin, and ILFB_AVG
//
#if(BUCK_LAUNCHPAD_SITE == 1)
    #define BUCK_VOUT_ADC_MODULE               ADCB_BASE
    #define BUCK_VOUT_ADC_SOC_NO               ADC_SOC_NUMBER0
    #define BUCK_VOUT_ADC_PIN                  ADC_CH_ADCIN2
    #define BUCK_VOUT_ADC_ISR_TRIGGER          INT_ADCB1
    #define BUCK_VOUT_ADCRESULTREGBASE         ADCBRESULT_BASE
    #define BUCK_CLA_ISR_TRIG                  CLA_TRIGGER_ADCB1
#elif(BUCK_LAUNCHPAD_SITE == 2)
    #define BUCK_VOUT_ADC_MODULE               ADCA_BASE         //ADCC_BASE
    #define BUCK_VOUT_ADC_SOC_NO               ADC_SOC_NUMBER0
    #define BUCK_VOUT_ADC_PIN                  ADC_CH_ADCIN1  //ADC_CH_ADCIN3
     #define BUCK_VOUT_ADC_ISR_TRIGGER          INT_ADCA1
    #define BUCK_VOUT_ADCRESULTREGBASE         ADCARESULT_BASE   //      ADCACRESULT_BASE
    #define BUCK_CLA_ISR_TRIG                  CLA_TRIGGER_ADCC1
#endif
#define BUCK_VOUT_ADC_TRIG_SOURCE          BUCK_DRV_EPWM_ADC_TRIGGER
#define BUCK_VOUT_ADC_ACQPS_NS             ((uint16_t)100)
#define BUCK_VOUT_ADC_ACQPS_SYS_TICKS      ((uint16_t)(BUCK_VOUT_ADC_ACQPS_NS / BUCK_SYSCLK_NS))
#define BUCK_VOUT_ADC_INT_NUMBER           ADC_INT_NUMBER1
#define BUCK_VOUT_ADC_INT_MODE             ADC_PULSE_END_OF_ACQ_WIN
#define BUCK_VOUT_ADC_ISR_PIE_GROUP        INTERRUPT_ACK_GROUP1
#define BUCK_VOUT_ADC_ISR_FUNCTION         ISR1

#if(BUCK_LAUNCHPAD_SITE == 1)
    #define BUCK_VIN_ADC_MODULE                ADCC_BASE
    #define BUCK_VIN_ADC_SOC_NO                ADC_SOC_NUMBER0
    #define BUCK_VIN_ADC_PIN                   ADC_CH_ADCIN0
    #define BUCK_VIN_ADCRESULTREGBASE          ADCCRESULT_BASE
#elif(BUCK_LAUNCHPAD_SITE == 2)
    #define BUCK_VIN_ADC_MODULE                ADCC_BASE
    #define BUCK_VIN_ADC_SOC_NO                ADC_SOC_NUMBER1
    #define BUCK_VIN_ADC_PIN                   ADC_CH_ADCIN3  //ADC_CH_ADCIN5        C3 for Vdc_in
    #define BUCK_VIN_ADCRESULTREGBASE          ADCCRESULT_BASE
#endif
#define BUCK_VIN_ADC_TRIG_SOURCE           BUCK_DRV_EPWM_ADC_TRIGGER
#define BUCK_VIN_ADC_ACQPS_NS              ((uint16_t)100)
#define BUCK_VIN_ADC_ACQPS_SYS_TICKS       ((uint16_t)(BUCK_VIN_ADC_ACQPS_NS / BUCK_SYSCLK_NS))

#if(BUCK_LAUNCHPAD_SITE == 1)
    #define BUCK_IL_FILT_ADC_MODULE            ADCB_BASE
    #define BUCK_IL_FILT_ADC_SOC_NO            ADC_SOC_NUMBER1
    #define BUCK_IL_FILT_ADC_PIN               ADC_CH_ADCIN0
    #define BUCK_IL_FILT_ADCRESULTREGBASE      ADCBRESULT_BASE
#elif(BUCK_LAUNCHPAD_SITE == 2)
    #define BUCK_IL_FILT_ADC_MODULE            ADCA_BASE
    #define BUCK_IL_FILT_ADC_SOC_NO            ADC_SOC_NUMBER1
    #define BUCK_IL_FILT_ADC_PIN               ADC_CH_ADCIN15
    #define BUCK_IL_FILT_ADCRESULTREGBASE      ADCARESULT_BASE
#endif
#define BUCK_IL_FILT_ADC_TRIG_SOURCE       BUCK_DRV_EPWM_ADC_TRIGGER
#define BUCK_IL_FILT_ADC_ACQPS_NS          ((uint16_t)100)
#define BUCK_IL_FILT_ADC_ACQPS_SYS_TICKS   ((uint16_t)(BUCK_IL_FILT_ADC_ACQPS_NS / BUCK_SYSCLK_NS))


//
// CMPSS configuration for detecting over-current trip on ILFB
//
#define BUCK_CMPSS_COMP_HI                 1
#define BUCK_CMPSS_COMP_LO                 2

#if(BUCK_LAUNCHPAD_SITE == 1)
    #define BUCK_OC_CMPSS_BASE                 CMPSS6_BASE
    #define BUCK_OC_CMPSS_CMP_HL               BUCK_CMPSS_COMP_LO
    #define BUCK_OC_CMPSS_ASYSCTRL_CMPMUX      ASYSCTL_CMPLPMUX_SELECT_6
    #define BUCK_OC_CMPSS_ASYSCTRL_MUX_VALUE   ((uint16_t)3)
    #define BUCK_OC_CMPSS_XBAR_MUX             XBAR_MUX11
    #define BUCK_OC_CMPSS_XBAR_MUX_VAL         XBAR_EPWM_MUX11_CMPSS6_CTRIPL
    #define BUCK_OC_CMPSS_XBAR_FLAG            XBAR_INPUT_FLG_CMPSS6_CTRIPL
#elif(BUCK_LAUNCHPAD_SITE == 2)
    #define BUCK_OC_CMPSS_BASE                 CMPSS1_BASE
    #define BUCK_OC_CMPSS_CMP_HL               BUCK_CMPSS_COMP_LO
    #define BUCK_OC_CMPSS_ASYSCTRL_CMPMUX      ASYSCTL_CMPLPMUX_SELECT_1
    #define BUCK_OC_CMPSS_ASYSCTRL_MUX_VALUE   ((uint16_t)3)
    #define BUCK_OC_CMPSS_XBAR_MUX             XBAR_MUX01
    #define BUCK_OC_CMPSS_XBAR_MUX_VAL         XBAR_EPWM_MUX01_CMPSS1_CTRIPL
    #define BUCK_OC_CMPSS_XBAR_FLAG            XBAR_INPUT_FLG_CMPSS1_CTRIPL
#endif
#define BUCK_OC_CMPSS_XBAR_TRIP            XBAR_TRIP4
#define BUCK_OC_CMPSS_HYSTERESIS_FACTOR    ((uint16_t)2)

#define BUCK_OC_CMPSS_FILTER_ENABLED       true
#if(BUCK_OC_CMPSS_FILTER_ENABLED == true)
    #define BUCK_OC_CMPSS_FILTER_SAMPWIN       ((uint16_t)24)
    #define BUCK_OC_CMPSS_FILTER_THRESH        ((uint16_t)23)

    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
        #define BUCK_OC_CMPSS_FILTER_PRESCALE      ((uint16_t)0)
    #elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        #define BUCK_OC_CMPSS_FILTER_PRESCALE      ((uint16_t)2)
    #endif
#endif

#if(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
    #if(BUCK_LAUNCHPAD_SITE == 1)
        #define BUCK_PCMC_CMPSS_BASE               CMPSS6_BASE
        #define BUCK_PCMC_CMPSS_ASYSCTRL_CMPMUX    ASYSCTL_CMPHPMUX_SELECT_6
        #define BUCK_PCMC_CMPSS_ASYSCTRL_MUX_VALUE ((uint16_t)3)
        #define BUCK_PCMC_CMPSS_XBAR_MUX           XBAR_MUX10
        #define BUCK_PCMC_CMPSS_XBAR_MUX_VAL       XBAR_EPWM_MUX10_CMPSS6_CTRIPH
        #define BUCK_PCMC_CMPSS_XBAR_FLAG          XBAR_INPUT_FLG_CMPSS6_CTRIPH
    #elif(BUCK_LAUNCHPAD_SITE == 2)
        #define BUCK_PCMC_CMPSS_BASE               CMPSS1_BASE
        #define BUCK_PCMC_CMPSS_ASYSCTRL_CMPMUX    ASYSCTL_CMPHPMUX_SELECT_1
        #define BUCK_PCMC_CMPSS_ASYSCTRL_MUX_VALUE ((uint16_t)3)
        #define BUCK_PCMC_CMPSS_XBAR_MUX           XBAR_MUX00
        #define BUCK_PCMC_CMPSS_XBAR_MUX_VAL       XBAR_EPWM_MUX00_CMPSS1_CTRIPH
        #define BUCK_PCMC_CMPSS_XBAR_FLAG          XBAR_INPUT_FLG_CMPSS1_CTRIPH
    #endif
    #define BUCK_PCMC_CMPSS_RAMPLOADSEL        true
    #define BUCK_PCMC_CMPSS_XBAR_TRIP          XBAR_TRIP5
    #define BUCK_PCMC_CMPSS_HYSTERESIS_FACTOR  ((uint16_t)2)
    #define BUCK_PCMC_CMPSS_RAMPMAX_MAX        ((uint16_t)((uint32_t)1 << 16) - 1)
    #define BUCK_PCMC_CMPSS_RAMPMAX_DEFAULT    ((uint16_t)0)
    #define BUCK_PCMC_CMPSS_DELAY              ((uint16_t)0)

    #define BUCK_PCMC_CMPSS_FILTER_ENABLED     false
    #if(BUCK_PCMC_CMPSS_FILTER_ENABLED == true)
        #define BUCK_PCMC_CMPSS_FILTER_PRESCALE    ((uint16_t)0)
        #define BUCK_PCMC_CMPSS_FILTER_SAMPWIN     ((uint16_t)5)
        #define BUCK_PCMC_CMPSS_FILTER_THRESH      ((uint16_t)4)
    #endif
#endif

//
// Synchronous buck ePWM configuration
//
#if(BUCK_LAUNCHPAD_SITE == 1)
    #define BUCK_DRV_EPWM_BASE                 EPWM6_BASE
    #define BUCK_DRV_EPWM_NUM                  ((uint16_t)6)
    #define BUCK_DRV_EPWM_H_GPIO               ((uint16_t)10)
    #define BUCK_DRV_EPWM_H_PIN_CONFIG_EPWM    GPIO_10_EPWM6A
    #define BUCK_DRV_EPWM_H_PIN_CONFIG_GPIO    GPIO_10_GPIO10
    #define BUCK_DRV_EPWM_L_GPIO               ((uint16_t)11)
    #define BUCK_DRV_EPWM_L_PIN_CONFIG_EPWM    GPIO_11_EPWM6B
    #define BUCK_DRV_EPWM_L_PIN_CONFIG_GPIO    GPIO_11_GPIO11
    #define BUCK_DRV_EPWM_ADC_TRIGGER          ADC_TRIGGER_EPWM6_SOCA
#elif(BUCK_LAUNCHPAD_SITE == 2)
    #define BUCK_DRV_EPWM_BASE                 EPWM1_BASE
    #define BUCK_DRV_EPWM_NUM                  ((uint16_t)1)
    #define BUCK_DRV_EPWM_H_GPIO               ((uint16_t)0)
    #define BUCK_DRV_EPWM_H_PIN_CONFIG_EPWM    GPIO_0_EPWM1_A       //GPIO_0_EPWM1A
    #define BUCK_DRV_EPWM_H_PIN_CONFIG_GPIO    GPIO_0_GPIO0
    #define BUCK_DRV_EPWM_L_GPIO               ((uint16_t)1)
    #define BUCK_DRV_EPWM_L_PIN_CONFIG_EPWM    GPIO_1_EPWM1_B      // GPIO_1_EPWM1B
    #define BUCK_DRV_EPWM_L_PIN_CONFIG_GPIO    GPIO_1_GPIO1
    #define BUCK_DRV_EPWM_ADC_TRIGGER          ADC_TRIGGER_EPWM1_SOCA
#endif
#define BUCK_DRV_EPWM_EPWMCLK_DIV          EPWM_CLOCK_DIVIDER_1
#define BUCK_DRV_EPWM_HSCLK_DIV            EPWM_HSCLOCK_DIVIDER_1
#if(BUCK_DRV_EPWM_HSCLK_DIV == EPWM_HSCLOCK_DIVIDER_1)
    #define BUCK_DRV_EPWM_TOTAL_CLKDIV     ((uint16_t)0x1 << BUCK_DRV_EPWM_EPWMCLK_DIV)
#else
    #define BUCK_DRV_EPWM_TOTAL_CLKDIV     (((uint16_t)0x1 << BUCK_DRV_EPWM_EPWMCLK_DIV) * (BUCK_DRV_EPWM_HSCLK_DIV << 1))
#endif
#define BUCK_DRV_EPWM_PERIOD_TICKS         ((uint32_t)(BUCK_EPWM_HZ / BUCK_DRV_EPWM_SWITCHING_FREQUENCY / BUCK_DRV_EPWM_TOTAL_CLKDIV))
#define BUCK_DRV_EPWM_TBPRD                ((uint32_t)BUCK_DRV_EPWM_PERIOD_TICKS - 1)
#define BUCK_DRV_EPWM_PERIOD_SEC           ((uint32_t)BUCK_DRV_EPWM_PERIOD_TICKS / BUCK_EPWM_HZ)
#define BUCK_DRV_EPWM_DEADBAND_RED         ((uint16_t)(150 / BUCK_EPWM_NS))
#define BUCK_DRV_EPWM_DEADBAND_FED         ((uint16_t)(150 / BUCK_EPWM_NS))
#define BUCK_DRV_EPWM_DC_TRIP_OC           EPWM_DC_TRIP_TRIPIN4
#define BUCK_DRV_EPWM_DC_TRIP_PCMC         EPWM_DC_TRIP_TRIPIN5

//
// DC blanking to reject switching noise
//
#define BUCK_DRV_EPWM_DC_BLANK_ENABLED     true

#if(BUCK_DRV_EPWM_DC_BLANK_ENABLED == true)
    //
    //  VMC: Filter acts upon Over Current trip signal
    // PCMC: Filter acts upon Peak Current trip signal
    //
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
        #define BUCK_DRV_EPWM_DC_BLANK_SOURCE      EPWM_DC_WINDOW_SOURCE_DCAEVT1
    #else
        #define BUCK_DRV_EPWM_DC_BLANK_SOURCE      EPWM_DC_WINDOW_SOURCE_DCAEVT2
    #endif
    #define BUCK_DRV_EPWM_DC_BLANK_PULSE       EPWM_DC_WINDOW_START_TBCTR_ZERO
    #define BUCK_DRV_EPWM_DC_BLANK_EARLY       ((uint16_t)5)
    #define BUCK_DRV_EPWM_DC_BLANK_OFFSET      ((uint16_t)BUCK_DRV_EPWM_TBPRD - BUCK_DRV_EPWM_DC_BLANK_EARLY)
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
        #define BUCK_DRV_EPWM_DC_BLANK_LENGTH      ((uint16_t)(190 / BUCK_EPWM_NS) + BUCK_DRV_EPWM_DC_BLANK_EARLY)
    #elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        #define BUCK_DRV_EPWM_DC_BLANK_LENGTH      ((uint16_t)(  0 / BUCK_EPWM_NS) + BUCK_DRV_EPWM_DC_BLANK_EARLY)
    #endif
#endif

#ifdef BUCK_CONTROL_RUNNING_ON_CPU
#define BUCK_DRV_EPWM_HR_ENABLED           true
#else
#define BUCK_DRV_EPWM_HR_ENABLED           false
#endif
#if(BUCK_DRV_EPWM_HR_ENABLED == true)
    #define BUCK_DRV_EPWM_CMPAHR_BITS          8
    #define BUCK_DRV_EPWM_CMPAHR_SCALE         ((uint16_t)0x1 << BUCK_DRV_EPWM_CMPAHR_BITS)
#endif

//
// BUCK_DRV_EPWM_UPDATE_LATENCY_TICKS - Estimated number of ePWM ticks
// between the ADC trigger and ePWM TBPRD when duty cycle is shadow loaded.
// Validate value by inspecting BUCK_isrDutyUpdateMinMargin_ticks.
//
#define BUCK_DRV_EPWM_UPDATE_LATENCY_TICKS ((uint16_t)170)
#define BUCK_DRV_EPWM_ADC_TRIGGER_TBCTR    (BUCK_DRV_EPWM_PERIOD_TICKS - BUCK_DRV_EPWM_UPDATE_LATENCY_TICKS)

//
// Active load ePWM configuration
//
#if(BUCK_LAUNCHPAD_SITE == 1)
    #define BUCK_LOAD_EPWM_BASE                EPWM3_BASE
    #define BUCK_LOAD_EPWM_GPIO                ((uint16_t)4)
    #define BUCK_LOAD_EPWM_PIN_CONFIG_EPWM     GPIO_4_EPWM3A
    #define BUCK_LOAD_EPWM_PIN_CONFIG_GPIO     GPIO_4_GPIO4
#elif(BUCK_LAUNCHPAD_SITE == 2)
    #define BUCK_LOAD_EPWM_BASE                EPWM2_BASE
    #define BUCK_LOAD_EPWM_GPIO                ((uint16_t)2)
    #define BUCK_LOAD_EPWM_PIN_CONFIG_EPWM     GPIO_2_EPWM2_A  // GPIO_2_EPWM2A
    #define BUCK_LOAD_EPWM_PIN_CONFIG_GPIO     GPIO_2_GPIO2
#endif
#define BUCK_LOAD_EPWM_EPWMCLK_DIV         EPWM_CLOCK_DIVIDER_4
#define BUCK_LOAD_EPWM_HSCLK_DIV           EPWM_HSCLOCK_DIVIDER_4
#define BUCK_LOAD_EPWM_PERIOD_TICKS        ((uint32_t)35000)
#define BUCK_LOAD_EPWM_CMPA_DISABLED       ((uint16_t)0)
#define BUCK_LOAD_EPWM_CMPA_DISCONTINUOUS  (BUCK_LOAD_EPWM_PERIOD_TICKS / 2)
#define BUCK_LOAD_EPWM_CMPA_CONTINUOUS     BUCK_LOAD_EPWM_PERIOD_TICKS

//
// Control Configuration
//
#define BUCK_DCL_FPU32                     1
#define BUCK_DCL_CLA                       2
#define BUCK_DCL_CORE                      BUCK_DCL_FPU32
#define BUCK_DCL_PI                        CONST_PI_32

#ifndef __TMS320C28XX_CLA__

    #define BUCK_DCL_CTRL_TYPE             DCL_DF22

    #define BUCK_DCL_CTRL_DEFAULTS         DF22_DEFAULTS
    #define BUCK_DCL_SPS_TYPE              DCL_DF22_SPS
    #define BUCK_DCL_SPS_DEFAULTS          DF22_SPS_DEFAULTS
    #define BUCK_DCL_RESET                 DCL_resetDF22
    #define BUCK_DCL_LOAD_PID              DCL_loadDF22asSeriesPID
    #define BUCK_DCL_LOAD_ZPK              DCL_loadDF22asZPK
    #define BUCK_DCL_UPDATE                DCL_updateDF22

    #define BUCK_DCL_RUN_IMMEDIATE     DCL_runDF22_C5
    #define BUCK_DCL_RUN_PARTIAL       DCL_runDF22_C6
    #define BUCK_DCL_RUN_CLAMP         DCL_runClamp_C2
#else
    //
//#####BEGIN_INTERNAL#####
//todo Figure out how to replace the FPU32 equivalents of _RESET, _LOAD_PID, _LOAD_ZPK, _UPDATE
//#####END_INTERNAL#####
    // NOTE: the above are still used in the CLA case. Just that they get defined out when compiling
    // the .cla file since they are not visible to it.
    //
    #define BUCK_DCL_CTRL_TYPE         DCL_DF22_CLA

    #define BUCK_DCL_RUN_IMMEDIATE     DCL_runDF22_L2
    #define BUCK_DCL_RUN_PARTIAL       DCL_runDF22_L3
    #define BUCK_DCL_RUN_CLAMP         DCL_runClamp_L1

#endif

//
// DCL TCM Vout datalog configuration
//
#define BUCK_VOUT_LOG_LEAD_SIZE            ((uint16_t)100)
#define BUCK_VOUT_LOG_CAP_SIZE             ((uint16_t)300)
#define BUCK_VOUT_LOG_SIZE                 BUCK_VOUT_LOG_LEAD_SIZE + BUCK_VOUT_LOG_CAP_SIZE
#define BUCK_VOUT_LOG_OVER_V               ((float32_t)0.035)
#define BUCK_VOUT_LOG_UNDER_V              ((float32_t)0.035)

//
// SFRA configuration
//
#ifdef BUCK_CONTROL_RUNNING_ON_CPU
    #define BUCK_SFRA_ENABLED              true
#else
    #define BUCK_SFRA_ENABLED              false
#endif

//#undef BUCK_SFRA_ENABLED
//#define BUCK_SFRA_ENABLED false

#if(BUCK_SFRA_ENABLED == true)
    #define BUCK_SFRA_ISR_FREQ             BUCK_DRV_EPWM_SWITCHING_FREQUENCY
    #define BUCK_SFRA_FREQ_START           ((float32_t)100)

    //
    // SFRA step Multiply = 10^(1/No of steps per decade(35))
    //
    #define BUCK_SFRA_FREQ_STEP_MULTIPLY   ((float32_t)1.0680004)
    #define BUCK_SFRA_FREQ_LENGTH          ((int16_t)100)
    #define BUCK_SFRA_AMPLITUDE            ((float32_t)0.005)
    #define BUCK_SFRA_SWEEP_SPEED          ((int16_t)1)

    #define BUCK_SFRA_GUI_SCI_BASE         SCIA_BASE
    #define BUCK_SFRA_GUI_SCI_BAUDRATE     ((uint32_t)57600)
    #define BUCK_SFRA_GUI_SCIRX_GPIO       ((uint16_t)28)
    #define BUCK_SFRA_GUI_SCIRX_PIN_CONFIG GPIO_28_SCIA_RX //GPIO_28_SCIRXDA
    #define BUCK_SFRA_GUI_SCITX_GPIO       ((uint16_t)29)
    #define BUCK_SFRA_GUI_SCITX_PIN_CONFIG GPIO_29_SCIA_TX //GPIO_29_SCITXDA

    #define BUCK_SFRA_GUI_LED_ENABLE       ((uint16_t)1)
    #define BUCK_SFRA_GUI_LED_GPIO         ((uint16_t)34)
    #define BUCK_SFRA_GUI_LED_PIN_CONFIG   GPIO_34_GPIO34
    #define BUCK_SFRA_GUI_PLOT_OPTION      ((uint16_t)1)
#endif

//
// Range limits for actuation
//
#define BUCK_DUTY_SET_MIN_PU               ((float32_t)0.00)
#define BUCK_DUTY_SET_MAX_PU               ((float32_t)0.25)  //0.25
#define BUCK_DAC_SET_MIN_PU                ((float32_t)0.00)
#define BUCK_DAC_SET_MAX_PU                ((float32_t)1.00)

#if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
    #define BUCK_UK_MIN_PU                     BUCK_DUTY_SET_MIN_PU
    #define BUCK_UK_MAX_PU                     BUCK_DUTY_SET_MAX_PU
#elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
    #define BUCK_UK_MIN_PU                     BUCK_DAC_SET_MIN_PU
    #define BUCK_UK_MAX_PU                     BUCK_DAC_SET_MAX_PU
#endif

//
// Minimum Vin threshold required to enable Vout regulation
//
#define BUCK_VOUT_ENABLE_VIN_THRESH        ((float32_t)0.8 * BUCK_VIN_NOM_V)

//
// Slew (virtual dampening) of change in Duty, Vout, or CMPSS
//
#define BUCK_DUTY_SLEW_MAX_PU              ((float32_t)0.01)
#define BUCK_VOUT_SLEW_MAX_PU              ((float32_t)0.01)
#define BUCK_DAC_SLEW_MAX_PU               ((float32_t)0.01)

//
// Default values for run-time configurable user variables
//
#if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
    #define BUCK_DCL_DEFAULT_FORMAT            Format_ZPK

    #define BUCK_DCL_DEFAULT_PID_SCALE         ((float32_t) 10.0)
    #define BUCK_DCL_DEFAULT_KP                ((float32_t) 16.5)
    #define BUCK_DCL_DEFAULT_KI                ((float32_t)  1.3)
    #define BUCK_DCL_DEFAULT_KD                ((float32_t)    0)
    #define BUCK_DCL_DEFAULT_BW_HZ             ((float32_t)10000)

    #define BUCK_DCL_DEFAULT_KDC               BUCK_DCL_COMPDES_KDC
    #define BUCK_DCL_DEFAULT_Z0                BUCK_DCL_COMPDES_Z0
    #define BUCK_DCL_DEFAULT_Z1                BUCK_DCL_COMPDES_Z1
    #define BUCK_DCL_DEFAULT_P1                BUCK_DCL_COMPDES_P1

    #define BUCK_DCL_DEFAULT_B0                BUCK_DCL_COMPDES_B0
    #define BUCK_DCL_DEFAULT_B1                BUCK_DCL_COMPDES_B1
    #define BUCK_DCL_DEFAULT_B2                BUCK_DCL_COMPDES_B2
    #define BUCK_DCL_DEFAULT_A1                BUCK_DCL_COMPDES_A1
    #define BUCK_DCL_DEFAULT_A2                BUCK_DCL_COMPDES_A2

    #define BUCK_USER_DEFAULT_LOAD_ENABLE      0
    #define BUCK_USER_DEFAULT_LOAD_CONT        0
#elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
    #define BUCK_DCL_DEFAULT_FORMAT            Format_PID

    #define BUCK_DCL_DEFAULT_PID_SCALE         ((float32_t) 10.0)
    #define BUCK_DCL_DEFAULT_KP                ((float32_t) 45.0)
    #define BUCK_DCL_DEFAULT_KI                ((float32_t)  4.5)
    #define BUCK_DCL_DEFAULT_KD                ((float32_t)    0)
    #define BUCK_DCL_DEFAULT_BW_HZ             ((float32_t)10000)

    #define BUCK_DCL_DEFAULT_KDC               ((float32_t)19.5)
    #define BUCK_DCL_DEFAULT_Z0                ((float32_t) 0.0001)
    #define BUCK_DCL_DEFAULT_Z1                ((float32_t)25.0)
    #define BUCK_DCL_DEFAULT_P1                ((float32_t) 3.0)

    #define BUCK_DCL_DEFAULT_B0                ((float32_t) 6.52500010)
    #define BUCK_DCL_DEFAULT_B1                ((float32_t)-9.25339317)
    #define BUCK_DCL_DEFAULT_B2                ((float32_t) 3.27820277)
    #define BUCK_DCL_DEFAULT_A1                ((float32_t)-1.72848952)
    #define BUCK_DCL_DEFAULT_A2                ((float32_t) 0.72848952)

    #define BUCK_USER_DEFAULT_LOAD_ENABLE      1
    #define BUCK_USER_DEFAULT_LOAD_CONT        1
#endif

#define BUCK_USER_DEFAULT_TRIP_A           ((float32_t)60)        //BUCK_ILOAD_NOM_A * 4)

//
// Heart beat LED on board
//
#define BUCK_RUN_LED_GPIO                  23
#define BUCK_RUN_LED_PIN_CONFIG            GPIO_23_GPIO23
#define BUCK_RUN_LED_PRESCALE              ((uint16_t)5)


#define EN91_GPIO                  7
#define EN91_GPIO_PIN_CONFIG            GPIO_7_GPIO7

#ifdef __cplusplus
}
#endif                                  /* extern "C" */

#endif //_BUCK_USERSETTINGS_H
