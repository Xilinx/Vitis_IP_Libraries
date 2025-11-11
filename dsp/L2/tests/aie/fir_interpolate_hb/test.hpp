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
Halfband Interpolator FIR graph class.
*/

#include <adf.h>
#include <vector>
#include "utils.hpp"

#include "uut_config.h"
#include "uut_static_config.h"
#include "test_utils.hpp"
#include "fir_common_traits.hpp"
#include "device_defs.h"
#include "fir_interpolate_hb_native_generated_graph/fir_interpolate_hb_generated_graph.h"

#ifndef UUT_GRAPH
#define UUT_GRAPH fir_interpolate_hb_graph
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
    static constexpr unsigned int SSR_OUT = P_SSR * P_PARA_INTERP_POLY;
    std::array<input_plio, P_SSR*(DUAL_INPUT_SAMPLES + 1)> in;
    std::array<output_plio, SSR_OUT*(NUM_OUTPUTS)> out;

#ifdef USING_UUT
    // Use Generated Graph
    using uut = fir_interpolate_hb_native_generated_graph;
#else
    // Use Graph class directly for reference model
    using uut = dsplib::fir::interpolate_hb::UUT_GRAPH<DATA_TYPE,
                                                       COEFF_TYPE,
                                                       FIR_LEN,
                                                       SHIFT,
                                                       ROUND_MODE,
                                                       INPUT_SAMPLES,
                                                       CASC_LEN,
                                                       DUAL_IP,
                                                       USE_COEFF_RELOAD,
                                                       NUM_OUTPUTS,
                                                       UPSHIFT_CT,
                                                       PORT_API,
                                                       P_SSR,
                                                       P_PARA_INTERP_POLY,
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

        // Modify m_taps_v, use formatUpshiftCt to bring center tap within UCT range, when used
        if (UPSHIFT_CT == 1) {
            m_taps_v[kNumTaps - 1] = formatUpshiftCt(m_taps_v[kNumTaps - 1]);
        }

#if (USE_COEFF_RELOAD != 0) // Reloadable coefficients
        static_assert(NITER % 2 == 0,
                      "ERROR: Please set NITER to be a multiple of 2 when reloadable coefficients are used");
#endif

        // Make connections
        createPLIOFileConnections<P_SSR, DUAL_INPUT_SAMPLES>(in, QUOTE(INPUT_FILE), "in");
        createPLIOFileConnections<SSR_OUT, (NUM_OUTPUTS - 1)>(out, QUOTE(OUTPUT_FILE), "out");

        for (unsigned int i = 0; i < P_SSR; ++i) {
            unsigned int plioBaseIdx = i * (DUAL_INPUT_SAMPLES + 1);
            unsigned int plioOutputBaseIdx = i * P_PARA_INTERP_POLY * NUM_OUTPUTS;
            connect<>(in[plioBaseIdx].out[0], firGraph.in[i]);

#if (DUAL_IP == 1 && PORT_API == 0)
            connect<>(in[plioBaseIdx].out[0], firGraph.in2[i]);
#endif
#if (DUAL_IP == 1 && PORT_API == 1)
            connect<>(in[plioBaseIdx + DUAL_INPUT_SAMPLES].out[0], firGraph.in2[i]);
#endif

            connect<>(firGraph.out[i], out[plioOutputBaseIdx].in[0]);
#if (NUM_OUTPUTS == 2)
            connect<>(firGraph.out2[i],
                      out[plioOutputBaseIdx + 1].in[0]); // firGraph.out[1] or similar when array ports are supported.
#endif
#if (P_PARA_INTERP_POLY == 2)
            connect<>(firGraph.out3[i], out[plioOutputBaseIdx + NUM_OUTPUTS].in[0]);
#if (NUM_OUTPUTS == 2)
            connect<>(firGraph.out4[i],
                      out[plioOutputBaseIdx + 3].in[0]); // firGraph.out[1] or similar when array ports are supported.
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
        const int inputBufferSize = PORT_API == 1 ? 0 : (FIR_LEN + INPUT_SAMPLES) * sizeof(DATA_TYPE);
        const int outputBufferSize =
            PORT_API == 1
                ? 0
                : (INPUT_SAMPLES * 2) *
                      sizeof(DATA_TYPE); // Due to interpolation, output buffer may be of greater size than input buffer

        if ((inputBufferSize > MAX_PING_PONG_SIZE) || (SINGLE_BUF == 1 && PORT_API == 0)) {
            for (int ssr = 0; ssr < P_SSR; ssr++) {
                single_buffer(firGraph.in[ssr]);
#if (DUAL_IP == 1)
                single_buffer(firGraph.in2[ssr]);

#endif
            }
            printf("INFO: Single Buffer Constraint applied to input buffers of kernel 0.\n");
        }

        if ((outputBufferSize > MAX_PING_PONG_SIZE) || (SINGLE_BUF == 1 && PORT_API == 0)) {
            for (int ssr = 0; ssr < P_SSR; ssr++) {
                single_buffer(firGraph.out[ssr]);
#if (NUM_OUTPUTS == 2)
                single_buffer(firGraph.out2[ssr]);
#endif
#if (P_PARA_INTERP_POLY == 2)
                single_buffer(firGraph.out3[ssr]);
#if (NUM_OUTPUTS == 2)
                single_buffer(firGraph.out4[ssr]);
#endif
#endif
            }
            printf("INFO: Single Buffer Constraint applied to output buffers of kernel %d.\n", P_SSR * CASC_LEN - 1);
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
        printf("========================\n");
    };
};
}
}
}
};

#endif // _DSPLIB_TEST_HPP_
