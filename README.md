[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-dark-latest.png#gh-dark-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=https://github.com/signaloid/Signaloid-Demo-Sensors-TexasInstrumentsTMCS112xConversionRoutines#gh-dark-mode-only)
[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-light-latest.png#gh-light-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=https://github.com/signaloid/Signaloid-Demo-Sensors-TexasInstrumentsTMCS112xConversionRoutines#gh-light-mode-only)

# Texas Instruments TMCS112x Precision Hall-Effect Current Sensor Use Case
Example demonstrating real-time output uncertainty estimation for calibrated ADC values from the TMCS112x sensor[^1].

## Introduction
The TMCS1123 is a precision Hall-effect current sensor.

## Getting started

The correct way to clone this repository to get the submodules is:
```sh
	git clone --recursive git@github.com:signaloid/Signaloid-Demo-Sensors-TexasInstrumentsTMCS112xConversionRoutines.git
```

If you forgot to clone with `--recursive` and end up with empty submodule directories, you can remedy this with:
```sh
	git submodule update --init
```

## Running the application locally
Apart from using Signaloid's Cloud Compute Platform, you can compile and run this application
locally. Local execution is essentially a native Monte Carlo implementation,
that uses GNU Scientific Library (GSL)[^2] to generate samples for the different input distributions.
In this mode the application stores the generated output samples, in a file called `data.out`.
The first line of `data.out` contains the execution time of the Monte Carlo implementation
in microseconds (μs), and each
next line contains a floating-point value corresponding to an output sample value.

In order to compile and run this application in the native Monte Carlo mode:

0. Install dependencies (e.g., on Linux):
```
sudo apt-get install libgsl-dev libgslcblas0
```
1. Compile natively (e.g., on Linux):
```
cd src/
gcc -I. -I/opt/local/include main.c utilities.c common.c uxhw.c -L/opt/local/lib -o native-exe -lgsl -lgslcblas -lm
```
2. Run the application in the MonteCarlo mode, using (`-M`) command-line option:
```
./native-exe -M 10000
```
The above program runs 10000 Monte Carlo iterations.
3. See the output samples generated by the local Monte Carlo execution:
```
cat data.out
```

## Inputs
The inputs to the TMCS1123x3A sensor conversion algorithms are the analog output voltage of the sensor ($V_{\mathrm{out}}$)
as well as the zero current reference output voltage ($V_{\mathrm{ref}}$). The algorithm models the ADC quantization
error as well as the uncertainty in the supply voltage using uniform distributions.

The uncertainty in $V_{\mathrm{out}}$ is modeled as a (`UniformDist(3.2, 3.4)`) Volts.

The uncertainty in $V_{\mathrm{ref}}$ is modeled as a (`UniformDist(2.4, 2.6)`) Volts.


## Outputs
The output is the calibrated electric current (in Ampere):
```math
\mathrm{Calibrated Sensor Output} = \frac{V_{\mathrm{out}} - V_{\mathrm{ref}}}{S}
```
where S is the sensitivity of the specific device and is modeled as a constant:
```math
\mathrm{S} = 0.075
```

Following is an example output, using Signaloid's C0Pro-S core.

![Example output plot](./docs/plots/output-C0-S.png)


## Usage
```
Example: TMCS1123x3A sensor conversion routines - Signaloid version

Usage: Valid command-line arguments are:
	[-o, --output <Path to output CSV file : str>] (Specify the output file.)
	[-S, --select-output <output : int>] (Compute 0-indexed output, by default 0.)
	[-M, --multiple-executions <Number of executions : int (Default: 1)>] (Repeated execute kernel for benchmarking.)
	[-T, --time] (Timing mode: Times and prints the timing of the kernel execution.)
	[-b, --benchmarking] (Benchmarking mode: Generate outputs in format for benchmarking.)
	[-j, --json] (Print output in JSON format.)
	[-h, --help] (Display this help message.)
```


---

[^1]: [TMCS1123 Precision 250kHz Hall-Effect Current Sensor With ±1.3kV Reinforced Isolation Working Voltage, Overcurrent Detection and Ambient Field Rejection](https://www.ti.com/lit/ds/symlink/tmcs1123.pdf).

[^2]: [GNU Scientific Library](https://www.gnu.org/software/gsl/).

