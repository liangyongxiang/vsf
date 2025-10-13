/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_STATIC_ASSERT_UNIQUE_H__
#define __VSF_STATIC_ASSERT_UNIQUE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 * @file vsf_static_assert_unique.h
 * @brief Generic static assertion macros for uniqueness checking
 *
 * This header file provides generic enum value uniqueness checking
 * functionality that can be used in any scenario requiring enum value
 * uniqueness validation.
 *
 * Main features:
 * - Check if enum values are unique (LOOSE mode)
 * - Check if enum values have no overlapping bits (STRICT mode)
 * - Support up to 255 parameters checking (0xFF)
 * - Automatically select correct macro based on argument count
 *
 * Usage:
 * 1. Include this header file
 * 2. Optional: Set VSF_HAL_CHECK_UNIQUE_CFG to control check mode
 * 3. Use VSF_CHECK_UNIQUE() macro to check enum value uniqueness
 *
 * Example:
 * @code
 * // Define enum values
 * #define MODE_A    0x01
 * #define MODE_B    0x02
 * #define MODE_C    0x04
 *
 * // Check uniqueness
 * VSF_CHECK_UNIQUE(MODE_A, MODE_B, MODE_C);
 * @endcode
 *
 * Configuration options:
 * - VSF_HAL_CHECK_UNIQUE_CFG: Set to ENABLED to enable strict mode
 * (bit-disjoint check) Set to DISABLED to enable loose mode (value-unique
 * check)
 */

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

/*\note Static assertions to validate enum value uniqueness.
 *      These assertions check that the redefined enum values are unique within
 *      each functional group using optimized macro-based approach.
 */

/* Configuration: Choose between STRICT mode (bit-disjoint) or LOOSE mode
 * (value-unique) */
#ifndef VSF_HAL_CHECK_UNIQUE_CFG
#define VSF_HAL_CHECK_UNIQUE_CFG DISABLED
#endif

/* Define the base comparison macro based on the selected mode */
#if VSF_HAL_CHECK_UNIQUE_CFG == ENABLED
/* STRICT MODE: Check that enum values have no overlapping bits */
/* This ensures safe bitwise operations and combinations */
#define ____VSF_CHECK_UNIQUE_BASE(a, b) VSF_STATIC_ASSERT(((a) & (b)) == 0)
#else
/* LOOSE MODE: Check that enum values are different (allows overlapping bits) */
/* This allows overlapping bits but ensures unique values - this is the
 * fundamental requirement */
#define ____VSF_CHECK_UNIQUE_BASE(a, b) VSF_STATIC_ASSERT((a) != (b))
#endif

/* Public interface macro */
#define __VSF_CHECK_UNIQUE_BASE(a, b) ____VSF_CHECK_UNIQUE_BASE(a, b)

#define ____VSF_CHECK_ELEMENT_WITH_OTHERS(                                     \
    __ELEM, __ARG01, __ARG02, __ARG03, __ARG04, __ARG05, __ARG06, __ARG07,     \
    __ARG08, __ARG09, __ARG0A, __ARG0B, __ARG0C, __ARG0D, __ARG0E, __ARG0F,    \
    __ARG10, __ARG11, __ARG12, __ARG13, __ARG14, __ARG15, __ARG16, __ARG17,    \
    __ARG18, __ARG19, __ARG1A, __ARG1B, __ARG1C, __ARG1D, __ARG1E, __ARG1F,    \
    __ARG20, __ARG21, __ARG22, __ARG23, __ARG24, __ARG25, __ARG26, __ARG27,    \
    __ARG28, __ARG29, __ARG2A, __ARG2B, __ARG2C, __ARG2D, __ARG2E, __ARG2F,    \
    __ARG30, __ARG31, __ARG32, __ARG33, __ARG34, __ARG35, __ARG36, __ARG37,    \
    __ARG38, __ARG39, __ARG3A, __ARG3B, __ARG3C, __ARG3D, __ARG3E, __ARG3F,    \
    __ARG40, __ARG41, __ARG42, __ARG43, __ARG44, __ARG45, __ARG46, __ARG47,    \
    __ARG48, __ARG49, __ARG4A, __ARG4B, __ARG4C, __ARG4D, __ARG4E, __ARG4F,    \
    __ARG50, __ARG51, __ARG52, __ARG53, __ARG54, __ARG55, __ARG56, __ARG57,    \
    __ARG58, __ARG59, __ARG5A, __ARG5B, __ARG5C, __ARG5D, __ARG5E, __ARG5F,    \
    __ARG60, __ARG61, __ARG62, __ARG63, __ARG64, __ARG65, __ARG66, __ARG67,    \
    __ARG68, __ARG69, __ARG6A, __ARG6B, __ARG6C, __ARG6D, __ARG6E, __ARG6F,    \
    __ARG70, __ARG71, __ARG72, __ARG73, __ARG74, __ARG75, __ARG76, __ARG77,    \
    __ARG78, __ARG79, __ARG7A, __ARG7B, __ARG7C, __ARG7D, __ARG7E, __ARG7F,    \
    __ARG80, __ARG81, __ARG82, __ARG83, __ARG84, __ARG85, __ARG86, __ARG87,    \
    __ARG88, __ARG89, __ARG8A, __ARG8B, __ARG8C, __ARG8D, __ARG8E, __ARG8F,    \
    __ARG90, __ARG91, __ARG92, __ARG93, __ARG94, __ARG95, __ARG96, __ARG97,    \
    __ARG98, __ARG99, __ARG9A, __ARG9B, __ARG9C, __ARG9D, __ARG9E, __ARG9F,    \
    __ARGA0, __ARGA1, __ARGA2, __ARGA3, __ARGA4, __ARGA5, __ARGA6, __ARGA7,    \
    __ARGA8, __ARGA9, __ARGAA, __ARGAB, __ARGAC, __ARGAD, __ARGAE, __ARGAF,    \
    __ARGB0, __ARGB1, __ARGB2, __ARGB3, __ARGB4, __ARGB5, __ARGB6, __ARGB7,    \
    __ARGB8, __ARGB9, __ARGBA, __ARGBB, __ARGBC, __ARGBD, __ARGBE, __ARGBF,    \
    __ARGC0, __ARGC1, __ARGC2, __ARGC3, __ARGC4, __ARGC5, __ARGC6, __ARGC7,    \
    __ARGC8, __ARGC9, __ARGCA, __ARGCB, __ARGCC, __ARGCD, __ARGCE, __ARGCF,    \
    __ARGD0, __ARGD1, __ARGD2, __ARGD3, __ARGD4, __ARGD5, __ARGD6, __ARGD7,    \
    __ARGD8, __ARGD9, __ARGDA, __ARGDB, __ARGDC, __ARGDD, __ARGDE, __ARGDF,    \
    __ARGE0, __ARGE1, __ARGE2, __ARGE3, __ARGE4, __ARGE5, __ARGE6, __ARGE7,    \
    __ARGE8, __ARGE9, __ARGEA, __ARGEB, __ARGEC, __ARGED, __ARGEE, __ARGEF,    \
    __ARGF0, __ARGF1, __ARGF2, __ARGF3, __ARGF4, __ARGF5, __ARGF6, __ARGF7,    \
    __ARGF8, __ARGF9, __ARGFA, __ARGFB, __ARGFC, __ARGFD, __ARGFE, __ARGFF,    \
    N, ...)                                                           \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_##N

#define __VSF_CHECK_ELEMENT_WITH_OTHERS(__ELEM, ...)                           \
  ____VSF_CHECK_ELEMENT_WITH_OTHERS(__ELEM, __VA_ARGS__, FF, FE, FD, FC, FB,   \
                                    FA, F9, F8, F7, F6, F5, F4, F3, F2, F1,    \
                                    F0, EF, EE, ED, EC, EB, EA, E9, E8, E7,     \
                                    E6, E5, E4, E3, E2, E1, E0, DF, DE, DD,     \
                                    DC, DB, DA, D9, D8, D7, D6, D5, D4, D3,     \
                                    D2, D1, D0, CF, CE, CD, CC, CB, CA, C9,     \
                                    C8, C7, C6, C5, C4, C3, C2, C1, C0, BF,      \
                                    BE, BD, BC, BB, BA, B9, B8, B7, B6, B5,     \
                                    B4, B3, B2, B1, B0, AF, AE, AD, AC, AB,     \
                                    AA, A9, A8, A7, A6, A5, A4, A3, A2, A1,     \
                                    A0, 9F, 9E, 9D, 9C, 9B, 9A, 99, 98, 97,     \
                                    96, 95, 94, 93, 92, 91, 90, 8F, 8E, 8D,     \
                                    8C, 8B, 8A, 89, 88, 87, 86, 85, 84, 83,     \
                                    82, 81, 80, 7F, 7E, 7D, 7C, 7B, 7A, 79,     \
                                    78, 77, 76, 75, 74, 73, 72, 71, 70, 6F,     \
                                    6E, 6D, 6C, 6B, 6A, 69, 68, 67, 66, 65,     \
                                    64, 63, 62, 61, 60, 5F, 5E, 5D, 5C, 5B,     \
                                    5A, 59, 58, 57, 56, 55, 54, 53, 52, 51,     \
                                    50, 4F, 4E, 4D, 4C, 4B, 4A, 49, 48, 47,     \
                                    46, 45, 44, 43, 42, 41, 40, 3F, 3E, 3D,     \
                                    3C, 3B, 3A, 39, 38, 37, 36, 35, 34, 33,     \
                                    32, 31, 30, 2F, 2E, 2D, 2C, 2B, 2A, 29,     \
                                    28, 27, 26, 25, 24, 23, 22, 21, 20, 1F,     \
                                    1E, 1D, 1C, 1B, 1A, 19, 18, 17, 16, 15,     \
                                    14, 13, 12, 11, 10, 0F, 0E, 0D, 0C, 0B,     \
                                    0A, 09, 08, 07, 06, 05, 04, 03, 02, 01)     \
                                    (__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_0(__ELEM)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_01(__ELEM, __ARG01)                    \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_02(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_01(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_03(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_02(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_04(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_03(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_05(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_04(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_06(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_05(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_07(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_06(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_08(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_07(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_09(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_08(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_0A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_09(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_0B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_0A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_0C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_0B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_0D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_0C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_0E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_0D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_0F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_0E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_10(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_0F(__ELEM, __VA_ARGS__)

/* Generated __VSF_CHECK_ELEMENT_WITH_OTHERS macros from 0x11 to 0xFF */
// Generated __VSF_CHECK_ELEMENT_WITH_OTHERS macros from 0x11 to 0xFF
#define __VSF_CHECK_ELEMENT_WITH_OTHERS_11(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_10(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_12(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_11(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_13(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_12(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_14(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_13(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_15(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_14(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_16(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_15(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_17(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_16(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_18(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_17(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_19(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_18(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_1A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_19(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_1B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_1A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_1C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_1B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_1D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_1C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_1E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_1D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_1F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_1E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_20(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_1F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_21(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_20(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_22(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_21(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_23(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_22(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_24(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_23(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_25(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_24(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_26(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_25(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_27(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_26(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_28(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_27(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_29(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_28(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_2A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_29(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_2B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_2A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_2C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_2B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_2D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_2C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_2E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_2D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_2F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_2E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_30(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_2F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_31(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_30(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_32(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_31(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_33(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_32(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_34(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_33(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_35(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_34(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_36(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_35(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_37(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_36(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_38(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_37(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_39(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_38(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_3A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_39(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_3B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_3A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_3C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_3B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_3D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_3C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_3E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_3D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_3F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_3E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_40(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_3F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_41(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_40(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_42(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_41(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_43(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_42(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_44(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_43(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_45(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_44(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_46(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_45(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_47(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_46(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_48(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_47(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_49(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_48(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_4A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_49(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_4B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_4A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_4C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_4B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_4D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_4C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_4E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_4D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_4F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_4E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_50(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_4F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_51(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_50(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_52(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_51(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_53(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_52(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_54(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_53(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_55(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_54(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_56(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_55(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_57(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_56(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_58(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_57(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_59(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_58(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_5A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_59(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_5B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_5A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_5C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_5B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_5D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_5C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_5E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_5D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_5F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_5E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_60(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_5F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_61(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_60(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_62(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_61(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_63(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_62(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_64(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_63(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_65(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_64(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_66(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_65(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_67(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_66(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_68(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_67(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_69(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_68(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_6A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_69(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_6B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_6A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_6C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_6B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_6D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_6C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_6E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_6D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_6F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_6E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_70(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_6F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_71(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_70(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_72(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_71(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_73(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_72(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_74(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_73(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_75(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_74(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_76(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_75(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_77(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_76(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_78(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_77(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_79(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_78(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_7A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_79(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_7B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_7A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_7C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_7B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_7D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_7C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_7E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_7D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_7F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_7E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_80(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_7F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_81(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_80(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_82(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_81(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_83(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_82(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_84(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_83(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_85(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_84(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_86(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_85(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_87(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_86(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_88(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_87(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_89(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_88(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_8A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_89(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_8B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_8A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_8C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_8B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_8D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_8C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_8E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_8D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_8F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_8E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_90(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_8F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_91(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_90(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_92(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_91(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_93(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_92(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_94(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_93(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_95(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_94(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_96(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_95(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_97(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_96(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_98(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_97(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_99(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_98(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_9A(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_99(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_9B(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_9A(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_9C(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_9B(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_9D(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_9C(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_9E(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_9D(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_9F(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_9E(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A0(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_9F(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A1(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A0(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A2(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A1(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A3(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A2(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A4(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A3(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A5(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A4(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A6(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A5(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A7(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A6(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A8(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A7(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_A9(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A8(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_AA(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_A9(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_AB(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_AA(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_AC(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_AB(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_AD(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_AC(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_AE(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_AD(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_AF(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_AE(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B0(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_AF(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B1(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B0(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B2(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B1(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B3(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B2(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B4(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B3(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B5(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B4(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B6(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B5(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B7(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B6(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B8(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B7(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_B9(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B8(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_BA(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_B9(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_BB(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_BA(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_BC(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_BB(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_BD(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_BC(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_BE(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_BD(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_BF(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_BE(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C0(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_BF(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C1(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C0(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C2(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C1(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C3(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C2(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C4(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C3(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C5(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C4(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C6(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C5(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C7(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C6(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C8(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C7(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_C9(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C8(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_CA(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_C9(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_CB(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_CA(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_CC(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_CB(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_CD(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_CC(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_CE(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_CD(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_CF(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_CE(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D0(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_CF(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D1(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D0(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D2(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D1(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D3(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D2(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D4(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D3(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D5(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D4(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D6(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D5(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D7(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D6(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D8(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D7(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_D9(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D8(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_DA(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_D9(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_DB(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_DA(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_DC(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_DB(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_DD(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_DC(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_DE(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_DD(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_DF(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_DE(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E0(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_DF(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E1(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E0(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E2(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E1(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E3(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E2(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E4(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E3(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E5(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E4(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E6(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E5(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E7(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E6(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E8(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E7(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_E9(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E8(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_EA(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_E9(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_EB(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_EA(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_EC(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_EB(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_ED(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_EC(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_EE(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_ED(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_EF(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_EE(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F0(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_EF(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F1(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F0(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F2(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F1(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F3(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F2(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F4(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F3(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F5(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F4(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F6(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F5(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F7(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F6(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F8(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F7(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_F9(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F8(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_FA(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_F9(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_FB(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_FA(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_FC(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_FB(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_FD(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_FC(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_FE(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_FD(__ELEM, __VA_ARGS__)

#define __VSF_CHECK_ELEMENT_WITH_OTHERS_FF(__ELEM, __ARG01, ...)               \
  __VSF_CHECK_UNIQUE_BASE(__ELEM, __ARG01);                                    \
  __VSF_CHECK_ELEMENT_WITH_OTHERS_FE(__ELEM, __VA_ARGS__)


#define __VSF_CHECK_UNIQUE_02(a, b) __VSF_CHECK_UNIQUE_BASE(a, b)

#define __VSF_CHECK_UNIQUE_03(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_02(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_04(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_03(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_05(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_04(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_06(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_05(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_07(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_06(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_08(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_07(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_09(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_08(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_0A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_09(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_0B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_0A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_0C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_0B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_0D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_0C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_0E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_0D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_0F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_0E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_10(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_0F(__VA_ARGS__)

/* Generated __VSF_CHECK_UNIQUE macros from 0x11 to 0xFF */
// Generated __VSF_CHECK_UNIQUE macros from 0x11 to 0xFF
#define __VSF_CHECK_UNIQUE_11(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_10(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_12(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_11(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_13(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_12(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_14(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_13(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_15(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_14(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_16(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_15(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_17(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_16(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_18(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_17(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_19(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_18(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_1A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_19(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_1B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_1A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_1C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_1B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_1D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_1C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_1E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_1D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_1F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_1E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_20(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_1F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_21(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_20(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_22(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_21(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_23(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_22(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_24(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_23(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_25(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_24(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_26(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_25(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_27(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_26(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_28(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_27(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_29(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_28(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_2A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_29(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_2B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_2A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_2C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_2B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_2D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_2C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_2E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_2D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_2F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_2E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_30(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_2F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_31(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_30(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_32(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_31(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_33(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_32(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_34(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_33(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_35(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_34(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_36(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_35(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_37(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_36(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_38(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_37(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_39(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_38(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_3A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_39(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_3B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_3A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_3C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_3B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_3D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_3C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_3E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_3D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_3F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_3E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_40(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_3F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_41(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_40(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_42(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_41(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_43(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_42(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_44(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_43(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_45(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_44(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_46(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_45(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_47(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_46(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_48(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_47(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_49(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_48(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_4A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_49(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_4B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_4A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_4C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_4B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_4D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_4C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_4E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_4D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_4F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_4E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_50(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_4F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_51(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_50(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_52(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_51(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_53(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_52(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_54(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_53(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_55(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_54(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_56(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_55(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_57(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_56(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_58(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_57(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_59(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_58(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_5A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_59(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_5B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_5A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_5C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_5B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_5D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_5C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_5E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_5D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_5F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_5E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_60(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_5F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_61(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_60(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_62(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_61(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_63(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_62(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_64(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_63(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_65(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_64(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_66(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_65(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_67(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_66(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_68(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_67(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_69(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_68(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_6A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_69(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_6B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_6A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_6C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_6B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_6D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_6C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_6E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_6D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_6F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_6E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_70(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_6F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_71(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_70(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_72(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_71(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_73(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_72(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_74(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_73(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_75(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_74(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_76(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_75(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_77(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_76(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_78(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_77(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_79(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_78(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_7A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_79(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_7B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_7A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_7C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_7B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_7D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_7C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_7E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_7D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_7F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_7E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_80(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_7F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_81(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_80(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_82(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_81(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_83(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_82(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_84(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_83(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_85(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_84(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_86(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_85(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_87(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_86(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_88(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_87(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_89(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_88(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_8A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_89(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_8B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_8A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_8C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_8B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_8D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_8C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_8E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_8D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_8F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_8E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_90(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_8F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_91(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_90(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_92(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_91(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_93(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_92(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_94(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_93(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_95(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_94(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_96(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_95(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_97(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_96(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_98(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_97(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_99(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_98(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_9A(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_99(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_9B(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_9A(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_9C(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_9B(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_9D(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_9C(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_9E(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_9D(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_9F(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_9E(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A0(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_9F(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A1(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A0(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A2(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A1(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A3(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A2(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A4(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A3(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A5(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A4(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A6(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A5(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A7(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A6(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A8(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A7(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_A9(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A8(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_AA(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_A9(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_AB(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_AA(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_AC(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_AB(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_AD(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_AC(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_AE(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_AD(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_AF(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_AE(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B0(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_AF(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B1(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B0(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B2(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B1(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B3(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B2(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B4(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B3(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B5(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B4(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B6(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B5(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B7(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B6(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B8(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B7(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_B9(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B8(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_BA(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_B9(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_BB(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_BA(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_BC(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_BB(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_BD(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_BC(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_BE(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_BD(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_BF(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_BE(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C0(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_BF(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C1(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C0(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C2(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C1(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C3(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C2(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C4(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C3(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C5(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C4(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C6(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C5(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C7(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C6(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C8(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C7(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_C9(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C8(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_CA(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_C9(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_CB(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_CA(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_CC(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_CB(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_CD(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_CC(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_CE(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_CD(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_CF(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_CE(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D0(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_CF(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D1(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D0(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D2(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D1(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D3(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D2(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D4(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D3(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D5(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D4(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D6(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D5(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D7(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D6(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D8(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D7(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_D9(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D8(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_DA(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_D9(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_DB(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_DA(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_DC(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_DB(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_DD(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_DC(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_DE(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_DD(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_DF(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_DE(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E0(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_DF(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E1(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E0(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E2(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E1(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E3(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E2(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E4(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E3(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E5(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E4(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E6(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E5(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E7(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E6(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E8(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E7(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_E9(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E8(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_EA(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_E9(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_EB(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_EA(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_EC(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_EB(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_ED(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_EC(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_EE(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_ED(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_EF(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_EE(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F0(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_EF(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F1(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F0(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F2(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F1(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F3(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F2(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F4(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F3(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F5(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F4(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F6(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F5(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F7(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F6(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F8(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F7(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_F9(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F8(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_FA(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_F9(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_FB(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_FA(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_FC(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_FB(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_FD(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_FC(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_FE(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_FD(__VA_ARGS__)

#define __VSF_CHECK_UNIQUE_FF(a, ...)                                          \
  __VSF_CHECK_ELEMENT_WITH_OTHERS(a, __VA_ARGS__);                             \
  __VSF_CHECK_UNIQUE_FE(__VA_ARGS__)


#define VSF_CHECK_UNIQUE(...)                                                  \
  VSF_CHECK_UNIQUE_IMPL(__VA_ARGS__, FF, FE, FD, FC, FB, FA, F9, F8, F7, F6,   \
                        F5, F4, F3, F2, F1, F0, EF, EE, ED, EC, EB, EA, E9,    \
                        E8, E7, E6, E5, E4, E3, E2, E1, E0, DF, DE, DD, DC,    \
                        DB, DA, D9, D8, D7, D6, D5, D4, D3, D2, D1, D0, CF,    \
                        CE, CD, CC, CB, CA, C9, C8, C7, C6, C5, C4, C3, C2,    \
                        C1, C0, BF, BE, BD, BC, BB, BA, B9, B8, B7, B6, B5,    \
                        B4, B3, B2, B1, B0, AF, AE, AD, AC, AB, AA, A9, A8,    \
                        A7, A6, A5, A4, A3, A2, A1, A0, 9F, 9E, 9D, 9C, 9B,    \
                        9A, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 8F, 8E,    \
                        8D, 8C, 8B, 8A, 89, 88, 87, 86, 85, 84, 83, 82, 81,    \
                        80, 7F, 7E, 7D, 7C, 7B, 7A, 79, 78, 77, 76, 75, 74,    \
                        73, 72, 71, 70, 6F, 6E, 6D, 6C, 6B, 6A, 69, 68, 67,    \
                        66, 65, 64, 63, 62, 61, 60, 5F, 5E, 5D, 5C, 5B, 5A,    \
                        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 4F, 4E, 4D,    \
                        4C, 4B, 4A, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40,    \
                        3F, 3E, 3D, 3C, 3B, 3A, 39, 38, 37, 36, 35, 34, 33,    \
                        32, 31, 30, 2F, 2E, 2D, 2C, 2B, 2A, 29, 28, 27, 26,    \
                        25, 24, 23, 22, 21, 20, 1F, 1E, 1D, 1C, 1B, 1A, 19,    \
                        18, 17, 16, 15, 14, 13, 12, 11, 10, 0F, 0E, 0D, 0C,    \
                        0B, 0A, 09, 08, 07, 06, 05, 04, 03, 02, 01)(__VA_ARGS__)

#define VSF_CHECK_UNIQUE_IMPL(__ARG01, __ARG02, __ARG03, __ARG04, __ARG05,     \
                              __ARG06, __ARG07, __ARG08, __ARG09, __ARG0A,     \
                              __ARG0B, __ARG0C, __ARG0D, __ARG0E, __ARG0F,     \
                              __ARG10, __ARG11, __ARG12, __ARG13, __ARG14,     \
                              __ARG15, __ARG16, __ARG17, __ARG18, __ARG19,     \
                              __ARG1A, __ARG1B, __ARG1C, __ARG1D, __ARG1E,     \
                              __ARG1F, __ARG20, __ARG21, __ARG22, __ARG23,     \
                              __ARG24, __ARG25, __ARG26, __ARG27, __ARG28,     \
                              __ARG29, __ARG2A, __ARG2B, __ARG2C, __ARG2D,     \
                              __ARG2E, __ARG2F, __ARG30, __ARG31, __ARG32,     \
                              __ARG33, __ARG34, __ARG35, __ARG36, __ARG37,     \
                              __ARG38, __ARG39, __ARG3A, __ARG3B, __ARG3C,     \
                              __ARG3D, __ARG3E, __ARG3F, __ARG40, __ARG41,     \
                              __ARG42, __ARG43, __ARG44, __ARG45, __ARG46,     \
                              __ARG47, __ARG48, __ARG49, __ARG4A, __ARG4B,     \
                              __ARG4C, __ARG4D, __ARG4E, __ARG4F, __ARG50,     \
                              __ARG51, __ARG52, __ARG53, __ARG54, __ARG55,     \
                              __ARG56, __ARG57, __ARG58, __ARG59, __ARG5A,     \
                              __ARG5B, __ARG5C, __ARG5D, __ARG5E, __ARG5F,     \
                              __ARG60, __ARG61, __ARG62, __ARG63, __ARG64,     \
                              __ARG65, __ARG66, __ARG67, __ARG68, __ARG69,     \
                              __ARG6A, __ARG6B, __ARG6C, __ARG6D, __ARG6E,     \
                              __ARG6F, __ARG70, __ARG71, __ARG72, __ARG73,     \
                              __ARG74, __ARG75, __ARG76, __ARG77, __ARG78,     \
                              __ARG79, __ARG7A, __ARG7B, __ARG7C, __ARG7D,     \
                              __ARG7E, __ARG7F, __ARG80, __ARG81, __ARG82,     \
                              __ARG83, __ARG84, __ARG85, __ARG86, __ARG87,     \
                              __ARG88, __ARG89, __ARG8A, __ARG8B, __ARG8C,     \
                              __ARG8D, __ARG8E, __ARG8F, __ARG90, __ARG91,     \
                              __ARG92, __ARG93, __ARG94, __ARG95, __ARG96,     \
                              __ARG97, __ARG98, __ARG99, __ARG9A, __ARG9B,     \
                              __ARG9C, __ARG9D, __ARG9E, __ARG9F, __ARGA0,     \
                              __ARGA1, __ARGA2, __ARGA3, __ARGA4, __ARGA5,     \
                              __ARGA6, __ARGA7, __ARGA8, __ARGA9, __ARGAA,     \
                              __ARGAB, __ARGAC, __ARGAD, __ARGAE, __ARGAF,     \
                              __ARGB0, __ARGB1, __ARGB2, __ARGB3, __ARGB4,     \
                              __ARGB5, __ARGB6, __ARGB7, __ARGB8, __ARGB9,     \
                              __ARGBA, __ARGBB, __ARGBC, __ARGBD, __ARGBE,     \
                              __ARGBF, __ARGC0, __ARGC1, __ARGC2, __ARGC3,     \
                              __ARGC4, __ARGC5, __ARGC6, __ARGC7, __ARGC8,     \
                              __ARGC9, __ARGCA, __ARGCB, __ARGCC, __ARGCD,     \
                              __ARGCE, __ARGCF, __ARGD0, __ARGD1, __ARGD2,     \
                              __ARGD3, __ARGD4, __ARGD5, __ARGD6, __ARGD7,     \
                              __ARGD8, __ARGD9, __ARGDA, __ARGDB, __ARGDC,     \
                              __ARGDD, __ARGDE, __ARGDF, __ARGE0, __ARGE1,     \
                              __ARGE2, __ARGE3, __ARGE4, __ARGE5, __ARGE6,     \
                              __ARGE7, __ARGE8, __ARGE9, __ARGEA, __ARGEB,     \
                              __ARGEC, __ARGED, __ARGEE, __ARGEF, __ARGF0,     \
                              __ARGF1, __ARGF2, __ARGF3, __ARGF4, __ARGF5,     \
                              __ARGF6, __ARGF7, __ARGF8, __ARGF9, __ARGFA,     \
                              __ARGFB, __ARGFC, __ARGFD, __ARGFE, __ARGFF,     \
                              N, ...)                                 \
  __VSF_CHECK_UNIQUE_##N

#endif /* __VSF_STATIC_ASSERT_UNIQUE_H__ */
