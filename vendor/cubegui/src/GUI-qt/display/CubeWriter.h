/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2021                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef CUBEWRITER_H
#define CUBEWRITER_H

#include <QHash>
#include <map>

namespace cube
{
class CubeProxy;
class Cube;
class Metric;
class Region;
class Cnode;
class Location;
class LocationGroup;
class Sysres;
class SystemTreeNode;
}

namespace cubegui
{
class Tree;
class TreeItem;

class CubeWriter
{
public:
    CubeWriter( cube::CubeProxy* cubeProxy,
                Tree*            metricTree,
                Tree*            callTree,
                Tree*            taskTree,
                Tree*            systemTree );
    void
    writeCube( const QString& filename );

private:
    void
    copyMetricTree( TreeItem*     parentItem,
                    cube::Metric* parentCopy );
    void
    copyCallTree( TreeItem* parentItem,
                  cube::Cnode* parentCopy,
                  QHash<cube::Region*, cube::Region*>& map );
    void
    copyTaskTree();
    void
    copySystemTree( TreeItem* root );
    void
    copyTopologies();
    void
    copySeverities();
    void
    copySystemTreeNode( TreeItem*             parentItem,
                        cube::SystemTreeNode* parentCopy );

    cube::Cube*      cube;       // copy
    cube::CubeProxy* cubeProxy;  // original
    Tree*            metricTree; // original metric tree
    Tree*            callTree;
    Tree*            taskTree;
    Tree*            systemTree;

    // mapping original cube->copy
    QHash<cube::Metric*, cube::Metric*> metricHash;
    QHash<cube::Cnode*,  cube::Cnode*>  cnodeHash;
    QHash<cube::Sysres*, cube::Sysres*> systemHash;

    // mapping copy -> original cube
    QHash<int, int> systemIdHash;
};
}

#endif // CUBEWRITER_H
