/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2020-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef CUBEGUI_CONCURRENT_H
#define CUBEGUI_CONCURRENT_H


/*-------------------------------------------------------------------------*/
/**
 *  @file  cube-concurrent.h
 *  @brief Provides the macro if QtConcurrent is found
 **/
/*-------------------------------------------------------------------------*/


/// @name Cube concurrent information
/// @{

/// 526 is a magic number , set by as_cube_concurrent.m4 if QT_CONCURRENT_LIB is found
#define CUBE_CONCURRENT 526
#if defined( CUBE_CONCURRENT ) && CUBE_CONCURRENT == 526
#define CUBE_CONCURRENT_LIB
#endif

/// @}


#endif   // !CUBEGUI_CONCURRENT_H
