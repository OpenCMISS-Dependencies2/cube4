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



#include "config.h"
#include "TaskCallTree.h"
#include "CubeProxy.h"

using namespace cubegui;
using namespace cube;

TaskCallTree::TaskCallTree( cube::CubeProxy* cube ) : DefaultCallTree( cube, std::vector< cube::Cnode* >() )
{
    treeType = TASKTREE;
    label    = tr( "Task tree" );
}

TreeItem*
TaskCallTree::createTree()
{
    TreeItem* top = new TreeItem( this, QString( "Invisible task call tree root" ), CALLITEM, 0 );

    std::vector< cube::Cnode* > nodes = cube->getArtificialCnodes();
    for ( cube::Cnode* cnode : nodes )
    {
        std::vector<cube::Cnode*> vec;
        vec.push_back( cnode );
        createItems<cube::Cnode>( top, vec, CALLITEM );
    }
    initTree();

    // replace "TASKS" with parent name
    for ( TreeItem* item : top->getChildren() )
    {
        Vertex* parent = item->getCubeObject()->get_parent();
        QString name   = getItemName( parent );
        item->name        = name;
        item->displayName = "";
    }

    // initial nodes = inclusive values of all root nodes
    initialNodes = cube::list_of_cnodes();
    for ( TreeItem* item : top->getChildren() )
    {
        cnode_pair cp;
        cp.first  =  static_cast<cube::Cnode*> ( item->getCubeObject() );
        cp.second = CUBE_CALCULATE_INCLUSIVE;
        initialNodes.push_back( cp );
    }
    activeNodes = initialNodes;

    return top;
}
