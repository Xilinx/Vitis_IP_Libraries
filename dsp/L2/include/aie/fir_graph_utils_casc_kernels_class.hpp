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
#ifndef _DSPLIB_FIR_GRAPH_UTILS_CASC_KERNELS_CLASS_HPP_
#define _DSPLIB_FIR_GRAPH_UTILS_CASC_KERNELS_CLASS_HPP_
/*
The file captures the definition of the graph utilities commonly used across various library elements
*/

#include <adf.h>
#include <vector>

#include "device_defs.h"
#include "fir_utils.hpp"
#include "graph_utils.hpp"

// #define _DSPLIB_FIR_GRAPH_UTILS_DEBUG_

namespace xf {
namespace dsp {
namespace aie {
using namespace adf;
/**
  * @cond NOCOMMENTS
  */

// Recursive cascaded kernel creation, any FIR variant with any params
template <typename casc_params = fir_params_defaults, template <typename> typename fir_type = fir_type_default>
class casc_kernels {
   private:
    static_assert(casc_params::Bdim >= 0, "ERROR: dim must be a positive integer");

    static constexpr bool thisCasc_in = (casc_params::BTP_CASC_IN || (casc_params::Bdim == 0 ? false : true));
    static constexpr bool thisCasc_out =
        (casc_params::BTP_CASC_OUT || (casc_params::Bdim == (casc_params::BTP_CASC_LEN - 1) ? false : true));

    static constexpr unsigned int thisKernelRange =
        fir_type<casc_params>::template getKernelFirRangeLen<casc_params::Bdim>();
    static constexpr unsigned int thisNumOutputs =
        (thisCasc_out)
            ? 1
            : casc_params::BTP_NUM_OUTPUTS; // overwrite nu_outputs for internally connected kernels (no output anyway)

    struct thisKernel_params : public casc_params {
        static constexpr int Bdim = casc_params::Bdim;
        static constexpr int BTP_CASC_IN = thisCasc_in;
        static constexpr int BTP_CASC_OUT = thisCasc_out;
        static constexpr int BTP_FIR_RANGE_LEN = thisKernelRange;
        static constexpr int BTP_KERNEL_POSITION = casc_params::Bdim;
        static constexpr int BTP_NUM_OUTPUTS = thisNumOutputs;
    };

    using thisKernelClass = fir_type<thisKernel_params>;
    using thisKernelParentClass = typename fir_type<thisKernel_params>::parent_class;

    static constexpr int nextBdim = casc_params::Bdim - 1;
    static constexpr unsigned int nextKernelRange = fir_type<casc_params>::template getKernelFirRangeLen<nextBdim>();

    // Kernels with cascade out only have one output.
    // This assumption may not be needed.

    // need to create parameters for downstream kernels, e.g. dim.
    // inherit and overwrite non-defaults
    struct nextKernel_params : public casc_params {
        static constexpr int Bdim = nextBdim;
        static constexpr int BTP_FIR_RANGE_LEN = nextKernelRange;
        static constexpr int BTP_KERNEL_POSITION = nextBdim;
    };
    using nextKernelClass = fir_type<nextKernel_params>;

    // recursive call with updated params
    using next_casc_kernels = casc_kernels<nextKernel_params, fir_type>;

    template <int kernelFirRangeLen, int kernelPosition, int cascLen>
    struct kernelPositionParams : public casc_params {
        static constexpr int BTP_FIR_RANGE_LEN = kernelFirRangeLen;
        static constexpr int BTP_KERNEL_POSITION = kernelPosition;
        static constexpr int BTP_CASC_LEN = cascLen;
    };

    static constexpr unsigned int fnFirRange(unsigned int TP_FL, unsigned int TP_CL, int TP_KP, int TP_Rnd = 1) {
        // TP_FL - FIR Length, TP_CL - Cascade Length, TP_KP - Kernel Position
        return ((fir::fnTrunc(TP_FL, TP_Rnd * TP_CL) / TP_CL) +
                ((TP_FL - fir::fnTrunc(TP_FL, TP_Rnd * TP_CL)) >= TP_Rnd * (TP_KP + 1) ? TP_Rnd : 0));
    }

    static constexpr unsigned int fnFirRangeRem(unsigned int TP_FL, unsigned int TP_CL, int TP_KP, int TP_Rnd = 1) {
        // TP_FL - FIR Length, TP_CL - Cascade Length, TP_KP - Kernel Position
        // this is for last in the cascade
        return ((fir::fnTrunc(TP_FL, TP_Rnd * TP_CL) / TP_CL) +
                ((TP_FL - fir::fnTrunc(TP_FL, TP_Rnd * TP_CL)) % TP_Rnd));
    }
    static constexpr unsigned int fnFirRangeOffset(
        unsigned int TP_FL, unsigned int TP_CL, int TP_KP, int TP_Rnd = 1, int TP_Sym = 1) {
        // TP_FL - FIR Length, TP_CL - Cascade Length, TP_KP - Kernel Position
        return (TP_KP * (fir::fnTrunc(TP_FL, TP_Rnd * TP_CL) / TP_CL) +
                ((TP_FL - fir::fnTrunc(TP_FL, TP_Rnd * TP_CL)) >= TP_Rnd * TP_KP
                     ? TP_Rnd * TP_KP
                     : (fir::fnTrunc(TP_FL, TP_Rnd) - fir::fnTrunc(TP_FL, TP_Rnd * TP_CL)))) /
               TP_Sym;
    }

#if __HAS_ACCUM_PERMUTES__ == 1
    // cint16/int16 combo can be overloaded with 2 column MUL/MACs.
    static constexpr unsigned int tdmColumnMultiple = (std::is_same<typename casc_params::BTT_DATA, cint16>::value &&
                                                       std::is_same<typename casc_params::BTT_COEFF, int16>::value)
                                                          ? 2
                                                          : 1;
#else
    static constexpr unsigned int tdmColumnMultiple = 1;
#endif

    static constexpr unsigned int getTdmFirRangeOffset(int pos) {
        unsigned int firRangeOffset =
            fnFirRangeOffset(casc_params::BTP_FIR_LEN, casc_params::BTP_CASC_LEN, pos, tdmColumnMultiple);
        return firRangeOffset;
    };

   public:
    static constexpr unsigned int getTdmFirRangeLen(int pos) {
        unsigned int firRangeLen =
            pos + 1 == casc_params::BTP_CASC_LEN
                ? fnFirRangeRem(casc_params::BTP_FIR_LEN, casc_params::BTP_CASC_LEN, pos, tdmColumnMultiple)
                : fnFirRange(casc_params::BTP_FIR_LEN, casc_params::BTP_CASC_LEN, pos, tdmColumnMultiple);
        return firRangeLen;
    };

    /**
     * @brief Separate taps into a specific cascade segment. Only used for TDM FIR (other FIRs - kernels take the full
     * array anyway)
     */
    template <unsigned int kernelPosition, unsigned int cascLen>
    static std::vector<typename casc_params::BTT_COEFF> segment_tdm_taps_array_for_cascade(
        const std::vector<typename casc_params::BTT_COEFF>& taps) {
        // internal struct of the method
        static constexpr unsigned int kernelFirRangeLen =
            fir_type<casc_params>::template getKernelFirRangeLen<kernelPosition>();
        using thisPositionParams = kernelPositionParams<kernelFirRangeLen, kernelPosition, cascLen>;

        // FIR Length divided by cascade length and at this point also by SSR
        // TDM doesn't require full array for each cascaded kernels, in addition to requiring a fraction of total array
        // for SSR purposes
        static constexpr unsigned int tdmTapOffsetPerPhase =
            fir_type<thisPositionParams>::getFirCoeffOffset(); // in the range of 0 to FL-1
        // lanes
        static constexpr unsigned int lanes = fir_type<thisPositionParams>::getLanes();
        static constexpr unsigned int firTapLanes = fir_type<thisPositionParams>::getFirRangeLen() * lanes;
        static constexpr unsigned int firTapChannels = CEIL(thisPositionParams::BTP_TDM_CHANNELS, lanes) / lanes;

        static constexpr unsigned int firTapOffsetPerPhase =
            thisPositionParams::BTP_TDM_CHANNELS == 1 ? 0 : tdmTapOffsetPerPhase * lanes;
        std::vector<typename casc_params::BTT_COEFF> cascTapsRange; //
        for (unsigned int j = 0; j < firTapChannels; j++) {
            for (unsigned int i = 0; i < firTapLanes; i++) {
                unsigned int tdmCascOffset = firTapOffsetPerPhase + j * lanes * thisPositionParams::BTP_FIR_LEN;
                unsigned int coefIndex = i + tdmCascOffset;
                // if (coefIndex < taps.size()) {
                if (coefIndex < (thisPositionParams::BTP_FIR_LEN * thisPositionParams::BTP_TDM_CHANNELS)) {
                    cascTapsRange.push_back(taps.at(coefIndex));

                } else {
                    // padding
                    cascTapsRange.push_back(fir::nullElem<typename casc_params::BTT_COEFF>());
                }
            }
        }

        // #undef _DSPLIB_FIR_GRAPH_UTILS_DEBUG_

        return cascTapsRange;
    }

    /**
     * @brief Separate taps into a specific cascade segment. Only used for TDM FIR (other FIRs - kernels take the full
     * array anyway)
     */
    static std::vector<typename casc_params::BTT_COEFF> segment_tdm_taps_array_for_cascade(
        const std::vector<typename casc_params::BTT_COEFF>& taps, unsigned int kernelNo, unsigned int cascLen) {
        // 8 tap, 4 tap per kernel. 16 channels. 128 taps in total, 64 taps each kernel
        // kernel (1) -> 0  - 31, 64 - 95
        // kernel (0) -> 32 - 63, 96 - 127

        // Kernel position in the cascade chain is reversed. Recursive calls fill the data from the last kernel
        // unsigned int kernelPosition =  cascLen - kernelNo - 1;
        unsigned int kernelPosition = kernelNo;
        unsigned int kernelFirRangeLen = getTdmFirRangeLen(kernelPosition);
        unsigned int kernelFirRangeOffset = getTdmFirRangeOffset(kernelPosition);

        // FIR Length divided by cascade length and at this point also by SSR
        // TDM doesn't require full array for each cascaded kernels, in addition to requiring a fraction of total array
        // for SSR purposes
        unsigned int tdmTapOffsetPerPhase =
            casc_params::BTP_FIR_LEN - kernelFirRangeLen - kernelFirRangeOffset; // in the range of 0 to FL-1
        // lanes
        unsigned int lanes = fir_type<casc_params>::getLanes();
        unsigned int firTapLanes = kernelFirRangeLen * lanes;
        unsigned int firTapChannels = CEIL(casc_params::BTP_TDM_CHANNELS, lanes) / lanes;

        unsigned int firTapOffsetPerPhase = casc_params::BTP_TDM_CHANNELS == 1 ? 0 : tdmTapOffsetPerPhase * lanes;
        std::vector<typename casc_params::BTT_COEFF> cascTapsRange; //
        for (unsigned int j = 0; j < firTapChannels; j++) {
            for (unsigned int i = 0; i < firTapLanes; i++) {
                unsigned int tdmCascOffset = firTapOffsetPerPhase + j * lanes * casc_params::BTP_FIR_LEN;
                unsigned int coefIndex = i + tdmCascOffset;
                // if (coefIndex < taps.size()) {
                if (coefIndex < (casc_params::BTP_FIR_LEN * casc_params::BTP_TDM_CHANNELS)) {
                    cascTapsRange.push_back(taps.at(coefIndex));

                } else {
                    // padding
                    cascTapsRange.push_back(fir::nullElem<typename casc_params::BTT_COEFF>());
                }
            }
        }

        // #undef _DSPLIB_FIR_GRAPH_UTILS_DEBUG_

        return cascTapsRange;
    }
    // #undef _DSPLIB_FIR_GRAPH_UTILS_DEBUG_

    /**
     * @brief Separate FIR taps into a specific cascade segment. Use kernel's class firReload method to reload taps.
     */
    template <unsigned int kernelPosition, unsigned int cascLen>
    static std::vector<typename casc_params::BTT_COEFF> segment_taps_array_for_cascade(
        const std::vector<typename casc_params::BTT_COEFF>& taps, unsigned int isSym = 0) {
        return segment_taps_array_for_cascade(taps, kernelPosition, cascLen, isSym);
    }

    /**
     * @brief Separate FIR taps into a specific cascade segment. Use kernel's class firReload method to reload taps.
     * Recursively checks if kernelNo equals thisKernel_params::Bdim, else calls itself with nextKernel_params.
     */
    static std::vector<typename casc_params::BTT_COEFF> segment_taps_array_for_cascade(
        const std::vector<typename casc_params::BTT_COEFF>& taps,
        unsigned int kernelNo,
        unsigned int cascLen,
        unsigned int isSym = 0) {
        if
            constexpr(casc_params::Bdim != 0) {
                if (kernelNo == casc_params::Bdim) {
                    std::vector<typename casc_params::BTT_COEFF> firReloadTaps =
                        fir_type<thisKernel_params>::firReload(taps); //
                    return firReloadTaps;
                } else {
                    return next_casc_kernels::segment_taps_array_for_cascade(taps, kernelNo, cascLen, isSym);
                }
            }
        else {
            // Handle case when casc_params::Bdim is 0. No more recursive calls. No need to check if kernelNo matches
            // Bdim?
            // std::vector<typename casc_params::BTT_COEFF> firReloadTaps =
            // fir_type<thisKernel_params>::firReload(taps); //
            // return firReloadTaps;
            return fir_type<thisKernel_params>::firReload(taps); //
        }
    }

    static void create_and_recurse(kernel firKernels[casc_params::BTP_CASC_LEN],
                                   const std::vector<typename casc_params::BTT_COEFF>& taps) {
        using namespace fir;
        if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kSrAsym) {
                std::vector<typename casc_params::BTT_COEFF> cascTaps(
                    segment_taps_array_for_cascade<thisKernel_params::Bdim, thisKernel_params::BTP_CASC_LEN>(taps));
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(cascTaps);
                // firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(taps);
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kSrSym) {
                std::vector<typename casc_params::BTT_COEFF> cascTaps(
                    segment_taps_array_for_cascade<thisKernel_params::Bdim, thisKernel_params::BTP_CASC_LEN>(taps));
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(cascTaps);
                // firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(taps);
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kResamp) {
                std::vector<typename casc_params::BTT_COEFF> cascTaps(
                    segment_taps_array_for_cascade<thisKernel_params::Bdim, thisKernel_params::BTP_CASC_LEN>(taps));
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(cascTaps);
                // firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(taps);
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kIntHB) {
                std::vector<typename casc_params::BTT_COEFF> cascTaps(
                    segment_taps_array_for_cascade<thisKernel_params::Bdim, thisKernel_params::BTP_CASC_LEN>(taps));
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(cascTaps);
                // firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(taps);
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kIntAsym) {
                std::vector<typename casc_params::BTT_COEFF> cascTaps(
                    segment_taps_array_for_cascade<thisKernel_params::Bdim, thisKernel_params::BTP_CASC_LEN>(taps));
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(cascTaps);
                // firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(taps);
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kDecHB) {
                std::vector<typename casc_params::BTT_COEFF> cascTaps(
                    segment_taps_array_for_cascade<thisKernel_params::Bdim, thisKernel_params::BTP_CASC_LEN>(taps));
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(cascTaps);
                // firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(taps);
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kDecAsym) {
                std::vector<typename casc_params::BTT_COEFF> cascTaps(
                    segment_taps_array_for_cascade<thisKernel_params::Bdim, thisKernel_params::BTP_CASC_LEN>(taps));
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(cascTaps);
                // firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(taps);
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kTDM) {
                std::array<typename casc_params::BTT_DATA, thisKernelClass::getInternalBufferSize()> internalBuffer{};
                std::vector<typename casc_params::BTT_COEFF> cascTaps(
                    segment_tdm_taps_array_for_cascade<casc_params::Bdim, casc_params::BTP_CASC_LEN>(taps));

                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(cascTaps, internalBuffer);
            }
        if
            constexpr(casc_params::Bdim != 0) { next_casc_kernels::create_and_recurse(firKernels, taps); }
    }
    static void create_and_recurse(kernel firKernels[casc_params::BTP_CASC_LEN]) {
        using namespace fir;
        if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kSrAsym) {
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>();
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kSrSym) {
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>();
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kResamp) {
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>();
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kIntHB) {
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>();
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kIntAsym) {
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>();
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kDecHB) {
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>();
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kDecAsym) {
                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>();
            }
        else if
            constexpr(thisKernelClass::getFirType() == eFIRVariant::kTDM) {
                std::array<typename casc_params::BTT_DATA, thisKernelClass::getInternalBufferSize()> internalBuffer{};

                firKernels[casc_params::Bdim] = kernel::create_object<thisKernelParentClass>(internalBuffer);
            }
        if
            constexpr(casc_params::Bdim != 0) { next_casc_kernels::create_and_recurse(firKernels); }
    }
};
/**
  * @endcond
  */
}
}
} // namespace braces
#endif //_DSPLIB_FIR_GRAPH_UTILS_CASC_KERNELS_CLASS_HPP_
