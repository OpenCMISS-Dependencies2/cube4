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

#ifndef CUBE_CONFIG_H
#define CUBE_CONFIG_H


/*-------------------------------------------------------------------------*/
/**
 *  @file  config.h
 *  @brief Provides the Cube package configuration.
 *
 *  This header file provides the Cube package configuration by including
 *  the appropriate header file generated by the configure script, depending
 *  on the build configuration (frontend, backend, backend-mpi). It provides
 *  a number of <tt>#define</tt> directives, each one indicating the
 *  presence of a particular feature that has been tested by configure.
 *  Therefore, it should be included in every source file before any other
 *  header file.
 **/
/*-------------------------------------------------------------------------*/


#if defined CROSS_BUILD
    #if defined FRONTEND_BUILD
        #include <cubew-config-frontend.h>
    #elif defined BACKEND_BUILD_NOMPI
        #include <cubew-config-backend.h>
    #else
        #error "You can not use config.h without defining either FRONTEND_BUILD, BACKEND_BUILD_NOMPI or BACKEND_BUILD_MPI."
    #endif
#elif defined NOCROSS_BUILD
    #if defined BACKEND_BUILD_NOMPI
        #include <cubew-config-backend.h>
    #else
        #error "You can not use config.h without defining either BACKEND_BUILD_NOMPI or BACKEND_BUILD_MPI."
    #endif
#else
    #error "You can not use config.h without defining either CROSS_BUILD or NOCROSS_BUILD."
#endif
#include <cubew-config-common.h>


#endif   // !CUBE_CONFIG_H