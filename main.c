/*****************************************************************************
* File Name: main.c
*
* Version: 3.0
*
* Description:
*   The main C file for the Temperature measurement with Energus Temp Sensor project. 
* 
* Note:
******************************************************************************
* VTM17
* Neil Moloney
******************************************************************************
* This software is based on software owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*****************************************************************************/
#include <device.h>
#include "measurement.h"
#include <stdio.h>
	
int main(void)
{
    // Local Variables
    int numAdcChannels = 2; // set to 10 for checking the measurement function 
    int i = 0;
    uint16 center = 0x5A; //this will be the address in EEPROM to start searching from ~ around room temp
    int32 vCenter = (EEPROM_1_ReadByte(0x5A) << 8) + EEPROM_1_ReadByte(0x5B);
    int32 vCompare;
    uint adr = 0;
    int vmin = 1450; //centivolts; used for range checking
    int vmax = 2350; //centivolts; used for range checking
    int vHighTemp = 1550; //centivolts; set to 55 deg C for testing. TODO: include a chart with temp settings
    
	/* Voltages across reference resistor and thermistor*/
	int16 vSense[numAdcChannels];
	
	/* Temperature value in 100ths of a degree C*/
	int16 iTemp[numAdcChannels];
    
    /*Decimal Temp*/
    int32 decTemp[numAdcChannels];
	
	/* Dispaly format string */
	char printBuf[16]={'\0'};

	/*Enable global interrupts*/
	CYGlobalIntEnable; 
	
	/*Start all the hardware components required*/
	ADC_SAR_Seq_1_Start();
    EEPROM_1_Start();
    UART_Start();
   
    /*State Machine Implementation */
    
    /*Define States*/
     enum tempSenseStates {
            MEASURE,
            CALCULATE,
            DATA_TRANSFER,
            SENSOR_FAULT,
            SHUTDOWN,
            REPORT_HOTSPOT,        //Used after shutdown to report a hotspot
            REPORT_SENSORFAULT,     //Used after shutdown to report a sensor fault
            DEFAULT
        };
        
    /*Initialize State Machine*/
    enum tempSenseStates state = MEASURE;
    
    //This will be used to determine if shutdown was triggered by an over temperature or sensor fault
    enum tempSenseStates previousState = MEASURE; 
    
	for(;;)
    {
        switch(state)
        {
            case MEASURE:
            
                /* Measure Voltage Across All Temperature Sensors*/
                i = 0;      //Reset the channel counter THIS WONT WORK FOR 
                while(i<numAdcChannels){
    	            vSense[i] = MeasureSensorVoltage(i); 
                    if (vSense[i] <= vHighTemp){
                        previousState = state;
                        state = SHUTDOWN;
                    }else{
                        //continue measurement; sensor faults will all be recorded at once in an array durring calculate?
                    }
                    i++;
                    //TODO - if a voltage is measured < 1.51 (60 C), the relay will be flipped - need state machine
                }
            
            break;
            
            case CALCULATE:
            
            break;
            
            case DATA_TRANSFER:
            
            break;
            
            case SENSOR_FAULT:
            
            break;
            
            case SHUTDOWN:
            
            break;
            
            case REPORT_HOTSPOT:
            
            break;
            
            case REPORT_SENSORFAULT:
            
            break;
                
            case DEFAULT:
                // should never get here - possibly include shutdown code here? 
            break;
        }
    }
    
    
    {
    	/* Measure Voltage Across All Temperature Sensors*/
        i = 0;      //Reset the channel counter THIS WONT WORK FOR 
        while(i<numAdcChannels){
    	    vSense[i] = MeasureSensorVoltage(i); 
            i++;
            //TODO - if a voltage is measured < 1.51 (60 C), the relay will be flipped - need state machine
        }
        
        /* Use the temperature lookup function call to obtain 
        the temperature corresponding to the resistance measured*/	
        i = 0; 
        adr = 2;
        
        //TODO: Ensure that voltage measured is in range
    	while(i<numAdcChannels){
            //TODO - SEARCH FOR first voltage > read in voltage, increment 1 and that is the temp
            // 180 entries in EEPROM, even are voltages, odd are temps
            if (vSense[i] > vCenter){ // check Temps below tCenter
                vCompare = (EEPROM_1_ReadByte(0x5A-adr) << 8) + EEPROM_1_ReadByte(0x5B-adr);
                while (vSense[i] > vCompare){
                    adr=adr+2;
                    vCompare = (EEPROM_1_ReadByte(0x5A-adr) << 8) + EEPROM_1_ReadByte(0x5B-adr);
                }
                iTemp[i] = (EEPROM_1_ReadByte(center-adr+182) << 8) + EEPROM_1_ReadByte(center-adr+183);
            }else if (vSense[i] < vCenter){ // check Temps above tCenter
                vCompare = (EEPROM_1_ReadByte(0x5A+adr) << 8) + EEPROM_1_ReadByte(0x5B+adr);
                while (vSense[i] < vCompare){
                    adr=adr+2;
                    vCompare = (EEPROM_1_ReadByte(0x5A+adr) << 8) + EEPROM_1_ReadByte(0x5B+adr);
                }
                iTemp[i] = (EEPROM_1_ReadByte(center+adr+182) << 8) + EEPROM_1_ReadByte(center+adr+183);
            }else{
                iTemp[i] = (EEPROM_1_ReadByte(0x5A+182) << 8) + EEPROM_1_ReadByte(0x5B+182);
            }
        
        	/*Display -- For Debug*/ //Probably faster to do all comm at once	

        	/*Cast the 16 bit Temp value to 32 bits to print over UART while preserving sign*/
            decTemp[i] = iTemp[i];
            
            /*format string to print out over UART*/
            sprintf(printBuf, "Temp=%ld", decTemp[i]); 
    		
            /*Print result over UART*/
            UART_PutString(printBuf);
            UART_PutString("\n\r");
            
            i++;
        }
    }
}

/* [] END OF FILE */
