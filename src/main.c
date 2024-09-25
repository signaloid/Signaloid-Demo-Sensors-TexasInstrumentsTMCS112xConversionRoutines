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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <uxhw.h>
#include <string.h>
#include "utilities.h"

/**
 *	@brief  Sets the Input Distributions via call to UxHw Parametric function.
 *
 *	@param  inputDistributions	: An array of double values, where the function writes the distributional data.
 */
void
setInputDistributionsViaUxHwCall(double *  inputDistributions)
{
	inputDistributions[kInputDistributionIndexVout] = UxHwDoubleUniformDist(
								kDefaultInputDistributionVoutUniformDistLow,
								kDefaultInputDistributionVoutUniformDistHigh);

	inputDistributions[kInputDistributionIndexVref] = UxHwDoubleUniformDist(
								kDefaultInputDistributionVrefUniformDistLow,
								kDefaultInputDistributionVrefUniformDistHigh);

	return;
}

/**
 *	@brief  Sensor calibration routine for TMCS1123x3A, taken from Section 7.1
 *		in page 13 of tmcs1123.pdf, 2024-07-04.
 *
 *	@param  inputDistributions	: The array of input distributions used in the calculation.
 * 	@param  outputDistributions	: An array of of output distributions. Writes the result to `outputDistributions[outputSelectValue]`.
 *
 *	@return	double			: Returns the distributional value calculated.
 */
static double
calculateSensorOutput(double *  inputDistributions, double *  outputDistributions)
{
	double	Vref;
	double	Vout;
	double	calibratedValue;

	Vref = inputDistributions[kInputDistributionIndexVref];
	Vout = inputDistributions[kInputDistributionIndexVout];

	calibratedValue = (Vout - Vref) / kSensorCalibrationConstantTMCS1123x3ASensitivity;
	outputDistributions[kOutputDistributionIndexCalibratedCurrent] = calibratedValue;

	return	calibratedValue;
}

int
main(int argc, char *  argv[])
{
	CommandLineArguments	arguments = {0};

	double			calibratedSensorOutput;
	double *		monteCarloOutputSamples = NULL;
	clock_t			start;
	clock_t			end;
	double			cpuTimeUsedSeconds;
	double			inputDistributions[kInputDistributionIndexMax];
	double			outputDistributions[kOutputDistributionIndexMax];
	const char *		outputVariableNames[kOutputDistributionIndexMax] =
				{
					"calibratedSensorOutput"
				};
	MeanAndVariance		meanAndVariance;

	/*
	 *	Get command line arguments.
	 */
	if (getCommandLineArguments(argc, argv, &arguments))
	{
		return kCommonConstantReturnTypeError;
	}

	if (arguments.common.isMonteCarloMode)
	{
		monteCarloOutputSamples = (double *) checkedMalloc(
							arguments.common.numberOfMonteCarloIterations * sizeof(double),
							__FILE__,
							__LINE__);
	}

	/*
	 *	Start timing.
	 */
	if (arguments.common.isTimingEnabled || arguments.common.isBenchmarkingMode)
	{
		start = clock();
	}

	for (size_t i = 0; i < arguments.common.numberOfMonteCarloIterations; i++)
	{
		/*
		 *	Set input distribution values, inside the main computation
		 *	loop, so that it can also generate samples in the native
		 *	Monte Carlo Execution Mode.
		 */
		setInputDistributionsViaUxHwCall(inputDistributions);

		calibratedSensorOutput = calculateSensorOutput(inputDistributions, outputDistributions);

		/*
		 *	For this application, calibratedSensorOutput is the item we track.
		 */
		if (arguments.common.isMonteCarloMode)
		{
			monteCarloOutputSamples[i] = calibratedSensorOutput;
		}
	}

	/*
	 *	If not doing Laplace version, then approximate the cost of the third phase of
	 *	Monte Carlo (post-processing), by calculating the mean and variance.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		meanAndVariance = calculateMeanAndVarianceOfDoubleSamples(
					monteCarloOutputSamples,
					arguments.common.numberOfMonteCarloIterations);
		calibratedSensorOutput = meanAndVariance.mean;
	}

	/*
	 *	Stop timing.
	 */
	if (arguments.common.isTimingEnabled || arguments.common.isBenchmarkingMode)
	{
		end = clock();
		cpuTimeUsedSeconds = ((double)(end - start)) / CLOCKS_PER_SEC;
	}

	if (arguments.common.isBenchmarkingMode)
	{
		/*
		 *	In benchmarking mode, we print:
		 *		(1) single result (for calculating Wasserstein distance to reference)
		 *		(2) time in microseconds (benchmarking setup expects cpu time in microseconds)
		 */
		printf("%lf %" PRIu64 "\n", calibratedSensorOutput, (uint64_t)(cpuTimeUsedSeconds*1000000));
	}
	else
	{
		/*
		 *	Print the results (either in JSON or standard output format).
		 */
		if (!arguments.common.isOutputJSONMode)
		{
			printCalibratedValueAndProbabilities(calibratedSensorOutput);
		}
		else
		{
			printJSONFormattedOutput(
				&arguments,
				&outputDistributions[kOutputDistributionIndexCalibratedCurrent],
				monteCarloOutputSamples,
				outputVariableNames[kOutputDistributionIndexCalibratedCurrent]);
		}

		/*
		 *	Print timing result.
		 */
		if (arguments.common.isTimingEnabled)
		{
			printf("\nCPU time used: %lf seconds\n", cpuTimeUsedSeconds);
		}

		/*
		 *	Write output data.
		 */
		if (arguments.common.isWriteToFileEnabled)
		{
			if (writeOutputDoubleDistributionsToCSV(
				arguments.common.outputFilePath,
				outputDistributions,
				outputVariableNames,
				kOutputDistributionIndexMax))
			{
				return kCommonConstantReturnTypeError;
			}
		}
	}

	/*
	 *	Save Monte carlo outputs in an output file.
	 *	Free dynamically-allocated memory.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		saveMonteCarloDoubleDataToDataDotOutFile(monteCarloOutputSamples, (uint64_t)(cpuTimeUsedSeconds*1000000), arguments.common.numberOfMonteCarloIterations);

		free(monteCarloOutputSamples);
	}

	return 0;
}
