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


#include "config.h"

#include <QFileDialog>
#include "Globals.h"
#include "ValueModusCombo.h"

using namespace std;
using namespace cubegui;

ValueModusCombo::ValueModusCombo( DisplayType display )
{
    displayType = display;
    setWhatsThis( tr( "Each tree view has its own value mode combo, a drop-down menu above the tree, where it is possible to change the way the severity values are displayed.\n\nThe default value mode is the Absolute value mode. In this mode, as explained below, the severity values from the Cube file are displayed. However, sometimes these values may be hard to interpret, and in such cases other value modes can be applied. Basically, there are three categories of additional value modes.\n\n- The first category presents all severities in the tree as percentage of a reference value. The reference value can be the absolute value of a selected or a root node from the same tree or in one of the trees on the left-hand-side. For example, in the Own root percent value mode the severity values are presented as percentage of the own root's (inclusive) severity value. This way you can see how the severities are distributed within the tree. The value modes 2-8 below fall into this category.\n\nAll nodes of trees on the left-hand-side of the metric tree have undefined values. (Basically, we could compute values for them, but it would sum up the severities over all metrics, that have different meanings and usually even different units, and thus those values would not have much expressiveness.) Since we cannot compute percentage values based on undefined reference values, such value modes are not supported. For example, if the call tree is on the left-hand-side, and the metric tree is in the middle, then the metric tree does not offer the Call root percent mode.\n\n- The second category is available for system trees only, and shows the distribution of the values within hierarchy levels. E.g., the Peer percent value mode displays the severities as percentage of the maximal value on the same hierarchy depth. The value modes 9-10 fall into this category.\n\n- Finally, the External percent value mode relates the severity values to severities from another external Cube file.\n\nDepending on the type and position of the tree, the following value modes may be available:\n\n1) Absolute (default)\n2) Metric root percent\n3) Metric selection percent\n4) Call root percent\n5) Call selection percent\n6) System root percent\n7) System selection percent\n8) Own root percent\n9) Peer percent\n10) Peer distribution\n11) External percent" ) );
    connect( this, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( setValueModus( int ) ) );
}

void
ValueModusCombo::setOrder( const QList<DisplayType>& o )
{
    order = o;
    updateLabel();
}

void
ValueModusCombo::setValueModus( ValueModus modus )
{
    if ( valueModus == modus )
    {
        return;
    }
    valueModus = modus;
    int idx = findData( modus );
    setCurrentIndex( idx == -1 ? 0 : idx );
}

void
ValueModusCombo::updateLabel()
{
    clear();

    addItem( tr( ABSOLUTE_NAME ), ABSOLUTE_VALUES );

    if ( displayType == SYSTEM )
    {
        if ( isOrder( METRIC, SYSTEM ) )
        {
            addItem( tr( OWNROOT_NAME ), OWNROOT_VALUES );
            addItem( tr( METRICROOT_NAME ), METRICROOT_VALUES );
            addItem( tr( METRICSELECTED_NAME ), METRICSELECTED_VALUES );
            addItem( tr( PEER_NAME ), PEER_VALUES );
            addItem( tr( PEERDIST_NAME ), PEERDIST_VALUES );
        }
        if ( order.indexOf( METRIC ) == 0 )
        {
            if ( isOrder( CALL, SYSTEM ) )
            {
                addItem( tr( CALLROOT_NAME ), CALLROOT_VALUES );
                addItem( tr( CALLSELECTED_NAME ), CALLSELECTED_VALUES );
            }
        }
    }
    else if ( displayType == CALL )
    {
        if ( isOrder( METRIC, CALL ) )
        {
            addItem( tr( OWNROOT_NAME ), OWNROOT_VALUES );
            addItem( tr( METRICROOT_NAME ), METRICROOT_VALUES );
            addItem( tr( METRICSELECTED_NAME ), METRICSELECTED_VALUES );
        }
        if ( order.indexOf( METRIC ) == 0 )
        {
            if ( isOrder( SYSTEM, CALL ) )
            {
                addItem( tr( SYSTEMROOT_NAME ), SYSTEMROOT_VALUES );
                addItem( tr( SYSTEMSELECTED_NAME ), SYSTEMSELECTED_VALUES );
            }
        }
    }
    else // type == METRICTAB
    {
        addItem( tr( OWNROOT_NAME ), OWNROOT_VALUES );
    }

    if ( order.indexOf( METRIC ) == 0 )
    {
        addItem( tr( EXTERNAL_NAME ), EXTERNAL_VALUES );
    }

    // set selected item to previously selected if available
    int idx = findData( valueModus );
    setCurrentIndex( idx == -1 ? 0 : idx );
}

bool
ValueModusCombo::isOrder( DisplayType left, DisplayType right )
{
    return order.indexOf( right ) > order.indexOf( left );
}

// Sets the value modus to the given index of valueCombo. See constants.h for the ValueModus type.
// Notifies all its tabs about the change.
void
ValueModusCombo::setValueModus( int )
{
    valueModus = static_cast<ValueModus> ( itemData( currentIndex() ).toInt() );

    if ( valueModus == EXTERNAL_VALUES )
    {
        emit externalValueModusSelected(); // emit request to select external value mode
    }
    else
    {
        setValueModus( valueModus );
        emit valueModusChanged( valueModus );
    }
}
