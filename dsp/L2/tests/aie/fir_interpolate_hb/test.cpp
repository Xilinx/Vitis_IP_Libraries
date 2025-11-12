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

/*
This file is the test harness for the Halfband Interpolator FIR graph class.
*/
#include <stdio.h>
#include "test.hpp"

xf::dsp::aie::testcase::test_graph filter;

int main(void) {
    filter.init();

#if (USE_COEFF_RELOAD == 1)
    filter.firGraph.update_rtp(filter, filter.m_taps_v, filter.coeff);
    // int rtpPortNumber = filter.firGraph.getTotalRtpPorts();

    // for (unsigned int i = 0; i < rtpPortNumber; i++) {
    //     // Extract the taps for the current RTP port
    //     std::vector<COEFF_TYPE> tapsForRtpPort = filter.firGraph.extractTaps(filter.m_taps_v, i);
    //     // Update the filter with the taps for the current RTP port
    //     filter.update(filter.coeff[i], tapsForRtpPort.data(), tapsForRtpPort.size());
    // }
    filter.run(NITER / 2);
    filter.wait();

    // Second update
    // filter.m_taps_v = generateTaps<COEFF_TYPE, COEFF_STIM_TYPE, FIR_LEN, COEFF_SEED>(QUOTE(COEFF_FILE));

    // for (unsigned int i = 0; i <  rtpPortNumber; i++) {
    //     // Extract the taps for the current RTP port
    //     std::vector<COEFF_TYPE> tapsForRtpPort = filter.firGraph.extractTaps(filter.m_taps_v, i);
    //     // Update the filter with the taps for the current RTP port
    //     filter.update(filter.coeff[i], tapsForRtpPort.data(), tapsForRtpPort.size());
    // }

    filter.run(NITER / 2);
#else
    filter.run(NITER);
#endif

    filter.end();

    return 0;
}
