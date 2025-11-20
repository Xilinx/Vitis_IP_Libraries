# Vitis DSP IP Library

## License
Licensed using the [Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0).

    Copyright (C) 2019-2022, Xilinx, Inc.
    Copyright (C) 2022-2025, Advanced Micro Devices, Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

## Notice

NOTICE - If the user has already downloaded AMD Vivado™ Design Suite, the user may, at its option, 
activate the vss_fft_ifft_1d IP generator that invokes parallel_fft IP in Vivado. The user acknowledges and agrees that 
(1) use of parallel_fft is governed by the Xilinx, Inc. End User License Agreement applicable to the version that the user has installed (the "Vivado EULA"),
(2) the parallel_fft is a “Distributable Component” (as defined in the Vivado EULA), and
(3) if the user compiles parallel_fft together with the open source files
 offered in this GitHub repo, any use of such compiled product must comply with the terms of the Vivado EULA .

## Overview

Vitis IP DSP library provides implementation of different L1/L2/L3 primitives for digital signal processing.
Current version provides:
- L2 level VSS implementation of FFT.
- L2 level AIE C++ graph implementation of FIRs.

## Source Files and Application Development
Vitis library is organized into L1, L2, and L3 folders, each relating to a different stage of application development.

**L1** :
      Makefiles and sources in L1 facilitate HLS based flow for quick checks. Tasks at this level include:

* Check the functionality of an individual kernel (C-simulation)
* Estimate resource usage, latency, etc. (Synthesis)
* Run cycle accurate simulations (Co-simulation)
* Package as IP and get final resource utilization/timing details (Export RTL)

	**Note**:  Once RTL (or XO file after packaging IP) is generated, the Vivado flow is invoked for XCLBIN file generation if required.

**L2** :
       Makefiles and sources in L2 facilitate building XCLBIN file from various sources (HDL, HLS or XO files) of kernels with host code written in OpenCL/XRT framework targeting a device. This flow supports:

* AIE x86 Functional Simulation for rapid AIE functionality check
* AIE SystemC Simulation for cycle approximate AIE performance check
* Software emulation to check the functionality
* Hardware emulation to simulate the entire system, including AI Engine graph and PL logic along with XRT-based host application to control the AI Engine and PL
* Build and test on hardware


## License

Copyright (C) 2019-2022, Xilinx, Inc.
Copyright (C) 2022-2025, Advanced Micro Devices, Inc.

Terms and Conditions <https://www.amd.com/en/corporate/copyright>