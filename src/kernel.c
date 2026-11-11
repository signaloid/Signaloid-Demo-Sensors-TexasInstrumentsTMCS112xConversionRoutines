/*
 *	Copyright (c) 2024-2026, Signaloid.
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

#include <stddef.h>
#include <uxhw.h>
#include "kernel.h"


double
TexasInstrumentsTMCS112x_calculateOutput(double * inputVariables, double *  outputVariables)
{
	double  Vref;
	double  Vout;
	double  calibratedValue;

	Vref    = inputVariables[kTexasInstrumentsTMCS112xInputVariableIndexVref];
	Vout    = inputVariables[kTexasInstrumentsTMCS112xInputVariableIndexVout];

	calibratedValue = (Vout - Vref) / kTexasInstrumentsTMCS112xSensorCalibrationConstantTMCS1123x3ASensitivity;
	outputVariables[kTexasInstrumentsTMCS112xOutputVariableIndexCalibratedCurrent] = calibratedValue;

	return calibratedValue;
}

/*
 *	Sets the input variables via calls to the UxHw parametric functions. In
 *	UxHw mode these return distributions. In native Monte Carlo mode they
 *	return one sample each.
 */
static void
TexasInstrumentsTMCS112xSetInputVariablesViaUxHwCall(double * inputVariables)
{
	inputVariables[kTexasInstrumentsTMCS112xInputVariableIndexVout] = UxHwDoubleUniformDist(
		kTexasInstrumentsTMCS112xDefaultInputDistributionVoutUniformDistLow,
		kTexasInstrumentsTMCS112xDefaultInputDistributionVoutUniformDistHigh
	);

	inputVariables[kTexasInstrumentsTMCS112xInputVariableIndexVref] = UxHwDoubleUniformDist(
		kTexasInstrumentsTMCS112xDefaultInputDistributionVrefUniformDistLow,
		kTexasInstrumentsTMCS112xDefaultInputDistributionVrefUniformDistHigh
	);

	return;
}

double
TexasInstrumentsTMCS112xCalculateOutputUxHw(double * outputVariables)
{
	double inputVariables[kTexasInstrumentsTMCS112xInputVariableIndexMax];

	TexasInstrumentsTMCS112xSetInputVariablesViaUxHwCall(inputVariables);

	return TexasInstrumentsTMCS112x_calculateOutput(inputVariables, outputVariables);
}

double
TexasInstrumentsTMCS112xCalculateOutputMonteCarlo(
	size_t      numberOfMonteCarloIterations,
	double *    outputVariables,
	double *    monteCarloOutputSamples)
{
	double  inputVariables[kTexasInstrumentsTMCS112xInputVariableIndexMax];
	double  calibratedSensorOutput = 0.0;

	for (size_t ii = 0; ii < numberOfMonteCarloIterations; ii++)
	{
		/*
		 *	Draw fresh input samples on every iteration.
		 */
		TexasInstrumentsTMCS112xSetInputVariablesViaUxHwCall(inputVariables);

		calibratedSensorOutput      = TexasInstrumentsTMCS112x_calculateOutput(inputVariables, outputVariables);
		monteCarloOutputSamples[ii] = calibratedSensorOutput;
	}

	return calibratedSensorOutput;
}
