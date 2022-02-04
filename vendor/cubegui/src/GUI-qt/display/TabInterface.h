/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef TABINTERFACE_H
#define TABINTERFACE_H

#include <QIcon>
#include "Constants.h"

class QMainWindow;

namespace cubegui
{
/**
 * @brief The TabInterface class is inherited from all widgets, which are placed in the
 * SystemTabWidget, CallTabWidget or SystemTabWidget
 */

class TabInterface
{
public:
    /** returns the widget which will be added as tab
     */
    virtual QWidget*
    widget() = 0;

    /** returns the label which is used for the tab widget
     */
    virtual QString
    label() const = 0;

    // ---- end of pure virtual functions -----------------------------------------------

    /** This method is called, if any value of the corresponding TreeWidget has changed.It is only called
     *  if the plugin is visible or if it becomes visible and the tree values have been changed in
     *  the meantime.
     *  A tree value changes, if the selected tree item of a tree left to the current one has changed,
     *  the tree structure has changed, if an item is expanded or collapsed or if the value modus has
     *  changed... */
    virtual void
    valuesChanged()
    {
    };

    /** called, if the value modus combo box has been changed */
    virtual void
    valueModusChanged( ValueModus )
    {
    };


    /** returns the icon which is displayed in the left part of the tab */
    virtual QIcon
    icon() const
    {
        return QIcon();
    }

    /** called if current Tab becomes active or inactive */
    virtual void
    setActive( bool )
    {
    }

    /** called, if current Tab gets detached; 1st parameter is the new window, 2nd is true, if it
     * is detached and false if the window is closed */
    virtual void
    detachEvent( QMainWindow*, bool )
    {
    }
};
}
#endif // TABINTERFACE_H
