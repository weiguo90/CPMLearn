//#############################################################################
//
// FILE:   buck_main.c
//
// TITLE:  This is the main file for the solution
//         Additional solution support files are
//             <solution>.c -> solution source file
//             <solution>.h -> solution header file
//             <solution>_settings.h -> powerSUITE generated settings
//             <solution>_hal.c -> device drivers source file
//             <solution>_hal.h -> device drivers header file
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

//*****************************************************************************
// includes section
//*****************************************************************************
#include <buck.h>

//
//---  State Machine Related ---
//
uint16_t vTimer0[4];         // Virtual Timers based on CPU Timer 0 (A events)
uint16_t vTimer1[4];         // Virtual Timers based on CPU Timer 1 (B events)
uint16_t vTimer2[4];         // Virtual Timers based on CPU Timer 2 (C events)

//
// Variable declarations for state machine
//
void (*Alpha_State_Ptr)(void); // Base States pointer
void (*A_Task_Ptr)(void);      // State pointer A branch
void (*B_Task_Ptr)(void);      // State pointer B branch
void (*C_Task_Ptr)(void);      // State pointer C branch

//
// State Machine function prototypes
//------------------------------------
// Alpha states
//
void A0(void);  //state A0
void B0(void);  //state B0
void C0(void);  //state C0

//
// A branch states
//
void A1(void);  //state A1
void A2(void);  //state A2
void A3(void);  //state A3

//
// B branch states
//
void B1(void);  //state B1
void B2(void);  //state B2
void B3(void);  //state B3

//
// C branch states
//
void C1(void);  //state C1
void C2(void);  //state C2
void C3(void);  //state C3

//
// main() function
//
void main(void)
{
    //
    // Set up the basic device configuration such as initializing PLL,
    // copying code from FLASH to RAM, and initializing the CPU timers that
    // are used in the background A, B, C tasks
    //
    BUCK_HAL_setupDevice();

    //
    // Place actuation pins (ePWM signals that control the
    // synchronous buck and active load) in a safe state for the system.
    // The pins are configured for GPIO function with static low output
    // while the system is initialized.
    //
    BUCK_HAL_setupSyncBuckPinsGpio();
    BUCK_HAL_setupActiveLoadPinGPIO();

    //
    // Tasks State-machine initialization
    //
    Alpha_State_Ptr = &A0;
    A_Task_Ptr = &A1;
    B_Task_Ptr = &B1;
    C_Task_Ptr = &C1;

    //
    // Stop ePWM clocks
    //
    BUCK_HAL_disableEpwmCounting();

    //
    // Set up ePWM for synchronous buck and active load control.
    // Include comparator monitoring of the inductor current feedback
    // signal (ILFB) for over-current trip protection.
    //
    BUCK_HAL_setupSyncBuckPwm();
    BUCK_HAL_setupSyncBuckTripAction();
    BUCK_HAL_setupActiveLoadPWM();
    BUCK_setupHrpwmMepScaleFactor();

    //
    // Configure the embedded ADC to sample Vin, Vout, ILFB, and ILFB_AVG
    //
    BUCK_HAL_setupAdc();
    BUCK_HAL_setupAdcTrigger();

    //
    // Configure the run LED GPIO
    //
    BUCK_HAL_setupRunLed();

    //
    // Initialize global variables used in solution
    //
    BUCK_initUserVariables();
    BUCK_initProgramVariables();

    //
    // Configure DCL and SFRA libraries
    //
    BUCK_setupDcl();
    BUCK_setupSfra();
    BUCK_setupSfraGui();

    //
    // Configure and enable system interrupt
    //
    BUCK_HAL_setupInterrupt();

    //
    // Start ePWM clocks
    //
    BUCK_HAL_enableEpwmCounting();

    //
    // Switch actuation pins over to ePWM function
    //
    BUCK_HAL_setupSyncBuckPinsEpwm();
    BUCK_HAL_setupActiveLoadPinEPWM();

    //
    // Clear spurious trip of over-current protection
    //
    BUCK_HAL_clearOverCurrentTripFlag();

    //
    // Background loop with periodic branches to state-machine tasks.
    // Frequency of task branching is configured in setupDevice() routine.
    //
    for(;;)
    {
        //
        // Background state machine entry & exit point
        //
        (*Alpha_State_Ptr)();   // jump to an Alpha state (A0,B0,...)
    }
} //END MAIN CODE

//
// ISR1() interrupt function
//

uint16_t CntIsr1=0;
#ifdef BUCK_CONTROL_RUNNING_ON_CPU
#pragma CODE_SECTION(ISR1,"isrcodefuncs");
#pragma INTERRUPT(ISR1, HPI)
interrupt void ISR1(void)
{
    //
    // ISR is triggered by the ADC every EPWM cycle after Vout is sampled.
    // ADC should sample early enough in the cycle for the ISR to write
    // the updated actuation values before the shadow loads take place.
    //
    CntIsr1++;
    BUCK_runIsr();
}

#endif
//
//=============================================================================
//  STATE-MACHINE SEQUENCING AND SYNCRONIZATION FOR SLOW BACKGROUND TASKS
//=============================================================================
//
//
//--------------------------------- FRAME WORK --------------------------------
//
void A0(void)
{
    //
    // loop rate synchronizer for A-tasks
    //
    if(GET_TASKA_TIMER_OVERFLOW_STATUS == 1)
    {
        CLEAR_TASKA_TIMER_OVERFLOW_FLAG;    // clear flag

        //
        // jump to an A Task (A1,A2,A3,...)
        //
        (*A_Task_Ptr)();

        vTimer0[0]++;           // virtual timer 0, instance 0 (spare)
    }
    Alpha_State_Ptr = &B0;      // Comment out to allow only A tasks
}

void B0(void)
{
    //
    // loop rate synchronizer for B-tasks
    //
    if(GET_TASKB_TIMER_OVERFLOW_STATUS  == 1)
    {
        CLEAR_TASKB_TIMER_OVERFLOW_FLAG;                // clear flag

        //
        // jump to an B Task (B1,B2,B3,...)
        //
        (*B_Task_Ptr)();

        vTimer1[0]++;           // virtual timer 1, instance 0 (spare)
    }

    Alpha_State_Ptr = &C0;      // Allow C state tasks
}

void C0(void)
{
    //
    // loop rate synchronizer for C-tasks
    //
    if(GET_TASKC_TIMER_OVERFLOW_STATUS  == 1)
    {
        CLEAR_TASKC_TIMER_OVERFLOW_FLAG;                // clear flag

        //
        // jump to an C Task (C1,C2,C3,...)
        //
        (*C_Task_Ptr)();

        vTimer2[0]++;           // virtual timer 2, instance 0 (spare)
    }

    Alpha_State_Ptr = &A0;      // Return to A state tasks
}

//
//=============================================================================
//  A - TASKS (executed at 1kHz)
//=============================================================================
//
void A1(void)
{
    //
    // Calculate the effective ePWM duty or Vout setting based on a
    // combination of user input and maximum slew rate allowed
    //
    #if(BUCK_LAB_NUMBER == BUCK_LAB_OPEN_LOOP_VMC)
        BUCK_updateDutyTarget();
        BUCK_updateDutySlewed();
    #elif(BUCK_LAB_NUMBER == BUCK_LAB_OPEN_LOOP_PCMC)
        BUCK_updateDacTarget();
        BUCK_updateDacSlewed();
    #else
        BUCK_updateVoutTarget();
        BUCK_updateVoutSlewed();
    #endif

    //
    // Execute task A2 the next time CpuTimer0 decrements to 0
    //
    A_Task_Ptr = &A2;
}

void A2(void)
{
    //
    // Service SCI link for SFRA GUI
    //
    BUCK_SFRA_GUI_RUN_COMMS(&BUCK_sfra);

    //
    // Execute task A3 the next time CpuTimer0 decrements to 0
    //
    A_Task_Ptr = &A3;
}

void A3(void)
{
    //
    // Calibrate HRPWM MEP Scale Factor
    //
    BUCK_updateHrpwmMepScaleFactor();

    //
    // Execute task A1 the next time CpuTimer0 decrements to 0
    //
    A_Task_Ptr = &A1;
}

//
//=============================================================================
//  B - TASKS (executed at 100Hz)
//=============================================================================
//
void B1(void)
{
    //
    // Toggle on-board LED to indicate program execution
    //
    BUCK_HAL_toggleRunLed();

    //
    // Execute task B2 the next time CpuTimer1 decrements to 0
    //
    B_Task_Ptr = &B2;
}

void B2(void)
{
    //
    // Manage SFRA sweep
    //
    BUCK_SFRA_RUN_BACKGROUND(&BUCK_sfra);

    //
    // Execute task B3 the next time CpuTimer1 decrements to 0
    //
    B_Task_Ptr = &B3;
}

void B3(void)
{
    //
    // SPARE
    //

    //
    // Execute task B1 the next time CpuTimer1 decrements to 0
    //
    B_Task_Ptr = &B1;
}

//
//=============================================================================
//  C - TASKS (executed at 10Hz)
//=============================================================================
//
void C1(void)
{
    //
    // Update user variables to show the state of over-current trip status
    // and most recent ADC readings of Vin, Vout, ILFB, and ILFB_AVG
    //
    BUCK_updateOverCurrentStatus();
    BUCK_updateSensedValues();

    //
    // Execute task C2 the next time CpuTimer2 decrements to 0
    //
    C_Task_Ptr = &C2;
}

void C2(void)
{
    //
    // Update the over-current trip limit and PID coefficients based on
    // user input variables
    //
    BUCK_updateOverCurrentLimit();
    BUCK_updateControllerCoefficients();

    //
    // Execute task C3 the next time CpuTimer2 decrements to 0
    //
    C_Task_Ptr = &C3;
}

void C3(void)
{
    //
    // Update the operation of the active load resistor based on
    // user input variables
    //
    BUCK_updateActiveLoad();
    BUCK_processIsrDutyUpdateMarginStats();
    BUCK_updateVoutLog();

    //
    // Execute task C1 the next time CpuTimer2 decrements to 0
    //
    C_Task_Ptr = &C1;
}

//
// End of buck_main.c
//
