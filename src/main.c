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
#include "kernel.h"

#ifdef NO_OS_AVAILABLE

void
returnZeroNoOS(void);
#endif

int
main(int argc, char *  argv[])
{
	CommandLineArguments arguments = { 0 };

	double *                    monteCarloOutputSamples = NULL;
	clock_t                     start   = 0;
	clock_t                     end     = 0;
	double                      cpuTimeUsedSeconds;
	double                      outputVariables[kTexasInstrumentsTMCS112xOutputVariableIndexMax] = { 0 };
	const char *                outputVariableNames[kTexasInstrumentsTMCS112xOutputVariableIndexMax] = {
		"calibratedSensorOutput"
	};
	const char *                outputVariableDescriptions[kTexasInstrumentsTMCS112xOutputVariableIndexMax] = {
		"Calibrated sensor output"
	};
	kOutputVariableTypeIndex    outputVariableTypes[kTexasInstrumentsTMCS112xOutputVariableIndexMax] = {
		[kTexasInstrumentsTMCS112xOutputVariableIndexCalibratedCurrent] = kOutputVariableTypeDistribution,
	};
	const char *                applicationDescription = "Texas Instruments TMCS112x Conversion Routines";

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
			__LINE__
		);
	}

	const bool isScalarOutputSelected = isSelectedOutputScalar(
		&arguments.common,
		outputVariableTypes,
		kTexasInstrumentsTMCS112xOutputVariableIndexMax
	);

	/*
	 *	Start timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		start = clock();
	}

	/*
	 *	Dispatch to the mode-specific kernel. The Monte Carlo loop lives
	 *	inside `TexasInstrumentsTMCS112xCalculateOutputMonteCarlo`. UxHw mode runs a single
	 *	distributional evaluation inside `TexasInstrumentsTMCS112xCalculateOutputUxHw`.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		TexasInstrumentsTMCS112xCalculateOutputMonteCarlo(
			arguments.common.numberOfMonteCarloIterations,
			outputVariables,
			monteCarloOutputSamples
		);

		/*
		 *	Approximate the cost of the third phase of Monte Carlo
		 *	(post-processing), by calculating the mean and variance.
		 */
		const MeanAndVariance meanAndVariance = calculateMeanAndVarianceOfDoubleSamples(
			monteCarloOutputSamples,
			arguments.common.numberOfMonteCarloIterations
		);
		outputVariables[arguments.common.outputSelect] = meanAndVariance.mean;
	}
	else
	{
		TexasInstrumentsTMCS112xCalculateOutputUxHw(outputVariables);
	}

	/*
	 *	Stop timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		end                 = clock();
		cpuTimeUsedSeconds  = ((double) (end - start)) / CLOCKS_PER_SEC;
	}

	/*
	 *	Print the results (either in JSON or standard output format).
	 */
	if (arguments.common.isOutputJSONMode)
	{
		printJSONFormattedOutput(
			&arguments.common,
			monteCarloOutputSamples,
			outputVariables,
			outputVariableNames,
			kTexasInstrumentsTMCS112xOutputVariableIndexMax,
			applicationDescription,
			outputVariableTypes
		);
	}
	else
	{
		printHumanConsumableOutput(
			&arguments.common,
			kTexasInstrumentsTMCS112xOutputVariableIndexMax,
			outputVariables,
			outputVariableNames,
			outputVariableDescriptions,
			monteCarloOutputSamples,
			outputVariableTypes
		);
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
				outputVariables,
				outputVariableNames,
				kTexasInstrumentsTMCS112xOutputVariableIndexMax
		))
		{
			return kCommonConstantReturnTypeError;
		}
	}

	/*
	 *	Save Monte carlo outputs in an output file.
	 *	Free dynamically-allocated memory.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		saveMonteCarloDoubleDataToDataDotOutFile(
			monteCarloOutputSamples, (uint64_t) (cpuTimeUsedSeconds * 1000000),
			isScalarOutputSelected ? 1 : arguments.common.numberOfMonteCarloIterations
		);

		free(monteCarloOutputSamples);
	}

#ifdef NO_OS_AVAILABLE
	returnZeroNoOS();
#else

	return 0;

#endif
}
