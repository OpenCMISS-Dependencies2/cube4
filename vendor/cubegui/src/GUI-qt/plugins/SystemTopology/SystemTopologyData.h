/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/




#ifndef _SYSTEMTOPOLOGYDATA_H
#define _SYSTEMTOPOLOGYDATA_H

#include <vector>
#include <math.h>
#include <iostream>
#include <map>

#include <QHash>

#include "Constants.h"
#include "PluginServices.h"

class SystemTopology;

namespace cube
{
class CubeProxy;
}

class SystemTopologyData : public QObject
{
    Q_OBJECT
signals:
    void
    dataChanged();
    void
    viewChanged();
    void
    rescaleRequest();

public slots:
    void
    selectedDimensionsChanged( const std::vector<long>& dims );
    void
    foldingDimensionsChanged( const std::vector<std::vector<int> >& );
    void
    splitLengthChanged( int length );
    void
    toggleAxis();

public:
    SystemTopologyData( SystemTopology* sys,
                        unsigned        topologyId );
    ~SystemTopologyData();

    // --------------------
    void
    initialize( cube::CubeProxy* cube ); //initialize topology from the cube object
    void
    updateColors();                      // re-compute the colors
    bool
    updateSelection();                   // re-mark selected items of topology

    // ------------------
    bool
    hasInvalidDimensions() const
    {
        return invalidDimensions;
    }
    const unsigned*
    getFoldingSeparators() const
    {
        if ( selectMode == FOLD )
        {
            return foldingDiv;
        }
        else
        {
            return 0;
        }
    }
    unsigned
    getDim( int i ) const;
    bool
    isSelected( int x,
                int y,
                int z ) const;
    cubepluginapi::TreeItem*
    getTreeItem( int x,
                 int y,
                 int z ) const;
    const QColor*
    getColor( int x,
              int y,
              int z ) const;
    QStringList
    getTooltipText( int x,
                    int y,
                    int z );

    // set the color for the lines framing processes/threads in the topology
    cubepluginapi::LineType
    getLineType()
    {
        return lineType;
    }
    void
    setLineType( cubepluginapi::LineType lineType );

    // toggle the flag if unused planes should be painted or not
    void
    toggleUnused();

    void
    setFoldingDimensions( std::vector<std::vector<int> > fdims )
    {
        foldingDimensions = fdims;
    }

    double
    getMinValue() const
    {
        return minValue;
    }
    double
    getMaxValue() const
    {
        return maxValue;
    }
    double
    getMinAbsValue() const
    {
        return minAbsValue;
    }
    double
    getMaxAbsValue() const
    {
        return maxAbsValue;
    }
    // view
    bool
    getAntialiasing() const;

    int
    getNeighbors( int x,
                  int y,
                  int z ) const;

    bool
    hasUniqueIds() const
    {
        return uniqueIds;
    }

    QList<cubepluginapi::TreeItem*>
    getNeighbors( cubepluginapi::TreeItem* item,
                  int                      steps = 1 );

    bool
    hasUnusedPlanes();

    int
    getUsedDimensions();

private:
    cubepluginapi::PluginServices* service;
    SystemTopology*                sys;

    cubepluginapi::LineType lineType;
    QColor*                 invalidColor;
    // ------ data
    // the dimensions to draw (1-3)
    unsigned dim[ 3 ];
    // mark merged dimensions (draw separator), if two dimensions are merged
    unsigned foldingDiv[ 3 ];

    // the cube object for the data
    cube::CubeProxy* cube;

    // the index of this topology in the cube object
    unsigned topologyId;

    // the topology itself: assignment of items to coordinates (without changed dim order)
    std::vector<std::vector<std::vector<cubepluginapi::TreeItem*> > > coordToItem;
    // assignment of displayed coordinates to items, an item may have multiple coordinates
    QHash<cubepluginapi::TreeItem*, std::vector<std::vector<long> > > itemToCoord;
    // if more than 3 dimensions: assignment of complete coordinates to items
    QHash<cubepluginapi::TreeItem*, const std::vector<long>* > itemToNDCoord;

    // used if one system id maps to several coordinates: contains neighbors with different id
    QMap<QString, int> neighbors;
    bool               uniqueIds;                              // system contains neighbors with same system id
    bool               isInitialised;
    int                usedDimensions;

    // for topologies with more than 3 dimensions:
    // value -1: show all elements, other: show only the given index
    std::vector<long> selectedDimensions;
    // 1. std::vector: displayed dimensions (2-3)
    // 2. std::vector: original dimension which are merged into current dimension
    std::vector<std::vector<int> > foldingDimensions;
    int                            splitLength; // split large one-dimensional topologies into chunks of splitLenth

    // colors for the topology items
    std::vector<std::vector<std::vector<const QColor*> > > colors;
    // flags whether an item is selected or not.
    std::vector<std::vector<std::vector<bool> > > selected_rects;

    // minimal and maximal current and abolute values
    double minValue, maxValue, minAbsValue, maxAbsValue;

    typedef enum { SELECT, FOLD } Mode;
    Mode selectMode;                     // if > 2 dimensions: call multiDimFold or multiDimSelect
    bool invalidDimensions;              // selection in SELECT-mode is not valid
    bool alignX;                         // if true, align one-dimensional splitted topologies at the x-axis
    // -----------------------------------------------------------------

    void
    rotateTo( const QPoint& endPoint );
    void
    setDims( unsigned dim1,
             unsigned dim2,
             unsigned dim3 );
    void
    multiDimSelect( const std::vector<long>& coord,
                    cubepluginapi::TreeItem* treeItem );
    void
    multiDimFold( const std::vector<long>& coord,
                  cubepluginapi::TreeItem* treeItem );
    void
    oneDimSplit( long                     coord,
                 cubepluginapi::TreeItem* vertex );

    void
    reinit();
    void
    determineNeighbors();

    QPair<QString, QString>
    coordToString( const std::vector<long>& coord );

    QPair<QString, QString>
    getOriginalCoordString( const std::vector<long>& coord3D );
};

#endif
