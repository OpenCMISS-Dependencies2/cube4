/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCALASCA_METRIC_INDETIFICATION_RULE_H
#define SCALASCA_METRIC_INDETIFICATION_RULE_H

#include <string>
#include "IdentificationRule.h"

namespace metric_identify
{
class ScalascaMetricIdentificationRule : public MetricIdentificationRule
{
public:
    ScalascaMetricIdentificationRule()
    {
        unique_names.push_back( "mpi_init_completion" );
        unique_names.push_back( "mpi_finalize_wait" );
        unique_names.push_back( "mpi_rma_wait_at_create" );
        unique_names.push_back( "mpi_rma_wait_at_free" );
        unique_names.push_back( "mpi_barrier_wait" );
        unique_names.push_back( "mpi_barrier_completion" );
        unique_names.push_back( "mpi_rma_sync_late_post" );
        unique_names.push_back( "mpi_rma_late_post" );
        unique_names.push_back( "mpi_rma_early_wait" );
        unique_names.push_back( "mpi_rma_late_complete" );
        unique_names.push_back( "mpi_rma_wait_at_fence" );
        unique_names.push_back( "mpi_rma_early_fence" );
        unique_names.push_back( "mpi_rma_early_transfer" );
        unique_names.push_back( "mpi_rma_sync_lock_competition" );
        unique_names.push_back( "mpi_rma_sync_wait_for_progress" );
        unique_names.push_back( "mpi_latesender" );
        unique_names.push_back( "mpi_latesender_wo" );
        unique_names.push_back( "mpi_lswo_different" );
        unique_names.push_back( "mpi_lswo_same" );
        unique_names.push_back( "mpi_latereceiver" );
        unique_names.push_back( "mpi_earlyreduce" );
        unique_names.push_back( "mpi_earlyscan" );
        unique_names.push_back( "mpi_latebroadcast" );
        unique_names.push_back( "mpi_wait_nxn" );
        unique_names.push_back( "mpi_nxn_completion" );
        unique_names.push_back( "mpi_rma_comm_late_post" );
        unique_names.push_back( "mpi_rma_comm_lock_competition" );
        unique_names.push_back( "mpi_rma_comm_wait_for_progress" );
        unique_names.push_back( "omp_management" );
        unique_names.push_back( "omp_fork" );
        unique_names.push_back( "omp_ebarrier_wait" );
        unique_names.push_back( "omp_ibarrier_wait" );
        unique_names.push_back( "omp_lock_contention_critical" );
        unique_names.push_back( "omp_lock_contention_api" );
        unique_names.push_back( "pthread_lock_contention_mutex_lock" );
        unique_names.push_back( "pthread_lock_contention_conditional" );
        unique_names.push_back( "syncs_send" );
        unique_names.push_back( "mpi_slr_count" );
        unique_names.push_back( "syncs_recv" );
        unique_names.push_back( "mpi_sls_count" );
        unique_names.push_back( "mpi_slswo_count" );
        unique_names.push_back( "syncs_coll" );
        unique_names.push_back( "mpi_rma_pairsync_count" );
        unique_names.push_back( "mpi_rma_pairsync_unneeded_count" );
        unique_names.push_back( "comms_send" );
        unique_names.push_back( "mpi_clr_count" );
        unique_names.push_back( "comms_recv" );
        unique_names.push_back( "mpi_cls_count" );
        unique_names.push_back( "mpi_clswo_count" );
        unique_names.push_back( "comms_cxch" );
        unique_names.push_back( "comms_csrc" );
        unique_names.push_back( "comms_cdst" );
        unique_names.push_back( "bytes_cout" );
        unique_names.push_back( "bytes_cin" );
        unique_names.push_back( "delay_latesender" );
        unique_names.push_back( "delay_latesender_longterm" );
        unique_names.push_back( "delay_latereceiver" );
        unique_names.push_back( "delay_latereceiver_longterm" );
        unique_names.push_back( "delay_barrier" );
        unique_names.push_back( "delay_barrier_longterm" );
        unique_names.push_back( "delay_n2n" );
        unique_names.push_back( "delay_n2n_longterm" );
        unique_names.push_back( "delay_12n" );
        unique_names.push_back( "delay_12n_longterm" );
        unique_names.push_back( "delay_ompbarrier" );
        unique_names.push_back( "delay_ompbarrier_longterm" );
        unique_names.push_back( "delay_ompidle" );
        unique_names.push_back( "delay_ompidle_longterm" );
        unique_names.push_back( "mpi_wait_propagating_ls" );
        unique_names.push_back( "mpi_wait_propagating_lr" );
        unique_names.push_back( "mpi_wait_indirect_latesender" );
        unique_names.push_back( "mpi_wait_indirect_latereceiver" );
        unique_names.push_back( "critical_path" );
        unique_names.push_back( "critical_path_activities" );
        unique_names.push_back( "critical_imbalance_impact" );
        unique_names.push_back( "inter_partition_imbalance" );
        unique_names.push_back( "non_critical_path_activities" );

        url_reg_rules.push_back( REGULAR_EXPRESSION( "@mirror@scalasca_patterns" ) );
    }
};
};
#endif // CUBE_TOOLS_H
