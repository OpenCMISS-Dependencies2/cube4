/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PERFORMANCE_TEST_INCL_H
#define PERFORMANCE_TEST_INCL_H

#define BSPOP_COMM_EFF_METRIC_URL ""
#define BSPOP_LB_EFF_METRIC_URL ""
#define BSPOP_PAR_EFF_METRIC_URL ""
#define BSPOP_IPC_METRIC_URL ""
#define BSPOP_STALLED_METRIC_URL ""
#define BSPOP_SER_EFF_METRIC_URL ""
#define BSPOP_TRANSFER_EFF_METRIC_URL ""

#define POP_COMM_EFF_METRIC_URL ""
#define POP_LB_EFF_METRIC_URL ""
#define POP_PAR_EFF_METRIC_URL ""
#define POP_IPC_METRIC_URL ""
#define POP_STALLED_METRIC_URL ""
#define POP_SER_EFF_METRIC_URL ""
#define POP_TRANSFER_EFF_METRIC_URL ""

#define JSC_COMM_EFF_METRIC_URL ""
#define JSC_LB_EFF_METRIC_URL ""
#define JSC_PAR_EFF_METRIC_URL ""
#define JSC_IPC_METRIC_URL ""
#define JSC_STALLED_METRIC_URL ""
#define JSC_SER_EFF_METRIC_URL ""
#define JSC_TRANSFER_EFF_METRIC_URL ""

#define KNL_UOPS_RETIRED_PACKED_SMD_URL ""
#define KNL_UOPS_RETIRED_SCALAR_SMD_URL ""
#define KNL_VPU_INTENSITY_ALL_URL ""
#define KNL_L1_COMPUTE_TO_DATA_ACCESS_RATIO_URL ""
#define KNL_MEMORY_TRANSFER_URL ""
#define KNL_MEMORY_BANDWIDTH_URL ""
#define TIME_METRIC_URL "http://www.vi-hps.org/upload/packages/scorep/scorep_metrics-2.0.2.html"


extern std::string cubepl_time_init_str;
extern std::string cubepl_time_set_str;
extern std::string cubepl_knl_init_str;


#endif
