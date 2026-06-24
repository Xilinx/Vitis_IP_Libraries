..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2026, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. _VSS_FFT:

============
VSS FFT/IFFT
============

This library element implements a single-channel DIT FFT using both AI Engine tiles and programmable logic to extract higher performance for larger point sizes. The VSS offers two modes of implementing the FFT: Mode 1 and Mode 2. Mode 1 performs more computation on AIE tiles compared to Mode 2. The two modes differ in the split of the algorithm between resources in AI Engines and programmable logic. The two modes, therefore, offer different trade-offs between performance and resource utilization.

Entry Point
===========

The entry points for the VSS are the ``vss_fft_ifft_params.cfg`` and ``vss_fft_ifft_1d.mk`` files present in the `L2/include/vss/vss_fft_ifft_1d/` directory in the DSP library. The ``vss_fft_ifft_1d.mk`` takes a user-configurable file, for example ``vss_fft_ifft_params.cfg``, as input and generates a .vss object as an output after performing all the intermediate steps such as generating the necessary AI Engine graph and PL products and stitching them together. You can then integrate this .vss object into your larger design. Refer to Vitis documentation on "Vitis Subsystems" for details on how to include a .vss object into your design.

Edit the parameters in the ``cfg`` file and provide it as input to the ``vss_fft_ifft_1d.mk`` file. An example of how to create a VSS and include a .vss object in your design is also provided in `L2/examples/vss_fft_ifft_1d/example.mk`. It creates a .vss object, links it to a larger system to create an xclbin, and runs hardware emulation of the full design.

Device Support
==============

The VSS FFT can generate VSS products for AIE, AIE-ML, and AIE-ML v2. The VSS is generated for the ``part`` that you provide in the input cfg file. All features are supported on these variants with the following differences:

- ``DATA_TYPE`` and ``TWIDDLE_TYPE``: AIE-ML does not support cfloat type.
- ``TWIDDLE_TYPE``: AIE supports cint32. AIE-ML does not.
- ``ROUND_MODE``: Supported round modes are the same for AIE-ML and AIE-ML v2 devices, but differ from those for AIE devices as for all library elements.

Supported Parameters
====================

The complete list of required parameters for the VSS FFT is shown in `L2/include/vss/vss_fft_ifft_1d/vss_fft_ifft_params.cfg`. Edit the parameters in this file to configure the VSS FFT. You can add to the [aie] section of the cfg file for other options to pass directly to the aiecompiler. 

Refer to the API reference on `vss_fft_ifft_1d_graph.hpp` for details on the AI Engine configurable parameters for VSS Mode 1 and see the `vss_fft_ifft_1d_front_only_graph.hpp` for the parameters for VSS Mode 2.

+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [Category] Parameter             | Description                                                                                                                                |
+==================================+============================================================================================================================================+
| part                             | Name of the part that the VSS compiles for                                                                                                 |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| freqhz                           | Frequency of the internal PL components of the VSS (in Hz)                                                                                |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [aie] enable_partition           | Configuration of the range of columns where the compiled AIE kernels are placed. You can update the name of the AIE partition.             |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] DATA_TYPE           | Used to set TT_DATA described in API Reference                                                                                             |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] TWIDDLE_TYPE        | Used to set TT_TWIDDLE described in API Reference                                                                                         |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] POINT SIZE          | Used to set TP_POINT_SIZE described in API Reference                                                                                       |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] FFT_NIFFT           | Used to set TP_FFT_NIFFT described in API Reference                                                                                        |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] SHIFT               | Used to set TP_SHIFT described in API Reference                                                                                            |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] API_IO              | Used to set TP_API described in API Reference                                                                                              |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] ROUND_MODE          | Used to set TP_RND described in API Reference                                                                                              |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] SAT_MODE            | Used to set TP_SAT described in API Reference                                                                                              |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] Twiddle Mode        | Used to set TP_TWIDDLE_MODE described in API Reference                                                                                     |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] SSR                 | Used to set TP_SSR described in API Reference                                                                                              |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] AIE_PLIO_WIDTH      | Sets the PLIO width of the AIE-PL interface                                                                                                |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] VSS_MODE            | Sets the mode of decomposition of the VSS. Choose between 1 and 2.                                                                        |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] ADD_FRONT_TRANSPOSE | Indicates whether to include a data rearrangement block at the input side of the VSS. Refer to design notes for more details.              |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] ADD_BACK_TRANSPOSE  | Indicates whether to include a data rearrangement block at the output side of the VSS. Refer to design notes for more details.             |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] POINT_SIZE_D1       | For expert mode only. This parameter controls the first dimension of decomposition of the FFT implemented by the first bank of FFTs. The second dimension is inferred as POINT_SIZE/POINT_SIZE_D1. |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] CASC_LEN            | Used to set TP_CASC_LEN described in API Reference                                                                                         |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+
| [APP_PARAMS] USE_WIDGETS         | Used to set TP_USE_WIDGETS described in API Reference. Applicable for VSS Mode 1 only.                                                     |
+----------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------+


Design Notes
============

.. _VSS_SSR_OPERATION:

Super Sample Rate
------------------

You can configure the VSS FFT for Super Sample Rate operation to achieve higher throughput. The design generates ``TP_SSR`` number of input and output ports.

The input data to the SSR input ports of the VSS FFT are expected to be distributed evenly in a "card-dealing" fashion. For example:

* **Port Number 1** gets samples: ``S_1``, ``S_SSR+1``, ``S_2*SSR+1``, ...
* **Port Number 2** gets samples: ``S_2``, ``S_SSR+2``, ``S_2*SSR+2``, ...
* ...
* **Port Number SSR** gets samples: ``S_SSR``, ``S_SSR+SSR``, ``S_2*SSR+SSR``, ...

.. _SSR_POINTSIZE_CONSTRAINTS:

Padding Input Data Based on Super Sample Rate and Point Size
-------------------------------------------------------------

If the point size is a multiple of SSR, the inputs can be passed as is to the FFT. Otherwise, every point size number of samples must be padded with zeros to the closest multiple of SSR before giving as input to the FFT. The output data also contains point size number of valid data samples padded to the closest multiple of SSR.

.. _ADD_FRONT_TRANSPOSE:

VSS Mode 1 creates a transpose block at its input to rearrange data that arrives in the natural SSR order described in :ref:`DSP_VSS_SSR_OPERATION` into an order needed by the first set of compute units within the VSS. This block uses buffers in the PL to rearrange the data. If you want to input data directly in the form needed by the compute units, you can save on memory resources by setting the ADD_FRONT_TRANSPOSE flag to 0.
If the front transpose is removed, ensure that the data arriving in each port satisfies the formula:

.. math::

   S[PORT\_IDX][SAMP\_IDX] = PORT\_IDX + \left(\left(SAMP\_IDX \bmod D1\right) \times D2\right) + \left\lfloor \frac{SAMP\_IDX}{D1} \right\rfloor \times SSR

where,

* ``PORT_IDX`` ranges from **0** to **SSR - 1**
* For **perfect square** point sizes: ``D1 = D2 = √(point\_size)``
* For **other** point sizes:

  * ``D1 = √(point\_size × 2)``
  * ``D2 = √(point\_size ÷ 2)``

**Example: Point Size = 512, SSR = 4**

* **Stream 0** carries samples:

  * ``SI_0``, ``SI_16``, ``SI_32``, ``SI_48``, ..., ``SI_496``, ``SI_4``, ``SI_20``, ``SI_36``, ..., ``SI_500``, ...

* **Stream 1** carries samples:

  * ``SI_1``, ``SI_17``, ``SI_33``, ``SI_49``, ..., ``SI_497``, ``SI_5``, ``SI_21``, ``SI_37``, ..., ``SI_501``, ...

* **Stream 2** carries samples:

  * ``SI_2``, ``SI_18``, ``SI_34``, ``SI_50``, ..., ``SI_498``, ``SI_6``, ``SI_22``, ``SI_38``, ..., ``SI_502``, ...

* **Stream 3** carries samples:

  * ``SI_3``, ``SI_19``, ``SI_35``, ``SI_51``, ..., ``SI_499``, ``SI_7``, ``SI_23``, ``SI_39``, ..., ``SI_503``, ...

.. _ADD_BACK_TRANSPOSE:

Both VSS Mode 1 and 2 include a transpose block after all their compute units to rearrange data into the SSR form as described in section :ref:`DSP_VSS_SSR_OPERATION`. This block uses buffers in the PL to rearrange the data. If you have downstream blocks that can directly accept the data in the form given out by the compute units, you can save on memory resources by setting the ADD_BACK_TRANSPOSE flag to 0.

If the back transpose is removed, data arriving in each output port differs between the 2 VSS modes for the same SSR and point size.

**VSS Mode 1 Output Formula**

For VSS Mode 1, the samples at the output of the VSS without the back transpose satisfy the formula:

.. math::

   S[PORT\_IDX][SAMP\_IDX] = PORT\_IDX + \left( \left( SAMP\_IDX \bmod D2 \right) \times D1 \right) + \left\lfloor \frac{SAMP\_IDX}{D2} \right\rfloor \times SSR

where,

* ``PORT_IDX`` ranges from **0** to **SSR - 1**
* For **perfect square** point sizes: ``D1 = D2 = √(point\_size)``
* For **other** point sizes:

  * ``D1 = √(point\_size × 2)``
  * ``D2 = √(point\_size ÷ 2)``

**VSS Mode 2 Output Formula**
------------------------------

For VSS Mode 2, the samples at the output of the VSS without the back transpose satisfy the formula:

.. math::

   S[PORT\_IDX][SAMP\_IDX] = PORT\_IDX + \left( SAMP\_IDX \bmod SSR \right) \times D1

where,

* ``PORT_IDX`` ranges from **0** to **SSR - 1**
* ``D1 = point\_size ÷ SSR``

Configuration Notes
===================
After you configure the `params.cfg`, run the meta_check target on the VSS generator Makefile to ensure that the configuration is valid. An example of this is available in the `L2/examples/vss_fft_ifft_1d/example.mk` file.
