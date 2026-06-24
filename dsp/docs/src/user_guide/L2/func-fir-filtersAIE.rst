..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2026, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. _FILTERS:

=======
Filters
=======

The DSPIPLib contains several variants of finite impulse response (FIR) filters. These include single-rate FIRs, half-band interpolation/decimation FIRs, as well as integer and fractional interpolation/decimation FIRs.

.. _FILTER_ENTRY:

Entry Point
===========

FIR filters are categorized into classes within the distinct namespace ``xf::dsp::aie::fir``, to prevent name collision in the global scope. Use namespace aliasing to shorten instantiations:

.. code-block::

    namespace dspiplib = xf::dsp::aie;

Each FIR filter has also been placed in its unique FIR type namespace. The available FIR filter classes and the corresponding graph entry point are listed as follows:

.. _tab-fir-filter-classes:

.. table:: FIR Filter Classes
   :align: center

   +----------------------------------+--------------------------------------------------------------+
   |    **Function**                  | **Namespace and Class Name**                                 |
   +==================================+==============================================================+
   | Single rate, asymmetrical        | dspiplib::fir::sr_asym::fir_sr_asym_graph                    |
   +----------------------------------+--------------------------------------------------------------+
   | Single rate, symmetrical         | dspiplib::fir::sr_sym::fir_sr_sym_graph                      |
   +----------------------------------+--------------------------------------------------------------+
   | Interpolation asymmetrical       | dspiplib::fir::interpolate_asym::fir_interpolate_asym_graph  |
   +----------------------------------+--------------------------------------------------------------+
   | Decimation, half-band            | dspiplib::fir::decimate_hb::fir_decimate_hb_graph            |
   +----------------------------------+--------------------------------------------------------------+
   | Interpolation, half-band         | dspiplib::fir::interpolate_hb::fir_interpolate_hb_graph      |
   +----------------------------------+--------------------------------------------------------------+
   | Decimation, asymmetric           | dspiplib::fir::decimate_asym::fir_decimate_asym_graph        |
   +----------------------------------+--------------------------------------------------------------+
   | Interpolation or decimation,     | dspiplib::fir::resampler::fir_resampler_graph                |
   | fractional, asymmetric           |                                                              |
   +----------------------------------+--------------------------------------------------------------+

Device Support
==============

The FIR filters support AIE, AIE-ML, and AIE-ML v2 for all features with the following exceptions:

- The ``cfloat`` data type is not supported on AIE-ML device.
- Round modes available and the enumerated values of round modes are the same for AIE-ML and AIE-ML v2 devices, but differ from those for AIE devices. Refer to :ref:`COMPILING_AND_SIMULATING`.

Supported Types
===============

All FIR filters can be configured for various types of data and coefficients. These types can be int16, int32, or float, and also real or complex. Certain combinations of data and coefficient type are not supported.

The following table lists the supported combinations of data type and coefficient type with notes for those combinations not supported.

.. _tab_supported_combos:

.. table:: Supported Combinations of Data Type and Coefficient Type
   :align: center

   +-----------------------------------+--------------------------------------------------------------------------+
   |                                   |                                 **Data Type**                            |
   |                                   +-----------+------------+-----------+------------+-----------+------------+
   |                                   | **Int16** | **Cint16** | **Int32** | **Cint32** | **Float** | **Cfloat** |
   |                                   |           |            |           |            |           | (note 4)   |
   +----------------------+------------+-----------+------------+-----------+------------+-----------+------------+
   | **Coefficient type** | **Int16**  | Supported | Supported  | Supported | Supported  | note 2    | note 2     |
   |                      |            | (note 3)  |            |           |            |           |            |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Cint16** | note 1    | Supported  | note 1    | Supported  | note 2    | note 2     |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Int32**  | Supported | Supported  | Supported | Supported  | note 2    | note 2     |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Cint32** | note 1    | Supported  | note 1    | Supported  | note 2    | note 2     |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Float**  | note 2    | note 2     | note 2    | note 2     | Supported | Supported  |
   |                      +------------+-----------+------------+-----------+------------+-----------+------------+
   |                      | **Cfloat** | note 2    | note 2     | note 2    | note 2     | note 1    | Supported  |
   |                      | (note 4)   |           |            |           |            |           |            |
   +----------------------+------------+-----------+------------+-----------+------------+-----------+------------+
   | 1. Complex coefficients are not supported for real-only data types.                                          |
   | 2. A mix of float and integer types is not supported.                                                        |
   | 3. The rate-changing FIR variants, that is, fir_decimate_asym, fir_decimate_sym, fir_interpolate_asym,       |
   |    and fir_resampler only support int16 data and int16 coeff type combination on AIE-ML or AIE-ML v2 devices. |
   | 4. The cfloat data type is not supported on AIE-ML or AIE-ML v2 devices.                                      |
   +--------------------------------------------------------------------------------------------------------------+

Template Parameters
===================

For a list of template parameters for each FIR variant, refer to :ref:`API_REFERENCE`.

Access Functions
================

For the access functions for each FIR variant, refer to :ref:`API_REFERENCE`.

Ports
=====

To see the ports for each FIR variant, refer to :ref:`API_REFERENCE`.

Design Notes
============

.. _DSP_IP_FEATURES:

Additional Features of FIRs in DSP IP Library
----------------------------------------------

Compared to the Vitis Library DSP IP offering, the following enhancements have been made:

   - Memory storage for coefficients has been reorganized. This results in reduced Program Memory (PM) usage and reduced data memory usage.

   - RTP ports for reloadable coefficients have been restructured, improving both memory footprint and throughput efficiency. No runtime checks are performed, allowing the RTP solution to achieve maximum throughput, nearly matching the performance of static coefficient solutions without any drawbacks.

   - Usability has been improved with the introduction of the ``update_rtp()`` method, which streamlines the process of updating coefficients at runtime to a single method call.

.. _COEFFS_FOR_FIRS:

Coefficient Array for Filters
------------------------------

Static Coefficients
^^^^^^^^^^^^^^^^^^^

For all non-reloadable filter configurations, the coefficient values are passed as an array argument to the constructor as either a std::array or std::vector.

Static Coefficients - Array Size
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Asymmetrical FIR**

| Asymmetrical filters expect the port to contain the full array of coefficients, that is, the coefficient array size is equal to the ``TP_FIR_LEN``.
| The length of the array expected is therefore ``(TP_FIR_LEN``, for example, for a filter of length 7, where coefficients are ``int16``:
| ``{1, 2, 3, 4, 5, 6, 7}``, the constructor expects an argument:
| ``std::array<int16, 7> tapsIn =  {1, 2, 3, 4, 5, 6, 7}``.

**Symmetrical FIR**

| In the case of symmetrical filters, only the first half (plus any odd center tap) need be passed, because the remaining values can be derived by symmetry.
| The length of the array expected is therefore ``(TP_FIR_LEN+1)/2``, for example, for a filter of length 7, where coefficients are ``int16``:
| ``{1, 2, 3, 5, 3, 2, 1}``, four non-zero tap values, including the center tap, are expected, that is, the constructor expects an argument:
| ``std::array<int16, 4> tapsIn =  {1, 2, 3, 5}``.

**Half-band FIR**

| For half-band filters, only the non-zero coefficients should be entered with the center tap last in the array.
| The length of the array expected is therefore ``(TP_FIR_LEN+1)/4+1``, for example, for a half-band filter of length 7, where coefficients are:
| ``{1, 0, 2, 5, 2, 0, 1}``, three non-zero tap values, including the center tap, are expected, that is, constructor expects an argument:
| ``std::array<int16, 3> tapsIn =  {1, 2, 5}``.


Reloadable Coefficients
^^^^^^^^^^^^^^^^^^^^^^^

Reloadable coefficients are available through the use of a runtime programmable (RTP) asynchronous input port, programmed by the processor subsystem (PS) at runtime.
Reloadable configurations do not require the coefficient array to be passed to the constructor at compile time.
Instead, the graph's `update()` (refer to `UG1079 Run-Time Parameter Update/Read Mechanisms <https://docs.amd.com/r/en-US/ug1079-ai-engine-kernel-coding/Runtime-Parameter-Update/Read-Mechanisms>`_ for usage instructions) method is used to input the coefficient array.

.. note:: The graph's `update()` method must be called after the graph has been initialized, but before the kernel starts operation on data samples.

Reloadable coefficients are available for single- and multi-kernel configuration, for example, using Cascade (``TP_CASC_LEN``) and/or Super Sample Rate (``TP_SSR``) modes of operation.

FIR taps are split and distributed to all kernels by ``TP_CASC_LEN``. Each part is sent to a specific kernel through its corresponding RTP port.

.. _UPDATE_RTP_FIR:

Reloadable Coefficients - `update_rtp()` Method
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An ease-of-use enhancement with the ``update_rtp()`` method is available for each FIR graph class. This method simplifies the process of updating coefficients at runtime to a single method call.

The following example demonstrates how to use the `update_rtp()` method in the context of a FIR filter graph class.

The `test_graph` class contains a member `firGraph`, which is a parameterized `fir_sr_asym_graph` FIR filter graph.
The `test_graph` class also contains an array of coefficient RTP ports. The size of the array is determined by the `getTotalRtpPorts()` method of the `fir_sr_asym_graph` class.

The `main` function of the host application initializes the graph, updates the RTP ports at runtime using `update_rtp` with coefficient values from the std::vector `taps`.

The `main` function then runs the filter for a specified number of iterations and properly ends the graph execution.

.. code-block:: cpp

   // This is a header file, e.g. "test.hpp" that is included in the corresponding cpp file.

   class test_graph : public adf::graph {
   public:

   xf::dsp::aie::fir::sr_asym::fir_sr_asym_graph<
      int16, //TT_DATA
      int32, //TT_COEFF
      32, //TP_FIR_LEN
      16, //TP_SHIFT
      0, //TP_RND
      256, //TP_INPUT_WINDOW_VSIZE
      1, //TP_CASC_LEN
      0, //TP_USE_COEFF_RELOAD
      1, //TP_NUM_OUTPUTS
      0, //TP_DUAL_IP
      0, //TP_API
      1, //TP_SSR
      0 //TP_SAT
   > firGraph;

   static constexpr int rtpPortNumber = firGraph::getTotalRtpPorts();
   port_array<input, rtpPortNumber> coeff;

.. code-block:: cpp

   #include "test.hpp"

   xf::dsp::aie::testcase::test_graph filter;

   int main(void) {
      filter.init();

      std::vector<int32> taps = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

      filter.firGraph.update_rtp(filter, taps, filter.coeff);

      filter.run(4);

      filter.end();

      return 0;
   }

The update_rtp method abstracts the implementation details of updating RTP ports with new coefficient values. It performs the following steps:

- Read the total number of RTP ports, using  ``getTotalRtpPorts()``. For details, refer to :ref:`RTP_PORTS_FOR_FIR`.
- For each port, read the size of the port, using ``getRtpPortSize(int port)``. For details, refer to :ref:`RTP_ARRAY_SIZE_FOR_FIR`.
- For each port, extract the corresponding taps using the ``extractTaps()`` method. For details, refer to :ref:`RTP_ARRAY_CONTENTS_FOR_FIR`.
- For each port, update the RTP port with the new taps using the graph's ``update()`` method. For details, refer to `UG1079 Run-Time Parameter Update/Read Mechanisms <https://docs.amd.com/r/en-US/ug1079-ai-engine-kernel-coding/Runtime-Parameter-Update/Read-Mechanisms>`_.

.. _RTP_PORTS_FOR_FIR:

Reloadable Coefficients - Number of Ports
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The total number of RTP ports created by a FIR is given by the formula:

``TotalRtpPorts =  (TP_SSR * TP_CASC_LEN)``

For example, if ``TP_SSR = 2`` and ``TP_CASC_LEN = 3``, the coefficient array is divided into ``2 * 3 = 6`` parts.

The FIR graph class provides a helper method: ``getTotalRtpPorts()`` to get the number of RTP ports. For more details, refer to: :ref:`API_REFERENCE`.

.. _RTP_ARRAY_SIZE_FOR_FIR:

Reloadable Coefficients - Array Size
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The array size is equal to the length of the FIR multiplied by the number of channels and divided equally by AI Engine kernels used, that is:

``TapsPerRtpPort = (TP_FIR_LEN) / (TP_SSR * TP_CASC_LEN)``

For example, if ``TP_FIR_LEN = 6``, if ``TP_SSR = 2`` and ``TP_CASC_LEN = 3``, the coefficient array is divided into ``2 * 3 = 6`` parts.

The FIR graph class provides a helper method: ``getTapsPerRtpPort(int kernelNo)`` to get the number of FIR taps per RTP port. For more details, refer to: :ref:`API_REFERENCE`.


.. _RTP_ARRAY_CONTENTS_FOR_FIR:

Reloadable Coefficients - Array Contents
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The FIR graph class provides a helper method: ``extractTaps(const std::vector<TT_COEFF>& taps, unsigned int kernelNo)`` to get coefficients for a given kernel. For more details, refer to: :ref:`API_REFERENCE`.

.. _WINDOW_API_FIRS:

Window Interface for Filters
------------------------------

On the AI Engine processor, data can be packetized into window buffers, which are mapped to the local memory.

Window buffers can be accessed with a 256-bit wide load/store operation, offering a throughput of up to 256 Gb/s (based on 1 GHz AI Engine clock).

In the case of FIRs, each window is extended by a margin so that the state of the filter at the end of the previous iteration of the window can be restored before the new computations begin.

Window buffers are implemented using a `ping-pong` mechanism, where the consumer kernel reads the `ping` portion of the buffer while the producer fills the `pong` portion of the buffer that is consumed in the next iteration.

In each iteration run, the kernel operates on a set number of samples from the window buffer, defined by the template parameter ``TP_INPUT_WINDOW_VSIZE``. To allow the kernel to safely operate on buffered data, a mechanism of lock acquires and releases is implemented.

.. note::  Window interface is not available in Super Sample Rate modes.

**Maximizing Throughput**

Buffer synchronization requirements introduce a fixed overhead when a kernel is triggered. To maximize throughput, set the window size to the maximum that the system allows.

For example, a four-tap single-rate symmetric FIR with a `2560` sample input/output window operating on ``int32`` data with ``int16`` implemented on AIE device produces an output window buffer in `354` clock cycles, which, taking into account kernel's startup overhead (around `40` lock cycles), equates to a throughput of close to `6500 MSa/s` (based on 1 GHz AI Engine clock).

.. note:: To achieve maximum performance, place the producer and consumer kernels in adjacent AI Engine tiles, so the window buffers can be accessed without a requirement for a MM2S/S2MM direct memory access (DMA) stream conversion.

**Latency**

Latency of a window-based FIR is predominantly due to the buffering in the input and output windows. Other factors that affect latency are data type and FIR length, though these tend to have a lesser effect.

For example, a 16-tap single-rate symmetric FIR with a `512` sample input/output window operating on ``cint16`` data with ``int16`` coefficients implemented on AIE device needs around `2.56 μs` (based on 1 GHz AI Engine clock) before the first full window of output samples is available for the consumer to read.
Subsequent iterations produce output data with reduced latency, due to the nature of the ping-pong buffering and pipelined operations.

To minimize the latency, set the buffer size to the minimum size that meets the required throughput.

.. _FIR_MULTIPLE_BUFFER_PORTS:

Multiple Buffer Ports
^^^^^^^^^^^^^^^^^^^^^

.. note:: AIE-ML and AIE-ML v2 devices only support a single input/output port.

**Multiple Input Ports**

Symmetric FIRs, including half-band FIRs, can be configured with two input buffers. Such an implementation is a trade-off between performance and resource utilization.
Symmetric FIRs with two input ports avoid the potential for memory read contention, which would otherwise result in stall cycles and therefore lower throughput.

Set the ``TP_DUAL_IP`` template parameter to 1 to create a FIR kernel with two input buffer ports.
In this scenario, a FIR kernel with two input ports is created where the FIR kernel expects each buffer to contain an exact copy of the same data.
Connect both input ports to the same data source through the FIR's graph.

**Multiple Output Ports**

All FIRs can be configured with two output buffers. Such a design allows greater routing flexibility that offers buffers to be connected directly to downstream components for further processing, avoiding a costly and limiting broadcast with a stream.

Set the ``TP_NUM_OUTPUTS`` template parameter to 2 to create a FIR kernel with two output buffer ports.
In this scenario, two exact copies of output data are produced in two independent memory buffers.

For example, a single-rate FIR with a `512` input sample buffer produces two output buffers, where each buffer is `512` samples.

.. _MAX_WINDOW_SIZE:

Maximum Window Size
^^^^^^^^^^^^^^^^^^^

| Window buffer is mapped into a local memory in the area surrounding the kernel that accesses it.
| A local memory storage is 32 kB (64 kB for AIE-ML and AIE-ML v2 devices), and the maximum size of the `ping-pong` window buffer must not exceed this limit.

.. _SINGLE_BUFFER_CONSTRAINT:

Single Buffer Constraint
^^^^^^^^^^^^^^^^^^^^^^^^

| It is possible to disable the `ping-pong` mechanism, so that the entire available data memory is available to the kernel for computation. However, the single-buffered window can be accessed only by one agent at a time, and it comes with a performance penalty.
| Achieve this by using the `single_buffer()` constraint applied to an input or output port of each kernel.

.. code-block::

    single_buffer(firGraph.getKernels()[0].in[0]);

.. _STREAM_API_FIRS:

Streaming Interface for Filters
---------------------------------

Streaming interfaces are now supported by all FIRs. Streaming interfaces are based on 32-bit AXI4-Stream and offer throughput of up to 32 Gb/s (based on 1 GHz clock) per stream used.

When ``TP_API = 1``, the FIR has stream API input and output ports, allowing greater interoperability and flexibility in placement of the design.
Streaming interfaces can be configured to connect single or dual stream inputs (driven by ``TP_DUAL_IP``) or one or two stream outputs (driven by ``TP_NUM_OUTPUTS``).

In general, stream based filters require less data buffering and therefore have lower memory requirements and lower latency than window API filters.

.. note:: AIE-ML and AIE-ML v2 devices only support a single input/output port.

.. note:: AIE-ML and AIE-ML v2 devices cannot take advantage of the symmetry of FIRs; therefore the FIRs implementation is always based on an asymmetric design.

**Asymmetric FIRs**

Asymmetric FIRs (single-rate, as well as rate-changing FIRs) use input and output streams directly.
As a result, there is no need for input/output buffering; asymmetric FIRs therefore offer very low latency and very low memory footprint.
Due to the lack of memory requirements, such designs can also operate on a very large number of samples within each kernel iteration (``TP_INPUT_WINDOW_VSIZE`` is limited to ``2^31 - 1``) achieving maximum performance and maximum throughput.

For example, a single kernel (``TP_CASC_LEN = 1``), 16-tap single-rate asymmetric FIR implemented on AIE device, using ``cint16`` data with frame size of `25600` and ``int16`` coefficients, offers throughput of `998 MSa/s` (based on 1 GHz AI Engine clock) and latency as low as tens of nanoseconds.

**Hybrid Streaming Interface for Symmetric and Half-band FIRs in Non-SSR Mode**

Symmetric FIRs, including half-band FIRs, cannot take full advantage of input streams when operating in a non-SSR mode, that is, ``TP_SSR``, ``TP_PARA_INTERP_POLY``, and ``TP_PARA_DECI_POLY`` are all set to 1.
Instead, the input stream is converted to a window buffer and the FIR kernels operate in a window-based architecture.
Output data is sent directly out through a stream port.
Such designs allow a more flexible connection and mapping onto AI Engine tiles.
Latency is reduced compared to a window-based equivalent, but is much greater compared with an asymmetric design. The lack of an output buffer also reduces the memory requirements.

For example, a 16-tap single-rate symmetric FIR implemented on AIE device with a `512` sample input/output window operating on ``cint16`` data and ``int16`` coefficients achieves a throughput of `978 MSa/s` (based on 1 GHz AI Engine clock) and needs around `1.4 μs` before a full window of samples is available for the consumer to read.

**Symmetric and Half-band FIRs in SSR Mode**

When operating in SSR mode, that is, ``TP_SSR``, ``TP_PARA_INTERP_POLY``, or ``TP_PARA_DECI_POLY`` are greater than 1, all Symmetric and Half-band FIRs, in addition to all Asymmetric FIRs, operate on input and output streams directly, offering very low latency and a minimal memory footprint.

For example, a 32-tap, single-rate symmetric FIR implemented on AIE device with a SSR set to 2 (``TP_SSR = 2``), using ``cint16`` data with frame size of `25600` and ``int16`` coefficients achieves throughput of `1998 MSa/s` (based on 1 GHz AI Engine clock) and latency as low as tens of nanoseconds.

.. _FIR_STREAM_OUTPUT:

Stream Output
^^^^^^^^^^^^^

Stream output allows computed data samples to be sent directly over the stream without the requirement for a ping-pong window buffer.
As a result, memory use and latency are reduced.
The streaming output also allows data samples to be broadcast to multiple destinations.

To maximize the throughput, FIRs can be configured with two output stream ports. However, this might not improve performance if the throughput is limited by other factors, that is, the input stream bandwidth or the vector processor.
Set the ``TP_NUM_OUTPUTS`` template parameter to 2 to create a FIR kernel with two output stream ports.
In this scenario, the output data from the two streams is split into chunks of 128-bits. For example:

* samples 0-3 to be sent over an output stream 0 for cint16 data type,

* samples 4-7 to be sent over an output stream 1 for cint16 data type.

Stream Input for Asymmetric FIRs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Stream input allows data samples to be written directly from the input stream to one of the Input Vector Registers without the requirement for a ping-pong window buffer.
As a result, the memory requirements and latency are reduced.

To maximize the throughput, FIRs can be configured with two input stream ports. Although this might not improve performance if the throughput is limited by other factors, that is, the output stream bandwidth or the vector processor.
Set ``TP_DUAL_IP`` to 1 to create a FIR instance with two input stream ports.
In such a case, the input data is merged from the two ports in 128-bit chunks, onto one data stream internally, for example:

* samples 0-3 to be received on an input stream 0 for cint16 data type,

* samples 4-7 to be received on an input stream 1 for cint16 data type.

.. note::  For the single rate asymmetric option, dual input streams offer no throughput gain if a single output stream is used. Therefore, dual input streams are only supported with two output streams.

Stream Input for Symmetric FIRs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Symmetric FIRs require access to data from two distinctive areas of the data stream and therefore require memory storage.
In symmetric FIRs, the stream input is connected to an input ping-pong window buffer through a DMA port of a Memory Module.

.. _FIR_FRAME_SIZE:

Setting FIR Frame Size
-----------------------

FIR frame size selection, through the FIR graph's ``TP_INPUT_WINDOW_VSIZE`` template parameter, is limited by a variety of factors, including the FIR variant or input interface type.
FIR kernels operate on the frame size provided using heavy pipelining and code repetition to schedule vector multiply-accumulate (VMAC) operations as frequently as possible.
As a result, FIR architectures can use a ``repetition_factor`` of up to 8 to achieve the best scheduling and therefore performance.
Taking into account that each vector operation might calculate four or eight output samples at a time, the selection of ``TP_INPUT_WINDOW_VSIZE`` is allowed in increments of, for example, 64.

Rate-changing FIRs also require the frame size to be divisible by ``TP_DECIMATION_FACTOR`` to fully process input data samples within the optimized loop.

In SSR mode (refer to :ref:`SSR_OPERATION_MODES`), the frame size you provide is also distributed across all input phases of the graph.

Therefore, the FIR graph's frame size ``TP_INPUT_WINDOW_VSIZE`` must be divisible by ``TP_SSR * TP_DECIMATION_FACTOR * Repetition_factor``.

An invalid selection reports a message in the form of a Metadata error or a `static_assert()` with rule violation details and a suggestion of how to fix it.

.. _FIR_LENGTH:

Setting FIR Length
-------------------

The minimum FIR length is set to 1.

Single Rate FIRs have no restriction placed on the FIR length selection.

However, rate-changing FIRs require each of the individual kernels to operate on a FIR length divisible by the Decimation or Interpolation factor, imposing limits on the FIR length, for example, ``TP_FIR_LEN % TP_DECIMATE_FACTOR == 0`` or ``TP_FIR_LEN % TP_INTERPOLATE_FACTOR == 0``.

In SSR modes (refer to :ref:`SSR_OPERATION_MODES`), coefficients are distributed equally across all output paths. As a result, the total number of FIR coefficients must be divisible by the number of paths, that is, ``TP_FIR_LEN % (TP_SSR) == 0``.
For rate-changing FIRs this raises additional limits on the FIR length, where the FIR length can be described by, for example: ``TP_FIR_LEN % (TP_SSR * TP_PARA_INTERP_POLY * TP_PARA_DECI_POLY) == 0``.

.. note:: An invalid selection reports a message in the form of a Metadata error or a `static_assert()` with the rule violation details and a suggestion of how to fix it.

.. _MAX_FIR_LENGTH:

Maximum FIR Length
-------------------

| The maximum FIR length that can be supported is limited by a variety of factors.
| Each of these factors, if exceeded, results in a compile-time failure with some indication of the nature of the limitation.

Maximum Window Based FIRs Length
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When using a window API, for example, the window buffer must fit into a 32 kByte memory bank. Because this includes the margin, it limits the maximum window size. Therefore, it also indirectly sets an upper limit on ``TP_FIR_LEN``.

The `single_buffer()` constraint is also needed to implement window buffers of > 16 kB. For more details, refer to: :ref:`SINGLE_BUFFER_CONSTRAINT`.

As a guide, a single rate symmetric FIR can support up to:

- 8k for 16-bit data, that is, int16 data

- 4k for 32-bit data, that is, cint16, int32, float

- 2k for 64-bit data, that is, cint32, cfloat

| Another limiting factor when considering an implementation of a high-order FIR is the Program Memory and sysmem requirements.
| Increasing the FIR length requires greater amounts of heap and stack memory to store coefficients. Program Memory footprint also increases as the number of instructions grows.
| As a result, a single FIR kernel can only support a limited number of coefficients. Longer FIRs have to be split into a design consisting of multiple FIR kernels using the ``TP_CASC_LEN`` parameter.

Maximum Stream Based FIRs Length
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| When using a stream-based API, the architecture uses internal vector registers to store data samples, instead of window buffers, which removes the limiting factors of the window-based equivalent architecture.
| However, the internal vector register is only 1024-bits wide, which greatly limits the number of data samples each FIR kernel can operate on.
| Data register storage capacity is also affected by decimation factors when a Decimation FIR is used.
| As a result, the number of taps each AI Engine kernel can process, limited by the capacity of the input vector register, depends on a variety of factors, such as data type, coefficient type, and decimation factor.

To help find the number of FIR kernels required (or desired) to implement the requested FIR length, refer to the helper functions: :ref:`MINIUM_CASC_LEN`, :ref:`OPTIMUM_CASC_LEN` described below.

.. _MINIUM_CASC_LEN:

Minimum Cascade Length
-----------------------

| To help find the minimum supported ``TP_CASC_LEN`` value for a given configuration, the following utility functions have been created in each FIR's graph file.
| The function signature for the single rate asymmetric filter is as follows:

.. code-block::

   template<int T_FIR_LEN, int T_API, typename T_D, typename T_C, unsigned int SSR>
   static constexpr unsigned int getMinCascLen();

where T_FIR_LEN is the tap length of the FIR; T_API refers to the type of port interface: 0 for a window API and 1 for a stream API. T_D and T_C are the data type and coeff type respectively. SSR is the parallelism factor set for the super sample rate operation.

.. code-block::

      using fir_graph = xf::dsp::aie::fir::sr_sym::fir_sr_asym_graph<DATA_TYPE, COEFF_TYPE, FIR_LEN, SHIFT, RND, INPUT_WINDOW_VSIZE>;

      static constexpr int kMinLen = fir_graph::getMinCascLen<FIR_LEN, API, DATA_TYPE, COEFF_TYPE, TP_SSR>();

      xf::dsp::aie::fir::sr_sym::fir_sr_asym_graph<DATA_TYPE, COEFF_TYPE, FIR_LEN, SHIFT, RND, INPUT_WINDOW_VSIZE,
                                                   kMinLen, USE_COEFF_RELOAD, NUM_OUTPUTS, API, SSR> firGraphWithMinLen;


More details are provided in the :ref:`API_REFERENCE`.

.. _OPTIMUM_CASC_LEN:

Optimum Cascade Length
-----------------------

| For FIR variants configured to use streaming interfaces, that is, ``TP_API = 1``, the optimum ``TP_CASC_LEN`` for a given configuration of the other parameters is a complex equation. Here, the optimum value of ``TP_CASC_LEN`` refers to the least number of kernels that the overall calculations can be divided into, when the interface bandwidth limits the maximum performance.
| To assist in this determination, utility functions have been created for FIR variants in their respective graph files.
| As an example, the function signature for the single rate asymmetric filter is shown below:

.. code-block::

   template<int T_FIR_LEN, typename T_D, typename T_C, int T_PORTS, unsigned int SSR>
   static constexpr unsigned int getOptCascLen();

where T_FIR_LEN is the tap length of the FIR, T_D and T_C are the data type and coeff type respectively, and T_PORTS refers to the single/dual ports. SSR is the parallelism factor set for the super sample rate operation.

The following example shows how to use ``getOptCascLen`` and ``getMinCascLen``. First declare a placeholder graph of the FIR type you need, then use it to call the static functions with the parameters you want to configure.

.. code-block::

      using fir_graph = xf::dsp::aie::fir::sr_sym::fir_sr_asym_graph<DATA_TYPE, COEFF_TYPE, FIR_LEN, SHIFT, RND, INPUT_WINDOW_VSIZE>;

      static constexpr int kOptLen = fir_graph::getOptCascLen<FIR_LEN, DATA_TYPE, COEFF_TYPE, NUM_OUTPUTS, TP_SSR>();

      xf::dsp::aie::fir::sr_sym::fir_sr_asym_graph<DATA_TYPE, COEFF_TYPE, FIR_LEN, SHIFT, RND, INPUT_WINDOW_VSIZE,
                                                   kOptLen, USE_COEFF_RELOAD, NUM_OUTPUTS, API, SSR> firGraphWithOptLen;


More details are provided in the :ref:`API_REFERENCE`.

.. _SSR_OPERATION:

Super Sample Rate
-----------------

The term Super Sample Rate strictly means the processing of more than one sample per clock cycle. Because the AI Engine is a vector processor, almost every operation is SSR by this definition, making it superfluous. Therefore, in the AI Engine context, SSR means an implementation using multiple computation paths to improve performance at the expense of additional resource use.

.. _SSR_OPERATION_MODES:

Super Sample Rate - Operation Modes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the FIR, SSR operation can be achieved using one of the following modes:

- :ref:`SSR_OPERATION_COEFF_DATA_DISTRO`: Driven by the ``TP_SSR`` template parameter. The mode creates an array of ``TP_SSR^2`` kernels and creates ``TP_SSR`` input and output ports.
- :ref:`SSR_OPERATION_PARA_DECI_POLY`: Driven by the ``TP_PARA_DECI_POLY`` template parameter. The mode creates a vector of ``TP_PARA_DECI_POLY`` kernels and creates ``TP_PARA_DECI_POLY`` input ports.
- :ref:`SSR_OPERATION_PARA_INTERP_POLY`: Driven by the ``TP_PARA_INTERP_POLY`` template parameter. The mode creates a vector of ``TP_PARA_INTERP_POLY`` kernels and creates ``TP_PARA_INTERP_POLY`` output ports.

.. _SSR_OPERATION_RESOURCE_UTILIZATION:

Super Sample Rate - Resource Utilization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The number of AI Engine tiles used by a FIR is given by the formula:

.. code-block::

  NUMBER_OF_AIE_TILES = TP_CASC_LEN * TP_SSR * TP_SSR * TP_PARA_INTERP_POLY * TP_PARA_DECI_POLY

Examples of this formula are given in the following table.


.. _SSR_OPERATION_RESOURCE_TABLE:

.. table:: FIR SSR Resource Usage Examples
   :align: center

   +---------+------------------------+-------------------------+--------------+-------------------+
   | TP_SSR  | TP_PARA_INTERP_POLY    | TP_PARA_DECI_POLY       | TP_CASC_LEN  |  Number of Tiles  |
   +=========+========================+=========================+==============+===================+
   |    1    |            1           |           1             |      3       |         3         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    1    |            1           |           2             |      3       |         6         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    1    |            2           |           1             |      3       |         6         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    1    |            3           |           1             |      3       |         9         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    1    |            2           |           3             |      1       |         6         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    1    |            3           |           2             |      1       |         6         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    2    |            1           |           1             |      1       |         4         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    2    |            1           |           2             |      1       |         8         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    2    |            1           |           1             |      2       |         8         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    2    |            2           |           1             |      2       |        16         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    3    |            1           |           1             |      2       |        18         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    4    |            1           |           1             |      2       |        32         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    4    |            1           |           1             |      3       |        48         |
   +---------+------------------------+-------------------------+--------------+-------------------+
   |    4    |            2           |           1             |      3       |        96         |
   +---------+------------------------+-------------------------+--------------+-------------------+

.. _SSR_OPERATION_PORT_UTILIZATION:

Super Sample Rate - Port Utilization and Throughput
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The number of input/output ports created by a FIR is given by the formula:

* Number of input ports: ``NUM_INPUT_PORTS  = TP_PARA_DECI_POLY x TP_SSR x (TP_DUAL_IP + 1)``

* Number of output ports: ``NUM_OUTPUT_PORTS  = TP_PARA_INTERP_POLY x TP_SSR x TP_NUM_OUTPUTS``

Therefore, the maximum throughput achievable for a given data type, for example, cint16 and 1 GHz AI Engine clock, can be estimated with:

* maximum theoretical sample rate at input: ``THROUGHPUT_IN  = NUM_INPUT_PORTS x 1 GSa/s``,

* maximum theoretical sample rate at output: ``THROUGHPUT_OUT  = NUM_OUTPUT_PORTS x 1 GSa/s``.


**AI Engine Tile Utilization Ratio**

A Super Sample Rate operation creates multiple computation paths used to produce the output samples.
Having multiple computation paths reduces the amount of computation required by each kernel.

The total number of FIR computation paths can be described with the following formula:

.. code-block::

  NUMBER_OF_COMPUTATION_PATHS = TP_CASC_LEN  * TP_SSR * TP_PARA_INTERP_POLY * TP_PARA_DECI_POLY

The FIR graph tries to split the requested FIR workload among the FIR kernels equally, which can mean that each kernel is tasked with a comparatively low computational effort.

In such a scenario, the bandwidth is limited by the number of ports, but the AI Engine tile utilization ratio (often defined as ratio of VMAC operations to cycles without VMAC operation) might be reduced.

For example, a 32-tap Single Rate FIR operating on a ``cint16`` data type and ``int16`` coefficients with ``TP_SSR`` set to 2 and a cascade length ``TP_CASC_LEN`` set to 2 performs at the bandwidth close to `2 GSa/s` (2 output stream paths). Each of the kernels is tasked with computing only eight coefficients. The design uses eight FIR kernels mapped to eight AI Engine tiles to achieve that.
However, a similarly configured FIR, a 32-tap Single Rate FIR operating on ``cint16`` data type and ``int16`` coefficients with ``TP_SSR`` set to 2, but without further cascade configuration (``TP_CASC_LEN`` set to 1), also performs at the bandwidth close to `2 GSa/s` but consumes only four kernels to achieve that.

**Rate-Changing FIR Throughput**

| For rate changers, the bandwidth of either the input or output port, depending on whether it is a decimator or an interpolator, can limit the throughput of the filter.
| For example, an interpolator with an interpolation factor of 3 produces three times the number of outputs as inputs. However, the AI Engine stream port bandwidth is the same for the input and output.
| Hence, if the output runs at maximum bandwidth, the input needs to run at 1/3rd its maximum bandwidth, and you are forced to underutilize the input stream of the filter at only 33 percent efficiency.
| However, if you are able to split the operation of the interpolator over three kernels, broadcast the input stream to their inputs, and operate the kernels at maximum performance, it is possible to use both the input and output bandwidths at their maximum bandwidths.

.. _SSR_OPERATION_COEFF_DATA_DISTRO:

Super Sample Rate - Coefficient and Data Distribution
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The base mode of SSR is driven by the use of the ``TP_SSR`` template parameter.
The ``TP_SSR`` parameter allows a trade of performance for resource use in the form of tiles used.

When used, a number of ``TP_SSR`` input phases and a number of ``TP_SSR`` output paths are created.
An array of ``TP_SSR^2`` FIR sub-graphs is created to connect the input phases and output paths.

Input data samples are distributed across the input phases in a round-robin, sample-by-sample mechanism where each input phase processes a fraction of the input samples, that is, ``TP_INPUT_WINDOW_VSIZE / TP_SSR``. For more details, refer to: :ref:`SSR_PORT_MAPPING`.

Coefficients are distributed such that each output path consists of all the FIR coefficients, but each FIR sub-graph in any given output path is only configured to operate on a fraction of the FIR length, that is, it operates on ``TP_FIR_LEN / TP_SSR`` number of coefficients.

As a result, each FIR sub-graph operates on a fraction of coefficients and a fraction of the data, giving an overall increased performance.

Each FIR sub-graph can also be further split into multiple FIR kernels with the use of a cascade interface, which is driven by the ``TP_CASC_LEN`` template parameter.

Super Sample Rate - Coefficient and Data Distribution - Resampling Limitations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The resampling process requires input data samples to be "reused" in a computation of multiple output samples, as per the ``TP_INTERPOLATION_FACTOR/TP_DECIMATION_FACTOR`` configuration. Because a given input data sample might be required by more than one output path, but not all the output paths, it is not possible to create efficient data distribution connections that take advantage of extra resources without fully decomposing into Interpolation Polyphases and Decimation Polyphases first.

Therefore, Resampling only supports ``TP_SSR`` mode when ``TP_PARA_INTERP_POLY = TP_INTERPOLATION_FACTOR`` and ``TP_PARA_DECI_POLY = TP_DECIMATION_FACTOR``.

.. _SSR_PORT_MAPPING:

Super Sample Rate - Sample to Port Mapping
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When a Super Sample Rate operation is used, data is input and output using multiple ports. These multiple ports on input or output act as one channel.

The input data channel must be split over multiple ports where each successive input sample is sent to a different input port in a round-robin fashion, that is, sample 0 goes to input port :code:`in[0]`, sample 1 to :code:`in[1]`, and so on up to ``N-1`` where ``N = TP_SSR``. Then sample N goes to :code:`in[0]`, sample N+1 goes to :code:`in[1]`, and so on. Output samples are output from the multiple output ports in the same fashion.

Where ``TP_DUAL_IP`` is also enabled, there are two sets of SSR input ports, :code:`in` and :code:`in2`, where data must be organized in a 128-bit interleaved pattern.
Allocate samples to ports 0 to N-1 of port :code:`in` in the round-robin fashion above until each port has 128-bits of data, then allocate the next samples in a round-robin fashion to ports 0 through N-1 of port :code:`in2` until these have 128-bits of data, then return to allocating samples to ports 0 through N-1 of :code:`in`, and repeat.

For example, if you have a data stream like :code:`int32 x = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ...`, then an SSR of 3 with dual input ports would look like:

.. code-block::

  in[0] = 0, 3, 6, 9, 24, 27, 30, 33, ...
  in[1] = 1, 4, 7, 10, 25, 28, 31, 34, ...
  in[2] = 2, 5, 8, 11, 26, 29, 32, 35, ...
  in2[0] = 12, 15, 18, 21, 36, 39, 42, 45, ...
  in2[1] = 13, 16, 19, 22, 37, 40, 43, 46, ...
  in2[2] = 14, 17, 20, 23, 38, 41, 44, 47, ...

The output data is produced in a similar method.
Samples are sent to each port in a round-robin fashion. When two output ports are in use (``TP_NUM_OUTPUTS`` set to 2), samples are also organized in 128-bit interleaved patterns.

.. _SSR_OPERATION_PARA_INTERP_POLY:

Super Sample Rate - Interpolation Polyphases
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

FIR can also decompose the interpolation process into multiple parallel polyphases using the ``TP_PARA_INTERP_POLY`` template parameter.

When used, ``TP_PARA_INTERP_POLY`` output paths are created.

.. note:: The total number of output paths is the result of the multiplication of: ``NUM_OUTPUT_PORTS  = TP_PARA_INTERP_POLY x TP_SSR``.

The polyphases are executed in parallel and the output data produced by each polyphase directly becomes the filter's output.
``TP_PARA_INTERP_POLY`` does not affect the number of input data paths. It is only useful when the filter has an interpolation factor greater than 1.

For example, when ``TP_SSR = 1``, and ``TP_PARA_INTERP_POLY = 3``, the input stream looks like:

.. code-block::

   in[0] = i0, i1, i2, i3, i4, i5, i6, i7, i8, ...

And the output stream looks like:

.. code-block::

   out[0] = o0, o3, o6, o9, o12, o15, ...
   out[1] = o1, o4, o7, o10, o13, o16, ...
   out[2] = o2, o5, o8, o11, o14, o17, ...

When ``TP_SSR = 1``, ``TP_PARA_INTERP_POLY = 4``, the input stream is the same as before, because ``TP_PARA_INTERP_POLY`` only affects the number of output streams.
The output stream looks like:

.. code-block::

   out[0] = o0, o4, o8, o12, ...
   out[1] = o1, o5, o9, o13, ...
   out[2] = o2, o6, o10, o14, ...
   out[3] = o3, o7, o11, o15, ...

And when ``TP_SSR = 2``, ``TP_PARA_INTERP_POLY = 3``, the input stream needs to look like this:

.. code-block::

   in[0] = i0, i2, i4, i6, i8, i10, ...
   in[1] = i1, i3, i5, i7, i9, i11, ...

The output stream produces data in this form:

.. code-block::

   out[0] = o0, 06, o12, o18, o24, ...
   out[1] = o1, o7, o13, o19, o25, ...
   out[2] = o2, o8, o14, o20, o26, ...
   out[3] = o3, o9, o15, o21, o27, ...
   out[4] = o4, o10, o16, o22, o28, ...
   out[5] = o5, o11, o17, o23, o29, ...

You can think of ``TP_SSR x TP_PARA_INTERP_POLY`` as an effective ``OUT_SSR`` which gives you the maximum output sample rate of the filter.

.. _SSR_OPERATION_PARA_DECI_POLY:

Super Sample Rate - Decimation Polyphases
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

FIR can also decompose the decimation process into multiple polyphases using the ``TP_PARA_DECI_POLY`` template parameter.

| The effect of ``TP_PARA_DECI_POLY`` is to remove the bottleneck posed by the input bandwidth on the overall throughput of the FIR filter.
| In decimators, every DECIMATE_FACTOR number of inputs produces one more output. So, when the input streams are utilized at their maximum bandwidth, the output stream can only be utilized at 1/DECIMATE_FACTOR of their maximum bandwidth. With ``TP_PARA_DECI_POLY > 1``, use the ``TP_PARA_DECI_POLY`` number of input phases to provide extra input stream bandwidth.
| The input data stream is split into ``TP_PARA_DECI_POLY`` input data phases. Outputs from these input phases are then added together to produce the overall filter's output.

.. note:: The total number of input phases is the result of the multiplication of: ``NUM_INPUT_PORTS  = TP_PARA_DECI_POLY x TP_SSR``.

For example, when ``TP_SSR = 1``, and ``TP_PARA_DECI_POLY = 3``, the input stream looks like:

.. code-block::

   in[0] = i0, i3, i6, i9, ...
   in[1] = i1, i4, i7, i10, ...
   in[2] = i2, i5, i8, i11, ...

The output stream has SSR output paths and looks like:

.. code-block::

   out[0] = o0, o1, o2, o3, o4, ...

For ``TP_SSR = 1``, ``TP_PARA_DECI_POLY = 4``, the input stream looks like:

.. code-block::

   in[0] = i0, i4, i8, ...
   in[1] = i1, i5, i9, ...
   in[2] = i2, i6, i10, ...
   in[3] = i3, i7, i11, ...

The output stream looks the same as in the previous configuration because ``TP_PARA_DECI_POLY`` only affects the number of input streams.

When ``TP_SSR = 2``, ``TP_PARA_DECI_POLY = 3``, the input stream looks like:

.. code-block::

   in[0] = i0, i6, i12, i18, i24, ...
   in[1] = i1, i7, i13, i19, i25, ...
   in[2] = i2, i8, i14, i20, i26, ...
   in[3] = i3, i9, i15, i21, i27, ...
   in[4] = i4, i10, i16, i22, i28, ...
   in[5] = i5, i11, i17, i23, i29, ...

The output stream looks like:

.. code-block::

   out[0] = o0, o2, o4, o6, o8, o10, ...
   out[1] = o1, o3, o5, o7, o9, o11, ...

For more details about how to configure the various parameters to meet various performance metrics, refer to :ref:`FIR_CONFIGURATION_NOTES`.

.. _FIR_CONSTRAINTS:

Constraints
-----------

To apply constraints within the FIR instance for successful design mapping, you need to know the internal instance names for graph and kernel objects.

Each FIR variant has a variety of access methods to help assign a constraint on a kernel and/or a net, for example:

- `getKernels()` which returns a pointer to an array of kernel pointers, or

- `getInNet()` which returns a pointer to a net indexed by method's argument(s).

More details are provided in the :ref:`API_REFERENCE`.

When configured for SSR operation, the FIR has a two-dimensional array (paths x phases) of units that are themselves FIRs, though each atomic FIR in this structure can itself be a series of kernels as described by ``TP_CASC_LEN``. The `getKernels()` access function returns a pointer to the array of kernels within the SSR FIR. This array has ``TP_SSR * TP_SSR * TP_CASC_LEN`` members. The index in the array is determined by its path number, phase number, and cascade position as shown in the following equation.

.. code-block::

   Kernel Index = Kernel Path * TP_SSR * TP_CASC_LEN + Kernel Phase * TP_CASC_LEN + Kernel Cascade index

For example, in a design with ``TP_CASC_LEN = 2`` and ``TP_SSR = 3``, the first kernel of the last path has an index 12.

The nets returned by the `getInNet()` function can be assigned custom fifo_depths values to override the defaults.

.. _FIR_CONFIGURATION_NOTES:

Configuration Notes
===================

This section provides guidance on configuring the FIRs for typical scenarios and for designs that prioritize a specific metric, such as resource use or performance.

**Configuring for Requirements Based on Performance Versus Resource Use**

The least resource-expensive method to obtain higher performance is to use the dual ports features, that is, ``TP_DUAL_IP`` = 1 and/or ``TP_NUM_OUTPUTS`` = 2.

| The next method that offers higher performance at lower resource costs is the ``TP_PARA_{INTERP/DECI}_POLY`` parameter.
| ``TP_PARA_X_POLY`` can take a minimum value of 1 and a maximum value equal to the interpolation factor or the decimation factor. It can increase in steps of the integer factors of the interpolation or decimation factor.
| Note that the advantage of higher throughput comes at the cost of additional AI Engine tiles. When you set the ``TP_PARA_X_POLY`` parameter, the graph creates ``TP_PARA_X_POLY`` polyphase paths. Each path contains ``TP_CASC_LEN`` kernels. The number of tiles used is ``TP_PARA_X_POLY * TP_CASC_LEN``, that is, ``TP_PARA_X_POLY`` is a single dimensional expansion.

| ``TP_SSR`` is the parameter that enables finer control over the throughput and AI Engine tiles use.
| The number of tiles used is ``TP_CASC_LEN * TP_SSR * TP_SSR``, that is, SSR is a 2-dimensional expansion. Both methods can work in addition to the ``TP_CASC_LEN`` parameter which also increases the number of tiles. ``TP_SSR`` can take any positive integer value and its maximum is only limited by the number of AI Engine tiles available. This can be used to prevent overutilization of kernels if the throughput requirement is not as high as the one offered by ``TP_PARA_X_POLY``.

``TP_CASC_LEN`` sets the number of kernels cascaded together to distribute the ``TP_FIR_LEN`` calculation. It works alongside ``TP_SSR`` and ``TP_PARA_X_POLY`` to overcome bottlenecks in the vector processor. The library provides access functions to find the ``TP_CASC_LEN`` value that gives optimum performance — that is, the minimum number of kernels that can deliver maximum performance. For more details, refer to :ref:`API_REFERENCE`.

If there is no constraint on the number of AI Engine tiles, the easiest way to get the required performance is to set the ``TP_PARA_X_POLY`` to the closest factor of the interpolation/decimation rate that is higher than the throughput needed. If, however, the goal is to obtain a performance using the least number of tiles, ``TP_SSR`` might need to be used as a finer tuning parameter to get the throughput you want.

**SCENARIO 1:**

| For a 64-tap interpolate by 5 filter that needs 4 GSa/s at output:
| ``TP_PARA_INTERP_POLY`` can only be set to 5; this needs at least five AI Engine tiles. The optimum cascade length is 2. This uses 10 AI Engine tiles and gives you 10 GSa/s at the output.
| On the other hand, setting ``TP_SSR = 2`` and ``TP_PARA_INTERP_POLY = 1`` achieves that in four AI Engine tiles, and the maximum throughput at the output is 4 GSa/s.

**SCENARIO 2:**

| For a 32-tap interpolate by 2 filter that needs 4 GSa/s at output:
| ``TP_PARA_INTERP_POLY`` can be set to 2. This creates two output paths and therefore at least two AI Engine tiles. Say that the optimum cascade length for the data_type/coeff_type combination is 2. Set ``TP_CASC_LEN = 2``.
| The optimum cascade lengths for the various parameters can be obtained using the helper functions in :ref:`API_REFERENCE`. With these two output paths, it is possible to obtain the required sample rate of 4 GSa/s.

**SCENARIO 3:**

| For a 32-tap interpolate by 2 filter that needs 8 GSa/s at output:
| ``TP_PARA_INTERP_POLY`` can be set to 2 (which is the maximum value). This creates a maximum of two output paths which can only have a maximum throughput of 4 GSa/s.
| Because ``TP_PARA_INTERP_POLY`` cannot be increased further, use the ``TP_SSR`` parameter to increase the available throughput. Setting ``TP_SSR = 2`` doubles the total available throughput by doubling the input and output paths.
| Note that the optimum cascade length in this case is different.


.. |trade|  unicode:: U+02122 .. TRADEMARK SIGN
   :ltrim:
.. |reg|    unicode:: U+000AE .. REGISTERED TRADEMARK SIGN
   :ltrim:
