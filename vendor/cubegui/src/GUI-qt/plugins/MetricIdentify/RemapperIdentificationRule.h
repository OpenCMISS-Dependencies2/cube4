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


#ifndef REMAPPER_METRIC_INDETIFICATION_RULE_H
#define REMAPPER_METRIC_INDETIFICATION_RULE_H

#include <string>
#include "IdentificationRule.h"

namespace metric_identify
{
class RemapperMetricIdentificationRule : public MetricIdentificationRule
{
public:
    RemapperMetricIdentificationRule()
    {
        unique_names.push_back( "execution" );
        unique_names.push_back( "comp" );
        unique_names.push_back( "mpi" );
        unique_names.push_back( "mpi_management" );
        unique_names.push_back( "mpi_init_exit" );
        unique_names.push_back( "mpi_mgmt_comm" );
        unique_names.push_back( "mpi_mgmt_file" );
        unique_names.push_back( "mpi_mgmt_win" );
        unique_names.push_back( "mpi_synchronization" );
        unique_names.push_back( "mpi_sync_collective" );
        unique_names.push_back( "mpi_rma_synchronization" );
        unique_names.push_back( "mpi_rma_sync_active" );
        unique_names.push_back( "mpi_rma_sync_passive" );
        unique_names.push_back( "mpi_communication" );
        unique_names.push_back( "mpi_point2point" );
        unique_names.push_back( "mpi_collective" );
        unique_names.push_back( "mpi_rma_communication" );
        unique_names.push_back( "mpi_io" );
        unique_names.push_back( "mpi_io_individual" );
        unique_names.push_back( "mpi_io_collective" );
        unique_names.push_back( "omp_management" );
        unique_names.push_back( "omp_time" );
        unique_names.push_back( "omp_synchronization" );
        unique_names.push_back( "omp_barrier" );
        unique_names.push_back( "omp_ebarrier" );
        unique_names.push_back( "omp_ibarrier" );
        unique_names.push_back( "omp_critical" );
        unique_names.push_back( "omp_lock_api" );
        unique_names.push_back( "omp_ordered" );
        unique_names.push_back( "omp_taskwait" );
        unique_names.push_back( "omp_flush" );
        unique_names.push_back( "omp_management" );
        unique_names.push_back( "pthread_time" );
        unique_names.push_back( "pthread_management" );
        unique_names.push_back( "pthread_synchronization" );
        unique_names.push_back( "pthread_lock_api" );
        unique_names.push_back( "pthread_conditional" );
        unique_names.push_back( "overhead" );
        unique_names.push_back( "omp_idle_threads" );
        unique_names.push_back( "omp_limited_parallelism" );
        unique_names.push_back( "syncs" );
        unique_names.push_back( "syncs_p2p" );
        unique_names.push_back( "syncs_rma" );
        unique_names.push_back( "syncs_rma_active" );
        unique_names.push_back( "syncs_rma_passive" );
        unique_names.push_back( "syncs_gats" );
        unique_names.push_back( "syncs_fence" );
        unique_names.push_back( "syncs_gats_access" );
        unique_names.push_back( "syncs_gats_exposure" );
        unique_names.push_back( "syncs_locks" );
        unique_names.push_back( "comms" );
        unique_names.push_back( "comms_p2p" );
        unique_names.push_back( "comms_coll" );
        unique_names.push_back( "comms_rma" );
        unique_names.push_back( "comms_rma_puts" );
        unique_names.push_back( "comms_rma_gets" );
        unique_names.push_back( "comms_rma_atomics" );
        unique_names.push_back( "mpi_file_ops" );
        unique_names.push_back( "mpi_file_iops" );
        unique_names.push_back( "mpi_file_irops" );
        unique_names.push_back( "mpi_file_iwops" );
        unique_names.push_back( "mpi_file_cops" );
        unique_names.push_back( "mpi_file_crops" );
        unique_names.push_back( "mpi_file_cwops" );
        unique_names.push_back( "mpi_file_irbytes" );
        unique_names.push_back( "mpi_file_iwbytes" );
        unique_names.push_back( "mpi_file_crbytes" );
        unique_names.push_back( "mpi_file_cwbytes" );
        unique_names.push_back( "mpi_file_cbytes" );
        unique_names.push_back( "mpi_file_ibytes" );
        unique_names.push_back( "mpi_file_bytes" );
        unique_names.push_back( "bytes" );
        unique_names.push_back( "bytes_p2p" );
        unique_names.push_back( "bytes_sent_p2p" );
        unique_names.push_back( "bytes_received_p2p" );
        unique_names.push_back( "bytes_coll" );
        unique_names.push_back( "bytes_sent_coll" );
        unique_names.push_back( "bytes_received_coll" );
        unique_names.push_back( "bytes_rma" );
        unique_names.push_back( "delay" );
        unique_names.push_back( "delay_mpi" );
        unique_names.push_back( "delay_p2p" );
        unique_names.push_back( "delay_latesender_aggregate" );
        unique_names.push_back( "delay_latereceiver_aggregate" );
        unique_names.push_back( "delay_collective" );
        unique_names.push_back( "delay_barrier_aggregate" );
        unique_names.push_back( "delay_n2n_aggregate" );
        unique_names.push_back( "delay_12n_aggregate" );
        unique_names.push_back( "delay_omp" );
        unique_names.push_back( "delay_ompbarrier_aggregate" );
        unique_names.push_back( "delay_ompidle_aggregate" );
        unique_names.push_back( "waitstates_propagating_vs_terminal" );
        unique_names.push_back( "mpi_wait_propagating" );
        unique_names.push_back( "mpi_wait_terminal" );
        unique_names.push_back( "mpi_wait_terminal_ls" );
        unique_names.push_back( "mpi_wait_terminal_lr" );
        unique_names.push_back( "waitstates_direct_vs_indirect" );
        unique_names.push_back( "mpi_wait_direct" );
        unique_names.push_back( "mpi_wait_direct_latesender" );
        unique_names.push_back( "mpi_wait_di_latesender" );
        unique_names.push_back( "mpi_wait_direct_latereceiver" );
        unique_names.push_back( "mpi_wait_indirect" );
        unique_names.push_back( "critical_path_imbalance" );
        unique_names.push_back( "performance_impact" );
        unique_names.push_back( "performance_impact_criticalpath" );
        unique_names.push_back( "intra_partition_imbalance" );
        unique_names.push_back( "imbalance" );
        unique_names.push_back( "imbalance_above" );
        unique_names.push_back( "imbalance_above_single" );
        unique_names.push_back( "imbalance_below" );
        unique_names.push_back( "imbalance_below_bypass" );
        unique_names.push_back( "imbalance_below_singularity" );
        unique_names.push_back( "ompss_time" );
        unique_names.push_back( "ompss_mgmt" );
        unique_names.push_back( "ompss_mgmt_task_create" );
        unique_names.push_back( "ompss_sync" );
        unique_names.push_back( "ompss_sync_barrier" );
        unique_names.push_back( "ompss_sync_task_wait" );

        url_reg_rules.push_back( REGULAR_EXPRESSION( "@mirror@scorep_metrics" ) );
        url_reg_rules.push_back( REGULAR_EXPRESSION( "@mirror@scalasca_patterns" ) );
    }
};
};
#endif // CUBE_TOOLS_H
