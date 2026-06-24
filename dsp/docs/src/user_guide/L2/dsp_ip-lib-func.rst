..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2026, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. _DSP_LIB_FUNC:

=====================
DSP Library Functions
=====================

The AMD Vitis |trade| digital signal processing (DSP) IP library (DSPIPLib) is a configurable library of elements for developing applications on AMD Versal |trade| AI Engines. This is an open source library for DSP applications. The entry point for each function in this library is a graph (L2 level). Each entry point graph class contains one or more L1 level kernels and can contain one or more graph objects. Direct use of kernel classes (L1 level) or any other graph class not identified as an entry point is not recommended because this may bypass legality checking.

The DSPLib consists of the following DSP elements:

.. toctree::
   :maxdepth: 2

   Filters <func-fir-filtersAIE.rst>
   FIR TDM <func-fir-TDM.rst>

.. |trade|  unicode:: U+02122 .. TRADEMARK SIGN
   :ltrim:
.. |reg|    unicode:: U+000AE .. REGISTERED TRADEMARK SIGN
   :ltrim:
