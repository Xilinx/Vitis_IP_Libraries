..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2026, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

========================
Vitis DSP IP Library
========================

The AMD Vitis |trade| digital signal processing (DSP) IP library (DSPIPLib) provides an implementation of different L1/L2/L3 elements for digital signal processing.


The DSPIPLib contains:

- :ref:`INTRODUCTION_AIE`.

.. _INTRODUCTION_AIE:

AI Engine DSP IP Library
========================

The AMD Vitis AI Engine DSP IP library consists of designs of various DSP algorithms, optimized to take full advantage of the processing power of AMD Versal |trade| Adaptive SoC devices, which contain an array of AI Engine high-performance vector processors.

The library is organized into three parts:

- L1 AI Engine kernels
- L2 AI Engine graphs and VSS Makefiles
- L3 software APIs

Currently, there are no L3 software APIs. The recommended entry point for all library elements is an L2 graph for designs that include only AI Engines and a VSS Makefile for designs that include both AI Engine and programmable logic (PL) components.

For more information, refer to :ref:`INTRODUCTION`.

The Vitis AIE DSP IP Library includes a VSS form of a fast Fourier transform (FFT), and finite impulse response (FIR) filters. For a full list of available DSP functions, refer to :ref:`DSP_LIB_FUNC`.

.. note::

   The VSS FFT/iFFT solution in the Vitis DSP IP Library requires the FFT/IFFT implementation from the Vitis Library. See `Vitis DSP Library documentation <https://docs.amd.com/r/en-US/Vitis_Libraries/dsp/index.html>`_ for more details.

.. note::

   The VSS FFT/iFFT solution in the Vitis DSP IP Library may use components in the Vivado IP suite in addition to the FFT/IFFT implementation from the Vitis Library.

.. toctree::
   :caption: Introduction
   :maxdepth: 1

   Overview <overview.rst>
   Release Note <release.rst>

.. toctree::
   :caption: L2 AIE DSP Library User Guide
   :maxdepth: 2

   Introduction <user_guide/L2/introduction.rst>
   DSP IP Library Functions <user_guide/L2/dsp_ip-lib-func.rst>
   Configuration <user_guide/L2/configuration.rst>
   Compiling and Simulating <user_guide/L2/compiling-and-simulating.rst>
   Benchmark/QoR <user_guide/L2/benchmark.rst>

.. toctree::
   :caption: API Reference

   API Reference Overview <user_guide/L2/api-reference.rst>
   FIRs <rst/group_fir_graphs.rst>

.. |trade|  unicode:: U+02122 .. TRADEMARK SIGN
   :ltrim:
.. |reg|    unicode:: U+000AE .. REGISTERED TRADEMARK SIGN
   :ltrim:
