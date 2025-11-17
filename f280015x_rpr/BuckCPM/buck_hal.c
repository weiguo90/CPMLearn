//#############################################################################
//
// FILE:  buck_hal.c
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

#include <buck_hal.h>

//
//=============================================================================
// variables
//=============================================================================
//
#if(BUCK_DRV_EPWM_HR_ENABLED == true)
    volatile uint32_t ePWM[9] =
                        {0, EPWM1_BASE, EPWM2_BASE, EPWM3_BASE, EPWM4_BASE,
                            EPWM5_BASE, EPWM6_BASE, EPWM7_BASE}; //, EPWM8_BASE};
#endif

//
// BUCK_HAL_setupDevice - Initialize device after boot and configure system
// resources (such as PLL) and moving ramfuncs from FLASH to RAM
//
void BUCK_HAL_setupDevice(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pull-ups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Initialize CPU timers for task scheduling
    //

    //
    // TASK A FREQUENCY
    //
    CPUTimer_setPeriod(TASKA_CPUTIMER_BASE, DEVICE_SYSCLK_FREQ / TASKA_FREQ_HZ);

    //
    // TASK B FREQUENCY
    //
    CPUTimer_setPeriod(TASKB_CPUTIMER_BASE, DEVICE_SYSCLK_FREQ / TASKB_FREQ_HZ);

    //
    // TASK C FREQUENCY
    //
    CPUTimer_setPeriod(TASKC_CPUTIMER_BASE, DEVICE_SYSCLK_FREQ / TASKC_FREQ_HZ);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(TASKA_CPUTIMER_BASE, 0);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(TASKB_CPUTIMER_BASE, 0);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(TASKC_CPUTIMER_BASE, 0);

    //
    // Make sure timer is stopped
    //
    CPUTimer_stopTimer(TASKA_CPUTIMER_BASE);
    CPUTimer_stopTimer(TASKB_CPUTIMER_BASE);
    CPUTimer_stopTimer(TASKC_CPUTIMER_BASE);
    CPUTimer_setEmulationMode(TASKA_CPUTIMER_BASE,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_setEmulationMode(TASKB_CPUTIMER_BASE,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_setEmulationMode(TASKC_CPUTIMER_BASE,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);

    //
    // Reload and start all counter register with period value
    //
    CPUTimer_startTimer(TASKA_CPUTIMER_BASE);
    CPUTimer_startTimer(TASKB_CPUTIMER_BASE);
    CPUTimer_startTimer(TASKC_CPUTIMER_BASE);
}

void BUCK_HAL_setupAdc(void)
{
    //
    // Enable all ADCs
    //
    ADC_setVREF(ADCA_BASE, BUCK_ADC_REF_MODE, BUCK_ADC_REF_V);
//    ADC_setVREF(ADCB_BASE, BUCK_ADC_REF_MODE, BUCK_ADC_REF_V);
    ADC_setVREF(ADCC_BASE, BUCK_ADC_REF_MODE, BUCK_ADC_REF_V);

    ADC_setPrescaler(ADCA_BASE, BUCK_ADC_PRESCALER);
//    ADC_setPrescaler(ADCB_BASE, BUCK_ADC_PRESCALER);
    ADC_setPrescaler(ADCC_BASE, BUCK_ADC_PRESCALER);

    ADC_enableConverter(ADCA_BASE);
//    ADC_enableConverter(ADCB_BASE);
    ADC_enableConverter(ADCC_BASE);

    DEVICE_DELAY_US(BUCK_ADC_POWERUP_US);

    //
    // ADC for measuring Vin Feedback
    //
    ADC_setupSOC(BUCK_VIN_ADC_MODULE,
                 BUCK_VIN_ADC_SOC_NO,
                 BUCK_VIN_ADC_TRIG_SOURCE,
                 BUCK_VIN_ADC_PIN,
                 BUCK_VIN_ADC_ACQPS_SYS_TICKS);

    //
    // ADC for measuring Vout Feedback
    //
    ADC_setupSOC(BUCK_VOUT_ADC_MODULE,
                 BUCK_VOUT_ADC_SOC_NO,
                 BUCK_VOUT_ADC_TRIG_SOURCE,
                 BUCK_VOUT_ADC_PIN,
                 BUCK_VOUT_ADC_ACQPS_SYS_TICKS);

    //
    // ADC for measuring Filtered IL Feedback
    //
    ADC_setupSOC(BUCK_IL_FILT_ADC_MODULE,
                 BUCK_IL_FILT_ADC_SOC_NO,
                 BUCK_IL_FILT_ADC_TRIG_SOURCE,
                 BUCK_IL_FILT_ADC_PIN,
                 BUCK_IL_FILT_ADC_ACQPS_SYS_TICKS);
}

void BUCK_HAL_setupAdcTrigger(void)
{
    //
    // Configure TBCTR=CMPB to generate SOCA ADC trigger
    //

    EPWM_setADCTriggerSource(BUCK_DRV_EPWM_BASE,
                             EPWM_SOC_A,
                             EPWM_SOC_TBCTR_U_CMPB);
    EPWM_setADCTriggerEventPrescale(BUCK_DRV_EPWM_BASE,
                                    EPWM_SOC_A,
                                    BUCK_ISR_CTRL_LOOP_RATIO);
    EPWM_setCounterCompareValue(BUCK_DRV_EPWM_BASE,
                                EPWM_COUNTER_COMPARE_B,
                                BUCK_DRV_EPWM_ADC_TRIGGER_TBCTR);
    EPWM_enableADCTrigger(BUCK_DRV_EPWM_BASE,
                          EPWM_SOC_A);
}

//
// BUCK_HAL_setupSyncBuckPinsGpio - Configure synchronous buck pins
// to be controlled by GPIO module with a static low output
//
void BUCK_HAL_setupSyncBuckPinsGpio(void)
{
    GPIO_writePin(BUCK_DRV_EPWM_H_GPIO, 0);
    GPIO_setDirectionMode(BUCK_DRV_EPWM_H_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(BUCK_DRV_EPWM_H_PIN_CONFIG_GPIO);

    GPIO_writePin(BUCK_DRV_EPWM_L_GPIO, 0);
    GPIO_setDirectionMode(BUCK_DRV_EPWM_L_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(BUCK_DRV_EPWM_L_PIN_CONFIG_GPIO);
}

//
// BUCK_HAL_setupSyncBuckPinsEpwm - Configure synchronous buck pins
// to be controlled by ePWM module
//
void BUCK_HAL_setupSyncBuckPinsEpwm(void)
{
    GPIO_setPinConfig(BUCK_DRV_EPWM_H_PIN_CONFIG_EPWM);
//    GPIO_setPinConfig(BUCK_DRV_EPWM_L_PIN_CONFIG_EPWM);
}

// Configure low-side pin as GPIO and drive low (disable synchronous rectification)
void BUCK_HAL_setLowSideGpio(void)
{
    GPIO_writePin(BUCK_DRV_EPWM_L_GPIO, 0);
    GPIO_setDirectionMode(BUCK_DRV_EPWM_L_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(BUCK_DRV_EPWM_L_PIN_CONFIG_GPIO);
}

//
// BUCK_HAL_setupSyncBuckPwm - Configure ePWM to actuate complementary
// Buck switches with variable duty-cycle control and deadband
//
void BUCK_HAL_setupSyncBuckPwm(void)
{
    //
    // Maximum supported ePWM clock speed is specified in the datasheet
    //
    EPWM_setClockPrescaler(BUCK_DRV_EPWM_BASE,
                           BUCK_DRV_EPWM_EPWMCLK_DIV,
                           BUCK_DRV_EPWM_HSCLK_DIV);

    EPWM_setEmulationMode(BUCK_DRV_EPWM_BASE, EPWM_EMULATION_FREE_RUN);

    //
    // Configure ePWM for count-up operation
    //
    EPWM_setTimeBaseCounter(BUCK_DRV_EPWM_BASE, 0);
    EPWM_setTimeBasePeriod(BUCK_DRV_EPWM_BASE, BUCK_DRV_EPWM_TBPRD);
    EPWM_setPeriodLoadMode(BUCK_DRV_EPWM_BASE, EPWM_PERIOD_SHADOW_LOAD);
    EPWM_setTimeBaseCounterMode(BUCK_DRV_EPWM_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_disablePhaseShiftLoad(BUCK_DRV_EPWM_BASE);

    //
    // Use shadow mode to update CMPA on TBPRD
    //
    EPWM_setCounterCompareValue(BUCK_DRV_EPWM_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                0);
    EPWM_setCounterCompareShadowLoadMode(BUCK_DRV_EPWM_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_PERIOD);

    #if(BUCK_DRV_EPWM_HR_ENABLED == true)
        //
        // Align HRPWM CMPA value and shadow mode with EPWM
        //
        HRPWM_setCounterCompareValue(BUCK_DRV_EPWM_BASE,
                                     HRPWM_COUNTER_COMPARE_A,
                                     0);
        HRPWM_setCounterCompareShadowLoadEvent(BUCK_DRV_EPWM_BASE,
                                               HRPWM_CHANNEL_A,
                                               HRPWM_LOAD_ON_CNTR_PERIOD);
    #endif

    //
    // Configure Action Qualifier SubModule to:
    //     Output High when TBCTR=0
    //     Output  Low when TBCTR=CMPA (for VMC only)
    //
    // Use shadow mode to update AQCTL
    //
    EPWM_setActionQualifierShadowLoadMode(BUCK_DRV_EPWM_BASE,
                                          EPWM_ACTION_QUALIFIER_A,
                                          EPWM_AQ_LOAD_ON_CNTR_PERIOD);
    EPWM_setActionQualifierAction(BUCK_DRV_EPWM_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);

    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
        EPWM_setActionQualifierAction(BUCK_DRV_EPWM_BASE,
                                      EPWM_AQ_OUTPUT_A,
                                      EPWM_AQ_OUTPUT_LOW,
                                      EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
        #if(BUCK_DRV_EPWM_HR_ENABLED == true)
            //
            // Align the HRPWM CMPA behavior with EPWM
            //
            HRPWM_setMEPEdgeSelect(BUCK_DRV_EPWM_BASE,
                                   HRPWM_CHANNEL_A,
                                   HRPWM_MEP_CTRL_FALLING_EDGE);

            HRPWM_setMEPControlMode(BUCK_DRV_EPWM_BASE,
                                    HRPWM_CHANNEL_A,
                                    HRPWM_MEP_DUTY_PERIOD_CTRL);
        #endif
    #endif

    //
    // Configure Dead Band Generator for active high complementary PWMs
    //
    EPWM_setDeadBandDelayMode(BUCK_DRV_EPWM_BASE, EPWM_DB_RED, true);
    EPWM_setDeadBandDelayMode(BUCK_DRV_EPWM_BASE, EPWM_DB_FED, true);

    EPWM_setRisingEdgeDeadBandDelayInput(BUCK_DRV_EPWM_BASE,
                                         EPWM_DB_INPUT_EPWMA);
    EPWM_setFallingEdgeDeadBandDelayInput(BUCK_DRV_EPWM_BASE,
                                          EPWM_DB_INPUT_EPWMA);

    EPWM_setDeadBandDelayPolarity(BUCK_DRV_EPWM_BASE,
                                  EPWM_DB_FED,
                                  EPWM_DB_POLARITY_ACTIVE_LOW);
    EPWM_setDeadBandDelayPolarity(BUCK_DRV_EPWM_BASE,
                                  EPWM_DB_RED,
                                  EPWM_DB_POLARITY_ACTIVE_HIGH);

    EPWM_setFallingEdgeDelayCount(BUCK_DRV_EPWM_BASE,
                                  BUCK_DRV_EPWM_DEADBAND_FED);
    EPWM_setRisingEdgeDelayCount(BUCK_DRV_EPWM_BASE,
                                 BUCK_DRV_EPWM_DEADBAND_RED);

    #if(BUCK_DRV_EPWM_HR_ENABLED == true)
        //
        // HRCAL uses the EPWM1 clock
        //
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM1);

        HRPWM_enableAutoConversion(BUCK_DRV_EPWM_BASE);
    #endif
}

//
// BUCK_HAL_setupSyncBuckBlankingWindow - Use ePWM blanking window (filter) to:
//     A) Avoid boundary conditions originating from comparator trips arriving
//        near the end of ePWM cycles, and
//     B) Switching noise at the start of ePWM cycles (VMC)
//
void BUCK_HAL_setupSyncBuckBlankingWindow(void)
{
    EPWM_setDigitalCompareFilterInput(BUCK_DRV_EPWM_BASE,
                                      BUCK_DRV_EPWM_DC_BLANK_SOURCE);
    EPWM_setDigitalCompareBlankingEvent(BUCK_DRV_EPWM_BASE,
                                        BUCK_DRV_EPWM_DC_BLANK_PULSE);
    EPWM_setDigitalCompareWindowOffset(BUCK_DRV_EPWM_BASE,
                                       BUCK_DRV_EPWM_DC_BLANK_OFFSET);
    EPWM_setDigitalCompareWindowLength(BUCK_DRV_EPWM_BASE,
                                       BUCK_DRV_EPWM_DC_BLANK_LENGTH);
    EPWM_enableDigitalCompareBlankingWindow(BUCK_DRV_EPWM_BASE);
}

//
// BUCK_HAL_setupSyncBuckOverCurrentTripAction - Disable the ePWM high signal
// when a comparator trip is detected
// This OST trip persists until is is cleared manually.
//
void BUCK_HAL_setupSyncBuckOverCurrentTripAction(void)
{
    //
    // Select CMPSS trip signal from ePWM XBAR as source for DCAL and DCBL
    //
    XBAR_setEPWMMuxConfig(BUCK_OC_CMPSS_XBAR_TRIP, BUCK_OC_CMPSS_XBAR_MUX_VAL);
    XBAR_enableEPWMMux(BUCK_OC_CMPSS_XBAR_TRIP, BUCK_OC_CMPSS_XBAR_MUX);

    EPWM_selectDigitalCompareTripInput(BUCK_DRV_EPWM_BASE,
                                       BUCK_DRV_EPWM_DC_TRIP_OC,
                                       EPWM_DC_TYPE_DCAL);

    EPWM_setTripZoneDigitalCompareEventCondition(BUCK_DRV_EPWM_BASE,
                                                 EPWM_TZ_DC_OUTPUT_A1,
                                                 EPWM_TZ_EVENT_DCXL_HIGH);

    EPWM_setDigitalCompareEventSyncMode(BUCK_DRV_EPWM_BASE,
                                        EPWM_DC_MODULE_A,
                                        EPWM_DC_EVENT_1,
                                        EPWM_DC_EVENT_INPUT_NOT_SYNCED);

    //
    // Select signal input source for DCAEVT1
    //
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_VMC)
        //
        // Use blanking window filtered DCAEVT1 signal for VMC
        //
        EPWM_setDigitalCompareEventSource(BUCK_DRV_EPWM_BASE,
                                          EPWM_DC_MODULE_A,
                                          EPWM_DC_EVENT_1,
                                          EPWM_DC_EVENT_SOURCE_FILT_SIGNAL);
    #elif(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        //
        // Use native DCxEVT1 signal for PCMC
        //
        EPWM_setDigitalCompareEventSource(BUCK_DRV_EPWM_BASE,
                                          EPWM_DC_MODULE_A,
                                          EPWM_DC_EVENT_1,
                                          EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);
    #endif


    //
    // Enable DCAEVT1 OST and configure ePWM to output on trip:
    //   H-DRV = LOW (Turn off Vin to L switch)
    //   L-DRV = DO NOTHING (Continue actuation of Gnd to L switch)
    //
    EPWM_enableTripZoneSignals(BUCK_DRV_EPWM_BASE,
                               EPWM_TZ_SIGNAL_DCAEVT1);

    EPWM_setTripZoneAction(BUCK_DRV_EPWM_BASE,
                           EPWM_TZ_ACTION_EVENT_TZA,
                           EPWM_TZ_ACTION_LOW);

    EPWM_setTripZoneAction(BUCK_DRV_EPWM_BASE,
                           EPWM_TZ_ACTION_EVENT_TZB,
                           EPWM_TZ_ACTION_DISABLE);
}

//
// BUCK_HAL_setOverCurrentTripLimit - Set the comparator digital trip limit to
// match the equivalent Amps value passed in through tripLimit_A. Returns the
// effective digital trip limit.
//
uint16_t BUCK_HAL_setOverCurrentTripLimit(float32_t tripLimit_A)
{
    uint16_t dacVal;

    //
    // Range check and correct desired limit; then convert to digital value
    //
    if(tripLimit_A >= BUCK_IL_MAX_SENSE_A)
    {
        dacVal = BUCK_CMPSSDAC_CODES - 1;
    }
    else if(tripLimit_A < (float32_t)0)
    {
        dacVal = 0;
    }
    else
    {
        dacVal = (uint16_t)(BUCK_CMPSSDAC_CODES *
                            tripLimit_A /
                            BUCK_IL_MAX_SENSE_A + 2025);
    }

    //
    // Set comparator trip value
    //
    #if(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_HI)
        CMPSS_setDACValueHigh(BUCK_OC_CMPSS_BASE, dacVal);
    #elif(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_LO)
        CMPSS_setDACValueLow(BUCK_OC_CMPSS_BASE, dacVal);
    #endif

    //
    // Return effective trip value
    //
    #if(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_HI)
        return( CMPSS_getDACValueHigh(BUCK_OC_CMPSS_BASE) );
    #elif(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_LO)
        return( CMPSS_getDACValueLow(BUCK_OC_CMPSS_BASE) );
    #endif
}

//
// BUCK_HAL_setupOverCurrentCmpssTrip - Configure the CMPSS to monitor the
// current feedback signal for an over-current fault.
//
void BUCK_HAL_setupOverCurrentCmpssTrip(void)
{
    uint16_t dacConfig;

    CMPSS_enableModule(BUCK_OC_CMPSS_BASE);

    //
    // Configure CMPSS input pinmux selection
    //
    #if(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_HI)
        ASysCtl_selectCMPHPMux(BUCK_OC_CMPSS_ASYSCTRL_CMPMUX,
                               BUCK_OC_CMPSS_ASYSCTRL_MUX_VALUE);
    #elif(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_LO)
        ASysCtl_selectCMPLPMux(BUCK_OC_CMPSS_ASYSCTRL_CMPMUX,
                               BUCK_OC_CMPSS_ASYSCTRL_MUX_VALUE);
    #endif

    //
    // Use VDDA as the reference for comparator DACs
    //
    dacConfig = CMPSS_DACVAL_SYSCLK |
            CMPSS_DACSRC_RAMP; // CMPSS_DACREF_VDDA;

    #if(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_HI)
        dacConfig |= CMPSS_DACSRC_SHDW;
    #endif

    CMPSS_configDAC(BUCK_OC_CMPSS_BASE, dacConfig);

    //
    // Set reference DAC to default trip value
    //
    BUCK_HAL_setOverCurrentTripLimit(BUCK_USER_DEFAULT_TRIP_A);

    //
    // Use reference DAC for NEG input and configure the CMPSS output
    //
    #if(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_HI)
        CMPSS_configHighComparator(BUCK_OC_CMPSS_BASE, CMPSS_INSRC_DAC );
        #if(BUCK_OC_CMPSS_FILTER_ENABLED == true)
            CMPSS_configFilterHigh(BUCK_OC_CMPSS_BASE,
                                   BUCK_OC_CMPSS_FILTER_PRESCALE,
                                   BUCK_OC_CMPSS_FILTER_SAMPWIN,
                                   BUCK_OC_CMPSS_FILTER_THRESH);
            CMPSS_initFilterHigh(BUCK_OC_CMPSS_BASE);
            CMPSS_configOutputsHigh(BUCK_OC_CMPSS_BASE, CMPSS_TRIP_FILTER);
        #else
            CMPSS_configOutputsHigh(BUCK_OC_CMPSS_BASE, CMPSS_TRIP_SYNC_COMP);
        #endif
    #elif(BUCK_OC_CMPSS_CMP_HL == BUCK_CMPSS_COMP_LO)
        CMPSS_configLowComparator(BUCK_OC_CMPSS_BASE, CMPSS_INSRC_DAC );
        #if(BUCK_OC_CMPSS_FILTER_ENABLED == true)
            CMPSS_configFilterLow(BUCK_OC_CMPSS_BASE,
                                  BUCK_OC_CMPSS_FILTER_PRESCALE,
                                  BUCK_OC_CMPSS_FILTER_SAMPWIN,
                                  BUCK_OC_CMPSS_FILTER_THRESH);
            CMPSS_initFilterLow(BUCK_OC_CMPSS_BASE);
            CMPSS_configOutputsLow(BUCK_OC_CMPSS_BASE, CMPSS_TRIP_FILTER);
        #else
            CMPSS_configOutputsLow(BUCK_OC_CMPSS_BASE, CMPSS_TRIP_SYNC_COMP);
        #endif
    #endif

    //
    // Use the Blanking signal from Sync Buck EPWM to reject switching noise
    //
    CMPSS_configBlanking(BUCK_OC_CMPSS_BASE, BUCK_DRV_EPWM_NUM);
    CMPSS_enableBlanking(BUCK_OC_CMPSS_BASE);

    //
    // Comparator hysteresis control
    //
    CMPSS_setHysteresis(BUCK_OC_CMPSS_BASE, BUCK_OC_CMPSS_HYSTERESIS_FACTOR);
}

//
// BUCK_HAL_setupSyncBuckPcmcTripAction - Extend ePWM configuration to
// toggle the PWM signals when Peak-Current trips are detected.
// CBC trips persist for the remaining duration of the ePWM cycle.
//
void BUCK_HAL_setupSyncBuckPcmcTripAction(void)
{
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        //
        // Select CMPSS trip signal from ePWM XBAR as source for DCAH
        //
        XBAR_setEPWMMuxConfig(BUCK_PCMC_CMPSS_XBAR_TRIP,
                              BUCK_PCMC_CMPSS_XBAR_MUX_VAL);
        XBAR_enableEPWMMux(BUCK_PCMC_CMPSS_XBAR_TRIP,
                           BUCK_PCMC_CMPSS_XBAR_MUX);

        EPWM_selectDigitalCompareTripInput(BUCK_DRV_EPWM_BASE,
                                           BUCK_DRV_EPWM_DC_TRIP_PCMC,
                                           EPWM_DC_TYPE_DCAH);

        //
        // Configure CBC DCAEVT2 = DCAH
        // Enable filtering of DCAH with BUCK_HAL_setupSyncBuckBlankingWindow()
        // Configure Action Qualifier SubModule to Output Low on T1 event
        //
        EPWM_setTripZoneDigitalCompareEventCondition(BUCK_DRV_EPWM_BASE,
                                                     EPWM_TZ_DC_OUTPUT_A2,
                                                     EPWM_TZ_EVENT_DCXH_HIGH);

        EPWM_setTripZoneAction(BUCK_DRV_EPWM_BASE,
                               EPWM_TZ_ACTION_EVENT_DCAEVT2,
                               EPWM_TZ_ACTION_DISABLE);

        #if(BUCK_DRV_EPWM_DC_BLANK_ENABLED == true)
            //
            // Configure T1 event = Filtered DCAL trip signal
            //
            EPWM_setActionQualifierT1TriggerSource(BUCK_DRV_EPWM_BASE,
                                         EPWM_AQ_TRIGGER_EVENT_TRIG_DC_EVTFILT);
        #else
            //
            // Configure T1 event = Raw DCAL trip signal
            //
            EPWM_setActionQualifierT1TriggerSource(BUCK_DRV_EPWM_BASE,
                                         EPWM_AQ_TRIGGER_EVENT_TRIG_DCA_2);
        #endif

        EPWM_setActionQualifierAction(BUCK_DRV_EPWM_BASE,
                                      EPWM_AQ_OUTPUT_A,
                                      EPWM_AQ_OUTPUT_LOW,
                                      EPWM_AQ_OUTPUT_ON_T1_COUNT_UP);

        HRPWM_setSyncPulseSource(BUCK_DRV_EPWM_BASE,
                                 HRPWM_PWMSYNC_SOURCE_PERIOD);
    #endif
}

//
// BUCK_HAL_setupPcmcCmpssTrip - Configure the CMPSS to monitor the
// current feedback signal for Peak-Current trips.
//
void BUCK_HAL_setupPcmcCmpssTrip(void)
{
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        CMPSS_enableModule(BUCK_PCMC_CMPSS_BASE);

        //
        // Configure CMPSS input pinmux selection
        //
        ASysCtl_selectCMPHPMux(BUCK_PCMC_CMPSS_ASYSCTRL_CMPMUX,
                               BUCK_PCMC_CMPSS_ASYSCTRL_MUX_VALUE);

        //
        // Use ramp generator for NEG input and configure the CMPSS output
        //
        CMPSS_configHighComparator(BUCK_PCMC_CMPSS_BASE, CMPSS_INSRC_DAC);
        #if(BUCK_PCMC_CMPSS_FILTER_ENABLED == true)
            CMPSS_configFilterHigh(BUCK_PCMC_CMPSS_BASE,
                                   BUCK_PCMC_CMPSS_FILTER_PRESCALE,
                                   BUCK_PCMC_CMPSS_FILTER_SAMPWIN,
                                   BUCK_PCMC_CMPSS_FILTER_THRESH);
            CMPSS_initFilterHigh(BUCK_PCMC_CMPSS_BASE);
        #endif

        //
        // Enable the clearing of the CMPSS high latch every ePWM cycle.
        // Read the CMPSS low latch clear setting in order to preserve
        // its preexisting setting.
        //
        if((HWREGH(BUCK_PCMC_CMPSS_BASE + CMPSS_O_COMPSTSCLR) &
            CMPSS_COMPSTSCLR_LSYNCCLREN) == 0)
        {
            CMPSS_configLatchOnPWMSYNC(BUCK_PCMC_CMPSS_BASE, true, false);
        }
        else
        {
            CMPSS_configLatchOnPWMSYNC(BUCK_PCMC_CMPSS_BASE, true, true);
        }

        //
        // Use the Blanking signal from Sync Buck EPWM to reject switching
        // noise and avoid state-machine boundary conditions
        //
        CMPSS_configBlanking(BUCK_PCMC_CMPSS_BASE, BUCK_DRV_EPWM_NUM);
        CMPSS_enableBlanking(BUCK_PCMC_CMPSS_BASE);

        //
        // Comparator hysteresis control
        //
        CMPSS_setHysteresis(BUCK_PCMC_CMPSS_BASE,
                            BUCK_PCMC_CMPSS_HYSTERESIS_FACTOR);

        CMPSS_configRamp(BUCK_PCMC_CMPSS_BASE,
                         BUCK_PCMC_CMPSS_RAMPMAX_DEFAULT,
                         BUCK_PCMC_CMPSS_SLOPE,
                         BUCK_PCMC_CMPSS_DELAY,
                         BUCK_DRV_EPWM_NUM,
                         BUCK_PCMC_CMPSS_RAMPLOADSEL);

        //
        // Use VDDA as the reference for comparator DACs
        //
        CMPSS_configDAC(BUCK_PCMC_CMPSS_BASE,
//                        CMPSS_DACREF_VDDA |
                        CMPSS_DACSRC_RAMP);
    #endif
}

//
// BUCK_HAL_setupSyncBuckTripAction - Enable CMPSS monitoring of the current
// feedback signal and configure the ePWM trip behavior
//
void BUCK_HAL_setupSyncBuckTripAction(void)
{
    BUCK_HAL_setupOverCurrentCmpssTrip();
    #if(BUCK_DRV_EPWM_DC_BLANK_ENABLED == true)
        BUCK_HAL_setupSyncBuckBlankingWindow();
    #endif
    BUCK_HAL_setupSyncBuckOverCurrentTripAction();
    #if(BUCK_CTL_MODE == BUCK_CTL_MODE_PCMC)
        BUCK_HAL_setupPcmcCmpssTrip();
        BUCK_HAL_setupSyncBuckPcmcTripAction();
    #endif
}

//
// BUCK_HAL_setupActiveLoadPinGPIO - Configure active load pin
// to be controlled by GPIO module with a static low output
//
void BUCK_HAL_setupActiveLoadPinGPIO(void)
{
    GPIO_writePin(BUCK_LOAD_EPWM_GPIO, 0);
    GPIO_setDirectionMode(BUCK_LOAD_EPWM_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(BUCK_LOAD_EPWM_PIN_CONFIG_GPIO);
}

//
// BUCK_HAL_setupActiveLoadPinEPWM - Configure active load pin
// to be controlled by ePWM module
//
void BUCK_HAL_setupActiveLoadPinEPWM(void)
{
    GPIO_setPinConfig(BUCK_LOAD_EPWM_PIN_CONFIG_EPWM);
}

//
// BUCK_HAL_setupActiveLoadPWM - Configure the active load ePWM to control the
// load resistor switch.
//
void BUCK_HAL_setupActiveLoadPWM(void)
{
    //
    // Maximum supported ePWM clock speed is specified in the datasheet
    //
    EPWM_setClockPrescaler(BUCK_LOAD_EPWM_BASE,
                           BUCK_LOAD_EPWM_EPWMCLK_DIV,
                           BUCK_LOAD_EPWM_HSCLK_DIV);

    //
    // Configure ePWM for count-up-down operation
    //
    EPWM_setTimeBaseCounter(BUCK_LOAD_EPWM_BASE, 0);

    EPWM_setTimeBasePeriod(BUCK_LOAD_EPWM_BASE,
                           BUCK_LOAD_EPWM_PERIOD_TICKS - 1);

    EPWM_setPeriodLoadMode(BUCK_LOAD_EPWM_BASE, EPWM_PERIOD_SHADOW_LOAD);
    EPWM_setTimeBaseCounterMode(BUCK_LOAD_EPWM_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_disablePhaseShiftLoad(BUCK_LOAD_EPWM_BASE);

    //
    // Use shadow mode to update CMPA on TBPRD
    //
    EPWM_setCounterCompareValue(BUCK_LOAD_EPWM_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                BUCK_LOAD_EPWM_CMPA_DISABLED);
    EPWM_setCounterCompareShadowLoadMode(BUCK_LOAD_EPWM_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    EPWM_setActionQualifierAction(BUCK_LOAD_EPWM_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

    EPWM_setActionQualifierAction(BUCK_LOAD_EPWM_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    BUCK_HAL_updateSyncBuckDuty(0);
}

void BUCK_HAL_disableEpwmCounting(void)
{
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

void BUCK_HAL_enableEpwmCounting(void)
{
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

void BUCK_HAL_setupRunLed(void)
{
    //
    // GPIO23 requires additional configuration on F28004x
    //
    #if(BUCK_RUN_LED_GPIO == 23)
        GPIO_setAnalogMode(BUCK_RUN_LED_GPIO, GPIO_ANALOG_DISABLED);
    #endif

    GPIO_setDirectionMode(BUCK_RUN_LED_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(BUCK_RUN_LED_PIN_CONFIG);
}

void BUCK_HAL_setup91Enable(void)
{
    //
    // GPIO23 requires additional configuration on F28004x
    //
    GPIO_setDirectionMode(EN91_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(EN91_GPIO_PIN_CONFIG);
}

void BUCK_HAL_toggleRunLed(void)
{
    static uint16_t ledCnt = 0;

    if(ledCnt == 0)
    {
        GPIO_togglePin(BUCK_RUN_LED_GPIO);
        ledCnt = BUCK_RUN_LED_PRESCALE;
    }
    else
    {
        ledCnt--;
    }
}

void BUCK_HAL_setupInterruptTrigger(void)
{
    ADC_setInterruptSource(BUCK_VOUT_ADC_MODULE,
                           BUCK_VOUT_ADC_INT_NUMBER,
                           BUCK_VOUT_ADC_SOC_NO);

    ADC_setInterruptPulseMode(BUCK_VOUT_ADC_MODULE,
                              BUCK_VOUT_ADC_INT_MODE);

    ADC_clearInterruptStatus(BUCK_VOUT_ADC_MODULE,
                             BUCK_VOUT_ADC_INT_NUMBER);

    ADC_enableInterrupt(BUCK_VOUT_ADC_MODULE,
                        BUCK_VOUT_ADC_INT_NUMBER);
}

void BUCK_HAL_setupInterrupt(void)
{
    BUCK_HAL_setupInterruptTrigger();
#if(BUCK_CONTROL_RUNNING_ON == C28X_CORE)
    Interrupt_register(BUCK_VOUT_ADC_ISR_TRIGGER, &BUCK_VOUT_ADC_ISR_FUNCTION);
    Interrupt_enable(BUCK_VOUT_ADC_ISR_TRIGGER);

#else

    BUCK_HAL_setupCLA();

#endif
    BUCK_HAL_clearInterruptFlags();
    ADC_clearInterruptOverflowStatus(BUCK_VOUT_ADC_MODULE,
                                     BUCK_VOUT_ADC_INT_NUMBER);

    EALLOW;
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global real-time interrupt DBGM
    EDIS;
}

void BUCK_HAL_setupCLA(void)
{
    //
    // setup CLA to register an interrupt
    //
#if(BUCK_CONTROL_RUNNING_ON == CLA_CORE)

    memcpy((uint32_t *)&Cla1ProgRunStart, (uint32_t *)&Cla1ProgLoadStart,
            (uint32_t)&Cla1ProgLoadSize );

    //
    // first assign memory to CLA
    //
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS0, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS1, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS2, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS3, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS4, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS5, MEMCFG_LSRAMMASTER_CPU_CLA1);

    //
    // Cla1Prog, .const_cla
    //
    MemCfg_setCLAMemType(MEMCFG_SECT_LS2, MEMCFG_CLA_MEM_PROGRAM);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS3, MEMCFG_CLA_MEM_PROGRAM);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS4, MEMCFG_CLA_MEM_PROGRAM);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS5, MEMCFG_CLA_MEM_PROGRAM);

    //
    // .bss (used by BUCK_runIsr()), .bss_cla, .scratchpad,
    //
    MemCfg_setCLAMemType(MEMCFG_SECT_LS0, MEMCFG_CLA_MEM_DATA);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS1, MEMCFG_CLA_MEM_DATA);

    //
    // Suppressing #770-D conversion from pointer to smaller integer
    // The CLA address range is 16 bits so the addresses passed to the MVECT
    // registers will be in the lower 64KW address space. Turn the warning
    // back on after the MVECTs are assigned addresses
    //
    #pragma diag_suppress = 770

    CLA_mapTaskVector(CLA1_BASE , CLA_MVECT_1, (uint16_t)&Cla1Task1);

    #pragma diag_warning = 770

    CLA_enableIACK(CLA1_BASE);
    CLA_enableTasks(CLA1_BASE, CLA_TASKFLAG_ALL);

    CLA_setTriggerSource(CLA_TASK_1, BUCK_CLA_ISR_TRIG);

#endif

}
void BuckPWMOff(void)
{
    //
    // Disable the PWM
    //

    EPWM_forceTripZoneEvent(BUCK_DRV_EPWM_BASE, EPWM_TZ_FORCE_EVENT_OST);

}

void BuckEnablePWM(void)
{

        EPWM_clearTripZoneFlag(BUCK_DRV_EPWM_BASE, EPWM_TZ_FLAG_OST);
}





//
// End of buck_hal.c
//
