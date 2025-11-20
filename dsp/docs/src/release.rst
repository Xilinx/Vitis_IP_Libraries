..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2025, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. _release_note:

=============
Release Notes
=============

.. toctree::
   :hidden:
   :maxdepth: 1

2025.2
======

First release of 2025.2 version of Vitis DSP IP Library.
Spin off from Vitis Library 2025.2.
Requires Vitis Library 2025.2.

The following features have been added to the library in this release:

* **VSS FFT** - New library element.

In this release a VSS (Vitis Sub-System) FFT/IFFT has been added to the DSPLIB.
This configurable design element implements a single-channel DIT FFT/IFFT, decomposing FFT algorithm into AIE Tiles and PL (programmable logic).

Supports AIE, AIE-ML and AIE-MLv2 devices.

Compared with Vitis Library VSS FFT, this new VSS FFT/IFFT offers improved resource utilization.

*  **FIRs** - New library elements.

Vitis IP library offers the below set of Finite Impulse Response (FIR) filters.

      - FIR Decimate Asymmetric

      - FIR Decimate Halfband

      - FIR Interpolate Asymmetric

      - FIR Interpolate Halfband

      - FIR Resampler

      - FIR Single Rate Asymmetric

      - FIR Single Rate Symmetric

Compared with Vitis Library FIRs, these new library elements offer improved performance and resource utilization, as well as ease-of-use utility features.
