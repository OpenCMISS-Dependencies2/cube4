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


#include "config.h"
#include <iostream>
#include <QHash>
#include "Cube.h"
#include "CubeWriter.h"
#include "Tree.h"
#include "MetricTree.h"
#include "DefaultCallTree.h"
#include "SystemTree.h"
#include "CubeProxy.h"

using namespace cube;
using namespace cubegui;
using namespace std;

/**
 * @brief CubeWriter::copyMetricTree recursively copies metric tree from parentItem to this->cube
 * @param parentItem all children of the parent item are copied to parentCopy
 * @param parentCopy the corresponding metric (of parentItem) of the copy
 */
void
CubeWriter::copyMetricTree( TreeItem* parentItem, Metric* parentCopy )
{
    for ( TreeItem* item : parentItem->getChildren() )
    {
        cube::Metric* metric     = static_cast<cube::Metric*> ( item->getCubeObject() );
        Metric*       nextParent = cube->def_met( metric->get_disp_name(),
                                                  metric->get_uniq_name(),
                                                  metric->get_dtype(),
                                                  metric->get_uom(),
                                                  metric->get_val(),
                                                  metric->get_url(),
                                                  metric->get_descr(),
                                                  parentCopy,
                                                  metric->get_type_of_metric(),
                                                  metric->get_expression(),
                                                  metric->get_init_expression(),
                                                  metric->get_aggr_plus_expression(),
                                                  metric->get_aggr_minus_expression(),
                                                  metric->get_aggr_aggr_expression(),
                                                  metric->isRowWise(),
                                                  metric->get_viz_type()
                                                  );
        metricHash[ metric ] = nextParent; // CubeProxy metric -> new metric
        copyMetricTree( item, nextParent );
    }
}
/**
 * @brief CubeWriter::copyCallTree
 * @param parentItem all children of the parent item are copied to parentCnode
 * @param parentCopy the corresponding cnode (of parentItem) of the copy
 * @param map callee of the original -> callee of the copy
 */
void
CubeWriter::copyCallTree( TreeItem* parentItem, Cnode* parentCopy, QHash<cube::Region*, cube::Region*>& map )
{
    for ( TreeItem* item : parentItem->getChildren() )
    {
        cube::Cnode*  cnode  = static_cast<cube::Cnode*> ( item->getCubeObject() );
        cube::Region* region = cnode->get_callee();

        if ( !map.contains( region ) )
        {
            Region* callee = cube->def_region(
                region->get_name(),
                region->get_mangled_name(),
                region->get_paradigm(),
                region->get_role(),
                region->get_begn_ln(),
                region->get_end_ln(),
                region->get_url(),
                region->get_descr(),
                region->get_mod()
                );
            map[ region ] = callee; // CubeProxy callee -> new callee
        }

        Cnode* nextParent = cube->def_cnode(
            map[ region ],
            cnode->get_mod(),
            cnode->get_line(),
            parentCopy
            );
        cnodeHash[ cnode ] = nextParent;
        copyCallTree( item, nextParent, map );
    }
}

void
CubeWriter::copyTaskTree()
{
    if ( taskTree )
    {
        // create artificial root nodes
        QHash<cube::Cnode*,  cube::Cnode*> artNodes;
        for ( cube::Cnode* cnode : cubeProxy->getArtificialCnodes() )
        {
            cube::Cnode*  parentCopy = cnodeHash[ cnode->get_parent() ];
            cube::Region* region     = cnode->get_callee();
            Region*       callee     = cube->def_region(
                region->get_name(),
                region->get_mangled_name(),
                region->get_paradigm(),
                region->get_role(),
                region->get_begn_ln(),
                region->get_end_ln(),
                region->get_url(),
                region->get_descr(),
                region->get_mod()
                );
            Cnode* taskNode = cube->def_cnode(
                callee,
                cnode->get_mod(),
                cnode->get_line(),
                parentCopy
                );
            artNodes[ cnode ] = taskNode;
        }

        QHash<cube::Region*, cube::Region*> map;
        for ( TreeItem* item : taskTree->getRootItem()->getChildren() )
        {
            cube::Cnode* cnode = static_cast<cube::Cnode*> ( item->getCubeObject() );
            copyCallTree( item, artNodes[ cnode ], map );
        }
    }
}

/**
 * @brief CubeWriter::copySystemTree
 * @param rootItem system tree root
 */
void
CubeWriter::copySystemTree( TreeItem* rootItem )
{
    /** Structure of the system tree: SystemTreeNode -> SystemTreeNode (repeated) -> LocationGroup -> Location */
    for ( TreeItem* item : rootItem->getChildren() )
    {
        copySystemTreeNode( item, nullptr ); // system nodes are children of the artificial root node
    }
}

/**
 * @brief CubeWriter::copySystemTreeNode recursively copies the given tree node
 * @param parentItem system tree item, whose node should be copied
 * @param parentCopy copy of the node or nullptr for root items
 */
void
CubeWriter::copySystemTreeNode( TreeItem* parentItem, SystemTreeNode* parentCopy )
{
    SystemTreeNode* parent = dynamic_cast<SystemTreeNode*> ( parentItem->getCubeObject() );
    parentCopy = cube->def_system_tree_node( parent->get_name(), parent->get_desc(), parent->get_class(), parentCopy );

    for ( TreeItem* childItem : parentItem->getChildren() )
    {
        Sysres* child = dynamic_cast<Sysres*> ( childItem->getCubeObject() );
        if ( child->isSystemTreeNode() ) // children of current node are SystemTreeNodes
        {
            copySystemTreeNode( childItem, parentCopy );
        }
        else if ( child->isLocationGroup() )  // children of current node are processes
        {
            LocationGroup* group     = dynamic_cast<LocationGroup*> ( child );
            LocationGroup* groupCopy = cube->def_location_group( group->get_name(), group->get_rank(), group->get_type(), parentCopy );

            for ( TreeItem* locationItem : childItem->getChildren() ) // all LocationGroup children
            {
                Location* location     = dynamic_cast<Location*> ( locationItem->getCubeObject() );
                Location* locationCopy = cube->def_location( location->get_name(), location->get_rank(), location->get_type(), groupCopy );
                systemHash[ group ]                        = groupCopy;
                systemIdHash[ locationCopy->get_sys_id() ] = locationCopy->get_sys_id();
            }
            if ( childItem->getChildren().size() == 0 )
            {
                Location* locationCopy = cube->def_location( "", 0, CUBE_LOCATION_TYPE_CPU_THREAD, groupCopy );
                systemHash[ group ]                        = groupCopy;
                systemIdHash[ locationCopy->get_sys_id() ] = locationCopy->get_sys_id();
            }
        }
    }
}

/**
 * @brief CubeWriter::copyTopologies copies the topologies from cubeProxy to cube
 */
void
CubeWriter::copyTopologies()
{
    const vector<Cartesian*>& cartv = cubeProxy->getCubeObject()->get_cartv();

    for ( Cartesian* cart : cartv )
    {
        Cartesian* newCart = cube->def_cart( cart->get_ndims(),
                                             cart->get_dimv(),
                                             cart->get_periodv() );

        newCart->set_name( cart->get_name() );
        newCart->set_namedims( cart->get_namedims() );

        for ( Sysres* sysP : systemHash.keys() )
        {
            Sysres*            sys      = systemHash[ sysP ];
            const TopologyMap& coordMap = cart->get_cart_sys(); // Sysres -> topology coordinates

            pair<TopologyMap::const_iterator, TopologyMap::const_iterator>ret;
            ret = coordMap.equal_range( sysP );
            if ( ret.first  != ret.second )
            {
                for ( TopologyMap::const_iterator it = ret.first; it != ret.second; ++it )
                {
                    cube->def_coords( newCart, sys, ( *it ).second );
                }
            }
        }
    }
}

/**
 * @brief CubeWriter::copyValues copies the severities of from cubeProxy to cube
 */
void
CubeWriter::copySeverities()
{
    QList<TreeItem*> callTreeItems = callTree->getItems();
    if ( taskTree )
    {
        for ( TreeItem* item : taskTree->getItems() )
        {
            callTreeItems.append( item->getChildren() );
        }
    }

    for ( TreeItem* mItem : metricTree->getItems() )
    {
        Metric*                  metric      = static_cast<Metric*>( mItem->getCubeObject() );
        cube::CalculationFlavour calc_flavor = ( metric->get_type_of_metric() == CUBE_METRIC_INCLUSIVE ) ?
                                               cube::CUBE_CALCULATE_INCLUSIVE : cube::CUBE_CALCULATE_EXCLUSIVE;

        if ( metric->get_type_of_metric() == CUBE_METRIC_POSTDERIVED ||
             metric->get_type_of_metric() == CUBE_METRIC_PREDERIVED_INCLUSIVE ||
             metric->get_type_of_metric() == CUBE_METRIC_PREDERIVED_EXCLUSIVE
             )
        {
            continue; // skip derived metrics
        }

        for ( TreeItem* cItem : callTreeItems )
        {
            cube::Cnode*    cnode = static_cast<cube::Cnode*> ( cItem->getCubeObject() );
            list_of_metrics metric_selection;
            metric_selection.push_back( std::make_pair( metric, cube::CUBE_CALCULATE_INCLUSIVE ) );
            list_of_cnodes cnode_selection;
            cnode_selection.push_back( std::make_pair( cnode, calc_flavor ) );
            std::vector< Value* >  inclusive_values;
            std::vector< Value* >  exclusive_values;
            std::vector< Value* >& values = inclusive_values;

            // get values for full tree
            cubeProxy->getSystemTreeValues( metric_selection,
                                            cnode_selection,
                                            inclusive_values,
                                            exclusive_values );

            const std::vector<Location*>& locs = cube->get_locationv();

            for ( cube::Location* loc : locs )
            {
                int    sysid = systemIdHash[ loc->get_sys_id() ];
                Value* value = values[ sysid ];
                if ( value != nullptr )
                {
                    cube->set_sev( metricHash[ metric ], cnodeHash[ cnode ], loc, value );
                    delete value;
                }
            }
        }
    }
}


CubeWriter::CubeWriter( cube::CubeProxy* cubeP, Tree* metricTree, Tree* callTree, Tree* taskTree, Tree* systemTree )
{
    this->cubeProxy  = cubeP;
    this->metricTree = metricTree;
    this->callTree   = callTree;
    this->taskTree   = taskTree;
    this->systemTree = systemTree;
}

void
CubeWriter::writeCube( const QString& filename )
{
    cube = new Cube();

    // create all trees
    copyMetricTree( metricTree->getRootItem(), nullptr );
    QHash<cube::Region*, cube::Region*> map;
    copyCallTree( callTree->getRootItem(), nullptr, map );
    copyTaskTree();

    copySystemTree( systemTree->getRootItem() );
    // create topologies
    copyTopologies();

    // init has to be called after cube structure has been created and before set_sev is called
    cube->initialize();

    // copy values to new cube
    copySeverities();

    // copy attributes
    const std::map< std::string, std::string >& attributes = cubeProxy->getAttributes();
    for ( auto i = attributes.begin(); i != attributes.end(); i++ )
    {
        string name  = i->first;
        string value = i->second;
        if ( name.rfind( "CLUSTER" ) > 0 ) // loop entry is handled later
        {
            DefaultCallTree* call = dynamic_cast<DefaultCallTree*>( callTree );
            if ( call->getLoopRootItem() )
            {
                cube->def_attr( name, value );
            }
        }
    }
    // create loop attribute
    DefaultCallTree* call = dynamic_cast<DefaultCallTree*>( callTree );
    if ( call->getLoopRootItem() )
    {
        cube::Cnode* loopRootOrig = static_cast<cube::Cnode*> ( call->getLoopRootItem()->getCubeObject() );
        int          loopID       = cnodeHash[ loopRootOrig ]->get_id();
        cube->def_attr( "CLUSTER ROOT CNODE ID", std::to_string( loopID ) );
    }

    cube->writeCubeReport( filename.toStdString() );

    delete cube;
}
