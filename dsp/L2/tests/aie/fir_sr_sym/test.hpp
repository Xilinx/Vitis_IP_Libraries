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
Single Rate Symmetrical FIR graph class.
*/

#include <adf.h>
#include <vector>
#include "utils.hpp"

#include "uut_config.h"
#include "uut_static_config.h"

#include "test_utils.hpp"
#include "fir_common_traits.hpp"
#include "device_defs.h"
#include "fir_sr_sym_native_generated_graph/fir_sr_sym_generated_graph.h"

#ifndef UUT_GRAPH
#define UUT_GRAPH fir_sr_sym_graph
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
    static constexpr unsigned int kNumTaps = (FIR_LEN + 1) / 2;

   public:
    std::array<input_plio, P_SSR*(DUAL_INPUT_SAMPLES + 1)> in;
    std::array<output_plio, P_SSR * NUM_OUTPUTS> out;

#ifdef USING_UUT
    // Use Generated Graph
    using uut = fir_sr_sym_native_generated_graph;
#else
    // Use Graph class directly for reference model
    using uut = dsplib::fir::sr_sym::UUT_GRAPH<DATA_TYPE,
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

#if (USE_COEFF_RELOAD == 1) // Reloadable coefficients
        static_assert(NITER % 2 == 0,
                      "ERROR: Please set NITER to be a multiple of 2 when reloadable coefficients are used");
#endif

        // Make connections
        createPLIOFileConnections<P_SSR, DUAL_INPUT_SAMPLES>(in, QUOTE(INPUT_FILE), "in");
        createPLIOFileConnections<P_SSR, (NUM_OUTPUTS - 1)>(out, QUOTE(OUTPUT_FILE), "out");

        for (unsigned int i = 0; i < P_SSR; ++i) {
            unsigned int plioIpBaseIdx = i * (DUAL_INPUT_SAMPLES + 1);
            unsigned int plioOpBaseIdx = i * (DUAL_INPUT_SAMPLES + 1);
            connect<>(in[plioIpBaseIdx].out[0], firGraph.in[i]);

#if (DUAL_IP == 1 && PORT_API == 0) // dual input to avoid contention
            // if not using interleaved streams, just use a duplicate of in1
            connect<>(in[0].out[0], firGraph.in2[i]); // firGraph.in2[i] later. Script performs clone. If it doesn't,
                                                      // remove DUAL_INPUT_SAMPLES here.
#endif
#if (DUAL_IP == 1 && PORT_API == 1)
            connect<>(in[plioIpBaseIdx + 1].out[0], firGraph.in2[i]); // firGraph.in2[i] once graph supports array ports
#endif

            connect<>(firGraph.out[i], out[plioOpBaseIdx].in[0]);
#if (NUM_OUTPUTS == 2)
#ifdef USING_UUT
            connect<>(firGraph.out2[i], out[plioOpBaseIdx + 1].in[0]);
#endif // USING_UUT
#endif
        }

#if (USE_COEFF_RELOAD == 1)
        for (int i = 0; i < rtpPortNumber; i++) {
            connect<>(coeff[i], firGraph.coeff[i]);
        }
#endif

#ifdef USING_UUT
        const int MAX_PING_PONG_SIZE = __DATA_MEM_BYTES__ / 2;
#if (P_SSR == 1) // Check buffer size is within limits, when buffer interface is used.
        const int bufferSize = (PORT_API == 1 ? 0 : (FIR_LEN + INPUT_SAMPLES) * sizeof(DATA_TYPE));
        if (bufferSize > MAX_PING_PONG_SIZE || (SINGLE_BUF == 1 && PORT_API == 0)) {
            single_buffer(firGraph.filter.getKernels()->in[0]);
            single_buffer(firGraph.filter.getKernels()[CASC_LEN - 1].out[0]);
            if (DUAL_IP == 1) {
                single_buffer(firGraph.filter.getKernels()->in[1]);
            }
            if (NUM_OUTPUTS == 2) {
                single_buffer(firGraph.filter.getKernels()[CASC_LEN - 1].out[1]);
            }
        }

        if (bufferSize > MAX_PING_PONG_SIZE || (SINGLE_BUF == 1 && PORT_API == 0)) {
            for (int ssr = 0; ssr < P_SSR; ssr++) {
                single_buffer(firGraph.filter.getKernels()[CASC_LEN * ssr * P_SSR].in[0]);
                printf("INFO: Single Buffer Constraint applied to input buffer-0 of kernel %d.\n",
                       CASC_LEN * ssr * P_SSR);

                single_buffer(firGraph.filter.getKernels()[CASC_LEN * (ssr + 1) * P_SSR - 1].out[0]);
                printf("INFO: Single Buffer Constraint applied to output buffer-0 of kernel %d.\n",
                       CASC_LEN * (ssr + 1) * P_SSR - 1);

                if (DUAL_IP == 1) {
                    single_buffer(firGraph.filter.getKernels()[CASC_LEN * ssr * P_SSR].in[1]);
                    printf("INFO: Single Buffer Constraint applied to input buffer-1 of kernel %d.\n",
                           CASC_LEN * ssr * P_SSR);
                }
                if (NUM_OUTPUTS == 2) {
                    single_buffer(firGraph.filter.getKernels()[CASC_LEN * (ssr + 1) * P_SSR - 1].out[1]);
                    printf("INFO: Single Buffer Constraint applied to output buffer-1 of kernel %d.\n",
                           CASC_LEN * (ssr + 1) * P_SSR - 1);
                }
            }
        }
#endif
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
        printf("========================\n");
    };
};
}
}
}
};

#endif // _DSPLIB_TEST_HPP_
