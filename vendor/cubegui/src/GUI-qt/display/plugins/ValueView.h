/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef VALUEVIEW_H
#define VALUEVIEW_H

#include <QPixmap>
#include "PluginServiceExports.h"

namespace cubepluginapi
{
using cubegui::TreeItem;

/**
 * @brief The ValueView class is used to display the Value objects in the trees
 */
class ValueView : public QObject
{
    Q_OBJECT
public:
    /** returns the name of this value view. This name is shown in the configure dialog */
    virtual QString
    getName() const = 0;

    /** return true, if a view for the given type is provided by this class */
    virtual bool
    isAvailable( cube::DataType type ) const = 0;

    /** returns the value, which is used to determine the color of the tree item and the sort */
    virtual double
    getDoubleValue( cube::Value* ) const = 0;

    /** return the string representation of the value of the given tree item. This value is shown left of the item label */
    virtual QString
    toString( const cubepluginapi::TreeItem* ) const = 0;

    /** returns a full description ot the tree item's value if toString doesn't show all details. */
    virtual QString
    getExtendedInfo( const TreeItem* )
    {
        return "";
    }
    virtual QWidget*
    getExtendedInfoWidget( const TreeItem* )
    {
        return 0;
    }

    /** returns the icon left of the tree item */
    virtual QPixmap
    getIcon( const cubepluginapi::TreeItem*,
             bool grayOut ) const = 0;

    /** returns size of the icon left of the tree item */
    virtual QSize
    getIconSize( const TreeItem* item ) const = 0;

    /** returns a configuration widget, that will be deleted by the caller after the config dialog is closed */
    virtual QWidget*
    getConfigWidget( cube::DataType )
    {
        return 0;
    }
    /** for config widget, uses currently selected config widget settings to display tree values */
    virtual void
    applyConfig()
    {
    }
    /** for config widget, resets the selected config widget settings */
    virtual void
    cancelConfig()
    {
    }
};
}

#endif // VALUEVIEW_H
