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
#ifndef _DSPLIB_FIR_GRAPH_UTILS_HPP_
#define _DSPLIB_FIR_GRAPH_UTILS_HPP_
/*
The file captures the definition of the graph utilities commonly used across various library elements
*/

#include <adf.h>
#include <vector>

#include "device_defs.h"
#include "fir_utils.hpp"
#include "graph_utils.hpp"
#include "fir_graph_utils_casc_kernels_class.hpp"
#include "fir_graph_utils_ssr_kernels_class.hpp"

namespace xf {
namespace dsp {
namespace aie {
using namespace adf;
template <int T_FIR_LEN, int kMaxTaps>
static constexpr unsigned int getMinCascLen() {
    return T_FIR_LEN < kMaxTaps ? 1 : T_FIR_LEN % kMaxTaps == 0 ? T_FIR_LEN / kMaxTaps : T_FIR_LEN / kMaxTaps + 1;
};

template <int kMaxTaps, int kRawOptTaps, int T_FIR_LEN, int SSR = 1>
constexpr unsigned int getOptCascLen() {
    constexpr int kOptTaps = kRawOptTaps < kMaxTaps ? kRawOptTaps : kMaxTaps;
    constexpr int firLenPerSSR = T_FIR_LEN / SSR;
    return (firLenPerSSR) % kOptTaps == 0 ? (firLenPerSSR) / kOptTaps : (firLenPerSSR) / kOptTaps + 1;
};

/**
 * @endcond
 */

/**
 * @defgroup graph_utils Graph utils
 *
 * The Graphs utilities contain helper functions and classes that ease usage of library elements.
 *
 */

//--------------------------------------------------------------------------------------------------
// convert_sym_taps_to_asym
//--------------------------------------------------------------------------------------------------
/**
 * @ingroup graph_utils
 *
 * @brief convert_sym_taps_to_asym is an helper function to convert users input coefficient array. \n
 * Function creates an asymmetric array in the area provided from a symmetric one. \n
 * Function can be used when run-time programmable coefficients are being passed to the FIR graph, \n
 * using the graph's class ``update()`` method.
 * @tparam TT_COEFF describes the type of individual coefficients of the filter taps.
 * @param[out] tapsOut  a pointer to the output taps array of uncompressed (flen) size.
 * @param[in] fLen  input argument defining the size of the uncompressed array.
 * @param[in] tapsIn pointer to the input taps array of compressed (fLen+1)/2 size.
 */
template <typename TT_COEFF>
void convert_sym_taps_to_asym(TT_COEFF* tapsOut, unsigned int fLen, TT_COEFF* tapsIn) {
    for (unsigned int i = 0; i < fLen; i++) {
        unsigned int coefIndex = i;
        if (coefIndex >= fLen / 2) {
            coefIndex = fLen - coefIndex - 1;
        }
        tapsOut[i] = tapsIn[coefIndex];
    }
};

//--------------------------------------------------------------------------------------------------
// convert_hb_taps_to_asym
//--------------------------------------------------------------------------------------------------
/**
 * @ingroup graph_utils
 *
 * @brief convert_hb_taps_to_asym is an helper function to convert users input coefficient array. \n
 * Function can be used when run-time programmable coefficients are being passed to the FIR graph, \n
 * using the graph's class ``update()`` method. \n
 * \n
 * HB taps arrays are compressed arrays of taps with the center tap at the end,  \n
 * with a length of: ``hbFirLen = (FIR Length + 1) / 4 + 1``.
 * When converting to Asym, we want to convert the symmetric taps to asymmetric,
 * but it's useful to have the center tap at the end,
 * (since it is processed by separate polyphase lane and is offloaded to a separate, dedicated kernel). \n
 * However for SSR cases, the array needs to be padded with zeros to a multiple of SSR factor. \n
 * For example, for a FIR Length of 7, where coeffs are: ``1, 0, 2, 5, 2, 0, 1`` \n
 * ``tapsIn: 1, 2, 5`` \n
 * ``hbFirLen: 3`` \n
 * For SSR: 1, \n
 * ``tapsOut: 1, 2, 2, 1, 5`` \n
 * For SSR: 3 \n
 * ``tapsOut: 1, 2, 2, 1, 0, 0, 5`` \n
 *
 * @tparam TT_COEFF describes the type of individual coefficients of the filter taps.
 * @param[out] tapsOut  a pointer to the output taps array of uncompressed (flen) size.
 * @param[in] hbFirLen  input argument defining the size of the uncompressed array.
 * @param[in] tapsIn pointer to the input taps array of compressed (fLen+1)/2 size.
 * @param[in] ssr ssr parameter
 */
template <typename TT_COEFF>
void convert_hb_taps_to_asym(TT_COEFF* tapsOut, unsigned int hbFirLen, TT_COEFF* tapsIn, unsigned int ssr) {
    int symFLen = (hbFirLen - 1) * 2; // (FIR_LEN+1)/2

    for (unsigned int i = 0; i < CEIL(symFLen, ssr); i++) {
        unsigned int coefIndex = i;
        if (coefIndex < symFLen) {
            if (coefIndex >= symFLen / 2) {
                coefIndex = symFLen - coefIndex - 1;
            }
            tapsOut[i] = tapsIn[coefIndex];
        } else {
            tapsOut[i] = fir::nullElem<TT_COEFF>();
        }
    }
    tapsOut[CEIL(symFLen, ssr)] = tapsIn[(symFLen + 1) / 2];
};
}
}
} // namespace braces
#endif //_DSPLIB_FIR_GRAPH_UTILS_HPP_
