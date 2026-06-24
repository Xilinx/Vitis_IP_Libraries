..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2026, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. _FIR_TDM:

=======
FIR TDM
=======

The DSPIPLib contains a Time-Division Multiplexing (TDM) variant of finite impulse response (FIR) filter. It is a multichannel FIR filter with configurable application parameters, for example, number of channels and FIR length, as well as implementation parameters, for example, I/O buffer size or super sample rate (SSR) operation mode.

.. _FIR_TDM_ENTRY:

Entry Point
===========

TDM FIR filters reside in the distinct namespace ``xf::dsp::aie::fir::tdm``.

The graph entry point is:

.. code-block::

    xf::dsp::aie::fir::tdm::fir_tdm_graph

Device Support
==============

The TDM FIR filter supports AIE, AIE-ML, and AIE-ML v2 for all features with the following exceptions:

- The ``cfloat`` data type is not supported on AIE-ML device.
- Round modes available and the enumerated values of round modes are the same for AIE-ML and AIE-ML v2 devices, but differ from those for AIE devices. Refer to :ref:`COMPILING_AND_SIMULATING`.

Supported Types
===============

TDM FIR filters can be configured for various types of data and coefficients. These types can be int16, int32, or float, and also real or complex. Certain combinations of data and coefficient type are not supported.

The following table lists the supported combinations of data type and coefficient type with notes for those combinations not supported.

.. _tdm_supported_combos:

.. table:: Supported Combinations of Data Type and Coefficient Type
   :align: center

   +-----------------------------------+--------------------------------------------------------------------------+
   |                                   |                                 **Data Type**                            |
   |                                   +-----------+------------+-----------+------------+-----------+------------+
   |                                   | **Int16** | **Cint16** | **Int32** | **Cint32** | **Float** | **Cfloat** |
   |                                   |           |            |           |            |           | (note 3)   |
   +----------------------+------------+-----------+------------+-----------+------------+-----------+------------+
   | **Coefficient type** | **Int16**  | Supported | Supported  | Supported | Supported  | note 2    | note 2     |
   |                      |            |           |            |           |            |           |            |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Cint16** | note 1    | Supported  | note 1    | Supported  | note 2    | note 2     |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Int32**  | Supported | Supported  | Supported | Supported  | note 2    | note 2     |
   |                      |            |           |            |           |            |           |            |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Cint32** | note 1    | Supported  | note 1    | Supported  | note 2    | note 2     |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Float**  | note 2    | note 2     | note 2    | note 2     | Supported | Supported  |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Cfloat** | note 2    | note 2     | note 2    | note 2     | note 1    | Supported  |
   |                      | (note 3)   |           |            |           |            |           |            |
   +----------------------+------------+-----------+------------+-----------+------------+-----------+------------+
   | 1. Complex coefficients are not supported for real-only data types.                                          |
   | 2. A mix of float and integer types is not supported.                                                        |
   | 3. The cfloat data type is not supported on AIE-ML or AIE-ML v2 devices.                                      |
   +--------------------------------------------------------------------------------------------------------------+

Template Parameters
===================

For template parameter details for the TDM FIR, refer to :ref:`API_REFERENCE`.

Access Functions
================

For access functions for each FIR variant, refer to :ref:`API_REFERENCE`.

Ports
=====

For the ports for each FIR variant, refer to :ref:`API_REFERENCE`.

Design Notes
============

.. _COEFFS_FOR_FIR_TDM:

Coefficient Array for Filters
------------------------------

Pass the coefficient array values to the constructor as either a single-dimension ``std::array`` or a ``std::vector``.

Coefficients - Array Size
^^^^^^^^^^^^^^^^^^^^^^^^^

TDM FIR coefficient array size is equal to the length of the FIR multiplied by the number of channels, that is:

``Coeff_Array_Size = TP_FIR_LEN * TP_TDM_CHANNELS``

.. _FIR_TDM_COEFF_ORGANIZATION:

Coefficients - Array Organization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create the coefficient vector by listing the taps for each channel in sequence. Pass it to the constructor in the following form:

.. code-block::

   std::vector<TT_COEFF> coeffVector = {
                                 C0.0, C0.1, C0.2, C0.3, ..., C0.M-2, C0.M-1,
                                 C1.0, C1.1, C1.2, C1.3, ..., C1.M-2, C1.M-1,
                                 C2.0, C2.1, C2.2, C2.3, ..., C2.M-2, C2.M-1,
                                 ...
                                 CN-2.0, CN-2.1, CN-2.2, CN-2.3, ..., CN-2.M-2, CN-2.M-1,
                                 CN-1.0, CN-1.1, CN-1.2, CN-1.3, ..., CN-1.M-2, CN-1.M-1,
                                 };

where:

- TT_COEFF - coefficient type, for example, ``int16``,
- N - FIR Length, that is, number of taps on each channel (``TP_FIR_LEN``),
- M - Number of TDM Channels  (``TP_TDM_CHANNELS``).

Reloadable Coefficients
^^^^^^^^^^^^^^^^^^^^^^^

Reloadable coefficients are provided through a runtime programmable (RTP) asynchronous input port, programmed by the processor subsystem (PS) at runtime.
Reloadable configurations do not require the coefficient array at compile time.
Instead, use the graph's `update()` method — refer to `UG1079 Run-Time Parameter Update/Read Mechanisms <https://docs.amd.com/r/en-US/ug1079-ai-engine-kernel-coding/Runtime-Parameter-Update/Read-Mechanisms>`_ for usage instructions — to supply the coefficient array at runtime.

.. note:: The graph's `update()` method must be called after the graph has been initialized, but before the kernel starts operation on data samples.

Reloadable coefficients are available for single- and multi-kernel configuration, for example, using Cascade (``TP_CASC_LEN``) and/or Super Sample Rate (``TP_SSR``) modes of operation.

TDM Channels are split by ``TP_SSR`` and FIR taps (for each channel) are split by ``TP_CASC_LEN``. Each part is sent to a specific kernel through its corresponding RTP port.

For more details on multi-kernel modes, refer to: :ref:`FIR_TDM_CASCADE_OPERATION` and :ref:`FIR_TDM_SSR_OPERATION`.

.. _UPDATE_RTP_FIR_TDM:

Reloadable Coefficients - `update_rtp()` Method
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``update_rtp()`` method is available for each FIR graph class and reduces the runtime coefficient update to a single call.

The following example demonstrates using the `update_rtp()` method in a FIR filter graph class.

The `test_graph` class contains a member `firGraph` — a parameterized `fir_sr_asym_graph` FIR filter graph — and an array of coefficient RTP ports sized by the `getTotalRtpPorts()` method.

The `main` function initializes the graph, calls `update_rtp` to update the RTP ports with coefficient values from the std::vector `taps`, runs the filter for a specified number of iterations, then ends graph execution.

.. code-block:: cpp

   // This is a header file, e.g. "test.hpp" that is included in the corresponding cpp file.

   class test_graph : public adf::graph {
   public:

   xf::dsp::aie::fir::tdm::fir_tdm_graph<
      cint16, //TT_DATA
      int16, //TT_COEFF
      4, //TP_FIR_LEN
      10, //TP_SHIFT
      0, //TP_RND
      128, //TP_INPUT_WINDOW_VSIZE
      32, //TP_TDM_CHANNELS
      1, //TP_NUM_OUTPUTS
      0, //TP_DUAL_IP
      // 0, //TP_API
      1, //TP_SSR
      0, //TP_SAT
      1, //TP_CASC_LEN
      cint32 //TT_OUT_DATA
   > firGraph;

   static constexpr int rtpPortNumber = firGraph::getTotalRtpPorts();
   port_array<input, rtpPortNumber> coeff;

.. code-block:: cpp

   #include "test.hpp"

   xf::dsp::aie::testcase::test_graph filter;

   int main(void) {
      filter.init();

      std::vector<int16> taps = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};

      filter.firGraph.update_rtp(filter, taps, filter.coeff);

      filter.run(4);

      filter.end();

      return 0;
   }

The ``update_rtp`` method abstracts the RTP port update sequence. It performs the following steps:

- Read the total number of RTP ports using ``getTotalRtpPorts()``. Refer to :ref:`RTP_PORTS_FOR_TDM_FIR`.
- For each port, read the port size using ``getRtpPortSize(int port)``. Refer to :ref:`RTP_ARRAY_SIZE_FOR_TDM_FIR`.
- For each port, extract the corresponding taps using the ``extractTaps()`` method. Refer to :ref:`RTP_ARRAY_CONTENTS_FOR_TDM_FIR`.
- For each port, update the RTP port with the new taps using the graph's ``update()`` method. Refer to `UG1079 Run-Time Parameter Update/Read Mechanisms <https://docs.amd.com/r/en-US/ug1079-ai-engine-kernel-coding/Runtime-Parameter-Update/Read-Mechanisms>`_.

.. _RTP_PORTS_FOR_TDM_FIR:

Reloadable Coefficients - Number of Ports
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The total number of RTP ports created by a TDM FIR is given by the formula:

``TotalRtpPorts =  (TP_SSR * TP_CASC_LEN)``

For example, if ``TP_SSR = 2`` and ``TP_CASC_LEN = 3``, the coefficient array is divided into ``2 * 3 = 6`` parts.

The FIR TDM graph class provides a helper method: ``getTotalRtpPorts()`` to get the number of RTP ports. For more details, refer to: :ref:`API_REFERENCE`.

.. _RTP_ARRAY_SIZE_FOR_TDM_FIR:

Reloadable Coefficients - Array Size
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The array size is equal to the length of the FIR multiplied by the number of channels and divided equally by AI Engine kernels used, that is:

``TapsPerRtpPort = (TP_FIR_LEN * TP_TDM_CHANNELS) / (TP_SSR * TP_CASC_LEN)``

For example, if ``TP_FIR_LEN = 6`` and ``TP_TDM_CHANNELS = 16``, if ``TP_SSR = 2`` and ``TP_CASC_LEN = 3``, the coefficient array is divided into ``2 * 3 = 6`` parts.

The FIR TDM graph class provides a helper method: ``getTapsPerRtpPort(int kernelNo)`` to get the number of FIR taps per RTP port. For more details, refer to: :ref:`API_REFERENCE`.


.. _RTP_ARRAY_CONTENTS_FOR_TDM_FIR:

Reloadable Coefficients - Array Contents
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TDM Channels are split by ``TP_SSR`` and FIR taps (for each channel) are split by ``TP_CASC_LEN``. Each part is sent to a specific kernel through its corresponding RTP port.

The FIR TDM graph class provides a helper method: ``extractTaps(const std::vector<TT_COEFF>& taps, unsigned int kernelNo)`` to get coefficients for a given kernel. For more details, refer to: :ref:`API_REFERENCE`.

.. _BUFFER_API_FIRS:

IO Buffer Interface for Filters
---------------------------------

On the AI Engine processor, data is packetized into IO buffers mapped to local memory.

IO buffers support 256-bit wide load/store operations, offering throughput of up to 256 Gb/s (based on a 1 GHz AI Engine clock).

IO buffers use a `ping-pong` mechanism: the consumer kernel reads the `ping` portion while the producer fills the `pong` portion, which is consumed in the next iteration.

In each iteration, the kernel operates on a fixed number of samples from the input buffer, set by the template parameter ``TP_INPUT_WINDOW_VSIZE``. Safe access to buffered data is coordinated through a lock acquire and release mechanism.

Margin
^^^^^^

The input buffer of a TDM FIR may be extended by a margin so that the state of the filter at the end of the previous iteration can be restored.

Calculate the margin required by TDM FIR using the formula:

``Margin_samples = (TP_FIR_LEN - 1) * TP_TDM_CHANNELS``

Internal Margin
^^^^^^^^^^^^^^^

For cases where margin data exceeds new arriving data, that is, when
``Margin_samples > TP_INPUT_WINDOW_VSIZE``,
margin is implemented as a separate buffer, internal to the kernel that operates on it.

As a result, the input buffers are not extended by margin data.

.. note:: Internal margin handling is not supported on AIE-ML and AIE-ML v2 devices.

Maximizing Throughput
^^^^^^^^^^^^^^^^^^^^^

Buffer synchronization introduces a fixed overhead each time a kernel is triggered.
To maximize throughput, set the input buffer size to the maximum the system allows.

.. note:: To achieve maximum performance, place the producer and consumer kernels in adjacent AI Engine tiles, so the window buffers can be accessed without a requirement for a MM2S/S2MM direct memory access (DMA) stream conversion.

Multiple Frames
^^^^^^^^^^^^^^^

TDM FIR supports batching multiple frames into a single input buffer to reduce kernel switching overhead and maximize performance.
A frame is a set of ``TP_TDM_CHANNELS`` input samples — one sample per TDM channel.

Set the input buffer size to an integer multiple of TDM channels, for example:
``TP_INPUT_WINDOW_VSIZE = TP_TDM_CHANNELS * NUMBER_OF_FRAMES``

A TDM FIR that processes multiple frames per kernel iteration produces an equal number of output frames.

Latency
^^^^^^^

The latency of a buffer-based TDM FIR is driven primarily by input and output buffer depth. Data type, coefficient type, and FIR length also affect latency, but to a lesser degree.

To minimize latency, set the buffer size to the smallest value that meets the required throughput.

.. _FIR_TDM_MAX_WINDOW_SIZE:

Maximum Window Size
^^^^^^^^^^^^^^^^^^^

The window buffer is mapped into a local memory in the area surrounding the kernel that accesses it.

A local memory storage is 32 kB (64 kB for AIE-ML and AIE-ML v2 devices), and the maximum size of the `ping-pong` window buffer must not exceed this limit.

.. note:: Input buffers may be extended by margin data, which can significantly reduce the maximum window size.


.. _FIR_TDM_SINGLE_BUFFER_CONSTRAINT:

Single Buffer Constraint
^^^^^^^^^^^^^^^^^^^^^^^^

Disabling the `ping-pong` mechanism makes the entire data memory available to the kernel for computation. However, the single-buffered window can be accessed by only one agent at a time, which incurs a performance penalty.
Disable it by applying the `single_buffer()` constraint to an input or output port of each kernel.

.. code-block::

    single_buffer(firGraph.getKernels()[0].in[0]);

.. _FIR_TDM_INPUT_ORGANIZATION:

Input Data Samples - Array Organization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Store input data samples in the input buffer by listing all samples for each channel in sequence, in the following form:

.. code-block::

   std::vector<TT_DATA> dataVector = {
                                 D0.0, D0.1, D0.2, D0.2, ..., D0.M-2, D0.M-1,
                                 D1.0, D1.1, D1.2, D1.2, ..., D1.M-2, D1.M-1,
                                 ...
                                 DF-2.0, DF-2.1, DF-2.2, DF-2.2, ..., DF-2.M-2, DF-2.M-1,
                                 DF-1.0, DF-1.1, DF-1.2, DF-1.2, ..., DF-1.M-2, DF-1.M-1,
                                 };

where:

- TT_DATA - data type, for example, ``cint16``,
- M - Number of TDM Channels  (``TP_TDM_CHANNELS``),
- F - Number of Frames within Input Buffer   (``NUMBER_OF_FRAMES = TP_INPUT_WINDOW_VSIZE / TP_TDM_CHANNELS``).

Streaming Interface for Filters
---------------------------------

Streaming interfaces are not supported by TDM FIR.


.. _FIR_TDM_CASCADE_OPERATION:

Cascaded Kernels
-----------------

Cascade - Operation Mode
^^^^^^^^^^^^^^^^^^^^^^^^^

Configure TDM FIR to operate across multiple cascaded AI Engine tiles using the ``TP_CASC_LEN`` template parameter.

When ``TP_CASC_LEN > 1``, an array of ``TP_CASC_LEN`` kernels is created and connected through the cascade interface. Each kernel processes a fraction of ``TP_FIR_LEN`` (``TP_FIR_LEN / TP_CASC_LEN``).

Taps are split to maximize efficiency across the cascade chain. For example, a 16-tap FIR over two cascaded kernels assigns 8 taps to each; a cascade of three assigns 6 taps to the first kernel and 5 taps to each of the remaining kernels.


Cascade - Resource Utilization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The number of AI Engine tiles used by a TDM FIR is an integer multiple of ``TP_CASC_LEN``.

Cascade - Port Utilization
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Configuring TDM FIR into a chain of cascaded kernels (``TP_CASC_LEN > 1``) does not affect input and output ports.


Output Type
-----------

Use the ``TT_OUT_DATA`` template parameter to select a 32-bit output type when the input type is 16-bit.


.. _FIR_TDM_SSR_OPERATION:

Super Sample Rate
-----------------

The term Super Sample Rate strictly means the processing of more than one sample per clock cycle. Because the AI Engine is a vector processor, almost every operation is SSR by this definition, making it superfluous. Therefore, in the AI Engine context, SSR means an implementation using multiple computation paths to improve performance at the expense of additional resource use.

.. _FIR_TDM_SSR_OPERATION_MODE:

Super Sample Rate - Operation Mode
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Configure TDM FIR for SSR mode using the ``TP_SSR`` template parameter. This creates ``TP_SSR`` kernels with ``TP_SSR`` input and output ports.

When ``TP_SSR > 1``, input samples — and therefore the corresponding TDM channels — are split across multiple parallel paths. Each path processes ``TP_TDM_CHANNELS / TP_SSR`` channels and ``TP_INPUT_WINDOW_VSIZE / TP_SSR`` input samples per iteration.

For port-to-sample mapping details, refer to :ref:`FIR_TDM_SSR_PORT_MAPPING`.


.. _FIR_TDM_SSR_OPERATION_RESOURCE_UTILIZATION:

Super Sample Rate - Resource Utilization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The number of AI Engine tiles used by a TDM FIR is given by the formula:

.. code-block::

  NUMBER_OF_AIE_TILES = TP_SSR x TP_CASC_LEN

The TDM FIR graph divides the FIR workload equally among kernels. Depending on the configuration, each kernel may carry a relatively low computational load.


.. _FIR_TDM_SSR_OPERATION_PORT_UTILIZATION:

Super Sample Rate - Port Utilization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The number of input/output ports created by a FIR is given by the formula:

* Number of input ports: ``NUM_INPUT_PORTS  = TP_SSR``

* Number of output ports: ``NUM_OUTPUT_PORTS  = TP_SSR``

.. _FIR_TDM_SSR_PORT_MAPPING:

Super Sample Rate - Sample to Port Mapping
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When a Super Sample Rate operation is used, data is input and output using multiple ports.

Split the input data channel across ports in round-robin fashion: sample 0 to :code:`in[0]`, sample 1 to :code:`in[1]`, and so on up to sample ``N-1`` (where ``N = TP_SSR``), then repeat from :code:`in[0]`. Output samples follow the same pattern.

For example, for the data stream :code:`int32 x = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ...` with SSR set to 3, split input samples as follows:

.. code-block::

  in[0] = 0, 3, 6, 9,  12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, ...
  in[1] = 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, ...
  in[2] = 2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44, 47, ...

The output data is produced in a similar method.

.. _FIR_TDM_CONSTRAINTS:

Constraints
-----------

TDM FIR provides access methods for assigning constraints to kernels and nets, for example:

- `getKernels()` — returns a pointer to an array of kernel pointers.

For more details, refer to :ref:`API_REFERENCE`.


.. |trade|  unicode:: U+02122 .. TRADEMARK SIGN
   :ltrim:
.. |reg|    unicode:: U+000AE .. REGISTERED TRADEMARK SIGN
   :ltrim:
