/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef TASKCALLTREE_H
#define TASKCALLTREE_H

#include "DefaultCallTree.h"

namespace cubegui
{
class TaskCallTree : public DefaultCallTree
{
public:
    TaskCallTree( cube::CubeProxy* cube );
protected:
    TreeItem*
    createTree();
};
}

#endif // TASKCALLTREE_H
