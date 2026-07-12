/************************************************************************************************************
* @internal
* @remark     Winbond Electronics Corporation
* @copyright  Copyright (c) 2023 by Winbond Electronics Corporation . All rights reserved
* @endinternal
*
* @file       qlib_targets.h
* @brief      This file contains qlib targets definitions
*
* ### project qlib
*
************************************************************************************************************/
#ifndef QLIB_TARGETS_H__
#define QLIB_TARGETS_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------------
                                                 DEFINITIONS
-----------------------------------------------------------------------------------------------------------*/
#define w77q128jw_revA (0x1u)
#ifdef QLIB_FPGA_TARGET
#define fpga_w77q128jw_revA (0x2u)
#endif // QLIB_FPGA_TARGET
#define w77q64jw_revA  (0x4u)
#define w77q128jv_revA (0x8u)
#define w77q64jv_revA  (0x10u)
#define w77q32jw_revB  (0x20u)
#ifdef QLIB_FPGA_TARGET
#define fpga_w77q32jw_revB (0x40u)
#endif // QLIB_FPGA_TARGET
#define w77q25nw_revB (0x80u)
#define w77t25nw_revB (0x100u)
#ifdef QLIB_FPGA_TARGET
#define fpga_w77q25nw_revA (0x200u)
#endif // QLIB_FPGA_TARGET
#define w77q25nw_revD (0x400u)
#define w77t25nw_revD (0x800u)
#ifdef QLIB_FPGA_TARGET
#define fpga_w77q25nw_revD (0x1000u)
#endif // QLIB_FPGA_TARGET
#define w77q64nw_revA (0x2000u)
#define w77t64nw_revA (0x4000u)
#ifdef QLIB_FPGA_TARGET
#define fpga_w77q64nw_revA (0x8000u)
#endif // QLIB_FPGA_TARGET
#define w77q25nw1_revA (0x10000u)
#define w77t25nw1_revA (0x20000u)
#ifdef QLIB_FPGA_TARGET
#define fpga_w77t25nw1_revA (0x40000u)
#endif // QLIB_FPGA_TARGET
#define w77q32rv_revA (0x80000u)
#ifdef QLIB_FPGA_TARGET
#define fpga_w77q32rv_revA (0x100000u)
#endif // QLIB_FPGA_TARGET

#ifdef QLIB_FPGA_TARGET
#define all_q2_targets   (fpga_w77q128jw_revA | fpga_w77q32jw_revB)
#define all_q3_0_targets (fpga_w77q25nw_revA)
#define all_q3_1_targets (fpga_w77q64nw_revA | fpga_w77t25nw1_revA)
#define q3_1_25_targets  (fpga_w77t25nw1_revA)
#define all_q3_2_targets (fpga_w77q32rv_revA)
#define q2_mcd_targets   (fpga_w77q32jw_revB)
#define q2_hcd18_targets (fpga_w77q128jw_revA)
#define q2_hcd33_targets (0u)
#define q2_hcd_targets   (fpga_w77q128jw_revA)
#define q3_t_targets     (all_q3_0_targets | all_q3_1_targets)
#else
#define all_q2_targets   (w77q128jw_revA | w77q64jw_revA | w77q128jv_revA | w77q64jv_revA | w77q32jw_revB)
#define all_q3_0_targets (w77q25nw_revB | w77t25nw_revB | w77q25nw_revD | w77t25nw_revD)
#define all_q3_1_targets (w77q64nw_revA | w77t64nw_revA | w77q25nw1_revA | w77t25nw1_revA)
#define q3_1_25_targets  (w77q25nw1_revA | w77t25nw1_revA)
#define all_q3_2_targets (w77q32rv_revA)
#define q2_mcd_targets   (w77q32jw_revB)
#define q2_hcd18_targets (w77q128jw_revA | w77q64jw_revA)
#define q2_hcd33_targets (w77q128jv_revA | w77q64jv_revA)
#define q2_hcd_targets   (w77q128jw_revA | w77q64jw_revA | w77q128jv_revA | w77q64jv_revA)
#define q3_t_targets     (w77t25nw_revB | w77t25nw_revD | w77t64nw_revA | w77t25nw1_revA)
#endif // QLIB_FPGA_TARGET
#define all_q3_targets (all_q3_0_targets | all_q3_1_targets | all_q3_2_targets)
#define all_targets    (all_q2_targets | all_q3_targets)

#ifndef QLIB_TARGET
#define QLIB_TARGET all_targets
#endif // QLIB_TARGET

#if ((QLIB_TARGET & all_targets) == 0u)
#error "wrong QLIB_TARGET"
#endif // QLIB_TARGET check

#define QLIB_TARGET_SUPPORTED(target) ((uint32_t)(target) & (uint32_t)QLIB_TARGET)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QLIB_TARGETS_H__
