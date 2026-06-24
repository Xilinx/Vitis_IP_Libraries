..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2026, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. _release_note:

=============
Release Notes
=============

.. toctree::
   :hidden:
   :maxdepth: 1

2026.1
======

The following features have been added to the library in this release:

* **VSS FFT/IFFT** - Extended configuration support for ``vss_fft_ifft_1d_graph``. Added `TP_POINT_SIZE_D1` to allow user-specified FFT decomposition, as well as `TP_CASC_LEN` and `TP_USE_WIDGETS` parameters to configure ``fft_ifft_dit_1ch_graph`` as requested.

2025.2
======

First release of the 2025.2 version of the Vitis DSP IP Library.
Spin off from Vitis Library 2025.2.
Requires Vitis Library 2025.2.

The following features have been added to the library in this release:

* **VSS FFT** - New library element.

In this release, a VSS (Vitis subsystem) FFT/IFFT has been added to the DSPLIB.
This configurable design element implements a single-channel DIT FFT/IFFT, decomposing the FFT algorithm into AIE tiles and programmable logic (PL).

Supports AIE, AIE-ML, and AIE-ML v2 devices.

Compared with the Vitis Library VSS FFT, this new VSS FFT/IFFT offers improved resource utilization.

* **FIRs** - New library elements.

The Vitis IP library offers the following set of finite impulse response (FIR) filters:

      - FIR Decimate Asymmetric

      - FIR Decimate Halfband

      - FIR Interpolate Asymmetric

      - FIR Interpolate Halfband

      - FIR Resampler

      - FIR Single Rate Asymmetric

      - FIR Single Rate Symmetric

Compared with the Vitis Library FIRs, these new library elements offer improved performance and resource utilization, as well as ease-of-use utility features.
