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
 * \file cubew.c
 * \brief Contains the realisation of the methods related to cube_writer.
 *
 *
 *
 */

#ifndef __CUBEW_C
#define __CUBEW_C 0

#include <config.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "cubew_cube.h"
#include "cubew_cubew.h"


/**
 * Creates a writer structure for given number of threads, writers and current rank. Takes as a parameter a filename of the cube. Returns the created structure, which controls the parallel writing of cube4 report.
    cube_writer creates a cube_t structure, which one has define properly (set up dimensions ) if the current rank "myrank" will be a writer. For example, if ony uses only one writer, only myrank==0 has to initialize the cube_t structure.
 */

cube_writer*
cubew_create( unsigned myrank, unsigned Nprocesses, unsigned Nwriters,  const char* cubename, enum bool_t compression )
{
    cube_writer* cubew;
    cubew = ( cube_writer* )malloc( sizeof( cube_writer ) );

    if ( Nwriters <= 0 )
    {
        Nwriters = 1;
    }
    if ( compression == CUBE_TRUE )
    {
        Nwriters = 1;
    }
#ifdef CUBE_LAYOUT_TAR
    Nwriters = 1;
#endif

    if ( Nwriters > Nprocesses )
    {
        Nwriters = Nprocesses;
    }

    cubew->Nprocesses = Nprocesses;
    cubew->myrank     = myrank;
    cubew->ArraySize  = 0;
    // this adjustment is not strictly needed. Should work also with simply (unsigned)Nthread/Nwriters.
    unsigned StepWriters = ( unsigned )ceil( ( double )Nprocesses / ( double )Nwriters );
    cubew->Nwriters = ( unsigned )floor( ( double )Nprocesses / ( double )StepWriters );

    cubew->StepWriters = StepWriters;
    if ( myrank == 0 )
    {                  /*  mark cube as master*/
    }
    if ( myrank % StepWriters == 0 )
    {
        cubew->me_writer = CUBE_TRUE;
    }
    else
    {
        cubew->me_writer = CUBE_FALSE;
    }

    cubew->ArrayParts = 0;
    cubew_reset( cubew );
    cubew->is_set_array = CUBE_FALSE;
    cubew->cube         = NULL;


    if ( myrank == 0 )
    {
        cubew->cube = cube_create( ( char* )cubename, CUBE_MASTER, compression );
        return cubew;
    }
    if ( cubew->me_writer == CUBE_TRUE )
    {
        cubew->cube = cube_create( ( char* )cubename, CUBE_WRITER, compression );
        return cubew;
    }
    cubew->cube = cube_create( ( char* )cubename, CUBE_SLAVE, compression );


//     cubew_print(cubew);

    return cubew;
}



/**
 * Sets the number of the callnodes, which have to be written. cube_writer runs from 0 till ArraySize and distributes the corresponding over the writers.
 */
void
cubew_set_array( cube_writer* cubew,  int ArraySize )
{
    cubew_reset( cubew );
    cubew->ArraySize    = ArraySize;
    cubew->ArrayParts   = ( unsigned )ceil( ( double )ArraySize / ( double )cubew->Nwriters );
    cubew->is_set_array = CUBE_TRUE;
}



/**
 * Sets the cube_writer structure in initial state.  Reset has to be done for every metric.
 */
void
cubew_reset( cube_writer* cubew )
{
    cubew->next_start   = 1;
    cubew->index        = 0;
    cubew->local_index  = 0;
    cubew->writerrank   = 0;
    cubew->flush        = CUBE_FALSE;
    cubew->first_call   = CUBE_TRUE;
    cubew->do_write     = CUBE_FALSE;
    cubew->write_id     = 0;
    cubew->ArraySize    = 0;
    cubew->ArrayParts   = 0;
    cubew->is_set_array = CUBE_FALSE;
}

/**
 * Actualise internals of the cube_writer for he next writing step. Changes the next to be writen cnode and the rank of the writer, which is responsible for the writing.
 *
 */
void
cubew_step( cube_writer* cubew )
{
    cubew->index++;
    cubew->writerrank  = cubew->writerrank + cubew->StepWriters;
    cubew->local_index = cubew->local_index + cubew->ArrayParts;
    if ( cubew->local_index >= cubew->ArraySize )
    {
        cubew->do_write    = CUBE_FALSE;
        cubew->local_index = cubew->next_start;
        cubew->next_start++;
        cubew->writerrank = 0;
    }
}
/**
 * Checks, if all writers got their cnodes and now can write its data to file. Actuall writing has to be initiated by the caller program.
 */
void
cubew_check_flush( cube_writer* cubew )
{
    if ( cubew->me_writer && ( cubew->writerrank == cubew->myrank ) )
    {
        cubew->do_write = CUBE_TRUE;
        cubew->write_id = cubew->local_index;
    }


    if ( ( cubew->local_index + cubew->ArrayParts ) >= cubew->ArraySize )
    {
        cubew->flush = CUBE_TRUE;
    }
    else
    {
        cubew->flush = CUBE_FALSE;
    }
}


/**
 * Checks, if there are still valid cnodes, which has to be writen. (Checks if all ArraySize indexes are distributed and writen.)
 */
enum bool_t
cubew_is_valid_id( cube_writer* cubew )
{
    if ( cubew->is_set_array == CUBE_FALSE )
    {
        fprintf( stderr, "The number of cnodes to be writen is not set yet. Please call cubew_set_array(cube_writer *, int ArraySize) with corresponding ArraySize.\n" );
        return CUBE_FALSE;
    }
    if ( cubew->ArraySize == 0 )
    {
        return CUBE_FALSE;
    }
    cubew_check_flush( cubew );
    if ( cubew->first_call == CUBE_TRUE )
    {
        cubew->first_call = CUBE_FALSE;
        return CUBE_TRUE;
    }

    if ( cubew->index >= ( cubew->ArraySize ) )
    {
        cubew_reset( cubew );
        return CUBE_FALSE;
    }

    return CUBE_TRUE;
}



/**
 * Checks, if the current rank is a writer and has to write. If it returns CUBE_TRUE, he caller has to call cube_write_sev_row(mycube, cube_metric* , cube_cnode*, double * ) with proper parameters to write the before collected data.
 */
enum bool_t
cubew_do_i_write( cube_writer* cubew )
{
    return cubew->do_write;
}


/**
 * Returns an id, which has to be writen by current rank.
 * If the current rank is not a writer, it gets still an id.
 * But by later call cubew_do_i write(cube_writer*) it gets a CUBE_FALSE and doesn't have to write any data.
 */
unsigned
cubew_get_id_to_write( cube_writer* cubew )
{
    return cubew->write_id;
}


/**
 * Returns an cnodeid, which data will be collected in current step.
 *
 * Correspnding data is collected via MPI_Gather call in caller program.
 */
unsigned
cubew_get_next_callnode_id( cube_writer* cubew )
{
    return cubew->local_index;
}


/**
 * Returns a rank, which is a writer of the cnodeid, returned in the current step (returnded by cubew_get_next_callnode_id(cube_writer *)).
 */
unsigned
cubew_get_writer( cube_writer* cubew )
{
    return cubew->writerrank;
}


/**
 * Signalize, that all writers got their data and ready to write. Caller program has to start writing.
 */
enum bool_t
cubew_is_flush( cube_writer* cubew )
{
    return cubew->flush;
}



/**
 * ABSOLETE
 */
enum bool_t
cubew_me_writer( cube_writer* cubew )
{
    return cubew->me_writer;
}


/**
 * Returns the created cube_t structure for the current rank. cube_t structure has differnt flavours, depending, whether the current rank is 0 (then meta part  and index has to be writen), "writer"  (then datagoes to the file .data) or "slave". It does nothing. For the further optimization, every cube gets a bitstring of known cnodes and will return a proper sequence of the cnodes, which has to be writen. The sequence is builded by the cube_t, because it depends on the type of the metrics data and has to be carried out in the cube_t structure. It has to happen on every rank, participating on writing process.
 */
cube_t*
cubew_get_cube( cube_writer* cubew )
{
    return cubew->cube;
}


/**
 * Releases the memory of the coresponding cube_t  and cube_writer. All memory, which was assigned to the cube_t and cube_writer will be released by calling corresponding free(vaoid *).
 */
void
cubew_finalize( cube_writer* cubew )
{
    if ( cubew->cube != 0 )
    {
        cube_free( cubew->cube ); // cube removes also cube structure
        cubew->cube = NULL;
    }
    free( cubew );
}





/**
 * Prints out the current state of the cube_writer. Used for debugging propose.
 */
void
cubew_print( cube_writer* cubew )
{
    printf( "CUBE_Writer:\n" );
    printf( "#processes:%u\n", cubew->Nprocesses );
    printf( "#writers:%u\n", cubew->Nwriters );
    printf( "myrank:%u\n", cubew->myrank );
    printf( "ArraySize:%u\n", cubew->ArraySize );
    printf( "ArrayParts:%u\n", cubew->ArrayParts );
    printf( "StepWriters:%u\n", cubew->StepWriters );
    printf( "next_start:%u\n", cubew->next_start );
    printf( "index:%u\n", cubew->index );
    printf( "local_index:%u\n", cubew->local_index );
    printf( "writerrank:%u\n", cubew->writerrank );
    printf( "writeid:%u\n", cubew->write_id );
    printf( "first_call?:%s\n", ( cubew->first_call == CUBE_TRUE ) ? "yes" : "no" );
    printf( "flush?:%s\n", ( cubew->flush == CUBE_TRUE ) ? "yes" : "no" );
    printf( "do write?:%s\n", ( cubew->do_write == CUBE_TRUE ) ? "yes" : "no" );
    printf( "me writer?:%s\n", ( cubew->me_writer == CUBE_TRUE ) ? "yes" : "no" );
}





/**
   Sets the bitstring of known cnodes in the cube.

   USed as an interface for programs, which are using cube3 and cube4 simultaniosly
 */

void
cubew_set_known_cnodes_for_metric( cube_writer* cubew, cube_metric* metric, char* known_cnodes_as_bitstring )
{
/*  in the case of cube3 -> is empty */
    if ( cubew->cube->cube_flavour == CUBE_MASTER )
    {
        cube_set_known_cnodes_for_metric( cubew->cube, metric, known_cnodes_as_bitstring );
    }
}


/** Writes a general row of data for given metric and cnode*/
void
cubew_write_sev_row( cube_writer* cubew, cube_metric* met,  cube_cnode* cnd, void* sevs )
{
    cube_write_sev_row( cubew->cube, met,  cnd, sevs );
}

/** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cubew_write_sev_row_of_doubles( cube_writer* cubew, cube_metric* met,  cube_cnode* cnd, double* sevs )
{
    cube_write_sev_row_of_doubles( cubew->cube, met,  cnd, sevs );
}
/** Writes a  row of unsigned long long data for given metric and cnode. Performs proper type casting internally*/
void
cubew_write_sev_row_of_uint64( cube_writer* cubew, cube_metric* met,  cube_cnode* cnd, uint64_t* sevs )
{
    cube_write_sev_row_of_uint64( cubew->cube, met,  cnd, sevs );
}








#endif
