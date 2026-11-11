# Source Code:

## `main.c`
Implementation of the main functionality of the application.

## `kernel.c` and `kernel.h`
Implementation of the calibrated sensor output calculation for the TMCS1123x3A sensor.
It dispatches to either the UxHw® or the Monte Carlo implementation.

## `utilities.c` and `utilities.h`
These contain utility methods for parsing, setting, and reporting
the usage of demo-specific command-line arguments of C/C++ demo applications.
These methods call similar methods from `common.c` for handling
command-line arguments common to all of our C/C++ demo applications.

## `common.c` and `common.h`
These contain utility methods for parsing, setting, and reporting
the usage of command-line arguments common to all of our C/C++ demo applications,
as well as other methods that we commonly make use across our
C/C++ demo applications, e.g., standard methods for I/O handling. These
source files are symlinks to the original files contained in the repository
[Signaloid-Demo-CommonUtilityRoutines](https://github.com/signaloid/Signaloid-Demo-CommonUtilityRoutines)
which is included as a submodule in `submodules/common`.

## `uxhw.c` and `uxhw.h`
These contain methods that implement the probabilistic versions of the methods
in the UxHw API (for example, `UxHwDoubleGaussDist`) and uses the GNU Scientific Library (GSL)
random number generators to achieve that. This allows building our C/C++ demo applications
natively (i.e., on conventional architectures) and running native Monte Carlo evaluations
of our C/C++ demo applications without modifying the source code.
These source files are symlinks to the original files and are contained in the repository
[Signaloid-Demo-UxHwCompatibilityForNativeExecution](https://github.com/signaloid/Signaloid-Demo-UxHwCompatibilityForNativeExecution)
which is included as a submodule in `submodules/compat`.

## `config.mk`
Signaloid cores use this file to identify the source codes they will use when
building the C/C++ demo application.

# To Build Natively on Non-Signaloid Platforms

From the repository root, build with:
```
make local-build
```

This produces the native Monte Carlo executable `demo-native-mc` at the repository
root. See the top-level `README.md` for how to install the build dependencies.
