# Vitis DSP IP Library

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