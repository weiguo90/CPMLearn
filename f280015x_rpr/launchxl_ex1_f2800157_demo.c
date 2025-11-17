//#############################################################################
//
// FILE:   launchxl_ex1_f2800157_demo.c
//
// TITLE:  F2800157 LaunchPad Out of Box Demo Example
//
//! \addtogroup driver_example_list
//! <h1> F2800157 LaunchPad Out of Box Demo Example </h1>
//!
//! This program is the demo program that comes pre-loaded on the F2800157
    
//! LEDs. After a few seconds the LEDs stop flashing and the device starts
//! sampling ADCINA6 once a second. If the sample is greater than midscale
//! the red LED on the board is lit, while if it is lower the green LED is lit.
//! Sample data is also displayed in a serial terminal via the board's back
//! channel UART. You may view this data by configuring a serial terminal
//! to the correct COM port at 115200 Baud 8-N-1.
//!
//! \b External \b Connections \n
//!  - Connect to COM port at 115200 Baud 8-N-1 for serial data
//!  - Connect signal to ADCINA6 to change LED based on value
//!
//! \b Watch \b Variables \n
//!  - None.
//!
//
//#############################################################################
//
//
// $Copyright:
// Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include <stdint.h>
#include <stdio.h>
#include <file.h>
#include <launchxl_ex1_sci_io_driverlib.h>

#include "launchxl_ex1_ti_ascii.h"
#include "driverlib.h"
#include "device.h"
#include <bitfield_support/f280015x_DEVICE.h>
#include <bitfield_support/f280015x_pievect.h>
#include <bitfield_support/f280015x_epwm.h>
//#include "BSW_MCAL_PWM.h"
#include "f280015x_epwm_defines.h"

//#include <bitfield_support/f280015x_device.h>     // f280015x Headerfile Include File
#include <buck_hal.h>
#include <buck.h>
//
// Defines
//

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
// Select which SCI/UART pinout is desired -
// Must configure switch S2 on LaunchPad board accordingly
//
#define SCIPinout  0    // GPIO28/29
//#define SCIPinout  1    // GPIO23/40


//
// Globals
//
static unsigned short indexX=0;
static unsigned short indexY=0;

const unsigned char escRed[] = {0x1B, 0x5B, '3','1', 'm'};
const unsigned char escWhite[] = {0x1B, 0x5B, '3','7', 'm'};
const unsigned char escLeft[] = {0x1B, 0x5B, '3','7', 'm'};
const unsigned char pucTempString[] = "ADCINA6 Sample:     ";
int16_t currentSample;

//
// sampleADC - ADCINA6
//
int16_t sampleADC(void)
{
    int16_t sample;

    //
    // Force start of conversion on SOC0
    //
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);

    //
    // Wait for ADCA to complete, then acknowledge flag
    //
    while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false)
    {
    }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    //
    // Get ADC sample result from SOC0
    //
    sample = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

    return(sample);
}

//
// drawTILogo - Draw the TI logo in the display
//
void drawTILogo(void)
{
    unsigned char ucChar, lastChar;

    putchar('\n');
    while(indexY<45)
    {
        if(indexY<45)
        {
            if(indexX<77)
            {
                ucChar = ti_ascii[indexY][indexX++] ;

                //
                // We are in the TI logo make it red
                //
                if((ucChar != '7') && (lastChar=='7'))
                {
                    putchar(escRed[0]);
                    putchar(escRed[1]);
                    putchar(escRed[2]);
                    putchar(escRed[3]);
                    putchar(escRed[4]);
                }

                //
                // We are in the TI logo make it red
                //
                if(ucChar == '7' && lastChar!='7')
                {
                    putchar(escWhite[0]);
                    putchar(escWhite[1]);
                    putchar(escWhite[2]);
                    putchar(escWhite[3]);
                    putchar(escWhite[4]);
                }

                putchar(ucChar);
                lastChar = ucChar;
            }

            else
            {
                ucChar = 10;
                putchar(ucChar);
                ucChar = 13;
                putchar(ucChar);
                indexX=0;
                indexY++;
            }
        }
    }
}

//
// clearTextBox - Clear the text box
//
void clearTextBox(void)
{
    putchar(0x08);

    //
    // Move back 24 columns
    //
    putchar(0x1B);
    putchar('[');
    putchar('2');
    putchar('6');
    putchar('D');

    //
    // Move up 3 lines
    //
    putchar(0x1B);
    putchar('[');
    putchar('3');
    putchar('A');

    //
    // Change to Red text
    //
    putchar(escRed[0]);
    putchar(escRed[1]);
    putchar(escRed[2]);
    putchar(escRed[3]);
    putchar(escRed[4]);

    printf((char*)pucTempString);

    //
    // Move down 1 lines
    //
    putchar(0x1B);
    putchar('[');
    putchar('1');
    putchar('B');

    //
    // Move back 20 columns
    //
    putchar(0x1B);
    putchar('[');
    putchar('2');
    putchar('0');
    putchar('D');

    //
    // Save cursor position
    //
    putchar(0x1B);
    putchar('[');
    putchar('s');
}

//
// updateDisplay - Update the serial display
//
void updateDisplay(void)
{
    //
    // Restore cursor position
    //
    putchar(0x1B);
    putchar('[');
    putchar('u');

    printf("%d                ", currentSample);
}

//
// scia_init - SCIA  8-bit word, baud rate 0x001A, default, 1 STOP bit,
// no parity
//
void scia_init()
{
    //
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    //

    //
    // 1 stop bit,  No loopback, No parity,8 char bits, async mode,
    // idle-line protocol
    //
    SCI_performSoftwareReset(SCIA_BASE);

    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));

    SCI_resetChannels(SCIA_BASE);
//    SCI_resetRxFIFO(SCIA_BASE);
//    SCI_resetTxFIFO(SCIA_BASE);
//    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
//    SCI_enableFIFO(SCIA_BASE);
    SCI_enableInterrupt(SCIA_BASE, SCI_INT_RXRDY_BRKDT | SCI_INT_TXRDY);
    SCI_enableModule(SCIA_BASE);
    SCI_performSoftwareReset(SCIA_BASE);

    return;
}

//
// initADCs - Function to configure and power up ADCs A and B.
//
void initADCs(void)
{
    //
    // Setup VREF as internal
    //
    ADC_setVREF(ADCA_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);

    //
    // Set ADCCLK divider to /4
    //
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);

    //
    // Set pulse positions to late
    //
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);

    //
    // Power up the ADCs and then delay for 1 ms
    //
    ADC_enableConverter(ADCA_BASE);

    DEVICE_DELAY_US(1000);
}

//
// initADCSOCs - Function to configure SOC 0 of ADC A.
//
void initADCSOCs(void)
{
    //
    // Configure SOCs of ADCA
    // - SOC0 will convert pin A6 with a sample window of 10 SYSCLK cycles.
    //
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                 ADC_CH_ADCIN6, 10);

    //
    // Set SOC0 to set the interrupt 1 flag. Enable the interrupt and make
    // sure its flag is cleared.
    //
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
}



uint16_t EN91=0;
//
// Main
//
void main()
{
    volatile int status = 0;
    uint16_t i;
    volatile FILE *fid;

    //
    // Initialize device clock and peripherals
    //
    BUCK_HAL_setupDevice();
//    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();
     // Switch actuation pins over: enable high-side as ePWM, keep low-side as GPIO
     // This disables synchronous rectification while allowing the high-side to be driven by ePWM
     BUCK_HAL_setupSyncBuckPinsEpwm();
    // Ensure low-side is GPIO (drive low) to disable synchronous rectification
    BUCK_HAL_setLowSideGpio();
    BUCK_HAL_setupActiveLoadPinGPIO();


    // Configure the run LED GPIO
    //
//    BUCK_HAL_setupRunLed();   CAN
    BUCK_HAL_setup91Enable();

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
    PieVectTable.ADCA1_INT=0;
    BUCK_HAL_setupInterrupt();



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
    // Start ePWM clocks
    //
    BUCK_HAL_enableEpwmCounting();

    //
    // Switch actuation pins over to ePWM function
    //
    BUCK_HAL_setupSyncBuckPinsEpwm();
    BUCK_HAL_setupActiveLoadPinEPWM();
    //
    // For this example, only init the pins for the SCI-A port.
    //
//    EALLOW;
#if !SCIPinout
    //
    // GPIO28 is the SCI Rx pin.
    //

    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCIRXDA);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCIRXDA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCIRXDA, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCIRXDA, GPIO_QUAL_ASYNC);

    //
    // GPIO29 is the SCI Tx pin.
    //

    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCITXDA);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCITXDA, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCITXDA, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCITXDA, GPIO_QUAL_ASYNC);

#else
    //
    // GPIO23 is the SCI Rx pin.
    //

    GPIO_setPinConfig(DEVICE_GPIO_PIN_SCIRXDB);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCIRXDB, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCIRXDB, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCIRXDB, GPIO_QUAL_ASYNC);

    //
    // GPIO40 is the SCI Tx pin.
    //

    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCITXDB);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCITXDB, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCITXDB, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCITXDB, GPIO_QUAL_ASYNC);
#endif

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
//    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
//    Interrupt_initVectorTable();

    //
    // Initialize SCIA
    //
    scia_init();

    //
    // Initialize GPIOs for the USER LEDs and turn them off
    // GPIO35 is LED4
    // GPIO49 is LED5
    //
    GPIO_setPinConfig(DEVICE_GPIO_CFG_LED1);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);

    // GPIO20 LED4 comes up in Analog mode by default, switch to Digital
//    GPIO_setAnalogMode(DEVICE_GPIO_PIN_LED1, GPIO_ANALOG_DISABLED);

    GPIO_setPinConfig(DEVICE_GPIO_CFG_LED2);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED2, GPIO_DIR_MODE_OUT);

    GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
    GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);

    // Analog PinMux for A6, GPIO228
    GPIO_setPinConfig(GPIO_228_GPIO228);
    // AGPIO -> Analog mode selected
    GPIO_setAnalogMode(228, GPIO_ANALOG_ENABLED);

    //
    // Enable global Interrupts and higher priority real-time debug events:
    //
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

    //
    // Set up ADCs, initializing the SOCs to be triggered by software
    //
//    initADCs();
//    initADCSOCs();

//    //
//    // Redirect STDOUT to SCI
//    //
//    status = add_device("scia", _SSA, SCI_open, SCI_close, SCI_read, SCI_write,
//                        SCI_lseek, SCI_unlink, SCI_rename);
//    fid = fopen("scia","w");
//    freopen("scia:", "w", stdout);
//    setvbuf(stdout, NULL, _IONBF, 0);

//    //
//    // Print a TI Logo to STDOUT
//    //
//    drawTILogo();
//
//    //
//    // Twiddle LEDs
//    //
//    GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0);
//    GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
//
//    for(i = 0; i < 50; i++)
//    {
//        GPIO_togglePin(DEVICE_GPIO_PIN_LED1);
//        GPIO_togglePin(DEVICE_GPIO_PIN_LED2);
//        DEVICE_DELAY_US(50000);
//    }
//
//    //
//    // LEDs off
//    //
//    GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
//    GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
//
//    //
//    // Clear out one of the text boxes so we can write more info to it
//    //
//    clearTextBox();
//
//    currentSample = sampleADC();
    BuckPWMOff();
    GPIO_writePin(EN91_GPIO, 1);
    //
    // Main program loop - continually sample temperature
    //
    for(;;)
    {
        //
        // Background state machine entry & exit point
        //
        (*Alpha_State_Ptr)();   // jump to an Alpha state (A0,B0,...)

        if ( EN91)
        {
            BuckEnablePWM();
        }
        else
        {
            BuckPWMOff();
        }

        // BuckOn1();
//        //
//        // Sample ADCINA6
//        //
//        currentSample = sampleADC();
//
//        //
//        // Update the serial terminal output
//        //
//        updateDisplay();
//
//        //
//        // If the sample is above midscale light one LED
//        //
//        if(currentSample > 2048)
//        {
//            GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0);
//            GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
//        }
//        else
//        {
//            //
//            // Otherwise light the other
//            //
//            GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
//            GPIO_writePin(DEVICE_GPIO_PIN_LED2, 0);
//        }
//
//        DEVICE_DELAY_US(1000000);
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



static void blueLedToggle(void)
{
    static uint32_t counter = 0;

    counter++;
    GPIO_writePin(DEVICE_GPIO_PIN_LED1, counter & 1);
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
    blueLedToggle();
//    BUCK_HAL_toggleRunLed();

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


//
// End of File
//


