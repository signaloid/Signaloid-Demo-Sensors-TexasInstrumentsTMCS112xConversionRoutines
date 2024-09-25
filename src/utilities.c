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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uxhw.h>
#include "utilities.h"

void
printUsage(void)
{
	fprintf(stderr, "Example: TMCS1123x3A sensor conversion routines - Signaloid version\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: Valid command-line arguments are:\n");
	fprintf(
		stderr,
		"\t[-o, --output <Path to output CSV file : str>] (Specify the output file.)\n"
		"\t[-S, --select-output <output : int>] (Compute 0-indexed output, by default 0.)\n"
		"\t[-M, --multiple-executions <Number of executions : int (Default: 1)>] (Repeated execute kernel for benchmarking.)\n"
		"\t[-T, --time] (Timing mode: Times and prints the timing of the kernel execution.)\n"
		"\t[-b, --benchmarking] (Benchmarking mode: Generate outputs in format for benchmarking.)\n"
		"\t[-j, --json] (Print output in JSON format.)\n"
		"\t[-h, --help] (Display this help message.)\n");
	fprintf(stderr, "\n");

	return;
}

static void
setDefaultCommandLineArguments(CommandLineArguments *  arguments)
{
	/*
	 *	Older GCC versions have a bug which gives a spurious warning for
	 *	the C universal zero initializer `{0}`. Any workaround makes the
	 *	code less portable or prevents the common code from adding new
	 *	fields to the `CommonCommandLineArguments` struct. Therefore, we
	 *	surpress this warning.
	 *
	 *	See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119.
	 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

	*arguments = (CommandLineArguments)
	{
		.common = (CommonCommandLineArguments) {0}
	};
#pragma GCC diagnostic pop

	return;
}

CommonConstantReturnType
getCommandLineArguments(
	int			argc,
	char *			argv[],
	CommandLineArguments *	arguments)
{
	DemoOption		demoSpecificOptions = {0};

	if (arguments == NULL)
	{
		fprintf(stderr, "Arguments pointer is NULL.\n");
		return kCommonConstantReturnTypeError;
	}

	setDefaultCommandLineArguments(arguments);

	/*
	 *	This application example has no application specific arguments.
	 */
	if (parseArgs(argc, argv, &arguments->common, &demoSpecificOptions) != 0)
	{
		fprintf(stderr, "Parsing command line arguments failed\n");
		printUsage();

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Process command-line arguments
	 */
	if (arguments->common.isHelpEnabled)
	{
		printUsage();

		exit(EXIT_SUCCESS);
	}

	if (arguments->common.isInputFromFileEnabled)
	{
		fprintf(stderr, "Reading inputs from CSV file is not currently supported\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Write to output file is not supported in MonteCarlo Mode.
	 */
	if (arguments->common.isWriteToFileEnabled && arguments->common.isMonteCarloMode)
	{
		fprintf(stderr, "Writing to output file is not supported in MonteCarlo Mode.\n");

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isVerbose)
	{
		fprintf(stderr, "Warning: Verbose mode not supported. Continuing in non-verbose mode.\n");
	}

	/*
	 *	If no output selected from CLA, set the default output select value.
	 */
	if (!arguments->common.isOutputSelected)
	{
		arguments->common.outputSelect = kOutputDistributionIndexCalibratedCurrent;
	}

	if (arguments->common.outputSelect >= kOutputDistributionIndexMax)
	{
		fprintf(
			stderr,
			"Output select value (-S option) should be less than the possible number of outputs: Provided %zd. Max: %d\n",
			arguments->common.outputSelect,
			kOutputDistributionIndexMax - 1);

		return kCommonConstantReturnTypeError;
	}

	return kCommonConstantReturnTypeSuccess;
}

void
printCalibratedValueAndProbabilities(double calibratedSensorOutput)
{
	/*
	 *	Note: the calculations of the quantities involving UxHwDoubleProbabilityGT()
	 *	are purposefully written so as to be self-explanatory and easily checkable,
	 *	not for efficiency or "cleverness". Also, beware the "percent greater than"
	 *	and "percent less than" are tricky for larger versus smaller so don't jump
	 *	to conclusions when you read the code.
	 */
	printf("Calibrated sensor output: %.2lf A.\n", calibratedSensorOutput);
	printf("\n");
	printf(
		"\tProbability that calibrated sensor output is   1%% or more smaller than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		1 - UxHwDoubleProbabilityGT(calibratedSensorOutput, calibratedSensorOutput * (1 - 0.01)));
	printf(
		"\tProbability that calibrated sensor output is   5%% or more smaller than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		1 - UxHwDoubleProbabilityGT(calibratedSensorOutput, calibratedSensorOutput * (1 - 0.05)));
	printf(
		"\tProbability that calibrated sensor output is  10%% or more smaller than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		1 - UxHwDoubleProbabilityGT(calibratedSensorOutput, calibratedSensorOutput * (1 - 0.1)));
	printf(
		"\tProbability that calibrated sensor output is  15%% or more smaller than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		1 - UxHwDoubleProbabilityGT(calibratedSensorOutput, calibratedSensorOutput * (1 - 0.15)));
	printf("\n");
	printf(
		"\tProbability that calibrated sensor output is   1%% or more greater than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		UxHwDoubleProbabilityGT(calibratedSensorOutput, 1.01 * calibratedSensorOutput));
	printf(
		"\tProbability that calibrated sensor output is   5%% or more greater than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		UxHwDoubleProbabilityGT(calibratedSensorOutput, 1.05 * calibratedSensorOutput));
	printf(
		"\tProbability that calibrated sensor output is  10%% or more greater than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		UxHwDoubleProbabilityGT(calibratedSensorOutput, 1.1 * calibratedSensorOutput));
	printf(
		"\tProbability that calibrated sensor output is  15%% or more greater than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		UxHwDoubleProbabilityGT(calibratedSensorOutput, 1.15 * calibratedSensorOutput));

	return;
}

void
printJSONFormattedOutput(
	CommandLineArguments *	arguments,
	double *		outputVariable,
	double *		monteCarloOutputSamples,
	const char *		variableDescription)
{
	/*
	 *	If in Monte Carlo mode, `pointerToOutputVariable` points to the beginning
	 *	of the `monteCarloOutputSamples` array. In this case, `arguments.common.numberOfMonteCarloIterations`
	 *	is the length of the `monteCarloOutputSamples` array. If not in Monte Carlo mode,
	 *	`pointerToOutputVariable` points to the `outputVariable` to be used.
	 */
	double *	pointerToOutputVariable = (arguments->common.isMonteCarloMode) ? monteCarloOutputSamples : outputVariable;
	/*
	 *	Print json formatted output.
	 */
	JSONVariable variables[] =
	{
		{
			.variableSymbol = "calibratedSensorOutput",
			.variableDescription = "",
			.values = (JSONVariablePointer){ .asDouble = pointerToOutputVariable },
			.type = kJSONVariableTypeDouble,
			.size = arguments->common.numberOfMonteCarloIterations
		},
	};

	strncpy(
		variables[kOutputDistributionIndexCalibratedCurrent].variableDescription,
		variableDescription,
		kCommonConstantMaxCharsPerJSONVariableDescription);

	printJSONVariables(
		variables,
		kOutputDistributionIndexMax,
		"TMCS1123x3A Sensor Calibration Use Case");

	return;
}
