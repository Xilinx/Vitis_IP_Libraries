..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2026, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. _COMPILING_AND_SIMULATING:

************************
Compiling and Simulating
************************

**Prerequisites**:

.. code-block::

        source <your-Vitis-install-path>/lin64/HEAD/Vitis/settings64.csh
        setenv PLATFORM_REPO_PATHS <your-platform-repo-install-path>
        source <your-XRT-install-path>/xbb/xrt/packages/xrt-2.1.0-centos/opt/xilinx/xrt/setup.csh

Library Element Unit Test
--------------------------

Each library element category comes supplied with a test harness.

For AI Engine library elements, the test harness resides in the `L2/tests/aie/<library_element>` directory and consists of JSON and C++ files, and a Makefile.

For VSS library elements, the test harness resides in the `L2/tests/vss/<library_element>` directory and contains a host file, helper Python scripts, and the JSON, C++ files, and Makefiles.

The JSON description of the test harness, defined in `L2/tests/<vss/aie>/<library_element>/description.json`, generates the Makefile. The `description.json` file also defines the parameters of the test harness, for example, a list of supported platforms.

Each Makefile uses a set of values for each library element parameter, stored in a JSON file at `L2/tests/aie/<library_element>/multi_params.json`. The parameters are combined into a named test case — the default name is `test_0_tool_canary_aie`. Edit the parameters as required to configure the library element for your needs.

C++ files serve as an example of how to use the library element subgraph in the context of a super-graph. These test harnesses (graphs) reside in the `L2/tests/aie/<library_element>/test.hpp` and `L2/tests/aie/<library_element>/test.cpp` files.

For AI Engine library elements, instantiate only L2 (graphs) library elements directly in your code. The kernels underlying the graphs reside in the `L1/include/aie/<library_element>.hpp` and `L1/src/aie/<library_element>.cpp` files.

For VSS library elements, use the top-level VSS Makefile found in `L2/include/vss/<library_element>` as the entry point.

The test harness run consists of the following steps:

- Generate input files.
- Validate configuration with metadata (in `L2/meta`).
- Compile and simulate the reference model to produce the `golden output`.
- Compile and simulate the unit under test (UUT) design.
- Post-process output (for example, process timestamps to produce throughput figures). The reference model output (`logs/ref_output.txt`) is verified against the AI Engine graphs output (`logs/uut_output.txt`).
- Generate status. On completion, `logs/status_<config_details>.txt` contains the compilation and simulation result, and indicates whether the reference and AI Engine model outputs match. The report also contains resource utilization and performance metrics.

Compiling Using the Makefile
----------------------------

Running Compilation
^^^^^^^^^^^^^^^^^^^

Use the following command to compile and simulate the reference model with the x86sim target, then compile and simulate the library element graph:

.. code-block::

        make cleanall run PLATFORM=vck190

.. note:: Run a ``cleanall`` stage before compiling the design, to ensure no stale objects interfere with the compilation process.

.. note:: Platform information (for example, PLATFORM=vck190) is required by the make build process. Supported platforms are listed in `L2/tests/aie/<library_element>/description.json` under the "platform_allowlist" key.

Configuring the Test Case
^^^^^^^^^^^^^^^^^^^^^^^^^

To overwrite the default set of parameters, edit the `multi_params.json` file, and add a dedicated named test case or edit one of the existing ones, for example:

.. code-block::

    "test_my_design":{
        "DATA_TYPE": "cint32",
        "COEFF_TYPE": "int32",
        (...)
        }

To run a test case, specify the test case name passed to the PARAMS argument, for example:

.. code-block::

        make cleanall run PLATFORM=vck190 PARAMS=test_my_design

For a list of all configurable parameters, refer to :ref:`CONFIGURATION_PARAMETERS`.

Selecting TARGET
^^^^^^^^^^^^^^^^

To perform an x86 compilation/simulation, run:

.. code-block::

    make run TARGET=x86sim.

The following list describes all Makefile targets:

.. code-block::

    make all TARGET=<aiesim/x86sim/hw_emu/hw> PLATFORM=<FPGA platform>
        Command to generate the design for specified Target and Shell.

    make run TARGET=<aiesim/x86sim/hw_emu/hw> PLATFORM=<FPGA platform>
        Command to run application in emulation.

    make clean
        Command to remove the generated non-hardware files.

    make cleanall
        Command to remove all the generated files.

.. note::
    For embedded platforms, the following setup steps are required:
        a. If the platform and common-image are downloaded from the Download Center (Suggested):
            | Run the `sdk.sh` script from the `common-image` directory to install sysroot using the command: ./sdk.sh -y -d ./ -p
            | Unzip the `rootfs` file : gunzip ./rootfs.ext4.gz
            | export SYSROOT=< path-to-platform-sysroot >
        b. You can also define SYSROOT, K_IMAGE, and ROOTFS individually:
            .. code-block::

                export SYSROOT=< path-to-platform-sysroot >
                export K_IMAGE=< path-to-Image-files >
                export ROOTFS=< path-to-rootfs >

Troubleshooting Compilation
---------------------------

Compilation Arguments
^^^^^^^^^^^^^^^^^^^^^

The test harness supplied with the library allows each library unit to be compiled and simulated in isolation. When you instantiate the library unit within your design, the compilation result may differ from the test harness result, because your system compilation may require additional arguments.

Search the Makefile for UUT_TARGET_COMPILE_ARGS. Each library element may have compile arguments that avoid errors or improve performance — for example, placing memories on separate banks to avoid wait states. These arguments are likely to change with each release as the compiler evolves.

Stack Size Allocation
^^^^^^^^^^^^^^^^^^^^^

The test harness also estimates the stack size required for a variety of cases and provides a formula to assign enough memory for stack purposes. When you instantiate the library unit within your design, compilation may fail due to insufficient stack for a specific kernel. The error message indicates the minimum stack size required.

Pass the compiler argument advised by the error message to allocate enough stack. Alternatively, search the Makefile for STACK_SIZE and use the formula provided to calculate and allocate an appropriate stack size.

Invalid Throughput and/or Latency
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Throughput and latency are only reported once stable operation is detected. Complex designs may take several iterations to reach a stable state. When a test case runs for too few iterations, the status report sets throughput and latency values to -1.

Increase the number of simulation iterations to reach a stable state and obtain accurate throughput and latency measurements.

Power Analysis
--------------

For DSPLIB elements, add the string `VCD` to the test name to harvest dynamic power consumption. This captures a VCD file of the simulation data, and Power Design Manager (PDM) calculates power metrics. Detailed power reports are in the `pwr_test` folder under the corresponding test result directory. The dynamic power result is also in the `logs/status_<config_details>.txt` file.

.. _CONFIGURATION_PARAMETERS:

Library Element Configuration Parameters
-----------------------------------------

.. _COMMON_CONFIG_PARAMETERS:

Common Configuration Parameters
---------------------------------

Many library elements perform arithmetic and offer a scaling feature through TP_SHIFT. During this operation, rounding and saturation can occur, controlled by TP_RND and TP_SAT. The modes and values for TP_RND are the same for AIE-ML and AIE-ML v2 devices, but differ from those for AIE devices, as shown in the following table.

.. table:: Common Configuration Parameters

    +------------------------+----------------+----------------+--------------------------------------+
    |     **Name**           |    **Type**    |  **Default**   |   Description                        |
    +========================+================+================+======================================+
    | SHIFT                  |    unsigned    |    8           | Acc results shift down value.        |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | ROUND_MODE             |    unsigned    |    0           | Rounding mode.                       |
    |                        |                |                |                                      |
    |                        |                |                +------------------+-------------------+
    |                        |                |                |     AIE          | AIE-ML or AIE-ML v2|
    |                        |                |                +------------------+-------------------+
    |                        |                |                |                  |                   |
    |                        |                |                | 0 - rnd_floor*   | 0 - rnd_floor*    |
    |                        |                |                |                  |                   |
    |                        |                |                | 1 - rnd_ceil*    | 1 - rnd_ceil*     |
    |                        |                |                |                  |                   |
    |                        |                |                | 2 - rnd_pos_inf  | 2 - rnd_sym_floor*|
    |                        |                |                |                  |                   |
    |                        |                |                | 3 - rnd_neg_inf  | 3 - rnd_sym_ceil* |
    |                        |                |                |                  |                   |
    |                        |                |                | 4 - rnd_sym_inf  | 8 - rnd_neg_inf   |
    |                        |                |                |                  |                   |
    |                        |                |                | 5 - rnd_sym_zero | 9 - rnd_pos_inf   |
    |                        |                |                |                  |                   |
    |                        |                |                | 6 - rnd_conv_even| 10 - rnd_sym_zero |
    |                        |                |                |                  |                   |
    |                        |                |                | 7 - rnd_conv_odd | 11 - rnd_sym_inf  |
    |                        |                |                |                  |                   |
    |                        |                |                |                  | 12 - rnd_conv_even|
    |                        |                |                |                  |                   |
    |                        |                |                |                  | 13 - rnd_conv_odd |
    |                        |                |                |                  |                   |
    +------------------------+----------------+----------------+------------------+-------------------+
    | SAT_MODE               |    unsigned    |    1           | Saturation mode.                     |
    |                        |                |                |                                      |
    |                        |                |                | 0 - none                             |
    |                        |                |                |                                      |
    |                        |                |                | 1 - saturate                         |
    |                        |                |                |                                      |
    |                        |                |                | 3 - symmetric saturate               |
    +------------------------+----------------+----------------+--------------------------------------+
    | NITER                  |    unsigned    |    8           | Number of iterations to execute.     |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | DIFF_TOLERANCE         |    unsigned    |    0           | Tolerance value when comparing       |
    |                        |                |                | output sample with reference model,  |
    |                        |                |                | for example, 0.0025 for floats and   |
    |                        |                |                | cfloats.                             |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | STIM_TYPE              |    unsigned    |    0           | Supported types:                     |
    |                        |                |                |                                      |
    |                        |                |                | 0: random                            |
    |                        |                |                |                                      |
    |                        |                |                | 3: impulse                           |
    |                        |                |                |                                      |
    |                        |                |                | 4: all ones                          |
    |                        |                |                |                                      |
    |                        |                |                | 5: incrementing pattern              |
    |                        |                |                |                                      |
    |                        |                |                | 6: sym incrementing pattern          |
    |                        |                |                |                                      |
    |                        |                |                | 8: sine wave                         |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | DATA_SEED              |    unsigned    |    1           | Seed used to generate random numbers |
    |                        |                |                | for the inputs.                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | NUM_OUTPUTS            |    unsigned    |    1           | Number of output ports.              |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | AIE_VARIANT            |    unsigned    |    1           | AI Engine variant to use for metadata|
    |                        |                |                | validation.                          |
    |                        |                |                | Ignored for compilation and          |
    |                        |                |                | simulation purposes.                 |
    |                        |                |                |                                      |
    |                        |                |                | 1: AIE                               |
    |                        |                |                |                                      |
    |                        |                |                | 2: AIE-ML                            |
    |                        |                |                |                                      |
    |                        |                |                | 22: AIE-ML v2                         |
    +------------------------+----------------+----------------+--------------------------------------+

.. _CONFIGURATION_PARAMETERS_FFT:

FFT Configuration Parameters
-----------------------------

For the FFT/iFFT library element, use the following list of configurable parameters and default values.

.. table:: FFT Configuration Parameters

    +------------------------+----------------+----------------+--------------------------------------+
    |     **Name**           |    **Type**    |  **Default**   |   Description                        |
    +========================+================+================+======================================+
    | DATA_TYPE              |    typename    |    cint16      | Data Type.                           |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | TWIDDLE_TYPE           |    typename    |    cint16      | Twiddle Type.                        |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | POINT_SIZE             |    unsigned    |    1024        | FFT point size.                      |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | SHIFT                  |    unsigned    |    17          | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | FFT_NIFFT              |    unsigned    |    0           | Forward (1) or reverse (0) transform.|
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | WINDOW_VSIZE           |    unsigned    |    1024        | Input/Output window size.            |
    |                        |                |                |                                      |
    |                        |                |                | By default, set to: $(POINT_SIZE).   |
    +------------------------+----------------+----------------+--------------------------------------+
    | CASC_LEN               |    unsigned    |    1           | Cascade length.                      |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | DYN_PT_SIZE            |    unsigned    |    0           | Enable (1) Dynamic Point size        |
    |                        |                |                | feature.                             |
    +------------------------+----------------+----------------+--------------------------------------+
    | API_IO                 |    unsigned    |    0           | Graph's port API.                    |
    |                        |                |                |                                      |
    |                        |                |                | 0: window                            |
    |                        |                |                |                                      |
    |                        |                |                | 1: stream                            |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | PARALLEL_POWER         |    unsigned    |   0            | Parallelism, controlling             |
    |                        |                |                | Super Sample Rate operation.         |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | NITER                  |    unsigned    |    4           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | DIFF_TOLERANCE         |    unsigned    |    0           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | STIM_TYPE              |    unsigned    |    0           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | ROUND_MODE             |    unsigned    |    0           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | SAT_MODE               |    unsigned    |    1           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+

.. note:: Parameter values are checked early in compilation to ensure support. Refer to :ref:`LEGALITY_CHECKING`.

.. _CONFIGURATION_PARAMETERS_FILTERS:

FIR Configuration Parameters
------------------------------

The following list consists of configurable parameters for FIR library elements with their default values.

.. table:: FIR Configuration Parameters

    +------------------------+----------------+----------------+--------------------------------------+
    |     **Name**           |    **Type**    |  **Default**   |   Description                        |
    +========================+================+================+======================================+
    | DATA_TYPE              |    typename    |    cint16      | Data Type.                           |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | COEFF_TYPE             |    typename    |    int16       | Coefficient Type.                    |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | FIR_LEN                |    unsigned    |    81          | FIR length.                          |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | SHIFT                  |    unsigned    |    16          | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | INPUT_WINDOW_VSIZE     |    unsigned    |    512         | Input window size.                   |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | CASC_LEN               |    unsigned    |    1           | Cascade length.                      |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | INTERPOLATE_FACTOR     |    unsigned    |    1           | Interpolation factor,                |
    |                        |                |                | see note below.                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | DECIMATE_FACTOR        |    unsigned    |    1           | Decimation factor,                   |
    |                        |                |                | see note below.                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | TDM_CHANNELS           |    unsigned    |    1           | Number of TDM Channels.              |
    |                        |                |                | Only used by TDM FIR,                |
    |                        |                |                | see note below.                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | DUAL_IP                |    unsigned    |    0           | Dual inputs used in FIRs,            |
    |                        |                |                | see note below.                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | NUM_OUTPUTS            |    unsigned    |    1           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | USE_COEFF_RELOAD       |    unsigned    |    0           | Use two sets of reloadable           |
    |                        |                |                | coefficients, where the second set   |
    |                        |                |                | deliberately corrupts a single,      |
    |                        |                |                | randomly selected coefficient.       |
    +------------------------+----------------+----------------+--------------------------------------+
    | PORT_API               |    unsigned    |    0           | Graph's port API.                    |
    |                        |                |                |                                      |
    |                        |                |                | 0: window                            |
    |                        |                |                |                                      |
    |                        |                |                | 1: stream                            |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | UUT_SSR                |    unsigned    |    1           | Super Sample Rate  SSR parameter.    |
    |                        |                |                | Defaults to 1.                       |
    |                        |                |                | see note below                       |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | NITER                  |    unsigned    |    16          | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | DIFF_TOLERANCE         |    unsigned    |    0           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | DATA_STIM_TYPE         |    unsigned    |    0           | See ``STIM_TYPE`` in                 |
    |                        |                |                | :ref:`COMMON_CONFIG_PARAMETERS`      |
    +------------------------+----------------+----------------+--------------------------------------+
    | COEFF_STIM_TYPE        |    unsigned    |    0           | See ``STIM_TYPE`` in                 |
    |                        |                |                | :ref:`COMMON_CONFIG_PARAMETERS`      |
    +------------------------+----------------+----------------+--------------------------------------+
    | USE_CUSTOM_CONSTRAINT  |    unsigned    |    0           | Overwrite default or non-existent.   |
    |                        |                |                |                                      |
    |                        |                |                | 0: no action                         |
    |                        |                |                |                                      |
    |                        |                |                | 1: use the Graph's access functions  |
    |                        |                |                | to set a location and                |
    |                        |                |                | overwrite a fifo_depth constraint.   |
    |                        |                |                | see also :ref:`FIR_CONSTRAINTS`      |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | ROUND_MODE             |    unsigned    |    0           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+
    | SAT_MODE               |    unsigned    |    1           | See :ref:`COMMON_CONFIG_PARAMETERS`  |
    |                        |                |                |                                      |
    +------------------------+----------------+----------------+--------------------------------------+

.. note:: Parameter values are checked early in compilation to ensure support. Refer to :ref:`LEGALITY_CHECKING`.

.. note:: Not all DSPIPLib elements support all configurable parameters. Unsupported parameters have no impact on execution. For example, `INTERPOLATE_FACTOR` is only supported by interpolation filters and is ignored by all other library elements.

.. |trade|  unicode:: U+02122 .. TRADEMARK SIGN
   :ltrim:
.. |reg|    unicode:: U+000AE .. REGISTERED TRADEMARK SIGN
   :ltrim:
