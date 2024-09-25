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

/*
 *	We use the specific parameters for the TMCS1123x3A, taken from Table 6.6
 *	in page 7 of TMCS1123 Datasheet, 2024-07-04.
 */
#define kSensorCalibrationConstantTMCS1123x3ASensitivity	(0.075)

#define kDefaultInputDistributionVoutUniformDistLow		(3.2)
#define kDefaultInputDistributionVoutUniformDistHigh		(3.4)
#define kDefaultInputDistributionVrefUniformDistLow		(2.4)
#define kDefaultInputDistributionVrefUniformDistHigh		(2.6)

/*
 *	Input Distributions:
 *		kInputDistributionIndexVout	: Analog Output Voltage (in Volt)
 *		kInputDistributionIndexVref	: Zero Current Reference Output Voltage(in Volt)
 */
typedef enum
{
	kInputDistributionIndexVout	= 0,
	kInputDistributionIndexVref	= 1,
	kInputDistributionIndexMax,
} InputDistributionIndex;

/*
 *	Output Distribution:
 *		kOutputDistributionIndexCalibratedCurrent	: Calibrated Current (in Ampere)
 */
typedef enum
{
	kOutputDistributionIndexCalibratedCurrent	= 0,
	kOutputDistributionIndexMax,
} OutputDistributionIndex;
