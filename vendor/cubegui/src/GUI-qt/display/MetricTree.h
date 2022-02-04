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


#ifndef METRICTREE_H
#define METRICTREE_H

#include "Tree.h"
#include <QHash>

namespace cubegui
{
class MetricTree : public Tree
{
public:
    MetricTree( cube::CubeProxy* cubeProxy );

    cube::list_of_metrics
    getMetrics( const QList<TreeItem*> items ) const;
    cube::list_of_metrics
    getSelectedMetrics() const;

    TreeItem*
    getTreeItem( std::string metricId ) const;

    /** returns the maximum value of the tree which contains the given item */
    virtual double
    getMaxValue( const TreeItem* item = 0 ) const;

    void
    computeExternalReferenceValues( cube::CubeProxy* cubeExternal );

    double
    getExternalReferenceValue( const QString& name );

protected:
    virtual TreeItem*
    createTree();

    virtual QString
    getItemName( cube::Vertex* vertex ) const;

    virtual bool
    itemIsValid( cube::Vertex* vertex );

    virtual void
    computeMaxValues();

    /**
     * Set the basic inclusive and exclusive values in the full tree.
     * @param left  List of trees to the left of this tree
     * @param right List of trees to the right of this tree
     * @param itemsToCalculate, if list is not empty, only the given items are calculated, otherwise all visible items are calculated
     */
    virtual QList<Task*>
    setBasicValues( const QList<Tree*>&     leftTrees,
                    const QList<Tree*>&     rightTrees,
                    const QList<TreeItem*>& itemsToCalculate = QList<TreeItem*>( ) );

private:
    // ---- calculation functions
    void
    computeValue( TreeItem* item );

    void
    computeValue( cube::list_of_cnodes& cv,
                  TreeItem*             item );
    void
    computeValue( cube::list_of_regions& rv,
                  TreeItem*              item );
    void
    computeValue( cube::list_of_sysresources& sv,
                  TreeItem*                   item );

    void
    computeValue( cube::list_of_regions&      rv,
                  cube::list_of_sysresources& sv,
                  TreeItem*                   item );
    void
    computeValue( cube::list_of_cnodes&       cv,
                  cube::list_of_sysresources& sv,
                  TreeItem*                   item );

    void
    initializeFullDependencyHash();

    QHash<TreeItem*, QList<TreeItem*> >
    getDependencyHash( const QList<TreeItem*>& metrics,
                       const QList<TreeItem*>& derivedMetrics );

    QHash< TreeItem*, QList<TreeItem*> > derivedDependencies; // see getDependencyHash()
    QList<TreeItem*>                     unavailableItems;    // see getDependencyHash()

    // value modus related attribute
    QHash<const TreeItem*, int> maxValueIndex;
    QList<double>               maxValues;

    // external metric items and their names for the external percentage value modus
    QList<TreeItem*>       externalMetricItems;
    QHash<QString, double> externalMetricValue;
    QList<std::string>     externalMetricNames;

    friend class MetricTreeTask;
};

/** for use in QConcurrent */
class MetricTreeTask : public TreeTask
{
public:
    MetricTreeTask( TreeItem* item_, const cube::list_of_metrics& m, const cube::list_of_cnodes& c, const cube::list_of_sysresources& s, QList<TreeItem*> deps ) :
        TreeTask( item_, m, c, s ), dependentItems( deps )
    {
    }

    virtual bool
    isReady();

private:
    QList<TreeItem*> dependentItems; // items which have to be calculated before current item can be calculated
};
}

#endif // METRICTREE_H
