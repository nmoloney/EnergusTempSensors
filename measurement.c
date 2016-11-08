/******************************************************************************
* File Name: measurement.c
*  Version 2.0
*
* Description:
*  This file contains the functions that are used to find the tempzenor voltage.
* 
* Note:
*
*******************************************************************************/
#include "measurement.h"
#include <device.h>

/*******************************************************************************
*  Function Name: MeasureResistorVoltage
********************************************************************************
* Summary:
* This functions returns the voltage across the resistor. Either Thermistor or Reference 
*
* Parameters:
* none
*
* Return: 
* resistor voltage in ADC counts

* Theory:
* Measure voltage across the resistor. Subtract offset from value, and filter value
*
* Side Effects:
* None
*******************************************************************************/
int16 MeasureSensorVoltage(uint8 channel)
{
	/* Voltage across resistor*/
	int16 result;
	
	/* Read voltage across  resistor*/
    ADC_SAR_Seq_1_StartConvert();
	
	ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
	result = ADC_SAR_Seq_1_GetResult16(channel);   
	
    result = ADC_SAR_Seq_1_CountsTo_mVolts(result);
    //TODO - Add a lowpass filter as described in http://www.cypress.com/file/127091/download

	return result;
	
}
