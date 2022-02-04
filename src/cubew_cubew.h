/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/**
 * \file cubew.h
 * \brief Header with definition of structure "cube_writer" and needed methods..
 */

#ifndef __CUBEW_H
#define __CUBEW_H 0


#include "cubew_types.h"

/**
 * A state structure, which collects all data for given rank, needed to perform parallel writing of the cube4 report.  This structure is created on all processes,
   participating on writing. It controls, which process writes which data and allows the
   caller program to perform needed gathering and writing the data.
 */
typedef struct cube_writer
{
    /*    unsigned Nthreads;     */ /*!< Numbner of threads on each process. Is for writing process not needed */
    unsigned    Nprocesses;   /*!< Number of processes, participating in the writing. */
    unsigned    Nwriters;     /*!< Number of writers */
    unsigned    myrank;       /*!< Saves the rank of the running process */
    unsigned    ArraySize;    /*!< Saves the range of ids, wich has to be distributed from 0 to ArraySize over Nwriters. */
    unsigned    ArrayParts;   /*!< Saves, how big are single piaces for every writer. */
    unsigned    StepWriters;  /*!< Saved the distance between ranks of the writers. */
    unsigned    next_start;   /*!< Indicate the starting id after writing act. */
    unsigned    index;        /*!< Running index from 0 to ArraySize. If index > ArraySize, all pieces are writen. */
    unsigned    local_index;  /*!< Index, which has to be writen now. Runs in steps of ArrayParts. */
    unsigned    write_id;     /*!< Saves the id, which has tio be writen by current process, if it is a writer. */
    unsigned    writerrank;   /*!< Runs from 0 to Nwriters to indicate a writer. Real rank is calculates like  writerrank x StepWriters */
    cube_t*     cube;         /*!< Pointer on cube4 structure. Will be created as MASTER, WRITER or SLAVE according to the current rank.*/
    enum bool_t first_call;   /*!< By first call condition for "writing on disk" is fullfiled. But should be ignored.*/
    enum bool_t flush;        /*!< Signalize, that it is time, to perform writing. */
    enum bool_t me_writer;    /*!< Signalize, that the current process is a writer. Used at the beggig */
    enum bool_t do_write;     /*!< Signalize that the curent process has to writer. Used in combination woth the flag "flush" and "me_writer" */
    enum bool_t is_set_array; /*!< Signalize, than one calles "cubew_set_array" even if there size 0. */
} cube_writer;


#ifdef __cplusplus
extern "C" {
#endif
cube_writer*
cubew_create( unsigned    myrank,
              unsigned    Nprocesses,
              unsigned    Nwriters,
              const char* cubename,
              enum bool_t compression );
void
cubew_set_array( cube_writer* cubew,
                 int          ArraySize );
void
cubew_reset( cube_writer* cubew );
void
cubew_step( cube_writer* cubew );
void
cubew_check_flush( cube_writer* cubew );
enum bool_t
cubew_is_valid_id( cube_writer* cubew );
unsigned
cubew_get_next_callnode_id( cube_writer* cubew );
unsigned
cubew_get_writer( cube_writer* cubew );
cube_t*
cubew_get_cube( cube_writer* cubew );
enum bool_t
cubew_is_flush( cube_writer* cubew );
enum bool_t
cubew_me_writer( cube_writer* cubew );
enum bool_t
cubew_do_i_write( cube_writer* cubew );
unsigned
cubew_get_id_to_write( cube_writer* cubew );





void
cubew_set_known_cnodes_for_metric( cube_writer* cubew,
                                   cube_metric* metric,
                                   char*        known_cnodes_as_bitstring );                                          /** */

void
cubew_write_sev_row( cube_writer* cubew,
                     cube_metric* met,
                     cube_cnode*  cnd,
                     void*        sevs );                                                                    /** Writes a general row of data for given metric and cnode*/
void
cubew_write_sev_row_of_doubles( cube_writer* cubew,
                                cube_metric* met,
                                cube_cnode*  cnd,
                                double*      sevs );                                                                       /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cubew_write_sev_row_of_uint64( cube_writer* cubew,
                               cube_metric* met,
                               cube_cnode*  cnd,
                               uint64_t*    sevs );                                                                        /** Writes a  row of unsigned long long data for given metric and cnode. Performs proper type casting internally*/


void
cubew_finalize( cube_writer* cubew );

void
cubew_print( cube_writer* cubew );

#ifdef __cplusplus
}
#endif

#endif
