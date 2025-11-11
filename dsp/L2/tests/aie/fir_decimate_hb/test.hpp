/*
 * Copyright (C) 2019-2022, Xilinx, Inc.
 * Copyright (C) 2022-2025, Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _DSPLIB_TEST_HPP_
#define _DSPLIB_TEST_HPP_

/*
This file holds the declaraion of the test harness graph class for the
Halfband Decimation FIR graph class.
*/

#include <adf.h>
#include <vector>
#include "utils.hpp"
#include "uut_config.h"
#include "uut_static_config.h"
#include "test_utils.hpp"
#include "fir_common_traits.hpp"
#include "device_defs.h"
#include "fir_decimate_hb_native_generated_graph/fir_decimate_hb_generated_graph.h"

#ifndef UUT_GRAPH
#define UUT_GRAPH fir_decimate_hb_graph
#endif

#include QUOTE(UUT_GRAPH.hpp)

using namespace adf;

namespace xf {
namespace dsp {
namespace aie {
namespace testcase {
namespace dsplib = xf::dsp::aie;

class test_graph : public graph {
   private:
    static constexpr unsigned int kNumTaps = (FIR_LEN + 1) / 4 + 1;

   public:
#ifdef USING_UUT
    static constexpr int DUAL_OUTPUT_SAMPLES = NUM_OUTPUTS;
#else
    static constexpr int DUAL_OUTPUT_SAMPLES = 1;
#endif
    static constexpr unsigned int IN_SSR = P_SSR * P_PARA_DECI_POLY;
    static constexpr unsigned int RTP_SSR = P_SSR;
    std::array<input_plio, IN_SSR*(DUAL_INPUT_SAMPLES + 1)> in;
    std::array<output_plio, P_SSR * NUM_OUTPUTS> out;

#ifdef USING_UUT
    // Use Generated Graph class
    using uut = fir_decimate_hb_native_generated_graph;
#else
    // Use Graph class directly for reference model
    using uut = dsplib::fir::decimate_hb::UUT_GRAPH<DATA_TYPE,
                                                    COEFF_TYPE,
                                                    FIR_LEN,
                                                    SHIFT,
                                                    ROUND_MODE,
                                                    INPUT_SAMPLES,
                                                    CASC_LEN,
                                                    DUAL_IP,
                                                    USE_COEFF_RELOAD,
                                                    NUM_OUTPUTS,
                                                    PORT_API,
                                                    P_SSR,
                                                    P_PARA_DECI_POLY,
                                                    SAT_MODE>;
#endif

    static constexpr int rtpPortNumber = uut::getTotalRtpPorts();
#if (USE_COEFF_RELOAD == 1)
    port_conditional_array<input, USE_COEFF_RELOAD == 1, rtpPortNumber> coeff;
#endif

    std::vector<COEFF_TYPE> m_taps_v =
        generateTaps<COEFF_TYPE, COEFF_STIM_TYPE, kNumTaps, COEFF_SEED>(QUOTE(COEFF_FILE));

    uut firGraph;

#if (USE_COEFF_RELOAD == 1)
    // Constructor
    test_graph() {
#else
    test_graph() : firGraph(m_taps_v) {
#endif
        printConfig();

// FIR sub-graph
#if (USE_COEFF_RELOAD == 1) // Reloadable coefficients
        static_assert(NITER % 2 == 0,
                      "ERROR: Please set NITER to be a multiple of 2 when reloadable coefficients are used");
#endif

        // Make connections
        createPLIOFileConnections<IN_SSR, DUAL_INPUT_SAMPLES>(in, QUOTE(INPUT_FILE), "in");
        createPLIOFileConnections<P_SSR, (NUM_OUTPUTS - 1)>(out, QUOTE(OUTPUT_FILE), "out");

        for (unsigned int i = 0; i < P_SSR; ++i) {
            unsigned int plioBaseIdx = P_PARA_DECI_POLY * i * (DUAL_INPUT_SAMPLES + 1);

            connect<>(in[plioBaseIdx].out[0], firGraph.in[i * P_PARA_DECI_POLY]);

#if (P_PARA_DECI_POLY == 2)
            connect<>(in[plioBaseIdx + (DUAL_INPUT_SAMPLES + 1)].out[0], firGraph.in[i * P_PARA_DECI_POLY + 1]);
#endif
#if (DUAL_IP == 1 && PORT_API == 0) // dual input to avoid contention
            // if not using interleaved streams, just use a duplicate of in1
            connect<>(in[plioBaseIdx].out[0], firGraph.in2[i]);
#endif
#if (DUAL_IP == 1 && PORT_API == 1)
            connect<>(in[plioBaseIdx + DUAL_INPUT_SAMPLES].out[0],
                      firGraph.in2[i * P_PARA_DECI_POLY]); // firGraph.in2[i] once graph supports array ports
#if (P_PARA_DECI_POLY == 2)
            connect<>(in[plioBaseIdx + 3].out[0], firGraph.in2[i * P_PARA_DECI_POLY + 1]);
#endif
#endif
        }
        for (unsigned int i = 0; i < P_SSR; ++i) {
            unsigned int plioOutputBaseIdx = i * NUM_OUTPUTS;
#if (USE_CHAIN == 1 && NUM_OUTPUTS == 1)
            // Chained connections mutually explusive with multiple outputs.
            connect<>(firGraph.out, firGraph2.in);
            connect<>(firGraph2.out, out[plioBaseIdx].in[0]);
#else
            connect<>(firGraph.out[i], out[plioOutputBaseIdx].in[0]);
#if (NUM_OUTPUTS == 2)
            connect<>(firGraph.out2[i], out[plioOutputBaseIdx + DUAL_OUTPUT_SAMPLES - 1]
                                            .in[0]); // firGraph.out[1] or similar when array ports are supported.
#endif
#endif
        }

#if (USE_COEFF_RELOAD == 1)
        for (int i = 0; i < rtpPortNumber; i++) {
            connect<>(coeff[i], firGraph.coeff[i]);
        }
#endif

#ifdef USING_UUT
        const int MAX_PING_PONG_SIZE = __DATA_MEM_BYTES__ / 2;
        if (PORT_API == 0) {
            const int bufferSize = ((FIR_LEN + INPUT_SAMPLES) * sizeof(DATA_TYPE));
            if ((bufferSize > MAX_PING_PONG_SIZE) || (SINGLE_BUF == 1)) {
                for (unsigned int ssr = 0; ssr < P_SSR; ++ssr) {
                    for (unsigned int i = 0; i < P_SSR; ++i) {
                        single_buffer(firGraph.filter.getKernels()[ssr * P_SSR * CASC_LEN + CASC_LEN * i].in[0]);
                    }

                    single_buffer(firGraph.in[ssr * P_PARA_DECI_POLY]);
                    printf("INFO: Single Buffer Constraint applied to input buffers of kernel %d.\n",
                           ssr * P_PARA_DECI_POLY);
#if (P_PARA_DECI_POLY == 2)
                    single_buffer(firGraph.in[ssr * P_PARA_DECI_POLY + 1]);
                    printf("INFO: Single Buffer Constraint applied to input buffers of kernel %d.\n",
                           ssr * P_PARA_DECI_POLY + 1);
#endif

#if (DUAL_IP == 1)
                    single_buffer(firGraph.in2[ssr]);
#endif

                    single_buffer(firGraph.out[ssr]);
#if (NUM_OUTPUTS == 2)
                    single_buffer(firGraph.out2[ssr]);
#endif
                    printf("INFO: Single Buffer Constraint applied to output buffers of kernel %d.\n", ssr);
                }
            }
        }

#endif

#ifdef USING_UUT
        // Report out for AIE Synthesizer QoR harvest
        if (firGraph.filter.getKernels() != NULL) {
            printf("KERNEL_ARCHS: [");
            int arch = firGraph.filter.getKernelArchs();
            printf("%d", arch);
            printf("]\n");
        }
#endif
    };
};
}
}
}
};
#endif // _DSPLIB_TEST_HPP_