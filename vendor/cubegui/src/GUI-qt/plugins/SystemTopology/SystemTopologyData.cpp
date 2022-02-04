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

/*
   Data may be changed by
   - loading new cube file
   - loading settings
   - topology toolbar elements (via 2nd class)
   - selection in system tree widget
   => emit signal to notify view
 */

#include "Tetragon.h"
#include "SystemTopology.h"
#include "SystemTopologyData.h"
#include "PrecisionWidget.h"
#include "ValueWidget.h"

#include "CubeCartesian.h"
// #include "CubeSysres.h"
// #include "CubeThread.h"
// #include "CubeProcess.h"
// #include "CubeNode.h"
#include "CubeTypes.h"
#include "CubeSysres.h"
#include "CubeSystemTreeNode.h"
#include "CubeLocationGroup.h"
#include "CubeLocation.h"
#include "CubeProxy.h"

#include <cassert>
#include <qdebug.h>

#define XX 0
#define YY 1
#define ZZ 2

using namespace std;
using namespace cubepluginapi;

/*************** constructor / destructor *************************/

SystemTopologyData::SystemTopologyData( SystemTopology* sys,
                                        unsigned        topologyId )
{
    this->sys        = sys;
    this->service    = sys->getService();
    this->cube       = 0;
    this->topologyId = topologyId;

    // the current dimensions
    dim[ 0 ] = 0;
    dim[ 1 ] = 0;
    dim[ 2 ] = 0;

    foldingDiv[ 0 ] = 0;
    foldingDiv[ 1 ] = 0;
    foldingDiv[ 2 ] = 0;

    invalidColor      = new QColor( Qt::lightGray );
    lineType          = BLACK_LINES;
    selectMode        = FOLD;
    invalidDimensions = false;
    uniqueIds         = false;
    isInitialised     = false;
    alignX            = false;

    splitLength = 1;
    minValue    = 0;
    maxValue    = 0;
    minAbsValue = 0;
    maxAbsValue = 0;
}

SystemTopologyData::~SystemTopologyData()
{
    delete invalidColor;
}


TreeItem*
SystemTopologyData::getTreeItem( int x, int y, int z ) const
{
    // coordToItem needs coordinates in original dim order
    return coordToItem[ x ][ y ][ z ];
}

bool
SystemTopologyData::isSelected( int x, int y, int z ) const
{
    return selected_rects[ x ][ y ][ z ];
}

const QColor*
SystemTopologyData::getColor( int x, int y, int z ) const
{
    return colors[ x ][ y ][ z ];
}

/**
 * for each coordinate: check if the 4 neighbors have the same ID
 */
void
SystemTopologyData::determineNeighbors()
{
    neighbors.clear();
    for ( uint x = 0; x < dim[ 0 ]; ++x )
    {
        for ( uint y = 0; y < dim[ 1 ]; ++y )
        {
            for ( uint z = 0; z < dim[ 2 ]; ++z )
            {
                int       val = 0;
                TreeItem* id  = coordToItem[ x ][ y ][ z ]; // system id nr of current item
                TreeItem* idN;                              // neighbor system id
                if ( x > 0 )
                {
                    idN = coordToItem[ x - 1 ][ y  ][ z ]; // P3-P0
                    if ( id == idN )
                    {
                        val |= 1 << 3;
                    }
                }
                if ( x + 1 < dim[ 0 ] )
                {
                    idN = coordToItem[ x + 1 ][ y ][ z ]; // P1-P2
                    if ( id == idN )
                    {
                        val |= 1 << 1;
                    }
                }
                if ( y > 0 )
                {
                    idN = coordToItem[ x ][ y - 1 ][ z ]; // P2-P3
                    if ( id == idN )
                    {
                        val |= 1 << 0;
                    }
                }
                if ( y + 1 < dim[ 1 ] )
                {
                    idN = coordToItem[ x ][ y + 1 ][ z ]; // P0-P1
                    if ( id == idN )
                    {
                        val |= 1 << 2;
                    }
                }
                if ( val != 0 )
                {
                    QString coord = QString( "%1,%2,%3" ).arg( x ).arg( y ).arg( z );
                    neighbors.insert( coord, val );
                }
            }
        }
    }
}

/**
 * return the neighbors of the given coordinate which have the same TreeItem
 * @return for each of the four directions, the corresponding bit of the return value is set to 1, if a
 * neighbor with the same treeItem exists
 */
int
SystemTopologyData::getNeighbors( int x, int y, int z ) const
{
    QString coord = QString( "%1,%2,%3" ).arg( x ).arg( y ).arg( z );
    return neighbors.value( coord );
}

/********************** initialization / clean up ********************************/

/** map to 3 dimensions (set treeItemToCoord and coordToTreeItem for the given coord)
    selection method: show 2-3 dimensions and select one index of the rest
    @param coord coordinates in n dimensions
    @param vertex unique id for each matrix element
 */
void
SystemTopologyData::multiDimSelect( const vector<long>& coord, TreeItem* treeItem )
{
    assert( cube->getCartesian( topologyId ).get_dimv().size() >= 3 );

    // initialize selectedDimensions if not set => show first 3 dimensions and index 0 of the rest
    if ( selectedDimensions.size() != coord.size() )
    {
        selectedDimensions.clear();
        for ( uint j = 0; j < coord.size(); j++ )
        {
            if ( j < 3 )
            {
                selectedDimensions.push_back( -( j + 1 ) ); // negative = display all for dimension (-j-1)
            }
            else
            {
                selectedDimensions.push_back( 0 ); // display index 0
            }
        }
    }

    bool notSelected = false;
    int  sdims       = 0; // number dimensions wich are fully displayed (2-3)
    int  sdim[ 3 ];       // index of the dimensions with are fully displayed
    for ( unsigned int j = 0; j < selectedDimensions.size(); j++ )
    {
        if ( selectedDimensions[ j ] >= 0 )              // only one element of this dimension is displayed
        {
            if ( coord[ j ] != selectedDimensions[ j ] ) // current element is not displayed
            {
                notSelected = true;
                break; // dimension is not displayed
            }
        }
        else   // all elements of dimension j are displayed
        {
            long dim = -selectedDimensions[ j ] - 1;
            sdim[ dim ] = j;
            sdims++;
        }
    }

    if ( !notSelected ) // element is visible
    {
        int val[ 3 ];   // x,y,z
        for ( int i = 0; i < sdims; i++ )
        {
            val[ i ] = coord[ sdim[ i ] ];
        }

        if ( sdims == 3 )
        {
            coordToItem[ val[ 0 ] ][ val[ 1 ] ][ val[ 2 ] ] = treeItem;
        }
        else if ( sdims == 2 ) // special case: slice to 2 dimensions
        {
            coordToItem[ val[ 0 ] ][ val[ 1 ] ][ 0 ] = treeItem;
        }

        vector<long> coord;
        for ( int j = 0; j < sdims; j++ )
        {
            coord.push_back( val[ j ] );
        }
        itemToCoord[ treeItem ].push_back( coord );
    } // !notSelected
}

/** merge several dimensions into one
 */
void
SystemTopologyData::multiDimFold( const vector<long>& coord, TreeItem* treeItem )
{
    assert( cube->getCartesian( topologyId ).get_dimv().size() >= 2 );
    // get the orginal dimensions
    const vector<long>& dims = cube->getCartesian( topologyId ).get_dimv();

    int dimensions = foldingDimensions.size();
    int val[ dimensions ];                      // x,y,[z]
    for ( int i = 0; i < dimensions; i++ )
    {
        // fold all dimensions of vector foldingDimensions[i] into one
        int elements = 1;
        val[ i ] = 0;
        for ( int j = foldingDimensions[ i ].size() - 1; j >= 0; j-- )
        {
            int dimIndex = foldingDimensions[ i ][ j ];
            val[ i ] += elements * coord[ dimIndex ];
            elements *= dims[ dimIndex ];
        }
    }

    coordToItem[ val[ 0 ] ][ val[ 1 ] ][ val[ 2 ] ] = treeItem;

    vector<long> newCoord;
    for ( int i = 0; i < dimensions; i++ )
    {
        newCoord.push_back( val[ i ] );
    }
    itemToCoord[ treeItem ].push_back( newCoord );
}

void
SystemTopologyData::toggleAxis()
{
    alignX = !alignX;
    reinit();
}

/**
 * @brief SystemTopologyData::oneDimSplit
 * splits a one dimensional topology into a two dimensional one
 */
void
SystemTopologyData::oneDimSplit( long coord, TreeItem* treeItem )
{
    int dim1 = coord / splitLength;
    int dim2 = coord % splitLength;

    if ( alignX )
    {
        std::swap( dim1, dim2 );
    }

    coordToItem[ dim1 ][ dim2 ][ 0 ] = treeItem;

    vector<long> newCoord;
    newCoord.push_back( dim1 );
    newCoord.push_back( dim2 );
    newCoord.push_back( 0 );
    itemToCoord[ treeItem ].push_back( newCoord );
}

QPair<QString, QString>
SystemTopologyData::coordToString( const vector<long>& coord )
{
    QString label, value;

    const vector<long>&       dims     = cube->getCartesian( topologyId ).get_dimv(); // original dims
    const vector<bool>&       period   = cube->getCartesian( topologyId ).get_periodv();
    const vector<std::string> dimNames = cube->getCartesian( topologyId ).get_namedims();

    for ( unsigned j = 0; j < coord.size(); j++ )
    {
        if ( dimNames.size() > 0 )
        {
            label.append( QString::fromStdString( dimNames[ j ] ) );
        }
        else
        {
            label.append( tr( "Coord: " ) ).append( QString::number( j ) );
        }
        value.append( QString::number( coord[ j ] ) );

        QString dimInfo = tr( " ( size " );
        dimInfo.append( QString::number( dims[ j ] ) );

        if ( period[ j ] )
        {
            dimInfo.append( tr( ", period " ) );
        }
        dimInfo.append( " ) " );

        label.append( dimInfo );
        if ( j != coord.size() - 1 )
        {
            label.append( "\n" );
            value.append( "\n" );
        }
    }

    return QPair<QString, QString>( label, value );
}

QPair<QString, QString>
SystemTopologyData::getOriginalCoordString( const vector<long>& coord3D )
{
    vector<long> coord; // original coords

    if ( selectMode == SELECT )
    {
        int i = 0;
        for ( unsigned int j = 0; j < selectedDimensions.size(); j++ )
        {
            if ( selectedDimensions[ j ] >= 0 )   // only one element of this dimension is displayed
            {
                coord.push_back( selectedDimensions[ j ] );
            }
            else
            {
                coord.push_back( coord3D[ i++ ] );
            }
        }
    }
    else   // FOLD
    {
        int                 dimensions = foldingDimensions.size();
        const vector<long>& dims       = cube->getCartesian( topologyId ).get_dimv(); // original dims
        coord.resize( dims.size() );
        for ( int i = 0; i < dimensions; i++ )
        {
            int idx3D = coord3D[ i ];
            for ( int j = foldingDimensions[ i ].size() - 1; j >= 0; j-- )
            {
                int dimIndex = foldingDimensions[ i ][ j ];
                coord[ dimIndex ] = idx3D % dims[ dimIndex ];
                idx3D            /= dims[ dimIndex ];
            }
        }
    }
    return coordToString( coord );
}

/* initializes the topology with data from the cube object
 */
void
SystemTopologyData::initialize( cube::CubeProxy* cube )
{
    this->cube    = cube;
    isInitialised = false;
    uniqueIds     = true;

    reinit();
}

/**
   @param resetValue if true, reset angles of topology and plane distance
 */
void
SystemTopologyData::reinit()
{
    QSet<TreeItem*> idSet;

    // get the dimensions
    const vector<long>&       dims     = cube->getCartesian( topologyId ).get_dimv();
    const vector<std::string> dimNames = cube->getCartesian( topologyId ).get_namedims();

    usedDimensions = 0;
    int usedDimension = -1;
    for ( unsigned i = 0; i < dims.size(); i++ )
    {
        if ( dims[ i ] > 1 )
        {
            usedDimensions++;
            usedDimension = i;
        }
    }
    usedDimension = std::max( usedDimension, 0 );

    bool isOneDimensional = ( usedDimensions <= 1 ); // only one element or a vector
    // set the dimensions
    if ( dims.size() == 0 )
    {
        return;
    }
    else if ( isOneDimensional )
    {
        if ( ( splitLength > 1 )  &&  ( dims[ usedDimension ] / ( 1. * splitLength ) > 1 ) )
        {
            // split one dimensional topology into two dimensions
            int dim1 = ( dims[ usedDimension ] + splitLength - 1 ) / splitLength;
            int dim2 = splitLength;
            if ( alignX )
            {
                std::swap( dim1, dim2 );
            }
            setDims( dim1, dim2, 1 );
        }
        else // default one dimensional representation on y-axis
        {
            setDims( 1, dims[ usedDimension ], 1 );
            foldingDiv[ 2 ] = 0;
            foldingDiv[ 1 ] = 0;
            foldingDiv[ 0 ] = 0;
        }
    }
    else // if more than 3 dimensions => reduce to 3 to display
    {    // display 2 ore 3 dimensions and only one item of the rest
        if ( selectMode == SELECT )
        {
            int sdims = 0; // number of displayed dimensions (2-3)
            int sdim[ 3 ]; // indices of the displayed dimensions in original vector (dims)
            // e.g. sdim[0] is the index of the first displayed dimension
            for ( uint j = 0; j < selectedDimensions.size() && sdims < 3; j++ )
            {
                if ( selectedDimensions[ j ] <= -1 )        // negative = show full dimension
                {
                    int dim = -selectedDimensions[ j ] - 1; // dim = position of selected dimension
                    sdim[ dim ] = j;
                    sdims++;
                }
            }
            int val[ sdims ]; // x,y,z
            for ( int i = 0; i < sdims; i++ )
            {
                val[ i ] = dims[ sdim[ i ] ]; // x,y,z -> number of elements
            }
            if ( sdims == 2 )
            {
                setDims( val[ 0 ], val[ 1 ], 1 );
            }
            else if ( sdims >= 3 )
            {
                setDims( val[ 0 ], val[ 1 ], val[ 2 ] );
            }
        }
        // folding is selected => merge dimensions
        else if ( selectMode == FOLD )
        {
            uint dimensions = foldingDimensions.size();
            int  foldingDims[ dimensions ];
            for ( uint i = 0; i < dimensions; i++ )
            {
                int elements = 1;
                for ( uint j = 0; j < foldingDimensions[ i ].size(); j++ )
                {
                    int dimIndex = foldingDimensions[ i ][ j ];
                    elements *= dims[ dimIndex ];
                }
                foldingDims[ i ] = elements;
            }

            setDims( foldingDims[ 0 ], foldingDims[ 1 ], foldingDims[ 2 ] );

            // mark merged dimensions (draw separator), if two dimensions are merged
            for ( int i = 0; i < 3; i++ )
            {
                foldingDiv[ i ] = foldingDimensions[ i ].size() == 2 ?
                                  dims[ foldingDimensions[ i ][ 1 ] ] : 1;
            }
            if ( dimensions == 2 )   // merged to 2-dimensional topology
            {
                foldingDiv[ 2 ] = foldingDiv[ 1 ];
                foldingDiv[ 1 ] = foldingDiv[ 0 ];
                foldingDiv[ 0 ] = 0;
            }
        }
    } // if >=3 dims

    assert( dim[ 0 ] > 0 );
    assert( dim[ 1 ] > 0 );
    assert( dim[ 2 ] > 0 );

    // initialize the topology;
    // the value -1 denotes that at that node no thread/process was running
    coordToItem.resize( dim[ 0 ] );
    for ( unsigned i = 0; i < dim[ 0 ]; i++ )
    {
        coordToItem[ i ].resize( dim[ 1 ] );
        for ( unsigned j = 0; j < dim[ 1 ]; j++ )
        {
            coordToItem[ i ][ j ].resize( dim[ 2 ] );
            for ( unsigned k = 0; k < dim[ 2 ]; k++ )
            {
                coordToItem[ i ][ j ][ k ] = 0;
            }
        }
    }

    itemToCoord.clear();
    itemToNDCoord.clear();

    const cube::TopologyMap& cubeSystemToCoord =
        cube->getCartesian( topologyId ).get_cart_sys();
    std::multimap<const cube::Sysres*, vector<long> >::const_iterator itr;

    const QList<TreeItem*>& items = service->getTreeItems( SYSTEM );
    foreach( TreeItem * item, items )
    {
        // toCompute tells if the system item is a thread or
        // a process in the single-threaded case
        bool valid = false;

        TreeItemType type = item->getType();
        if ( type == LOCATIONITEM )
        {
            itr   = cubeSystemToCoord.find( ( cube::Sysres* )item->getCubeObject() );
            valid = true;
        }
        else if ( type == LOCATIONGROUPITEM && item->isLeaf() )
        {
            // single-threaded processes
            if ( ( ( cube::Process* )( item->getCubeObject() ) )->num_children() == 1 )
            {
                // the topology can be defined either on processes or on threads, we
                // therefore check both...
                itr = cubeSystemToCoord.find( ( cube::Sysres* )( ( ( cube::Process* )item->getCubeObject() )->get_child( 0 ) ) );
                if ( itr == cubeSystemToCoord.end() )
                {
                    itr = cubeSystemToCoord.find( ( cube::Sysres* )( item->getCubeObject() ) );
                }
                valid = true;
            }
        }

        if ( valid )
        {
            if ( itr == cubeSystemToCoord.end() )
            {
                continue; // item is not displayed in current topology
            }
            std::multimap <const cube::Sysres*, vector<long> >::const_iterator                                                                                it;
            std::pair< std::multimap<const cube::Sysres*, vector<long> >::const_iterator, std::multimap<const cube::Sysres*, vector<long> >::const_iterator > ret;

            ret = cubeSystemToCoord.equal_range( itr->first );
            if (  ret.first  != ret.second )
            {
                for ( it = ret.first; it != ret.second; ++it ) // run over all coordinates of this thread
                {
                    const vector<long>& coord = it->second;

                    itemToNDCoord[ item ] = &coord;

                    if ( isOneDimensional )
                    {
                        if ( ( splitLength > 1 )  &&  ( dims[ usedDimension ] / ( 1. * splitLength ) > 1 ) )
                        {
                            oneDimSplit( coord[ usedDimension ], item );
                        }
                        else
                        {
                            coordToItem[ 0 ][ coord[ usedDimension ] ][ 0 ] = item;
                            vector<long> newCoord;
                            newCoord.push_back( 0 );
                            newCoord.push_back( coord[ usedDimension ] );
                            newCoord.push_back( 0 );
                            itemToCoord[ item ].push_back( newCoord );
                        }
                    }
                    else if ( coord.size() >= 2 )
                    {
                        if ( selectMode == SELECT )
                        {
                            multiDimSelect( coord, item );
                        }
                        else
                        {
                            multiDimFold( coord, item );
                        }
                    }
                    if ( !isInitialised ) // check only one time, if ids are unique for each coordinate
                    {
                        if ( idSet.contains( item ) )
                        {
                            uniqueIds     = false;
                            isInitialised = true;
                        }
                        else
                        {
                            idSet.insert( item );
                        }
                    }
                }
            }
        } // toCompute
    }
    isInitialised = true;

    // we store in the tab widget if unused topology planes should be painted
    if ( !sys->getShowUnusedTopologyPlanes() )
    {
        toggleUnused();
    }

    if ( !uniqueIds )
    {
        determineNeighbors();
    }
    colors.clear();
    updateColors();
    updateSelection();
    emit rescaleRequest();
} // reinit

/************************* coloring  ***********************/
inline static void
updateMinMaxValues( bool& first, double value, double& minValue, double& maxValue )
{
    if ( first )
    {
        first    = false;
        minValue = value;
        maxValue = value;
    }
    else
    {
        if ( minValue > value )
        {
            minValue = value;
        }
        else if ( maxValue < value )
        {
            maxValue = value;
        }
    }
}

void
SystemTopologyData::updateColors()
{
    if ( dim[ 0 ] == 0 || dim[ 1 ] == 0 || dim[ 2 ] == 0 )
    {
        return;
    }

    if ( colors.size() == 0 )
    {
        selected_rects.clear();
        colors.resize( dim[ 0 ] );
        selected_rects.resize( dim[ 0 ] );
        for ( unsigned i = 0; i < dim[ 0 ]; i++ )
        {
            colors[ i ].resize( dim[ 1 ] );
            selected_rects[ i ].resize( dim[ 1 ] );
            for ( unsigned j = 0; j < dim[ 1 ]; j++ )
            {
                colors[ i ][ j ].resize( dim[ 2 ] );
                selected_rects[ i ][ j ].resize( dim[ 2 ] );
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    colors[ i ][ j ][ k ]         = NULL;
                    selected_rects[ i ][ j ][ k ] = false;
                }
            }
        }
    }

    bool   first    = true;
    bool   firstAbs = true;
    double value;

    // since we consider threads or single-threaded processes only,
    // the min/max values differ from that of the system tree,
    // thus we must compute them;

    minValue    = 0.0;
    maxValue    = 0.0;
    minAbsValue = 0.0;
    maxAbsValue = 0.0;

    // compute minimal and maximal values

    for ( unsigned i0 = 0; i0 < dim[ 0 ]; i0++ )
    {
        for ( unsigned i1 = 0; i1 < dim[ 1 ]; i1++ )
        {
            for ( unsigned i2 = 0; i2 < dim[ 2 ]; i2++ )
            {
                // get the id of the item at the current topology position
                TreeItem* treeItem = coordToItem[ i0 ][ i1 ][ i2 ];
                if ( treeItem )
                {
                    // get its absolute value from the system topology widget
                    value = treeItem->getAbsoluteValue();
                    // update the variables storing the minimal and maximal absolute values
                    updateMinMaxValues( firstAbs, value, minAbsValue, maxAbsValue );

                    // if the current value modus is not the absolute value modus
                    if ( !( service->getValueModus() == ABSOLUTE_VALUES ) )
                    {
                        // get the current value for the item at the current
                        // topology position from the system tree widget
                        value = treeItem->getValue();
                        // update the variables for the minimal and maximal values
                        updateMinMaxValues( first, value, minValue, maxValue );
                    }

                    // assign each item a color
                    colors[ i0 ][ i1 ][ i2 ] = &treeItem->getColor();
                }
                else // sysid < 0
                {
                    colors[ i0 ][ i1 ][ i2 ] = invalidColor;
                }
            }     // for
        }
    }
    if ( minValue < 0.0 )
    {
        minValue = 0.0;
    }
    if ( minAbsValue < 0.0 )
    {
        minAbsValue = 0.0;
    }

    if ( service->getValueModus() == ABSOLUTE_VALUES )
    {
        minValue = minAbsValue;
        maxValue = maxAbsValue;
    }
}

/**
   initializes selected_rects from service->getSelections(SYSTEM)
   @returns true, if the selection has been changed
 */
bool
SystemTopologyData::updateSelection()
{
    /* Put all leaf elements of systemTreeWidget->selectedItems() into selectedLeafs.
     * If selected tree item is collapsed, select all its leafs, otherwise ignore.
     */
    QList<TreeItem*> selected = service->getSelections( SYSTEM );
    QList<TreeItem*> selectedLeafs;
    foreach( TreeItem * item, selected )
    {
        if ( item->isLeaf() )
        {
            selectedLeafs.append( item );
        }
        else if ( !item->isExpanded() && !item->isTopLevelItem() )
        {
            // select all leaf childs of collapsed selection but don't select leafs of root items
            // (otherwise all items would be selected at startup)
            selectedLeafs.append( item->getLeafs() );
        }
    }

    /*
     * mark items of selectedLeafs in the coordinate array selected_rects
     */
    bool changed = false;
    for ( unsigned i0 = 0; i0 < dim[ 0 ]; i0++ )
    {
        for ( unsigned i1 = 0; i1 < dim[ 1 ]; i1++ )
        {
            for ( unsigned i2 = 0; i2 < dim[ 2 ]; i2++ )
            {
                TreeItem* treeItem = coordToItem[ i0 ][ i1 ][ i2 ];
                if ( !treeItem )
                {
                    selected_rects[ i0 ][ i1 ][ i2 ] = false;
                }
                else
                {
                    bool marked = false;
                    foreach( TreeItem * item, selectedLeafs )
                    {
                        if ( item == treeItem )
                        {
                            marked = true;
                            break;
                        }
                    }
                    if ( selected_rects[ i0 ][ i1 ][ i2 ] != marked )
                    {
                        changed = true;
                    }
                    selected_rects[ i0 ][ i1 ][ i2 ] = marked;
                }
            }
        }
    }
    return changed;
}


unsigned
SystemTopologyData::getDim( int i ) const
{
    if ( i <= 3 )
    {
        return dim[ i ];
    }
    else
    {
        return 0;
    }
}


/**************************** miscellaneous **********************/

// set the line type for topologies: black, gray, white, or no lines
//
void
SystemTopologyData::setLineType( LineType lineType )
{
    this->lineType = lineType;
    if ( cube != NULL )
    {
        emit viewChanged();
    }
}

bool
SystemTopologyData::hasUnusedPlanes()
{
    // check first dimension for unused planes
    for ( unsigned i = 0; i < dim[ 0 ]; i++ )
    {
        bool unused = true;
        for ( unsigned j = 0; ( j <  dim[ 1 ] ) && unused; j++ )
        {
            for ( unsigned k = 0; ( k < dim[ 2 ] ) && unused; k++ )
            {
                if ( coordToItem[ i ][ j ][ k ] )
                {
                    unused = false;
                }
            }
        }
        if ( unused )
        {
            return true;           // found one unused plane
        }
    }

    for ( unsigned i = 0; i < dim[ 1 ]; i++ )
    {
        bool unused = true;
        for ( unsigned j = 0; ( j <  dim[ 0 ] ) && unused; j++ )
        {
            for ( unsigned k = 0; ( k < dim[ 2 ] ) && unused; k++ )
            {
                if ( coordToItem[ j ][ i ][ k ] )
                {
                    unused = false;
                }
            }
        }
        if ( unused )
        {
            return true;           // found one unused plane
        }
    }

    for ( unsigned i = 0; i < dim[ 2 ]; i++ )
    {
        bool unused = true;
        for ( unsigned j = 0; ( j <  dim[ 0 ] ) && unused; j++ )
        {
            for ( unsigned k = 0; ( k < dim[ 1 ] ) && unused; k++ )
            {
                if ( coordToItem[ j ][ k ][ i ] )
                {
                    unused = false;
                }
            }
        }
        if ( unused )
        {
            return true;           // found one unused plane
        }
    }

    return false;
}

int
SystemTopologyData::getUsedDimensions()
{
    return usedDimensions;
}

// toggle the state if unused planes should be displayed or not
//
void
SystemTopologyData::toggleUnused()
{
    // first case: if unused topology planes are not shown
    if ( !sys->getShowUnusedTopologyPlanes() )
    {
        if ( dim[ 0 ] == 0 || dim[ 1 ] == 0 || dim[ 2 ] == 0 )
        {
            return;
        }
        // check the first dimension for unused planes
        for ( unsigned i = 0; i < dim[ 0 ]; i++ )
        {
            bool unused = true;
            for ( unsigned j = 0; j < dim[ 1 ]; j++ )
            {
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    if ( coordToItem[ i ][ j ][ k ] )
                    {
                        unused = false;
                        break;
                    }
                }
                if ( !unused )
                {
                    break;
                }
            }
            if ( unused )
            {
                for ( unsigned i2 = i + 1; i2 < dim[ 0 ]; i2++ )
                {
                    coordToItem[ ( int )i2 - 1 ] = coordToItem[ i2 ];
                }
                setDims( dim[ 0 ] - 1, dim[ 1 ], dim[ 2 ] );
                coordToItem.resize( dim[ 0 ] );
                i--;
            }
        }

        // check the second dimension for unused planes
        for ( unsigned j = 0; j < dim[ 1 ]; j++ )
        {
            bool unused = true;
            for ( unsigned i = 0; i < dim[ 0 ]; i++ )
            {
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    if ( coordToItem[ i ][ j ][ k ] )
                    {
                        unused = false;
                        break;
                    }
                }
                if ( !unused )
                {
                    break;
                }
            }

            if ( unused )
            {
                for ( unsigned i = 0; i < dim[ 0 ]; i++ )
                {
                    for ( unsigned j2 = j + 1; j2 < dim[ 1 ]; j2++ )
                    {
                        coordToItem[ i ][ ( int )j2 - 1 ] = coordToItem[ i ][ j2 ];
                    }
                    coordToItem[ i ].resize( dim[ 1 ] - 1 );
                }
                setDims( dim[ 0 ], dim[ 1 ] - 1, dim[ 2 ] );
                j--;
            }
        }

        // check the third dimension for unused planes
        for ( unsigned k = 0; k < dim[ 2 ]; k++ )
        {
            bool unused = true;
            for ( unsigned i = 0; i < dim[ 0 ]; i++ )
            {
                for ( unsigned j = 0; j < dim[ 1 ]; j++ )
                {
                    if ( coordToItem[ i ][ j ][ k ] )
                    {
                        unused = false;
                        break;
                    }
                }
                if ( !unused )
                {
                    break;
                }
            }
            if ( unused )
            {
                for ( unsigned i = 0; i < dim[ 0 ]; i++ )
                {
                    for ( unsigned j = 0; j < dim[ 1 ]; j++ )
                    {
                        for ( unsigned k2 = k + 1; k2 < dim[ 2 ]; k2++ )
                        {
                            coordToItem[ i ][ j ][ ( int )k2 - 1 ] = coordToItem[ i ][ j ][ k2 ];
                        }
                        coordToItem[ i ][ j ].resize( dim[ 2 ] - 1 );
                    }
                }
                setDims( dim[ 0 ], dim[ 1 ], dim[ 2 ] - 1 );
                k--;
            }
        }
        for ( unsigned i = 0; i < dim[ 0 ]; i++ )
        {
            for ( unsigned j = 0; j < dim[ 1 ]; j++ )
            {
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    TreeItem* item = coordToItem[ i ][ j ][ k ];
                    if ( item )
                    {
                        itemToCoord[ item ].clear();
                    }
                }
            }
        }
        for ( unsigned i = 0; i < dim[ 0 ]; i++ )
        {
            for ( unsigned j = 0; j < dim[ 1 ]; j++ )
            {
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    TreeItem* item = coordToItem[ i ][ j ][ k ];
                    if ( item )
                    {
                        vector< long > coord;
                        coord.resize( 3 );
                        coord[ 0 ] = i;
                        coord[ 1 ] = j;
                        coord[ 2 ] = k;
                        itemToCoord[ item ].push_back( coord );
                    }
                }
            }
        }

        colors.clear();
        updateColors();
        updateSelection();
        emit rescaleRequest();
    }
    else
    {
        // second case: unused planes are displayed, too
        // in this case just re-initialize
        reinit();
    }
}

bool
SystemTopologyData::getAntialiasing() const
{
    return sys->getAntialiasing();
}


/**************************** dimension settings *****************/

// set the original dimensions
void
SystemTopologyData::setDims( unsigned dim0, unsigned dim1, unsigned dim2 )
{
    dim[ 0 ] = dim0;
    dim[ 1 ] = dim1;
    dim[ 2 ] = dim2;
}


/**
   used for topologies with more than 3 dimensions
   the method is called, if the user has changed the displayed dimensions
 */
void
SystemTopologyData::selectedDimensionsChanged( const vector<long>& dims )
{
    if ( dims.size() > 0 )
    {
        invalidDimensions  = false;
        selectedDimensions = dims;
        selectMode         = SELECT;
        reinit();
    }
    else
    {
        invalidDimensions = true;
    }
    emit dataChanged();
}

void
SystemTopologyData::foldingDimensionsChanged( const vector<vector<int> >& fold )
{
    if ( fold.size() > 0 )
    {
        foldingDimensions = fold;
        invalidDimensions = false;
        selectMode        = FOLD;
        reinit(); // count/size of dimensions may have changed by folding
    }
    else
    {
        invalidDimensions = true;
    }
    emit dataChanged();
}

void
SystemTopologyData::splitLengthChanged( int length )
{
    splitLength = length;
    reinit();
    emit dataChanged();
}

QStringList
SystemTopologyData::getTooltipText( int x, int y, int z )
{
    QStringList ret;

    if ( ( x >= 0 ) && ( y >= 0 ) && ( z >= 0 ) && ( x < ( int )dim[ 0 ] ) &&
         ( y < ( int )dim[ 1 ] ) && ( z < ( int )dim[ 2 ] ) )
    {
        QString nameStr, valueStr, absValueStr, rankStr, idStr, nodeStr, numberOfElemStr;

        // though we always store internally 3 dimensions, in the output
        // only the real dimensions should be displayed
        TreeItem* item = this->getTreeItem( x, y, z );

        // generate the strings of the info box
        vector<long> coord3D;
        coord3D.push_back( x );
        coord3D.push_back( y );
        coord3D.push_back( z );

        QPair<QString, QString> coordInfo;

        if ( !item )
        {
            coordInfo       = getOriginalCoordString( coord3D );
            nameStr         = "-";
            valueStr        = "-";
            absValueStr     = "-";
            rankStr         = "-";
            idStr           = "-";
            nodeStr         = "-";
            numberOfElemStr = "-";
        }
        else
        {
            bool   userDefinedMinMaxValues;
            double userMinValue, userMaxValue;
            userDefinedMinMaxValues = service->getUserDefinedMinValues( SYSTEM, userMinValue, userMaxValue );

            const vector<long>* elem = itemToNDCoord[ item ];
            coordInfo = coordToString( *elem );
            vector<vector<long> >& all_coords = itemToCoord[ item ];
            numberOfElemStr.append( QString::number( all_coords.size() ) );

            nameStr.append( item->getName() );
            bool intType = service->intMetricSelected();

            // compute percentage of the value on the color scale
            double value = item->getValue();
            valueStr.append(  service->formatNumber( value, FORMAT_DOUBLE, intType ) );
            double percent = ( userDefinedMinMaxValues ? userMaxValue - userMinValue : getMaxValue() - getMinValue() );
            percent = ( percent == 0.0 ? 100.0 : 100.0 * ( value - ( userDefinedMinMaxValues ? userMinValue : getMinValue() ) ) / percent );
            valueStr.append( " (" );
            valueStr.append( service->formatNumber( percent, false ) );
            valueStr.append( "%)" );

            if ( service->getValueModus() != ABSOLUTE_VALUES )
            {
                double absValue = item->getAbsoluteValue();
                absValueStr = service->formatNumber( absValue, FORMAT_DOUBLE, intType );
                // compute percentage of the absolute value on the color scale
                double absPercent = ( getMaxAbsValue() - getMinAbsValue() );
                absPercent = ( absPercent == 0.0 ? 100.0 : 100.0 * ( absValue - getMinAbsValue() ) / absPercent );
                absValueStr.append( " (" );
                absValueStr.append( service->formatNumber( absPercent, false ) );
                absValueStr.append( "%)" );
            }

            // get rank, node, and id
            if ( item->getType() == LOCATIONITEM )
            {
                cube::Sysres* sysres = ( ( cube::Sysres* )item->getCubeObject() );
                assert( ( ( cube::Thread* )sysres )->get_parent() != NULL );
                assert( ( ( cube::Thread* )sysres )->get_parent()->get_parent() != NULL );
                rankStr.append( QString::number( ( ( cube::Thread* )sysres )->get_parent()->get_rank() ) );
                idStr.append( QString::number( ( ( cube::Thread* )sysres )->get_rank() ) );
                nodeStr.append( QString::fromStdString( ( ( cube::Thread* )sysres )->get_parent()->get_parent()->get_name() ) );
            }
            else
            {
                cube::Sysres* sysres = ( ( cube::Sysres* )item->getCubeObject() );
                assert( item->getType() == LOCATIONGROUPITEM );
                rankStr.append( QString::number( ( ( cube::Process* )sysres )->get_rank() ) );
                if ( ( ( cube::Process* )sysres )->num_children() == 0 )
                {
                    idStr.append( "-" );
                }
                else
                {
                    assert( ( ( cube::Process* )sysres )->num_children() == 1 );
                    idStr.append( QString::number( ( ( cube::Process* )sysres )->get_child( 0 )->get_rank() ) );
                }
                nodeStr.append( QString::fromStdString( ( ( cube::Process* )sysres )->get_parent()->get_name() ) );
            }
        } // item is valid

        QString coordLabel = coordInfo.first;
        QString coordStr   = coordInfo.second;
        QString text1;

        text1.append( coordLabel );
        text1.append( "\n" );

        text1.append( tr( "Node:" ) );
        text1.append( "\n" );

        text1.append( tr( "Name:" ) );
        text1.append( "\n" );

        text1.append( tr( "MPI rank:" ) );
        text1.append( "\n" );

        text1.append( tr( "Thread id:" ) );
        text1.append( "\n" );

        text1.append( tr( "Value:" ) );

        if ( service->getValueModus() != ABSOLUTE_VALUES )
        {
            text1.append( "\n" );
            text1.append( tr( "Absolute:" ) );
        }
        text1.append( "\n" );
        text1.append( tr( "Number of elements:" ) );
        // text1.append( "\n" );

        QString text2;

        text2.append( coordStr );
        text2.append( "\n" );

        text2.append( nodeStr );
        text2.append( "\n" );

        text2.append( nameStr );
        text2.append( "\n" );

        text2.append( rankStr );
        text2.append( "\n" );

        text2.append( idStr );
        text2.append( "\n" );

        text2.append( valueStr );

        if ( service->getValueModus() != ABSOLUTE_VALUES )
        {
            text2.append( "\n" );
            text2.append( absValueStr );
        }
        text2.append( "\n" );
        text2.append( numberOfElemStr );
        // text2.append( "\n" );

        ret.append( text1 );
        ret.append( text2 );
    }

    return ret;
}

/**
 * calculate all neighbors of item treeItem in the current topology which can be
 * reached with the given steps
 */
QList<TreeItem*>
SystemTopologyData::getNeighbors( TreeItem* item, int steps )
{
    const cube::Cartesian& cart = cube->getCartesian( topologyId );
    const vector<bool>&    p    = cart.get_periodv();
    vector<bool>           period;
    for ( int i = 0; i < 3; i++ )
    {
        period.push_back( false );
    }

    if ( selectMode == SELECT )
    {
        for ( unsigned int i = 0; i < selectedDimensions.size(); i++ )
        {
            int dim = selectedDimensions[ i ];
            period[ i ] = p[ dim ];
        }
    }
    else     // selectMode == FOLD
    {
        int dimensions = foldingDimensions.size();
        for ( int i = 0; i < dimensions; i++ )
        {
            if ( foldingDimensions[ i ].size() == 1 )
            {
                int dim = foldingDimensions[ i ][ 0 ];
                period[ i ] = p[ dim ];
            }
        }
    }

    QList<TreeItem*> vec;
    if ( steps == 0 )
    {
        return vec;
    }
    vector< vector<long> >& all_coords = itemToCoord[ item ];
    vector<long>            _coord     = all_coords.at( 0 );
    unsigned                x          = ( unsigned )_coord[ 0 ];
    unsigned                y          = ( unsigned )_coord[ 1 ];
    unsigned                z          = ( unsigned )_coord[ 2 ];

    for ( int dx = -steps; dx <= steps; dx++ )
    {
        for ( int dy = -steps; dy <= steps; dy++ )
        {
            for ( int dz = -steps; dz <= steps; dz++ )
            {
                if ( abs( dx ) + abs( dy ) + abs( dz ) > steps )
                {
                    continue;
                }
                int nx = x + dx;
                int ny = y + dy;
                int nz = z + dz;
                if ( period.at( XX ) == true )
                {
                    nx = ( nx + dim[ XX ] ) % dim[ XX ];
                }
                else if ( nx < 0 || nx >= ( int )dim[ XX ] )
                {
                    continue;
                }
                if ( period.at( YY ) == true )
                {
                    ny = ( ny + dim[ YY ] ) % dim[ YY ];
                }
                else if ( ny < 0 || ny >= ( int )dim[ YY ] )
                {
                    continue;
                }
                if ( period.at( ZZ ) == true )
                {
                    nz = ( nz + dim[ ZZ ] ) % dim[ ZZ ];
                }
                else if ( nz < 0 || nz >= ( int )dim[ ZZ ] )
                {
                    continue;
                }
                TreeItem* treeItem = getTreeItem( nx, ny, nz );
                if ( treeItem != item )
                {
                    vec.push_back( treeItem );
                }
            }
        }
    }

    return vec;
}
