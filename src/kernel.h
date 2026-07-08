/*
 *	Copyright (c) 2024, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#pragma once


/*
 *	We use the specific parameters for the TMCS1123x3A, taken from Table 6.6
 *	in page 7 of TMCS1123 Datasheet, 2024-07-04.
 */
#define kTexasInstrumentsTMCS112xSensorCalibrationConstantTMCS1123x3ASensitivity (0.075)

#define kTexasInstrumentsTMCS112xDefaultInputDistributionVoutUniformDistLow     (3.2)
#define kTexasInstrumentsTMCS112xDefaultInputDistributionVoutUniformDistHigh    (3.4)
#define kTexasInstrumentsTMCS112xDefaultInputDistributionVrefUniformDistLow     (2.4)
#define kTexasInstrumentsTMCS112xDefaultInputDistributionVrefUniformDistHigh    (2.6)

/*
 *	Input Variables:
 *		kInputVariableIndexVout	: Analog Output Voltage (in Volt)
 *		kInputVariableIndexVref	: Zero Current Reference Output Voltage (in Volt)
 */
typedef enum
{
	kTexasInstrumentsTMCS112xInputVariableIndexVout = 0,
	kTexasInstrumentsTMCS112xInputVariableIndexVref = 1,
	kTexasInstrumentsTMCS112xInputVariableIndexMax,
} TexasInstrumentsTMCS112xInputVariableIndex;

/*
 *	Output Variable:
 *		kOutputVariableIndexCalibratedCurrent	: Calibrated Current (in Ampere)
 */
typedef enum
{
	kTexasInstrumentsTMCS112xOutputVariableIndexCalibratedCurrent = 0,
	kTexasInstrumentsTMCS112xOutputVariableIndexMax,
} TexasInstrumentsTMCS112xOutputVariableIndex;

/**
 *	@brief	Sensor calibration routine for TMCS1123x3A, taken from Section 7.1
 *		in page 13 of tmcs1123.pdf, 2024-07-04.
 *
 *	@param	inputVariables		: The array of input variables used in the calculation.
 *	@param	outputVariables		: An array of output variables. Writes the result to
 *					  `outputVariables[kOutputVariableIndexCalibratedCurrent]`.
 *	@return	double			: Returns the distributional value calculated.
 */
double
TexasInstrumentsTMCS112x_calculateOutput(double * inputVariables, double *  outputVariables);
